//
//  CloudsQuestion.cpp
//  CLOUDS
//
//  Created by James George on 6/20/13.
//
//

#include "CloudsQuestion.h"
ofVec3f randomPointOnSphere(){
	
	float theta = ofRandom(TWO_PI);
	float u = ofRandomf();
	float usqrtinv = sqrt(1. - powf(u,2.));
	return ofVec3f(cos(theta) * usqrtinv,
				   sin(theta) * usqrtinv, u);
};

CloudsQuestion::CloudsQuestion(){
	hovering = false;
	isSetup = false;
	radius = 10;
	cam = NULL;
	introQuestion = false;
	charsPerSecond = 20;
	//secondsToWriteQuestion = 2;
	secondsToConsiderSelected = 3;
	font = NULL;
}

void CloudsQuestion::setup(){
	if(!isSetup){
		ofRegisterMouseEvents(this);
		isSetup = true;
		
		question = ofToUpper(question);
//square
//		geometry.addVertex(ofVec3f(-.5,-.5,0)) ;
//		geometry.addVertex(ofVec3f(.5,-.5,0));
//		geometry.addVertex(ofVec3f(.5,.5,0));
//		geometry.addVertex(ofVec3f(-.5,.5,0));

		//diamonds
		geometry.addVertex(ofVec3f(0,-.5,0));
		geometry.addVertex(ofVec3f(.5,0,0));
		geometry.addVertex(ofVec3f(0,.5,0));
		geometry.addVertex(ofVec3f(-.5,0,0));
		
		geometry.addVertex(ofVec3f(0,-.5,0)); //connect
		
		geometry.addVertex(ofVec3f(0,0,.5));
		geometry.addVertex(ofVec3f(0,.5,0));
		geometry.addVertex(ofVec3f(0,0,-.5));
		geometry.addVertex(ofVec3f(0,-.5,0));
		
		geometry.setMode(OF_PRIMITIVE_LINE_STRIP);
		
		for(int i = 0; i < 200; i++){
			sphereGeo.addVertex(randomPointOnSphere()*.5);
		}
		
//		rot1.makeRotate(ofRandom(.1,3), randomPointOnSphere());
//		rot2.makeRotate(ofRandom(.2,4), randomPointOnSphere());
		
		rot1.makeRotate(ofRandom(.1,3), ofVec3f(0,1,0));
		
		//rot2.makeRotate(ofRandom(.2,4), ofVec3f(0,0,1));
		
	}
}

void CloudsQuestion::update(){
	
	ofVec3f screenPoint = cam->worldToScreen(position);
	currentScreenPoint = ofVec2f(screenPoint.x,screenPoint.y);
	
	ofVec3f screenPointTop = cam->worldToScreen(position + ofVec3f(0,radius,0));
	screenRadius = abs( screenPointTop.y - currentScreenPoint.y );
	//currentRot *= (rot1*rot2);
	currentRot = rot1 * currentRot;
}

void CloudsQuestion::draw(){
	ofPushStyle();

	if(hovering){
		ofFill();
	}
	else{
		ofNoFill();
	}

	float expandPercent;
	if(hovering){
		expandPercent = ofMap(ofGetElapsedTimef() - hoveringStartTime, 0, secondsToConsiderSelected, .2, 1.0, true);
	}
	else{
		expandPercent = .2;
	}
	ofPushMatrix();
	ofTranslate(position);
	//ofRotate(ofGetFrameNum()*4, 0, 1, 0);
	ofVec3f axis;
	float angle;
	currentRot.getRotate(angle, axis);
	ofRotate(angle,axis.x,axis.y,axis.z);
	
	ofScale(radius*expandPercent, radius*expandPercent, radius*expandPercent);
	//geometry.draw();
	sphereGeo.drawVertices();
	//ofCircle(0,0,radius);

	ofPopMatrix();

	ofPopStyle();
}

void CloudsQuestion::orientToCenter(){
	ofNode n;
	n.setPosition(position);
	ofVec3f centeredPos(0,0,position.z);
	n.lookAt(centeredPos);
	
	currentRot = n.getOrientationQuat();
}

void CloudsQuestion::startHovering(){
	if(!hovering){
		hovering = true;
		hoveringStartTime = ofGetElapsedTimef();
	}
}

void CloudsQuestion::stopHovering(){
	hovering = false;
}

bool CloudsQuestion::isSelected(){
	return hovering && ofGetElapsedTimef() - hoveringStartTime > secondsToConsiderSelected;
}
void CloudsQuestion::drawOverlay(){
	if(hovering){
		glDisable(GL_DEPTH_TEST);

		
		float width = font->stringWidth(question);
		ofVec2f screenPosition(ofGetWidth()/2 - width/2, ofGetHeight() * .66);
		
		float secondsToWriteQuestion = question.size() / charsPerSecond;
		int charactersToType = ofMap(ofGetElapsedTimef() - hoveringStartTime, 0, secondsToWriteQuestion, 0, question.size(), true);

		string substring = question.substr(0, charactersToType);
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
		ofSetColor(255,20);
		ofRect(screenPosition.x, screenPosition.y-25, width+40, 50);
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofPopStyle();
//		cout << "hovering overlay should draw " << charactersToType << " of (" << question << ")" << endl;
		if(font != NULL){
			font->drawString(substring, screenPosition.x+10, screenPosition.y);
		}
		else{
			ofDrawBitmapString(substring, screenPosition);
		}
		
//		ofVec3f currentScreenPoint3d(currentScreenPoint.x,currentScreenPoint.y,0);
		
		ofPushStyle();
		ofMesh progress;
		progress.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
		
		progress.addVertex(ofVec3f(screenPosition.x+10,screenPosition.y+5,0));
		progress.addVertex(ofVec3f(screenPosition.x+10,screenPosition.y+10,0));
		
		
		float percentToSelection = ofMap(ofGetElapsedTimef() - hoveringStartTime, 0, secondsToConsiderSelected, 0, 1.0, true);
		
		progress.addVertex(ofVec3f(screenPosition.x+10 + width*percentToSelection + 5, screenPosition.y+5,0));
		progress.addVertex(ofVec3f(screenPosition.x+10 + width*percentToSelection, screenPosition.y+10,0));

		progress.addColor(ofFloatColor::white * .7);
		progress.addColor(ofFloatColor::white * .7);
		
		float oscatten = sin(ofGetElapsedTimef()*10)*.5+.5;
		ofFloatColor flash = ofFloatColor::white.getLerped(ofFloatColor::crimson, percentToSelection*oscatten);
		progress.addColor(flash);
		progress.addColor(flash);
		progress.draw();
		
		ofSetLineWidth(2);
		ofLine(screenPosition.x+15 + width, screenPosition.y+6,
			   screenPosition.x+10 + width, screenPosition.y+11);

		ofSetColor(flash, 128);
		ofSetLineWidth(1);
		ofLine(screenPosition.x,screenPosition.y-25, currentScreenPoint.x,currentScreenPoint.y);

		ofPopStyle();
		glEnable(GL_DEPTH_TEST);
	}
}

void CloudsQuestion::mousePressed(ofMouseEventArgs& args){
		
}

void CloudsQuestion::mouseMoved(ofMouseEventArgs& args){
//	if(cam == NULL){
//		return;
//	}
//	hovering = currentScreenPoint.distance( ofVec2f(args.x,args.y) ) < screenRadius;
}

void CloudsQuestion::mouseReleased(ofMouseEventArgs& args){
	
}

void CloudsQuestion::mouseDragged(ofMouseEventArgs& args){
	
}
