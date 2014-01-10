/*
 *  CloudsPortal.cpp
 *  Empty
 *
 *  Created by Jim on 12/21/13.
 *  Copyright 2013 University of Washington. All rights reserved.
 *
 */

#include "CloudsPortal.h"
#include "CloudsGlobal.h"
#include "CloudsInput.h"

ofShader CloudsPortal::shader = ofShader();

void CloudsPortal::reloadShader(){
	CloudsPortal::shader.load(GetCloudsVisualSystemDataPath("RGBD") + "shaders/portal");
}

CloudsPortal::CloudsPortal(){

	scale = 1.0f;
	
    ringStartRadius = 100;
	ringSegments = 360/30.;
	ringRadiusStep = 10;
	ringThickness = 10;
	ringThicknessMultiplier = 1.2;
	ringDensity = 10; //geo per segment
	ringCount = 20;
	
	selectedPercentComplete = 0.0;
	selectAnimationDuration = 3.0;
	
	selected = false;
	hovering = false;
	bLookAtCamera = false;
	
	hoverPercentComplete = 0.;
	
	minSelectDistance = 20.; //screenspace distance from node to hover
	maxHoverTime = 5.0; //how long to hover before select
    
    charsPerSecond = 45;
}

CloudsPortal::~CloudsPortal(){

}

void CloudsPortal::setup(){
	for(int i = 0; i < ringCount; i++){
		CloudsPortalRing r;
		r.setup(this, portalGeo, i, true);
		rings.push_back(r);
	}
	
	innerTimerRing.setup(this, timerGeo, 0, false);
	outerTimerRing.setup(this, timerGeo, 1, false);
	for(int i = 0; i < portalGeo.getNumVertices(); i++){
		portalGeo.addTexCoord(ofVec2f(0.0,0.0)); //NOT A TIMER
	}
	for(int i = 0; i < timerGeo.getNumVertices(); i++){
		timerGeo.addTexCoord(ofVec2f(1.0,0.0)); //IS A TIMER
	}
	
	portalGeo.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	timerGeo.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	
	reloadShader();
}

void CloudsPortal::startHovering(){
    if(!hovering){
        hoverStartTime = ofGetElapsedTimef();
    }
    hovering = true;
    hoverPercentComplete = ofClamp((ofGetElapsedTimef() - hoverStartTime) / maxHoverTime, 0,1.0);
}

void CloudsPortal::stopHovering(){
	hovering = false;
    hoverPercentComplete = 0;
}

bool CloudsPortal::isSelected(){
	return hovering && selected;
}

void CloudsPortal::update(){
    if(cam != NULL){
		screenPosition = cam->worldToScreen(hoverPosition);
        if(hovering){
            hoverPercentComplete = ofClamp((ofGetElapsedTimef() - hoverStartTime) / maxHoverTime, 0,1.0);
            
            if(!selected && hoverPercentComplete == 1.0){
                selected = true;
                selectedTime = ofGetElapsedTimef();
            }

        }
        else if(selected){
            selected = false;
        }
	}
	
	if(selected){
		selectedPercentComplete = ofClamp((ofGetElapsedTimef() - selectedTime) / selectAnimationDuration, 0,1.0);
	}

//	cout << "selectedPercent " << selectedPercentComplete << endl;
//	for(int i = 0; i < rings.size(); i++){
//		rings[i].hoverPercentComplete = hoverPercentComplete;
//		rings[i].update();
//	}

}

void CloudsPortal::toggleFakeSelection(){
//	selected = !selected;
//	selectedTime = ofGetLastFrameTime();
}

void CloudsPortal::draw(){
	ofPushStyle();
	ofPushMatrix();
    ofTranslate(hoverPosition);
	if(bLookAtCamera && cam != NULL){
		ofNode n;
		n.setPosition(hoverPosition);
		n.lookAt(cam->getPosition());
		
		ofQuaternion rot = n.getOrientationQuat();
		float angle;
		ofVec3f axis;
		rot.getRotate(angle, axis);
		ofRotate(angle, axis.x, axis.y, axis.z);
	}
	
    ofScale(scale, scale, scale);
	
	ofEnableAlphaBlending();
	shader.setUniform1i("selected", selected ? 1 : 0);
	shader.setUniform1f("maxRingPosition", ringCount);
	shader.setUniform1f("rotate", ofGetElapsedTimef()*2.);
	shader.setUniform1f("hoverPercent", hoverPercentComplete);
	shader.setUniform1f("selectionAnimationPercent", selectedPercentComplete);
	shader.setUniform1i("drawingTimer", 0);
	portalGeo.draw();
	shader.setUniform1i("drawingTimer", 1);
	timerGeo.draw();
				
	ofPopMatrix();
	ofPopStyle();
}

void CloudsPortal::drawOverlay(bool anchorToScreen){

	//TODO MOVE TO HUDD
	if(hovering){
		
        ofPushStyle();
		
		float width = font->stringWidth(question);
		ofVec2f textPosition;
		if(anchorToScreen){
			textPosition = ofVec2f(20,20);
		}
		else{
			if( screenPosition.x > ofGetWidth()/2){
				textPosition = screenPosition - ofVec2f(width + 40, -25);
			}
			else{
				textPosition = screenPosition;
			}
		}
		float secondsToWriteQuestion = question.size() / charsPerSecond;
		int charactersToType = ofMap(ofGetElapsedTimef() - hoverStartTime, 0, secondsToWriteQuestion, 0, question.size(), true);
		
		string substring = question.substr(0, charactersToType);
        font->drawString(substring, textPosition.x+10, textPosition.y);
        font->drawString(substring, textPosition.x+10, textPosition.y);
		
		ofPopStyle();

	}
    
}

