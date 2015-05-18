//
//  CloudsHUDLayer.cpp
//  CloudsHUDDesigner
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsHUDLayer.h"
#include "ofxXmlSettings.h"
#include "CloudsGlobal.h"

ofShader CloudsHUDLayer::lineShader;

CloudsHUDLayer::CloudsHUDLayer(){
	
    fadeStartTime = 0;
    hoverStartTime = 0;

	duration = 0;
	//delayTime = 0;
	//percentComplete = ofVec2f(0,0);
   	animateOnPercentComplete = 0;
    hoverOnPercentComplete = 0;
 
	animatingFade = false;
    animatingHover = false;
    bIsOpen = false;
    bIsHovering = false;
    bForceHover = false;
    
	//maxUpdateInterval = 1./60.;
	//lastUpdateTime = 0;
	//startPercent.x = .8;
	//startPercent.y = .0;
}

void CloudsHUDLayer::load(string svgFilePath){

	svg.load(svgFilePath);
	
	//find text boxes
	for(int i = 0; i < svg.getMeshes().size(); i++){
		if( ofToLower(svg.getMeshes()[i].id).find("textbox") != string::npos ) {
			textBoxIds.push_back(svg.getMeshes()[i].id);
//			cout << svg.sourceFileName <<  " has text box: " << svg.getMeshes()[i].id << " with bounds " << svg.getMeshes()[i].bounds.x << " " << svg.getMeshes()[i].bounds.y << " " << svg.getMeshes()[i].bounds.width << " " << svg.getMeshes()[i].bounds.height << endl;
		}
	}
}


//void CloudsHUDLayer::parseDirectory(string svgDirectoryPath){
////    cout << "Loading SVG directory :: " << svgDirectoryPath << endl;
//    
//    svg.loadDirectory(svgDirectoryPath);
//    
//    //find text boxes
//	for(int i = 0; i < svg.getMeshes().size(); i++){
//		if( ofToLower(svg.getMeshes()[i].id).find("textbox") != string::npos ) {
//			textBoxIds.push_back(svg.getMeshes()[i].id);
//		}
//	}
//}

void CloudsHUDLayer::start(bool animate){
    if( bIsOpen ){
        return;
    }
    
    bIsOpen = true;
    
    if (animate) {
        fadeStartTime = ofGetElapsedTimef();
        animatingFade = true;
    }
}

void CloudsHUDLayer::close(bool animate) {
    if( !bIsOpen ){
        return;
    }
    bIsOpen = false;
    
    hoverOff();
    
    if (animate) {
        fadeStartTime = ofGetElapsedTimef();
        animatingFade = true;
    }
}

void CloudsHUDLayer::hoverOn(){
    if( bIsHovering ){
        return;
    }
    
    bIsHovering = true;
    
    hoverStartTime = ofGetElapsedTimef();
    animatingHover = true;
    
}

void CloudsHUDLayer::hoverOff(){
    if( !bIsHovering ){
        return;
    }
    
    bIsHovering = false;
    
    hoverStartTime = ofGetElapsedTimef();
    animatingHover = true;
}

void CloudsHUDLayer::update(){
	float time = ofGetElapsedTimef();
	if(animatingFade){
		animateOnPercentComplete = ofxTween::map(time,
										   fadeStartTime,
										   fadeStartTime+duration,
										   0.,1.0,true,ofxEasingExpo(),
										   ofxTween::easeOut );
    
        animatingFade = animateOnPercentComplete != 1.0;
        if( !bIsOpen ){
            animateOnPercentComplete = 1.0 - animateOnPercentComplete;
        }
	}
    
    if(bForceHover){
        hoverOnPercentComplete = animateOnPercentComplete;
    }
    else if(animatingHover){
        hoverOnPercentComplete = ofxTween::map(time,
                                               hoverStartTime,
                                               hoverStartTime+duration,
                                               0.,1.0,true,ofxEasingExpo(),
                                               ofxTween::easeOut );
        
        animatingHover = hoverOnPercentComplete != 1.0;
        if(!bIsHovering){
            hoverOnPercentComplete = 1.0 - hoverOnPercentComplete;
        }
    }
}

void CloudsHUDLayer::draw(){
	
//	ofSetColor(255, 0, 0, 10);
	if(!lineShader.isLoaded()){
		lineShader.load(GetCloudsDataPath() + "shaders/secondaryDisplay");
	}
    
	ofPushStyle();
	lineShader.begin();
    lineShader.setUniform1i("lineWork", 0);
	lineShader.setUniform1f("alphaAmt", animateOnPercentComplete);

    svg.drawFills();
    
    lineShader.setUniform1i("lineWork", 1);
	lineShader.setUniform1f("alphaAmt", hoverOnPercentComplete);

	svg.drawStrokes();

	lineShader.end();

//    ofNoFill();
//    ofRect(svg.getBounds());
    
	ofPopStyle();
    

}

bool CloudsHUDLayer::isOpen(){
    return bIsOpen;
}

bool CloudsHUDLayer::isHovering(){
    return bIsHovering;
}