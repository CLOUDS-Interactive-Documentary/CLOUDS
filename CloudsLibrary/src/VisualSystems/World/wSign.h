//
//  wSign.h
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/27/13.
//
//

#pragma once

#include "ofMain.h"

class wSign : public ofPoint {
public:
	wSign();
    
    void    setTarget( ofPoint *_target );
    
    float   getAlitude();
    
	void	applyForce(ofPoint _force){acc += _force;};
    
    void    addRepulsionForce(ofPoint p, float radius, float scale);
    void    addAttractionForce(ofPoint p, float radius, float scale);
    
	void	update(float _speed);
	void	draw();
    
    vector<ofPoint> tail;
    
    ofPoint *target;
    ofPoint vel,acc;
    
    int     nFrame;
    bool    bDead;
    
private:
    void drawWaves( ofPoint _pos, float _pct, float _size);
};