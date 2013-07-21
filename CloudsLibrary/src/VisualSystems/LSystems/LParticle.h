/*
 *  LParticle.h
 *
 *  Created by Patricio González Vivo on 24/01/11.
 *  Copyright 2011 PatricioGonzalezVivo.com. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"

class LParticle : public ofPoint {
public:
	LParticle();
    
	void	applyForce(ofPoint _force){acc += _force;};
    
    void    addRepulsionForce(ofPoint p, float radius, float scale);
    void    addAttractionForce(ofPoint p, float radius, float scale);
    void    addRepulsionForce(LParticle *p, float radius, float scale);
    void    addAttractionForce(LParticle *p, float radius, float scale);

	void	update(float _speed);
	void	draw();
    
    vector<ofPoint> tail;
    ofFloatColor    color;
    ofPoint         vel,acc;
    
    LParticle   *connect;
    bool        bTrail;
    
private:
    ofPoint	steer(ofPoint _target, bool _slowdown);
};
