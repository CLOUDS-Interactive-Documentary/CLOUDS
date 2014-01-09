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
	
	selected = false;
	hovering = false;
	bLookAtCamera = false;
	
	hoverPercentComplete = 0.;
	
	minSelectDistance = 20.; //screenspace distance from node to hover
	maxHoverTime = 10.; //how long to hover before select
    
    charsPerSecond = 45;
}

CloudsPortal::~CloudsPortal(){

}

void CloudsPortal::setup(){
	for(int i = 0; i < 3; i++){
		CloudsPortalRing r;
		r.setup(this, portalGeo, i);
		rings.push_back(r);
	}
	portalGeo.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	
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
                selectedTime = ofGetLastFrameTime();
            }
        }
        else if(selected){
            selected = false;
        }
	}

	for(int i = 0; i < rings.size(); i++){
		rings[i].hoverPercentComplete = hoverPercentComplete;
		rings[i].update();
	}

}

void CloudsPortal::toggleFakeSelection(){
	selected = !selected;
	selectedTime = ofGetLastFrameTime();
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
//	shader.begin();
	shader.setUniform1f("rotate", ofGetElapsedTimef()*2.);
	shader.setUniform1f("hoverPercent", hoverPercentComplete);
	portalGeo.draw();
//	shader.end();
	ofPopMatrix();
	ofPopStyle();
}

void CloudsPortal::drawOverlay(bool anchorToScreen){
	if(hovering){
		
		glDisable(GL_DEPTH_TEST);
        
		float width = font->stringWidth(question);
		//ofVec2f screenPosition(ofGetWidth()/2 - width/2, ofGetHeight() * .66);
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
		
		//DRAW BACKBOX
        //		ofPushStyle();
        //		ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
        //		ofSetColor(255,20);
        //		ofRect(screenPosition.x, screenPosition.y-25, width+40, 50);
        //		ofPopStyle();
		
		float secondsToWriteQuestion = question.size() / charsPerSecond;
		int charactersToType = ofMap(ofGetElapsedTimef() - hoverStartTime, 0, secondsToWriteQuestion, 0, question.size(), true);
		string substring = question.substr(0, charactersToType);
        //		if(font != NULL){
        //			ofPushStyle();
        //			ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
        //			ofSetColor(50);
        //			font->drawString(substring, screenPosition.x+12, screenPosition.y+2);
        //			ofSetColor(255);
        //			ofEnableBlendMode(OF_BLENDMODE_ADD);
        font->drawString(substring, textPosition.x+10, textPosition.y);
        font->drawString(substring, textPosition.x+10, textPosition.y);
        //			ofPopStyle();
        //		}
        //		else{
        //			ofDrawBitmapString(substring, screenPosition);
        //		}
		
        /* EZ: Disabled all that for now
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
         */
		
		
		ofPopStyle();
		glEnable(GL_DEPTH_TEST);
	}
    
	ofPushStyle();
	//debug
    //	ofSetColor(255, 0, 0, 100);
    //	ofCircle(currentScreenPoint, screenRadius);
	ofPopStyle();
}

