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
    rippleScale = ofRandom(50,500);
    noisePeaks = NULL;
    noiseThreshold = NULL;
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
    ofPoint pos = *this*ofGetElapsedTimef()*0.001;
    noise = powf( *noisePeaks ,ofNoise( sin(pos.x),pos.y,pos.z*0.1));
    nNoise = ofMap(noise,0.001,*noisePeaks,0.0,1.0,true);
    
    if (noiseThreshold != NULL && noisePeaks != NULL){
        if (nNoise > *noiseThreshold ){
            
            if (!bRipple && *noisePeaks > 0.0)
                bRipple = true;
        }
    }
    
    if (bRipple){
        if (ripplePct<3.0){
            ripplePct += 0.01;
        } else {
            ripplePct = 0.0;
            bRipple = false;
        }
    }
}

void wPoint::draw(){
    
    if (noisePeaks != NULL){
        ofPoint head = *this - ofPoint(0,0,0);
        head.normalize();
        head *= noise;
        color.setHue( 20+nNoise*30 );
        ofSetColor(color);
        ofLine(*this, *this+head);
    }
    
    if (bRipple){
        drawRipple(ripplePct*0.61);
        drawRipple(ripplePct);
    }
}

void wPoint::drawRipple( float _pct ){
    //  Jen equation
    //
    float totalGlobeRadio = rippleScale;
    float rippleDps = _pct*0.025;
    float rippleHeight = rippleDps*totalGlobeRadio;
    float rippleRadio = sqrt( 2*rippleHeight*totalGlobeRadio-powf(rippleHeight,2) );
    
    ofPoint vectorToCenter = ofPoint(0,0,0)-*this;
    vectorToCenter.normalize();
    ofPoint rippleCenter = *this + vectorToCenter * rippleHeight;
    
    ofPoint objectLookAt = ofVec3f(0,0,1);
    float theta = objectLookAt.angle(vectorToCenter);
    ofPoint rippleAngle = vectorToCenter.crossed(objectLookAt);
    rippleAngle.normalize();
    
    ofPushStyle();
    ofFloatColor rippleColor = color;
    rippleColor.a = (1.0-_pct)*0.2;
    ofSetColor(rippleColor);
    
    //  Ripple
    //
    ofPushMatrix();
    ofTranslate(rippleCenter);
    glRotatef(-theta, rippleAngle.x, rippleAngle.y, rippleAngle.z);
    ofNoFill();
    
    ofCircle(0,0,0, rippleRadio);
    ofPopMatrix();
    
    ofPopStyle();
}