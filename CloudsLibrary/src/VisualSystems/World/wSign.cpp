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
    
    nFrame = 0;
    bDead = false;
    
    target = NULL;
}

void  wSign::setTarget( ofPoint *_target ){
    target = _target;
    
    vel = *target - *this;
    vel.normalize();
    vel *= 1.0;
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
        if ( distance(*target) <= 10.0 ){
            
            if ( tail.size() > 0 ){
                tail.erase(tail.begin());
            } else {
                bDead = true;
            }
            
        } else {
            
            if ((nFrame)%5 == 0){
                tail.push_back(*this);
            }
            
            while (tail.size() > 10) {
                tail.erase(tail.begin());
            }
        }
    }
    
    addAttractionForce(*target, 150, 0.5);
    
    vel += acc;
    vel *= _speed;
    *this += vel;
    acc *= 0;
    
    nFrame++;
}

void wSign::draw(){
    ofPushStyle();
    
//    ofMesh mesh;
//    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
//    for (int i = tail.size()-1 ; i >= 1; i--){
//        float alpha = ofMap(i, 0,tail.size()-1, 0.5, 0.0);
//        
//        mesh.addColor(ofFloatColor( 0.5, 1.0, 0.3, alpha) );
//        mesh.addVertex(tail[i]);
//    }
//    ofFill();
//    ofSetColor( 255 );
//    mesh.draw();
    
    glBegin(GL_POINTS);
    glVertex3f(x,y,z);
    glEnd();
    
    for (int i = tail.size()-1 ; i >= 1; i--){
        float pct = ofMap(tail.size()-1-i, 0, tail.size(), 0.0, 1.0,true);
        ofPoint vectorToCenter = tail[i-1]-tail[i];
        
        vectorToCenter.normalize();
        ofPoint objectLookAt = ofVec3f(0,0,1);
        float theta = objectLookAt.angle(vectorToCenter);
        ofPoint rippleAngle = vectorToCenter.crossed(objectLookAt);
        rippleAngle.normalize();
        
        ofPushStyle();
        ofFloatColor waveColor;
        waveColor.set(1.0);
        waveColor.a = pct;
        ofSetColor(waveColor);

        ofPushMatrix();
        ofTranslate( tail[i] );
        glRotatef(-theta, rippleAngle.x, rippleAngle.y, rippleAngle.z);
        ofNoFill();
        ofCircle(0,0,0, 5*(1.0-pct));
        ofPopMatrix();
        
        ofPopStyle();
    }
    
    ofPopStyle();
}

void wSign::drawWaves( ofPoint _pos, float _pct, float _size){
    
   
}