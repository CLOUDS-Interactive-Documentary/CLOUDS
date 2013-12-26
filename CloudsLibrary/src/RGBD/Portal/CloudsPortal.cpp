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

CloudsPortal::CloudsPortal(){
	
	ringStartRadius = 100;
	ringSegments = 360/30.;
	ringRadiusStep = 10;
	ringThickness = 10;
	ringThicknessMultiplier = 1.2;
	ringDensity = 10; //geo per segment
	
	selected = false;
	hovering = false;
	
	minSelectDistance = 20.; //screenspace distance from node to hover
	maxHoverTime = 30.; //how long to hover before select
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
	
	portalShader.load(GetCloudsVisualSystemDataPath("RGBD") + "shaders/portal");
}

void CloudsPortal::update(){
	if(cam != NULL){
		screenPosition = cam->worldToScreen(hoverPosition);
		if( screenPosition.distance( ofVec2f(GetCloudsInputX(),GetCloudsInputY())) < minSelectDistance ){
			if(!hovering){
				hoverStartTime = ofGetElapsedTimef();
			}
			hovering = true;
			hoverPercentComplete = ofClamp((ofGetElapsedTimef() - hoverStartTime) / maxHoverTime, 0,1.0);
		}
		else{
			hovering = false;
			hoverPercentComplete = 0;
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
	ofEnableAlphaBlending();
	portalShader.begin();
	portalShader.setUniform1f("rotate", ofGetElapsedTimef()*2.);
	
	portalGeo.draw();
	portalShader.end();
	ofPopMatrix();
	ofPopStyle();
}

