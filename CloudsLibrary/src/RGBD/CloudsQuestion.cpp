//
//  CloudsQuestion.cpp
//  CLOUDS
//
//  Created by James George on 6/20/13.
//
//

#include "CloudsQuestion.h"

CloudsQuestion::CloudsQuestion(){
	hovering = false;
	isSetup = false;
	radius = 10;
	cam = NULL;
	introQuestion = false;
}

void CloudsQuestion::setup(){
	if(!isSetup){
		ofRegisterMouseEvents(this);
		isSetup = true;
		
	}
}

void CloudsQuestion::update(){
	
	ofVec3f screenPoint = cam->worldToScreen(position);
	currentScreenPoint = ofVec2f(screenPoint.x,screenPoint.y);
	
	ofVec3f screenPointTop = cam->worldToScreen(position + ofVec3f(0,radius,0));
	screenRadius = abs( screenPointTop.y - currentScreenPoint.y );
	
	if(introQuestion){
		
	}

}

void CloudsQuestion::draw(){
	ofPushStyle();

	if(hovering){
		ofFill();
	}
	else{
		ofNoFill();
	}
	
	ofPushMatrix();
	ofTranslate(position);
	ofRotate(ofGetFrameNum(), 0, 1, 0);
	
	ofCircle(0,0,radius);

	ofPopMatrix();

	ofPopStyle();
}

void CloudsQuestion::drawOverlay(){
	if(hovering){
		font->drawString( clip.getQuestionForTopic(topic), currentScreenPoint.x, currentScreenPoint.y);
	}
}

void CloudsQuestion::mousePressed(ofMouseEventArgs& args){
		
}

void CloudsQuestion::mouseMoved(ofMouseEventArgs& args){
	if(cam == NULL){
		return;
	}
	

	hovering = currentScreenPoint.distance( ofVec2f(args.x,args.y) ) < screenRadius;
}

void CloudsQuestion::mouseReleased(ofMouseEventArgs& args){
	
}

void CloudsQuestion::mouseDragged(ofMouseEventArgs& args){
	
}
