//
//  CloudsHUDLabel.cpp
//  CloudsHUDDesigner
//
//  Created by Charlie Whitney on 12/30/13.
//
//

#include "CloudsHUDLabel.h"
#include "CloudsLocalization.h"
#include "CloudsGlobal.h"

CloudsHUDLabel::CloudsHUDLabel(){
    text = "";
    
    layout = NULL;
    font   = NULL;
    tab = false;
    tabSelected = false;
    
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
    
    if(!bIsVisible && !bIsAnimatingOut){
        return;
    }
    
    ofColor fillColor;
    if(bIsClicked || (tab && tabSelected) ){
        fillColor = CloudsColorTabFillSelectd;
        hoverAlpha = 1.0;
    }
    else if(bIsPressed){
        fillColor = CloudsColorTabFillActive;
        hoverAlpha = 1.0;
    }
    else if(bIsHovered){
        fillColor = CloudsColorTabFillHover;
        hoverAlpha = ofMap(ofGetElapsedTimef() - hoverChangedTime, 0, .5, 0.0, 1.0, true);
    }
    else{
        fillColor = CloudsColorTabFillStatic;
        hoverAlpha = ofMap(ofGetElapsedTimef() - hoverChangedTime, 0, .5, 1.0, 0.0, true);
    }
    
    fillColor.a *= hoverAlpha;
    
    ofColor textColor = ofColor::white;
    
    ofPushStyle();
    ofFill();
    ofSetColor(fillColor);
    ofRect(baseInteractiveBounds);
    ofPopStyle();
    
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
        pct = textAlpha = ofMap( ofGetElapsedTimef(), beginTime, beginTime+fadeOutSpeed, 1., 0., true );

        if(textAlpha <= 0.0){
            if(clearTextOnAnimateOut){
                text = "";
                clearTextOnAnimateOut = false;
            }
            bIsAnimatingOut = false;
        }
    }
    else{
        textAlpha = 1.0;
        pct = 1.0;
    }

    
    string t = utf8_substr(text, playhead);
    if(caps){
        t = ofToUpper(t);
    }
    ofPushStyle();
    textColor.a *= textAlpha;
    ofSetColor(textColor);
    
    if(type == "LAYOUT" && layout != NULL ){
        layout->drawString( t, bounds.x, bounds.y + layout->getStringBoundingBox("W", 0, 0).height );
    }
    else if (type == "FONT" && font != NULL){
        font->drawString( t, bounds.x, bounds.y + font->getStringBoundingBox("W", 0, 0).height );
    }
    
    ofPopStyle();

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
    
    bool releasedInBounds = scaledInteractiveBounds.inside(mouse.x,mouse.y);
    if(bIsPressed && releasedInBounds){
        bIsClicked = true;
    }
    else if(!tab){
        bIsClicked = false;
    }
    
    if(!releasedInBounds){
        if(bIsHovered){
            hoverChangedTime = ofGetElapsedTimef();
        }
        bIsHovered = false;
        bIsPressed = false;
    }

}

string CloudsHUDLabel::getText(){
	return text;
}

void CloudsHUDLabel::animateIn(bool force) {
    if (!force && bIsVisible) return;
    //if we animate on again when its animating off
    if(bIsAnimatingOut && clearTextOnAnimateOut){
        text = "";
        clearTextOnAnimateOut = false;
    }
    bIsVisible = true;
    
    beginTime = ofGetElapsedTimef();
    bIsAnimatingIn = true;
    bIsAnimatingOut = false;
    textAlpha = 255;
}

void CloudsHUDLabel::animateOut(bool clear) {
    if (!bIsVisible){
        if(clear){
            text = "";
        }
        return;
    }
    
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

void CloudsHUDLabel::deselect(){
    bIsPressed = false;
    bIsClicked = false;
}
