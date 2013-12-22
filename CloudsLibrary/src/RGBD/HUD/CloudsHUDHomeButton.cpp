//
//  CloudsHUDHomeButton.cpp
//  CloudsHUDDesigner
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsHUDHomeButton.h"
#include "CloudsGlobal.h"

CloudsHUDHomeButton::CloudsHUDHomeButton(){
	maxHoverTime = 10.;
//	sampleImage.loadImage( GetCloudsDataPath() + "HUD/home.png" );
}

void CloudsHUDHomeButton::setup(){
	
}

void CloudsHUDHomeButton::update(){
	homeLocation = ofVec2f(100, ofGetHeight()-100);
}

void CloudsHUDHomeButton::draw(){
//	sampleImage.draw(homeLocation);
	
}
