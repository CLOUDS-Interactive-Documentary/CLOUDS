//
//  wArc.cpp
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 7/19/13.
//
//

#include "wArc.h"

void wArc::init(wCity _from, wCity _to){
    from = _from.getQuaternion();
    to = _to.getQuaternion();
    dst = _to;
    pct = 0.0;
    maxAltitud = _from.distance(_to);
    
    speed = ofNoise( maxAltitud )*0.05;
    bActive = true;
    bDead = false;
}

bool wArc::doesArrive(){
    return (pct >= 1.0)?true:false;
}

bool wArc::doesDie(){
    return bDead && (tail.size()==0)?true:false;
}

wCity wArc::getDst(){
    return dst;
}

void wArc::kill(){
    bActive = false;
    
    if ( doesArrive() ){
        bDead = true;
    }
}

void wArc::update(){
    
    if ( pct <= 1.0){
        ofQuaternion rotation;
        rotation.slerp( pct , from, to);

        ofPoint pos = rotation * ofVec3f(0,0,-300-abs(sin(PI*pct)*maxAltitud*0.25) );
        tail.push_back(pos);
        
        pct = ofLerp(pct, 1.1, speed);
    }
    
    if (bDead){
        if (tail.size() > 0) {
            tail.erase(tail.begin());
        }
    }
    
}

void wArc::draw(){
    ofNoFill();
    ofBeginShape();
    for(int i = 0; i < tail.size(); i++){
        ofVertex(tail[i]);
    }
    ofEndShape();
    
    if ( tail.size() > 0){
        ofPoint loc = tail[ tail.size()-1 ];
        ofPushStyle();
        glPointSize(2);
        glBegin(GL_POINTS);
        glVertex3f(loc.x,loc.y,loc.z);
        glEnd();
        ofPopStyle();
    }
}