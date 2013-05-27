#include "VoroParticle.h"
#include "ofMain.h"

//------------------------------------------------------------
VoroParticle::VoroParticle(){
	set(0,0,0);
    vel.set(ofRandom(-10,10), ofRandom(-10,10));
    acc.set(0,0,0);
    
    trailLength = ofRandom(50,100);
	damping = 0.07f;
    
    color.set(1.0);
}

void VoroParticle::init( ofPoint _pos, ofPoint _vel){
    this->set(_pos);
    vel.set(_vel);
}

//------------------------------------------------------------
void VoroParticle::addForce(ofPoint &_force){
    acc += _force;
}

//------------------------------------------------------------
void VoroParticle::update(){
	
    vel += acc;
    vel *= 1.0f - damping;
    *this += vel;
    
    acc *= 0;
    
    //    if (vel.length() > 0.1){
    //        trail.push_back(*this);
    //    }
    //
    //    if( trail.size() > 50){
    //        trail.erase(trail.begin());
    //    }
    
}

//------------------------------------------------------------
void VoroParticle::draw(){
    
    /*
     ofMesh mesh;
     mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
     for (int i = 0; i < trail.size(); i++){
     
     int i_m_1 = MAX(i-1,0);
     int i_p_1 = MIN(i+1, trail.size()-1);
     
     ofPoint pta = trail[i_m_1];
     ofPoint ptb = trail[i];
     ofPoint ptc = trail[i_p_1];
     
     ofPoint diff = ptc - pta;
     float angle = atan2(diff.y, diff.x);
     angle += PI/2;
     
     float width = size*0.8 + diff.length()*0.2;
     
     ofPoint offsetA;
     offsetA.x = ptb.x + width * cos(angle);
     offsetA.y = ptb.y + width * sin(angle);
     offsetA.z = 0.0;
     
     ofPoint offsetB;
     offsetB.x = ptb.x - width * cos(angle);
     offsetB.y = ptb.y - width * sin(angle);
     offsetB.z = 0.0;
     
     //  Map the position on the array with the alpha to geting alfa gradient
     //
     float alpha = ofMap(i+1, 1,trail.size(), 0.0, 1.0);
     
     mesh.addColor(ofFloatColor( color, alpha) );
     mesh.addVertex(offsetA);
     mesh.addColor(ofFloatColor( color, alpha) );
     mesh.addVertex(offsetB);
     }
     
     // draw the mesh
     //
     ofSetColor( 255 );
     ofFill();
     mesh.draw();
     */
    
    
    //    ofSetColor(color);
    ofCircle(*this, size);
}

void VoroParticle::infinitWalls(){
	
	// what are the walls
	float minx = 0;
	float miny = 0;
	float maxx = ofGetWidth();
	float maxy = ofGetHeight();
	
	if (x > maxx){
		x = minx;
	} else if (x < minx){
		x = maxx;
	}
	
	if (y > maxy){
		y = miny;
	} else if (y < miny){
		y = maxy;
	}
}

//------------------------------------------------------------
void VoroParticle::addRepulsionForce(ofPoint posOfForce, float radius, float scale){
    
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
void VoroParticle::addAttractionForce(ofPoint posOfForce, float radius, float scale){
	
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= (*this) - posOfForce;
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
void VoroParticle::addRepulsionForce(VoroParticle *p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(*p);
	
	// ----------- (2) calculate the difference & length
	
	ofVec2f diff	= (*this) - posOfForce;
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

//------------------------------------------------------------
void VoroParticle::addRepulsionForce(VoroParticle *p, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(*p);
	
	// ----------- (2) calculate the difference & length
	
	ofVec2f diff	= (*this) - posOfForce;
	float length	= diff.length();
    float radius    = size*2.f + p->size*2.f;
	
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

//------------------------------------------------------------
void VoroParticle::addAttractionForce(VoroParticle *p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(*p);
	
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= (*this) - posOfForce;
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

//------------------------------------------------------------
void VoroParticle::addClockwiseForce(VoroParticle *p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(*p);
	
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= (*this) - posOfForce;
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
		acc.x -= diff.y * scale * pct;
        acc.y += diff.x * scale * pct;
		p->acc.x += diff.y * scale * pct;
        p->acc.y -= diff.x * scale * pct;
    }
}

//------------------------------------------------------------
void VoroParticle::addCounterClockwiseForce(VoroParticle *p, float radius, float scale){
	
	// ----------- (1) make a vector of where this particle p is:
	ofPoint posOfForce;
	posOfForce.set(*p);
	
	// ----------- (2) calculate the difference & length
	
	ofPoint diff	= (*this) - posOfForce;
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
		acc.x += diff.y * scale * pct;
        acc.y -= diff.x * scale * pct;
		p->acc.x -= diff.y * scale * pct;
        p->acc.y += diff.x * scale * pct;
		
    }
}

//------------------------------------------------------------
void VoroParticle::bounceOffWalls(){
	
	// sometimes it makes sense to damped, when we hit
	bool bDampedOnCollision = true;
	bool bDidICollide = false;
	
	// what are the walls
	float minx = 0;
	float miny = 0;
	float maxx = ofGetWidth();
	float maxy = ofGetHeight();
	
	if (x > maxx){
		x = maxx; // move to the edge, (important!)
		vel.x *= -1;
		bDidICollide = true;
	} else if (x < minx){
		x = minx; // move to the edge, (important!)
		vel.x *= -1;
		bDidICollide = true;
	}
	
	if (y > maxy){
		y = maxy; // move to the edge, (important!)
		vel.y *= -1;
		bDidICollide = true;
	} else if (y < miny){
		y = miny; // move to the edge, (important!)
		vel.y *= -1;
		bDidICollide = true;
	}
	
	if (bDidICollide == true && bDampedOnCollision == true){
		vel *= 0.3;
	}
	
}
