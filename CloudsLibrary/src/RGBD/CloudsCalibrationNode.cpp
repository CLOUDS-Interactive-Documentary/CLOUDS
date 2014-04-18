
#include "CloudsCalibrationNode.h"
#include "CloudsVisualSystem.h"

float CalibrationNode::nodeActivatedTime = 0;

CalibrationNode::CalibrationNode(){
	
	cursorDistance = .0;
	hover = false;
	finished = false;
	finishedTime = .0;
	percentComplete = 0.0;
	hoverStartTime = 0.0;
	gazePercent = 0.0;
	multiplier = 0;
	clickSound = NULL;
	selectSound = NULL;
	nodeActivatedTime = 0;
	titleTypeOffset = 0;
	introNode = false;
	cam = NULL;
	
}

void CalibrationNode::updatePosition(){
	worldPosition = ofVec3f( baseOffset.x, -baseOffset.y, baseOffset.z + titleTypeOffset);
	worldPosition.x *= multiplier;
	worldPosition.y *= multiplier;
	
	if(cam != NULL){
//		worldPosition  = cam->getModelViewMatrix() * worldPosition;
		worldPosition  = cam->getOrientationQuat() * worldPosition;
		worldPosition += cam->getPosition();
	}
	
	//hack for intro node
	if(introNode){
		if(multiplier == 0){
			worldPosition.y -= 10; //plus 20 just to move everything down a little bit
		}
		else {
			worldPosition.z *= .5; //bring the side nodes closer in Z
		}
	}

#ifdef OCULUS_RIFT
	ofRectangle viewport = CloudsVisualSystem::getOculusRift().getOculusViewport();
	ofVec3f screenPos = CloudsVisualSystem::getOculusRift().worldToScreen(worldPosition, true);
#else
	//kinect version
#endif
	screenPosition = ofVec2f(screenPos.x,screenPos.y);
	cursorDirection = screenPosition - viewport.getCenter();
	cursorDistance  = cursorDirection.length();
	cursorDirection.normalize();
}

void CalibrationNode::updateInteraction(){
	
	if(finished){
		return;
	}

	if(!hover && cursorDistance < activationDistance.min){
		hover = true;
		hoverStartTime = ofGetElapsedTimef();
		if(clickSound!=NULL){
			clickSound->setPosition(0);
			clickSound->play();
		}
	}
	else if(hover){
		if(cursorDistance > activationDistance.max){
			hover = false;
			percentComplete = 0.0;
		}
		else if(percentComplete >= 1.0){
			finished = true;
			finishedTime = nodeActivatedTime = ofGetElapsedTimef(); //used for alpha on helper text
			if(selectSound != NULL){
				selectSound->setPosition(0);
				selectSound->play();
			}
		}
		else{
			percentComplete = ofMap(ofGetElapsedTimef(), hoverStartTime, hoverStartTime+holdTime, 0.0, 1.0,true);
		}
	}
}

void CalibrationNode::draw(){
	ofPushMatrix();
	ofPushStyle();
	
	ofFloatColor baseColor;
	if(finished){
		baseColor = tint;
		tint.a = 200*nodeAlphaAttenuate;
	}
	else if(hover){
		baseColor = ofFloatColor(1.0, 100/255.0,100/255.0,200/255.0);
	}
	else{
		baseColor = ofFloatColor(1.0, 200.0/255.0);
	}
	ofSetColor(baseColor);
	
#ifdef OCULUS_RIFT
	ofVec3f up = ofVec3f(0,1,0);
	if(cam != NULL) up = cam->getUpDir();
	CloudsVisualSystem::getOculusRift().multBillboardMatrix(worldPosition,up);
#endif
	
	float afterFinishScalar = 0.0;
	ofNoFill();
	if(percentComplete > 0.0){
		float nodeSize = nodeBaseSize;
		ofColor arcColor = ofGetStyle().color;
		if(finished){
			afterFinishScalar = powf(ofMap(ofGetElapsedTimef(), finishedTime, finishedTime+.2, 0.0, 1.0, true), 2.0f);
			nodeSize = nodeBaseSize + ( afterFinishScalar * 8.0);
			arcColor.a = 255*(1.0-afterFinishScalar);
		}
		ofPath arc;
		arc.setFilled(false);
		arc.setStrokeWidth(3);
		arc.setStrokeColor(arcColor);
		arc.arc(ofVec3f(0,0,0), nodeSize, nodeSize, 90, 360*percentComplete + 90, true);
		arc.draw();
	}
	
	ofCircle(0,0,0, nodeBaseSize * (1.0-afterFinishScalar) );
	ofFill();
	baseColor.a *= .1;
	ofSetColor(baseColor);
	ofCircle(0,0,0, nodeBaseSize * (1.0-afterFinishScalar) );
	
	ofPopStyle();
	ofPopMatrix();
}