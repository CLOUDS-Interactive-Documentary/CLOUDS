/*
 *  vParticle.h
 *
 *  Created by Patricio González Vivo on 24/01/11.
 *  Copyright 2011 PatricioGonzalezVivo.com. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"
#include "VoroParticle.h"

class vParticle : public ofPoint {
public:
	vParticle();
    
	void	applyForce(ofPoint _force){acc += _force;};
    
    void    addRepulsionForce(ofPoint p, float radius, float scale);
    void    addAttractionForce(ofPoint p, float radius, float scale);
    
    void    addRepulsionForce(VoroParticle *p, float radius, float scale);
    void    addAttractionForce(VoroParticle *p, float radius, float scale);
    
	void	applyFlockingForce(ofPoint *_offSet, float _neighbordhood, float _independece);

	void	update(float _speed);
	void	draw();
    
    vector<ofPoint> tail;
    
    ofPoint     loc,vel,acc;
	ofPoint     localOffset;
	ofColor     color;
    
private:
    ofPoint	steer(ofPoint _target, bool _slowdown);
};
