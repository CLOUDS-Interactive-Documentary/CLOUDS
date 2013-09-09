//
//  CloudsQuestion.cpp
//  CLOUDS
//
//  Created by James George on 6/20/13.
//
//

#include "CloudsQuestion.h"
#include "CloudsGlobal.h"
#include "ofxTween.h"

ofShader CloudsQuestion::shader = ofShader();

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

	secondsToConsiderSelected = 3;
	font = NULL;
	
	expandPercent = .1;
	selectPercent = 0;

}

void CloudsQuestion::setup(){
	if(!isSetup){
		ofRegisterMouseEvents(this);
		isSetup = true;
		
		if(clip.getAllTopicsWithQuestion().size() > 0){
			topic = clip.getAllTopicsWithQuestion()[0];
			question = ofToUpper(clip.getQuestionForTopic(topic));
		}
		else{
			ofLogError("CloudsQuestion::setup") << "no topic associated with quesiton clip " << clip.getLinkName() << endl;
		}
		

		float clockInnerRadius = .9;
		float clockThickness = .1;
		float clockOuterRadius = clockInnerRadius + clockThickness;
		
		for(float i = 0; i <= 1.0; i += 1.0/36.0){
			float angle = i * 360;
			ofVec3f inner(0,1,0);
			inner.rotate(angle, ofVec3f(0,0,1));
			
			if(i < 1.0){
				dottedCircle.addColor(ofFloatColor(1.0, 0.0, 0));
				dottedCircle.addNormal(inner);
				dottedCircle.addVertex(inner * .5);
				
				dottedCircle.addColor(ofFloatColor(1.0, 0.0, 0));
				dottedCircle.addNormal(inner);
				dottedCircle.addVertex(inner * 1.5);
			}
			
			//inner
			progressRing.addColor(ofFloatColor(0, i, 0));
			progressRing.addNormal(inner*clockInnerRadius);
			progressRing.addVertex(inner*.25);
			
			//outer
			progressRing.addColor(ofFloatColor(0, i, 0));
			progressRing.addNormal(inner*clockOuterRadius);
			progressRing.addVertex(inner*.25);
		}
			
		//close the loop
//		progressRing.addColor(ofFloatColor(1, 0, 0));
//		progressRing.addNormal(ofVec3f(0,clockInnerRadius,0));
//		progressRing.addVertex(ofVec3f(0,1,0)*.1);
//		
//		progressRing.addColor(ofFloatColor(1, 0, 0));
//		progressRing.addNormal(ofVec3f(0,1.0,0));
//		progressRing.addVertex(ofVec3f(0,1.0,0));
		
		progressRing.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
		dottedCircle.setMode(OF_PRIMITIVE_LINES);
	}
}

void CloudsQuestion::startShader(){
	CloudsQuestion::shader.begin();
}
void CloudsQuestion::endShader(){
	CloudsQuestion::shader.end();
}

void CloudsQuestion::reloadShader(){
	cout << "RELOADING QUESTION SHADER" << endl;
	CloudsQuestion::shader.load(getDataPath() + "/shaders/question");
}

void CloudsQuestion::update(){
	
	ofVec3f screenPoint = cam->worldToScreen(position);
	currentScreenPoint = ofVec2f(screenPoint.x,screenPoint.y);
	
	ofVec3f screenPointTop = cam->worldToScreen(position + ofVec3f(0,radius+(radius*expandPercent),0));
	screenRadius = abs( screenPointTop.y - currentScreenPoint.y );
}

void CloudsQuestion::draw(){
	ofPushStyle();

	if(hovering){
		ofFill();
	}
	else{
		ofNoFill();
	}

	if(hovering){
		ofxEasingCubic cub;
		expandPercent = ofxTween::map(ofGetElapsedTimef() - hoveringStartTime, 0, secondsToConsiderSelected*.3, .2, 1.0, true, cub, ofxTween::easeOut);
		selectPercent = ofxTween::map(ofGetElapsedTimef() - hoveringStartTime, 0, secondsToConsiderSelected, 0, 1.0, true, cub, ofxTween::easeOut);
	}
	else{
		expandPercent += (.1 - expandPercent)*.2;
		selectPercent += (0 - selectPercent)*.4;
	}
	
	CloudsQuestion::shader.setUniform1f("expandPercent", expandPercent);
	CloudsQuestion::shader.setUniform1f("maxExpand", radius);
	CloudsQuestion::shader.setUniform1f("selectPercent", selectPercent);

	ofPushMatrix();
	ofTranslate(position);
	ofNode n;
	n.setPosition(position);
	n.lookAt(cam->getPosition());
	
	currentRot.slerp(.1, currentRot, n.getOrientationQuat());
	
	ofVec3f axis;
	float angle;
	currentRot.getRotate(angle, axis);
	ofVec3f dial = n.getLookAtDir();
	ofRotate(-ofGetElapsedTimef()*100,dial.x,dial.y,dial.z);
	ofRotate(angle,axis.x,axis.y,axis.z);
	
//	ofScale(radius*expandPercent, radius*expandPercent, radius*expandPercent);
	dottedCircle.draw();
	progressRing.draw();
	
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
		//ofVec2f screenPosition(ofGetWidth()/2 - width/2, ofGetHeight() * .66);
		ofVec2f screenPosition;
		if( currentScreenPoint.x > ofGetWidth()/2){
			screenPosition = currentScreenPoint - ofVec2f(width + 40, -25);
		}
		else{
			screenPosition = currentScreenPoint;
		}

		//DRAW BACKBOX
//		ofPushStyle();
//		ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
//		ofSetColor(255,20);
//		ofRect(screenPosition.x, screenPosition.y-25, width+40, 50);
//		ofPopStyle();
		
		float secondsToWriteQuestion = question.size() / charsPerSecond;
		int charactersToType = ofMap(ofGetElapsedTimef() - hoveringStartTime, 0, secondsToWriteQuestion, 0, question.size(), true);
		string substring = question.substr(0, charactersToType);
//		if(font != NULL){
//			ofPushStyle();
//			ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
//			ofSetColor(50);
//			font->drawString(substring, screenPosition.x+12, screenPosition.y+2);
//			ofSetColor(255);
//			ofEnableBlendMode(OF_BLENDMODE_ADD);
			font->drawString(substring, screenPosition.x+10, screenPosition.y);
			font->drawString(substring, screenPosition.x+10, screenPosition.y);
//			ofPopStyle();
//		}
//		else{
//			ofDrawBitmapString(substring, screenPosition);
//		}
		
		//DRAW PROGRESS BAR
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
		//progress.draw(); //disabling for now
		
		
		ofPopStyle();
		glEnable(GL_DEPTH_TEST);
	}

	ofPushStyle();
	//debug
//	ofSetColor(255, 0, 0, 100);
//	ofCircle(currentScreenPoint, screenRadius);
	ofPopStyle();
}

void CloudsQuestion::mousePressed(ofMouseEventArgs& args){
		
}

void CloudsQuestion::mouseMoved(ofMouseEventArgs& args){
	if(cam == NULL){
		return;
	}
	
	if(!introQuestion){
		bool insideHover = currentScreenPoint.distance( ofVec2f(args.x,args.y) ) < screenRadius;
		if(!hovering && insideHover) {
			startHovering();
		}
		else if(hovering && !insideHover){
			stopHovering();
		}
	}
}

void CloudsQuestion::mouseReleased(ofMouseEventArgs& args){
	
}

void CloudsQuestion::mouseDragged(ofMouseEventArgs& args){
	
}
