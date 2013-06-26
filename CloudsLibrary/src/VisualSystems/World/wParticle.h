/*
 *  wParticle.h
 *
 *  Created by Patricio González Vivo on 24/01/11.
 *  Copyright 2011 PatricioGonzalezVivo.com. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"

class wParticle : public ofPoint {
public:
	wParticle();
    
    void    place(float _lat, float _long);
    
	void	applyForce(ofPoint _force){acc += _force;};
	void	applyGravityForce(float _pct);
    
    void    addRepulsionForce(ofPoint p, float radius, float scale);
    void    addAttractionForce(ofPoint p, float radius, float scale);
    void    addRepulsionForce(wParticle *p, float radius, float scale);
    void    addAttractionForce(wParticle *p, float radius, float scale);
    
	void	applyFlockingForce(ofPoint *_offSet, float _neighbordhood, float _independece);
    
    ofPoint	steer(ofPoint _target, bool _slowdown);
    
	void	update(float _speed);
	void	draw();
    
    vector<ofPoint> tail;
    
    wParticle   *connect;
    
    ofPoint     loc,vel,acc;
	ofPoint     localOffset;
	ofColor     color;
    
    bool        bTrail;
};
