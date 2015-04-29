//
//  CloudsHUDLabel.cpp
//  CloudsHUDDesigner
//
//  Created by Charlie Whitney on 12/30/13.
//
//

#include "CloudsHUDLabel.h"
#include "CloudsLocalization.h"

CloudsHUDLabel::CloudsHUDLabel(){
    text = "";
    
    bIsAnimatingIn = false;
    bIsAnimatingOut = false;
    bIsVisible = false;
    bIsClicked = false;
    
    bIsHovered = false;
    bIsPressed = false;
    
    clearTextOnAnimateOut = false;
    caps = true;
    
    animationClamp.min = 0.3;
    animationClamp.max = 3.0;
    fadeOutSpeed = 0.3;
    
    hoverChangedTime = 0.;
    
    beginTime = 0.;
    baseAnimationSpeed = 0.5;   // mess with this one to change animation speed
    animationSpeed = 0.0;
    pct = 0.;
    playhead = 0;
    textAlpha = 255;
}

void CloudsHUDLabel::setup( ofxFTGLSimpleLayout *textLayout, ofRectangle textBounds ){
    layout = textLayout;
    bounds = textBounds;
    type = "LAYOUT";
}

void CloudsHUDLabel::setup( ofxFTGLFont *textFont, ofRectangle textBounds ){
    font = textFont;
    bounds = textBounds;
    type = "FONT";
}

void CloudsHUDLabel::draw(){
    
    if(bIsHovered){
        ofPushStyle();
        
        float hoverAlpha = ofMap(ofGetElapsedTimef() - hoverChangedTime, 0, .5, 0.0, 1.0, true);
        
        //TODO: Change color to design
        ofFill();
        ofSetColor(200,30,0, 255*hoverAlpha*.3);
        ofRect(baseInteractiveBounds);
        ofNoFill();
        ofSetColor(200,30,0, 255*hoverAlpha*.7);
        ofRect(baseInteractiveBounds);
        ofPopStyle();
        
    }
    
	if( bIsAnimatingIn ){
        if(animationSpeed != 0){
            pct = ofMap( ofGetElapsedTimef(), beginTime, beginTime+animationSpeed, 0., 1., true );
        }
        else{
            pct = beginTime;
        }
        if(pct >= 1.0){
			playhead = text.length();
		}
		else{
			playhead = ((int)(floor(text.length() * pct)) / 2) * 2;
		}
		////////////////////////
		//playhead = text.length();
		/////////////////////////
        if( pct >= 1.0 ){
            bIsAnimatingIn = false;
        }
    }
    
    if( bIsAnimatingOut ){
        pct = ofMap( ofGetElapsedTimef(), beginTime, beginTime+fadeOutSpeed, 1., 0., true );
        textAlpha = floor( 255. * pct );
        if(textAlpha <= 0.0){
            if(clearTextOnAnimateOut){
                text = "";
                clearTextOnAnimateOut = false;
            }
            bIsAnimatingOut = false;
        }
    }
    
    
    if(type == "LAYOUT"){
        if( layout ){
            ofPushStyle();{
                ofSetColor(255, 255, 255, textAlpha);
//				string t = text.substr(0, playhead );
				string t = utf8_substr(text, playhead);
                if(caps){
                    t = ofToUpper(t);
                }
                layout->drawString( t, bounds.x, bounds.y + layout->getStringBoundingBox("W", 0, 0).height );
            }ofPopStyle();
        }
    }
    else if (type == "FONT"){
        ofPushStyle();{
            ofSetColor(255, 255, 255, textAlpha);
            //string t = text.substr(0, playhead );
			string t = utf8_substr(text, playhead);
            
			if(caps){
                t = ofToUpper(t);
			}
            font->drawString( t, bounds.x, bounds.y + font->getStringBoundingBox("W", 0, 0).height );
        }ofPopStyle();
    }
    

}

void CloudsHUDLabel::setText(const string& newText, bool forceOn){
    text = newText;
    
    if(caps){
        text = ofToUpper(newText);
    }
    
    playhead = 0;
    
    if( text != "" ){
        animationSpeed = baseAnimationSpeed * (text.length() / 300.);
        animationSpeed = ofClamp(animationSpeed, animationClamp.min, animationClamp.max);
        if (forceOn) {
            animateIn(true);
        }
    }
 
}

int CloudsHUDLabel::getRightEdge(){
    if(type == "LAYOUT"){
        return layout->getStringBoundingBox(text, bounds.x, bounds.y).getRight();
	}
    else if (type == "FONT"){
        return font->getStringBoundingBox(text, bounds.x, bounds.y).getRight();
	}
	return 0;
}

void CloudsHUDLabel::mouseMoved(ofVec2f mouse){
    bool wasHovered = bIsHovered;
	bIsHovered = isVisible() && scaledInteractiveBounds.inside(mouse.x,mouse.y);
    if(wasHovered != bIsHovered){
		hoverChangedTime = ofGetElapsedTimef();
	}

}

void CloudsHUDLabel::mousePressed(ofVec2f mouse){

	bIsPressed = isVisible() && scaledInteractiveBounds.inside(mouse.x,mouse.y);
    
}

void CloudsHUDLabel::mouseReleased(ofVec2f mouse){
    
    bIsClicked = bIsPressed && scaledInteractiveBounds.inside(mouse.x,mouse.y);
    
}

string CloudsHUDLabel::getText(){
	return text;
}

void CloudsHUDLabel::animateIn(bool force) {
    if (!force && bIsVisible) return;
    bIsVisible = true;
    
    beginTime = ofGetElapsedTimef();
    bIsAnimatingIn = true;
    bIsAnimatingOut = false;
    textAlpha = 255;
}

void CloudsHUDLabel::animateOut(bool clear) {
    if (!bIsVisible) return;
    
    clearTextOnAnimateOut = clear;
    bIsVisible = false;
    
    beginTime = ofGetElapsedTimef();
    bIsAnimatingIn = false;
    bIsAnimatingOut = true;
}

void CloudsHUDLabel::instantIn() {
    bIsVisible = true;
    
    pct = 1.0;
    playhead = floor(text.length() * pct);
    textAlpha = 255;
}

void CloudsHUDLabel::instantOut() {

	bIsVisible = false;
    
    pct = 0.0;
    textAlpha = floor( 255. * pct );
//    text = "";
}

bool CloudsHUDLabel::isVisible() {
    return bIsVisible;
}

bool CloudsHUDLabel::isHovered() {
    return bIsHovered;
}

bool CloudsHUDLabel::isClicked(){
    bool ret = bIsClicked;
    bIsClicked = false;
    return ret;
}
