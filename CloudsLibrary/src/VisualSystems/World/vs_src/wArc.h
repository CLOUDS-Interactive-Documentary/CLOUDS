//
//  wArc.h
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 7/19/13.
//
//

#pragma once
#include "ofMain.h"
#include "wCity.h"

class wArc {
public:
    wArc();
	~wArc();
    void init(wCity _from, wCity _to);
    void init(wArc _fromArcDst, wCity _to);
    
    void kill();
    bool doesArrive();
    bool doesDie();
    wCity getDst();
    
    void update();
    void draw();
    
	ofVboMesh tail;
//	vector<ofIndexType> indeces;
    bool bActive;
private:
    
//    vector<ofPoint> tail;
    ofQuaternion from,to;
    wCity   dst;
    
    float maxAltitud;
    float pct, speed;
    
    bool  bDead;
};