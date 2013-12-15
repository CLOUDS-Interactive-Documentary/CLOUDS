//
//  CloudsHUDLayer.cpp
//  CloudsHUDDesigner
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsHUDLayer.h"

CloudsHUDLayer::CloudsHUDLayer(string svgFilePath){
	startTime = 0;
	duration = 0;
	delayTime = 0;
	percentComplete = ofVec2f(0,0);
	animating = false;
	svg.load(svgFilePath);
	maxUpdateInterval = 1./60.;
	lastUpdateTime = 0;
	startPercent.x = .8;
	startPercent.y = .0;
	
}

void CloudsHUDLayer::start(){
	startTime = ofGetElapsedTimef();
	animating = true;
}

void CloudsHUDLayer::update(){
	float time = ofGetElapsedTimef();
	if(animating && time - lastUpdateTime > maxUpdateInterval){
		float totalpercent = ofxTween::map(time,
										   delayTime+startTime,
										   delayTime+startTime+duration,
										   0.,1.0,true,ofxEasingExpo(),
										   ofxTween::easeOut );
		
		percentComplete.x = ofMap(totalpercent,startPercent.x,1.,0,1.0,true);
		percentComplete.y = ofMap(totalpercent,startPercent.y,1.,0,1.0,true);

		drawRect = ofRectangle(startPoint.x,startPoint.y,0,0);
		ofVec2f p;
		p.x = ofLerp(startPoint.x, endPoint.x, percentComplete.x);
		p.y = ofLerp(startPoint.y, endPoint.y, percentComplete.y);
		
		drawRect.growToInclude(p);
		animating = percentComplete.x != 1.0 || percentComplete.y != 1.0;
		lastUpdateTime = time;
	}
		
}

void CloudsHUDLayer::draw(){
	
	ofPushStyle();
//	ofSetColor(255, 0, 0, 10);
//	
//	ofRect(drawRect);
	glEnable(GL_SCISSOR_TEST);
	//invert and flip
	float screenHeight = ofGetHeight();
	glScissor(drawRect.x, screenHeight - drawRect.y - drawRect.height,
			  drawRect.width, drawRect.height);
	
	svg.draw();
	glDisable(GL_SCISSOR_TEST);
		
	ofPopStyle();

}
