//
//  CloudsHUDController.cpp
//  CloudsPointcloudViewer
//
//  Created by James George on 12/14/13.
//
//



#include "CloudsHUDController.h"
#include "CloudsGlobal.h"

CloudsHUDController::CloudsHUDController(){
	hudGui = NULL;
}

void CloudsHUDController::setup(){
	testImage.loadImage( GetCloudsDataPath() + "HUD/overlayTest.png" );
	
	testLayer.svg.load(GetCloudsDataPath() + "HUD/CLOUDS_UI_v34_SVG-09.svg" );
}

void CloudsHUDController::draw(){
	
	ofPushStyle();
	ofPushMatrix();
	ofEnableAlphaBlending();
//	
//	ofScale(ofGetMouseX()*.01,
//			ofGetMouseY()*.01);
	
	testLayer.svg.draw();
	
	ofPopMatrix();
	ofPopStyle();
}

void CloudsHUDController::saveGuiSettings(){
	
}

void CloudsHUDController::toggleGuis(){
	
}

