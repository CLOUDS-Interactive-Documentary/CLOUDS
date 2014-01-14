//
//  CloudsHUDLayer.cpp
//  CloudsHUDDesigner
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsHUDLayer.h"
#include "ofxXmlSettings.h"

CloudsHUDLayer::CloudsHUDLayer(){
	startTime = 0;
	duration = 0;
	delayTime = 0;
	percentComplete = ofVec2f(0,0);
	animating = false;
    bIsOpen = false;
	maxUpdateInterval = 1./60.;
	lastUpdateTime = 0;
	startPercent.x = .8;
	startPercent.y = .0;
}

void CloudsHUDLayer::parse(string svgFilePath){

	svg.load(svgFilePath);
	
	//find text boxes
	for(int i = 0; i < svg.getMeshes().size(); i++){
		if( ofToLower(svg.getMeshes()[i].id).find("textbox") != string::npos ) {
			textBoxIds.push_back(svg.getMeshes()[i].id);
            
//			cout << svg.sourceFileName <<  " has text box: " << svg.getMeshes()[i].id << " with bounds " << svg.getMeshes()[i].bounds.x << " " << svg.getMeshes()[i].bounds.y << " " << svg.getMeshes()[i].bounds.width << " " << svg.getMeshes()[i].bounds.height << endl;
		}
	}
}

void CloudsHUDLayer::parseDirectory(string svgDirectoryPath){
//    cout << "Loading SVG directory :: " << svgDirectoryPath << endl;
    
    svg.loadDirectory(svgDirectoryPath);
    
    //find text boxes
	for(int i = 0; i < svg.getMeshes().size(); i++){
		if( ofToLower(svg.getMeshes()[i].id).find("textbox") != string::npos ) {
			textBoxIds.push_back(svg.getMeshes()[i].id);
		}
	}
}

void CloudsHUDLayer::start(){
    if( bIsOpen ){
        return;
    }
    bIsOpen = true;
    
	startTime = ofGetElapsedTimef();
	animating = true;
}

void CloudsHUDLayer::close() {
    if( !bIsOpen ){
        return;
    }
    bIsOpen = false;
    
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
        
        if( !bIsOpen ){
            percentComplete.x = 1.0 - percentComplete.x;
            percentComplete.y = 1.0 - percentComplete.y;
        }

		drawRect = ofRectangle(startPoint.x,startPoint.y,0,0);
		ofVec2f p;
		p.x = ofLerp(startPoint.x, endPoint.x, percentComplete.x);
		p.y = ofLerp(startPoint.y, endPoint.y, percentComplete.y);
		
		drawRect.growToInclude(p);
        if( bIsOpen ){
            animating = percentComplete.x != 1.0 || percentComplete.y != 1.0;
        }else{
            animating = percentComplete.x != 0.0 || percentComplete.y != 0.0;
        }
		lastUpdateTime = time;
	}
}

void CloudsHUDLayer::draw(){
	
	ofPushStyle();
	ofSetColor(255, 0, 0, 10);
	
	glEnable(GL_SCISSOR_TEST);
	//invert and flip
	float screenHeight = ofGetHeight();
	glScissor(drawRect.x, screenHeight - drawRect.y - drawRect.height,
			  drawRect.width*2., drawRect.height);
	svg.draw();
    glDisable(GL_SCISSOR_TEST);

	ofPopStyle();

}

bool CloudsHUDLayer::isOpen(){
    return bIsOpen;
}
