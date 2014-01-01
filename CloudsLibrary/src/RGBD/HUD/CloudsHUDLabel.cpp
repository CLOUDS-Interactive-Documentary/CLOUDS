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
    }
    
    if( layout ){
        ofPushStyle();{
            ofSetColor(255, 255, 255, textAlpha);
            layout->drawString( ofToUpper(text.substr(0, playhead )), bounds.x, bounds.y + layout->getStringBoundingBox("W", 0, 0).height );
        }ofPopStyle();
    }
}

void CloudsHUDLabel::setText( string newText ){
    text = newText;
    playhead = 0;
    
    if( text != "" ){
        animationSpeed = baseAnimationSpeed * (text.length() / 300.);
        animationSpeed = ofClamp(animationSpeed, animationClamp.min, animationClamp.max);
        animateIn();
    }
}

void CloudsHUDLabel::animateIn() {
    beginTime = ofGetElapsedTimef();
    bIsAnimatingIn = true;
    bIsAnimatingOut = false;
    textAlpha = 255;
}

void CloudsHUDLabel::animateOut() {
    beginTime = ofGetElapsedTimef();
    bIsAnimatingIn = false;
    bIsAnimatingOut = true;
}
