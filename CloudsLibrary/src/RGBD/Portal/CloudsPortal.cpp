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

CloudsPortal::CloudsPortal(){
	
	ringStartRadius = 100;
	ringSegments = 360/30.;
	ringRadiusStep = 10;
	ringThickness = 10;
	ringThicknessMultiplier = 1.2;
	ringDensity = 10; //geo per segment
	selected = false;
}

CloudsPortal::~CloudsPortal(){

}

void CloudsPortal::setup(){
	for(int i = 0; i < 20; i++){
		CloudsPortalRing r;
		r.setup(this, portalGeo, i);
		rings.push_back(r);
	}
	portalGeo.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	
	portalShader.load(GetCloudsVisualSystemDataPath("RGBD") + "shaders/portal");
}

void CloudsPortal::update(){
	for(int i = 0; i < rings.size(); i++){
		rings[i].update();
	}
}

void CloudsPortal::toggleFakeSelection(){
	selected = !selected;
	selectedTime = ofGetLastFrameTime();
}

void CloudsPortal::draw(){
	ofPushStyle();
//	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	ofPushMatrix();
	portalShader.begin();
	portalShader.setUniform1f("rotate", ofGetElapsedTimef()*2.);
	ofTranslate(ofGetWidth()/2,ofGetHeight()/2);
	portalGeo.draw();
	portalShader.end();
	ofPopMatrix();
	ofPopStyle();
}

