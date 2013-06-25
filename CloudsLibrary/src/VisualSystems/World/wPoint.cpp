//
//  wPoint.cpp
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/24/13.
//
//

#include "wPoint.h"

wPoint::wPoint(){
    color.set(255,0,0);
    
    rippleDeepnes = 0;
    noisePeaks = NULL;
    bRipple = false;
}

void wPoint::place(float _lat, float _long){
    ofVec3f displacementFromCenter = ofVec3f(0,0,-300);
    
    latitud = _lat;
    longitud = _long;
    
    latRot.makeRotate( latitud, 1, 0, 0);
    longRot.makeRotate( longitud, 0, 1, 0);
    
    set(latRot * longRot * displacementFromCenter);
}

void wPoint::update(){
}

void wPoint::draw(){
    
    if (noisePeaks != NULL){
        ofPoint head = *this - ofPoint(0,0,0);
        head.normalize();
        ofPoint pos = *this*ofGetElapsedTimef()*0.001;
        float noise = powf( *noisePeaks,ofNoise( sin(pos.x),pos.y,pos.z*0.1));
        head *= noise;
        color.setHue(ofMap(noise,0.001,*noisePeaks,20,50,true ));
        ofSetColor(color);
        ofLine(*this, *this+head);
    }
    
    if (bRipple){
        

        ofPoint toCenter = ofPoint(0,0,0)-*this;
        toCenter.normalize();
        ofPoint rippleCenter = *this + toCenter*rippleDeepnes*300;
        float   rippleRadio = cos(ofMap(rippleDeepnes,0.0,1.0,PI/2.0,0.0))*300;
        
        ofPushMatrix();
        ofPushStyle();
        ofSetLineWidth(1.5);
        ofSetColor(255, 0, 0);
       
        ofLine(*this,ofPoint(0,0,0));

        ofTranslate(*this);
        toCenter.
//        ofRotate( ofRadToDeg(longitud), 0, 1, 0);
//        ofRotate( ofRadToDeg(latitud), 1, 0, 0);
        
        ofFill();
        ofCircle(0,0,0, 10);
        
        ofTranslate(0, 0, rippleDeepnes*300);
        
        
        ofNoFill();
        ofCircle(0,0,0, rippleRadio);
        
        ofPopStyle();
        ofPopMatrix();
    }
}