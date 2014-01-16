//
//  CloudsHUDLabel.cpp
//  CloudsHUDDesigner
//
//  Created by Charlie Whitney on 12/30/13.
//
//

#include "CloudsHUDLabel.h"

CloudsHUDLabel::CloudsHUDLabel(){
    text = "";
    
    bIsAnimatingIn = false;
    bIsAnimatingOut = false;
    bIsVisible = false;
    caps = true;
    
    animationClamp.min = 0.3;
    animationClamp.max = 3.0;
    
    fadeOutSpeed = 0.3;
    
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
    
    if( bIsAnimatingIn ){
        pct = ofMap( ofGetElapsedTimef(), beginTime, beginTime+animationSpeed, 0., 1., true );
        playhead = floor(text.length() * pct);
        if( pct >= 1.0 ){
            bIsAnimatingIn = false;
        }
    }
    
    if( bIsAnimatingOut ){
        pct = ofMap( ofGetElapsedTimef(), beginTime, beginTime+fadeOutSpeed, 1., 0., true );
        textAlpha = floor( 255. * pct );
        if(textAlpha <= 0.0){
            text == "";
            bIsAnimatingOut = false;
        }
    }
    
    if(type == "LAYOUT"){
        if( layout ){
            ofPushStyle();{
                ofSetColor(255, 255, 255, textAlpha);
                string t = text.substr(0, playhead );
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
            string t = text.substr(0, playhead );
            if(caps)
                t = ofToUpper(t);
            font->drawString( t, bounds.x, bounds.y + font->getStringBoundingBox("W", 0, 0).height );
        }ofPopStyle();
    }
}

void CloudsHUDLabel::setText(string newText, bool forceOn){
    cout << "CloudsHUDLabel::setText: " << newText;
    text = newText;
    
    if(caps)
        text = ofToUpper(newText);
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
    if(type == "LAYOUT")
        return layout->getStringBoundingBox(text, bounds.x, bounds.y).getRight();
    else if (type == "FONT")
        return font->getStringBoundingBox(text, bounds.x, bounds.y).getRight();
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

void CloudsHUDLabel::animateOut(bool force) {
    if (!force && !bIsVisible) return;
    bIsVisible = false;
    
    beginTime = ofGetElapsedTimef();
    bIsAnimatingIn = false;
    bIsAnimatingOut = true;
}

bool CloudsHUDLabel::isVisible() {
    return bIsVisible;
}
