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
    rippleColor.set(1.0,0.0,0.0);
    rippleScale = ofRandom(0.01,0.05);
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
    if (bRipple){
        if (ripplePct<1.0){
            ripplePct += 0.01;
        } else {
            ripplePct = 0.0;
            bRipple = false;
        }
    }
    
    rippleDeepnes = ripplePct*rippleScale;
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
        
        //  Jen equation
        //
        float totalGlobeRadio = 300;
        float rippleHeight = rippleDeepnes*totalGlobeRadio;
        float rippleRadio = sqrt( 2*rippleHeight*totalGlobeRadio-powf(rippleHeight,2) );
        
        ofPoint vectorToCenter = ofPoint(0,0,0)-*this;
        vectorToCenter.normalize();
        ofPoint rippleCenter = *this + vectorToCenter * rippleHeight;
    
        ofPoint objectLookAt = ofVec3f(0,0,1);
        float theta = objectLookAt.angle(vectorToCenter);
        ofPoint rippleAngle = vectorToCenter.crossed(objectLookAt);
        rippleAngle.normalize();
        
        ofPushStyle();
        rippleColor.a = (1.0-ripplePct);
        ofSetColor(rippleColor);
       
        //  City dot
        //
        ofPushMatrix();
        ofTranslate(*this);
        glRotatef(-theta, rippleAngle.x, rippleAngle.y, rippleAngle.z);
        ofFill();
        ofCircle(0,0,0, 2);
        ofPopMatrix();
        
        //  Ripple
        //
        ofPushMatrix();
        ofTranslate(rippleCenter);
        glRotatef(-theta, rippleAngle.x, rippleAngle.y, rippleAngle.z);
        ofNoFill();
        ofSetLineWidth(1.5);
        ofCircle(0,0,0, rippleRadio);
        ofPopMatrix();

        ofPopStyle();
        
    }
}