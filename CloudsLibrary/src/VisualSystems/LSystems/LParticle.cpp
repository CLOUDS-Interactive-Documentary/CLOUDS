/*
 *  LParticle.cpp
 *
 *  Created by Patricio González Vivo on 24/01/11.
 *  Copyright 2011 PatricioGonzalezVivo.com. All rights reserved.
 *
 */

#include "LParticle.h"

LParticle::LParticle(){
	set(ofRandom(ofGetWidth()),ofRandom(ofGetHeight()));
	vel.set(0,0,0);
	acc.set(0,0,0);
    
	color.set(255,255,255);

    connect = NULL;
    bTrail = NULL;
}

void LParticle::addRepulsionForce(ofPoint posOfForce, float radius, float scale){
    
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

//------------------------------------------------------------
void LParticle::addRepulsionForce(LParticle *p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(*p);
	
	// ----------- (2) calculate the difference & length
	
	ofVec2f diff	= *this - posOfForce;
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
		p->acc -= diff * scale * pct;
    }
}

void LParticle::addAttractionForce(ofPoint posOfForce, float radius, float scale){
	
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

//------------------------------------------------------------
void LParticle::addAttractionForce(LParticle *p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(*p);
	
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
		p->acc += diff * scale * pct;
    }
	
}

ofPoint LParticle::steer(ofPoint target, bool arrival) {
    ofPoint steer;
	
	if (arrival){
		ofPoint desired = target - *this;
		float d = desired.length();
        
		if (d > 0) {
			desired.normalize();
			if (d < 1) 
				desired *= (d); // This damping is somewhat arbitrary
			//else 
			//desired *= 2;
			// Steering = Desired minus Velocity
			steer = desired - vel; //PVector.sub(desired,vel);
			//steer.limit(0.9);  // Limit to maximum steering force
		} else
			steer = ofPoint(0,0,0);

	} else {
		steer = target - *this;
		//steer.limit(maxforce);
	}
	
    return steer;
}

void LParticle::update(float _speed){
	
    vel += acc;
    vel *= _speed;
    *this += vel;
    acc *= 0;
    
    if (bTrail){
        tail.push_back(*this);
        while (tail.size() > 100) {
            tail.erase(tail.begin());
        }
    }
}

void LParticle::draw(){
    ofPushStyle();
    
    if (bTrail){
        
//        ofSetColor(color);
//        ofNoFill();
//        ofBeginShape();
//        for(int i = 0; i < tail.size(); i++){
//            ofVertex(tail[i]);
//        }
//        ofEndShape();
        
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
        for (int i = 0; i < tail.size(); i++){
            float alpha = ofMap(i+1, 1,tail.size(), 0.0, 0.9);
            
            mesh.addColor(ofFloatColor( 1.0, alpha) );
            mesh.addVertex(tail[i]);
        }
        ofFill();
        ofSetColor( 255 );
        mesh.draw();
        
    } else {
        glBegin(GL_POINTS);
        glVertex3f(x,y,z);
        glEnd();
    }
    
    if (connect != NULL){
        ofLine(*this, *connect);
    }
    
    ofPopStyle();
}