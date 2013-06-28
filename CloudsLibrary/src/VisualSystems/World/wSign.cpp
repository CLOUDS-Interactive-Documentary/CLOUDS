//
//  wSign.cpp
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/27/13.
//
//

#include "wSign.h"

wSign::wSign(){
	set(ofRandom(-300.0f,300.0f),ofRandom(-300.0f,300.0f),ofRandom(-300.0f,300.0f));
	vel.set(0,0,0);
	acc.set(0,0,0);
    
    ripplePnt.set(0,0);
    ripplePct = 0.0;
    rippleScl = 0.05;
    
    bDead = false;
    
    target = NULL;
}

float wSign::getAlitude(){
    return distance(ofPoint(0,0,0)) - 300;
}

void wSign::addRepulsionForce(ofPoint posOfForce, float radius, float scale){
    
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= *this - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
        diff.normalize();
        acc += diff * scale * pct;
    }
}

void wSign::addAttractionForce(ofPoint posOfForce, float radius, float scale){
	
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= *this - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		acc -= diff * scale * pct;
    }
}

void wSign::update(float _speed){
	
    if (target != NULL){
        if ( distance(*target) <= 0.1 ){
            ripplePct += 0.01;
            ripplePnt = *this;
            tail.clear();
        } else {
            addAttractionForce(*target, 150, 0.5);
        }
    }
    
    if (ripplePct > 0.0 ){
        if (ripplePct < 1.0)
            ripplePct += 0.01;
        else
            bDead = true;
    } else {
        vel += acc;
        vel *= _speed;//*0.85;
        *this += vel;
        acc *= 0;
    }
    
    tail.push_back(*this);
    while (tail.size() > 50) {
        tail.erase(tail.begin());
    }
}

void wSign::draw(){
    ofPushStyle();
    
//    ofMesh mesh;
//    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
//    for (int i = 0; i < tail.size(); i++){
//        float alpha = ofMap(i+1, 1,tail.size(), 0.0, 0.9);
//        
//        mesh.addColor(ofFloatColor( 1.0, alpha) );
//        mesh.addVertex(tail[i]);
//    }
//    ofFill();
//    ofSetColor( 255 );
//    mesh.draw();
    
    for (int i = 0; i < tail.size(); i+= 5){
        float alpha = ofMap(i+1, 1,tail.size(), 1.0, 0.0);
        drawRipple( tail[i], alpha, 10 );
    }
    
    if (ripplePct > 0.0 ){
        drawRipple(ripplePnt, ripplePct, 100, true);
    }
    
    ofPopStyle();
}

void wSign::drawRipple( ofPoint _pos, float _pct, float _size, bool _dir ){
    //  Jen equation
    //
    float rippleDps = _pct*rippleScl;
    float rippleHeight = rippleDps*_size;
    float rippleRadio = sqrt( 2*rippleHeight*_size-powf(rippleHeight,2) );
    
    ofPoint vectorToCenter = vel;
    if (_dir)
        vectorToCenter = ofPoint(0,0,0)- _pos;
    
    vectorToCenter.normalize();
    ofPoint rippleCenter = _pos + vectorToCenter * rippleHeight;
    
    ofPoint objectLookAt = ofVec3f(0,0,1);
    float theta = objectLookAt.angle(vectorToCenter);
    ofPoint rippleAngle = vectorToCenter.crossed(objectLookAt);
    rippleAngle.normalize();
    
    ofPushStyle();
    ofFloatColor rippleColor;
    rippleColor.set(1.0);
    rippleColor.a = (1.0-_pct)*0.7;
    ofSetColor(rippleColor);
    
    //  Ripple
    //
    ofPushMatrix();
    ofTranslate(rippleCenter);
    glRotatef(-theta, rippleAngle.x, rippleAngle.y, rippleAngle.z);
    ofNoFill();
    
    //    ofSetLineWidth(1.5);
    ofCircle(0,0,0, rippleRadio);
    ofPopMatrix();
    
    ofPopStyle();
}