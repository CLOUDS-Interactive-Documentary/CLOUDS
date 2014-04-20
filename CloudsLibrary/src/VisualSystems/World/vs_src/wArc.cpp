//
//  wArc.cpp
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 7/19/13.
//
//

#include "wArc.h"


wArc::wArc(){

}

wArc::~wArc(){
	
//	if(arcMesh == NULL) return;
//	
//	while(indeces.size() > 0){
//		arcMesh->removeVertex( *indeces.begin() );
//		indeces.erase(indeces.begin());
//	}
	
}

void wArc::init(wCity _from, wCity _to){
    from = _from.getQuaternion();
    to = _to.getQuaternion();
    dst = _to;
    pct = 0.0;
    maxAltitud = _from.distance(_to);
    
    speed = ofNoise( maxAltitud )*0.05;
    bActive = true;
    bDead = false;
	
	tail.setUsage(GL_STREAM_DRAW);
	tail.setMode(OF_PRIMITIVE_LINE_STRIP);
}

bool wArc::doesArrive(){
    return pct >= 1.0;
}

bool wArc::doesDie(){
    return bDead && tail.getNumVertices() == 0;
}

wCity wArc::getDst(){
    return dst;
}

void wArc::kill(){
    bActive = false;
    
    if (doesArrive()){
        bDead = true;
    }
}

void wArc::update(){
    
    if ( pct <= 1.0){
        ofQuaternion rotation;
        rotation.slerp( pct , from, to);

        ofPoint pos = rotation * ofVec3f(0,0,-300-abs(sin(PI*pct)*maxAltitud*0.25) );
        tail.addVertex(pos);
//		ofIndexType start = arcMesh->getNumVertices();
//		if(indeces.size() > 0){
//			arcMesh.addVertex( arcMesh->( indeces.back()) );
//		}
//		else {
//			arcMesh->addVertex( pos );
//		}
//		arcMesh->addVertex(pos);
//		indeces.push_back(start);
//		indeces.push_back(start+1);
		
        pct = ofLerp(pct, 1.1, speed);
    }
    
    if (bDead){
//		if(indeces.size() > 0){
//			arcMesh->removeVertex( *indeces.begin() );
//			indeces.erase(indeces.begin());
//			arcMesh->removeVertex( *indeces.begin() );
//			indeces.erase(indeces.begin());
//		}

        if (tail.getNumVertices() > 0) {
            tail.removeVertex(0);
        }
    }
    
}

void wArc::draw(){
	tail.draw();
//    ofNoFill();
//    ofBeginShape();
//    for(int i = 0; i < tail.size(); i++){
//        ofVertex(tail[i]);
//    }
//    ofEndShape();
//    
//    if ( tail.size() > 0){
//        ofPoint loc = tail[ tail.size()-1 ];
//        ofPushStyle();
//        glPointSize(2);
//        glBegin(GL_POINTS);
//        glVertex3f(loc.x,loc.y,loc.z);
//        glEnd();
//        ofPopStyle();
//    }
}