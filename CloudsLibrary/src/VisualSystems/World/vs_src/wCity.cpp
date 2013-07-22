//
//  wCity.cpp
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/24/13.
//
//

#include "wCity.h"

wCity::wCity(){
    color.set(255,0,0);
    rippleScale = ofRandom(50,500);
    noisePeaks = NULL;
    noiseThreshold = NULL;
    bRipple = false;
    size = 1.0;
}

ofQuaternion wCity::getQuaternion(){
    return quaternion;
}

void wCity::place(float _lat, float _long){
    ofVec3f displacementFromCenter = ofVec3f(0,0,-300);
    
    ofQuaternion latRot;
    latRot.makeRotate( _lat, 1, 0, 0);
    
    ofQuaternion longRot;
    longRot.makeRotate( _long, 0, 1, 0);
    
    quaternion = latRot * longRot;
    set(latRot * longRot * displacementFromCenter);
    
    color.set(255,0,0);
    color.setHue(20+ofNoise(x*0.1,y*0.1,z*0.4)*30);
    color.setBrightness(100+ofNoise(x*0.1,y*0.1,z*0.1)*155);
    color.a = 200;
    
    freq = ofNoise(_lat*0.1,_long*0.1);
}

void wCity::update(){
    ofPoint pos = *this*ofGetElapsedTimef()*0.001;
    
    if ( noisePeaks != NULL){
        if ( *noisePeaks > 0.0){
        noise = powf( *noisePeaks ,ofNoise( sin(pos.x),pos.y,pos.z*0.1));
        nNoise = ofMap(noise,0.001,*noisePeaks,0.0,1.0,true);
        color.setHue( 20+nNoise*30 );
        size = nNoise*5.0;
        }
    } else {
        float blink = abs(sin(ofGetElapsedTimef()*freq));
        color.setHue( 20+ blink * 30 );
        size =  blink * 2.0;
    }
    
    if (noiseThreshold != NULL && noisePeaks != NULL){
        if (nNoise > *noiseThreshold ){
            
            if (!bRipple && *noisePeaks > 0.0)
                bRipple = true;
        }
    }
    
    if (bRipple){
        if (ripplePct<1.0){
            ripplePct += 0.01;
        } else {
            ripplePct = 0.0;
            bRipple = false;
        }
    }
}

void wCity::draw(float _alpha){
    
    ofPushStyle();
    
    if (noisePeaks != NULL){
        if ( *noisePeaks > 0.1 ){
            ofSetColor(color,_alpha*255);
            
            ofPoint head = *this - ofPoint(0,0,0);
            head.normalize();
            head *= noise;
            ofLine(*this, *this+head);
        }
        
        ofSetColor(color,_alpha*255);
    }
    
    ofSetColor(color,_alpha*255);
    glPointSize(size);
    glBegin(GL_POINTS);
    glVertex3f(x,y,z);
    glEnd();
    
    if (bRipple){
        drawRipple(ripplePct*0.61);
        drawRipple(ripplePct);
    }
    
    ofPopStyle();
}

void wCity::drawRipple( float _pct ){
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