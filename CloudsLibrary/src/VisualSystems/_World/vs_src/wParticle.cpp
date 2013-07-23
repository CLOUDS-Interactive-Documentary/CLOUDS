/*
 *  wParticle.cpp
 *
 *  Created by Patricio González Vivo on 24/01/11.
 *  Copyright 2011 PatricioGonzalezVivo.com. All rights reserved.
 *
 */

#include "wParticle.h"

wParticle::wParticle(){
	loc.set(ofRandom(-300.0f,300.0f),ofRandom(-300.0f,300.0f),ofRandom(-300.0f,300.0f));
	vel.set(0,0,0);
	acc.set(0,0,0);
	
	localOffset.set(ofRandom(1.0f),ofRandom(1.0f),ofRandom(1.0f));
	
	color.set(255,255,255);
    
    ripplePnt.set(0,0);
    ripplePct = 0.0;
    rippleScl = 0.05;
    bDead = false;
    bLanching = true;
    
    connect = NULL;
    bTrail = NULL;
}

void wParticle::place(float _lat, float _long){
    ofQuaternion latRot, longRot;
    latRot.makeRotate( _lat, 1, 0, 0);
    longRot.makeRotate( _long, 0, 1, 0);
    
    this->set(latRot * longRot * ofPoint(0,0,-300));
}

float wParticle::getAlitude(){
    return loc.distance(ofPoint(0,0,0)) - 300;
}

void wParticle::applyGravityForce(float _pct){
    acc += steer(*this,true)*_pct;
//    addAttractionForce(*this, 100, _pct);
}

void wParticle::addRepulsionForce(ofPoint posOfForce, float radius, float scale){
    
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= loc - posOfForce;
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
void wParticle::addRepulsionForce(wParticle *p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(*p);
	
	// ----------- (2) calculate the difference & length
	
	ofVec2f diff	= loc - posOfForce;
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

void wParticle::addAttractionForce(ofPoint posOfForce, float radius, float scale){
	
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= loc - posOfForce;
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
void wParticle::addAttractionForce(wParticle *p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(*p);
	
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= loc - posOfForce;
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

ofPoint wParticle::steer(ofPoint target, bool arrival) {
    ofPoint steer;
	
	if (arrival){
		ofPoint desired = target - loc;
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
		steer = target - loc;
		//steer.limit(maxforce);
	}
	
    return steer;
}

void wParticle::applyFlockingForce(ofPoint * _offset, float _neighbordhood, float _independece){
	acc += ofVec3f(
	 ofNoise(
	 loc.x / _neighbordhood + _offset->x + localOffset.x * _independece,
	 loc.y / _neighbordhood,
	 loc.z / _neighbordhood)
	 - .5,
	 ofNoise(
	 loc.x / _neighbordhood,
	 loc.y / _neighbordhood + _offset->y  + localOffset.y * _independece,
	 loc.z / _neighbordhood)
	 - .5,
	 ofNoise(
	 loc.x / _neighbordhood,
	 loc.y / _neighbordhood,
	 loc.z / _neighbordhood + _offset->z + localOffset.z * _independece)
	 - .5);
}

void wParticle::update(float _speed){
	
    if (getAlitude() < 20 && ripplePct == 0.0){
        
        if (!bLanching){
            ripplePct += 0.01;
            ripplePnt = loc;
        }
        
    } else {
        bLanching = false;
    }
    
    if (ripplePct > 0.0 ){
        
        if (ripplePct < 3.0)
            ripplePct += 0.01;
        else
            bDead = true;
        
    } else {
        vel += acc;
        vel *= _speed;
        loc += vel;
        acc *= 0;
    }
        
    if (bTrail){
        tail.push_back(loc);
        while (tail.size() > 100) {
            tail.erase(tail.begin());
        }
    }
}

void wParticle::draw(){
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
        glVertex3f(loc.x,loc.y,loc.z);
        glEnd();
    }
    
    if (connect != NULL){
        ofLine(loc, connect->loc);
    }
    
    if (ripplePct > 0.0 ){
        drawRipple(ripplePct*0.3);
        drawRipple(ripplePct*0.61);
        drawRipple(ripplePct);
    }
    
    ofPopStyle();
}

void wParticle::drawRipple( float _pct ){
    //  Jen equation
    //
    float totalGlobeRadio = 100;
    float rippleDps = _pct*rippleScl;
    float rippleHeight = rippleDps*totalGlobeRadio;
    float rippleRadio = sqrt( 2*rippleHeight*totalGlobeRadio-powf(rippleHeight,2) );
    
    ofPoint vectorToCenter = ofPoint(0,0,0)-ripplePnt;
    vectorToCenter.normalize();
    ofPoint rippleCenter = ripplePnt + vectorToCenter * rippleHeight;
    
    ofPoint objectLookAt = ofVec3f(0,0,1);
    float theta = objectLookAt.angle(vectorToCenter);
    ofPoint rippleAngle = vectorToCenter.crossed(objectLookAt);
    rippleAngle.normalize();
    
    ofPushStyle();
    ofFloatColor rippleColor;
    rippleColor.set(1.0);
    rippleColor.a = (1.0-_pct)*0.2;
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