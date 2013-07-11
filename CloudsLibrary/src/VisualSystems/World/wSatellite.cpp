//
//  wSatellite.cpp
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/27/13.
//
//

#include "wSatellite.h"

wSatellite::wSatellite(){
    lat = ofRandom(-50,50);
    lon = ofRandom(-180,180);
    freq = ofNoise(lat*0.01,lon*0.01);
}

void wSatellite::place( float _alt, ofVec3f _orbit ){
    altitud.set(0,0,_alt);
    orbit = _orbit;//.normalize();
    freq = ofNoise(lat*0.01,lon*0.01);
}

void wSatellite::update(){
    
    lat += orbit.x;
    lon += orbit.y;
    
    if (lat < -180.0f)
        lat += 360.0f;
    
    if (lat > 180.0f)
        lat -= 360.0f;
    
    if (lon < -180.0f)
        lon += 360.0f;
    
    if (lon > 180.0f)
        lon -= 360.0f;
    
    ofQuaternion latRot;
    latRot.makeRotate( lat, 1, 0, 0);
    
    ofQuaternion longRot;
    longRot.makeRotate( lon, 0, 1, 0);
    
    set(latRot * longRot * altitud);
}

void wSatellite::draw(){
    ofPoint vectorToCenter = ofPoint(0,0,0)-*this;
    vectorToCenter.normalize();
    ofPoint objectLookAt = ofVec3f(0,0,1);
    float theta = objectLookAt.angle(vectorToCenter);
    ofPoint angleToCenter = vectorToCenter.crossed(objectLookAt);
    angleToCenter.normalize();
    
    ofPushMatrix();
    ofTranslate(*this);
    glRotatef(-theta, angleToCenter.x, angleToCenter.y, angleToCenter.z);
    float blinking = abs(sin(ofGetElapsedTimef()*freq));
    ofSetColor(200,blinking*200,blinking*200,200);
    ofFill();
    ofDrawBox(ofPoint(0,0,0), 1);
    
    ofPopMatrix();
}