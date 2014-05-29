//
//  wStar.cpp
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/25/13.
//
//

#include "wStar.h"

wStar::wStar(){
    connect = NULL;
    constAlpha = 0.03;
}

void wStar::place( float _ra, float _dec){
    float ra = _ra*(360.0/24.0)-180;
    float dec = _dec;
    
    ofQuaternion decRot, raRot;
    decRot.makeRotate( dec, 1, 0, 0);
    raRot.makeRotate( ra, 0, 1, 0);
    
    this->set(decRot * raRot * ofPoint(0,0,-1000));
}

void wStar::draw(ofMesh& starMesh, ofMesh& lineMesh){
 //   ofPushStyle();
 
	starMesh.addColor(ofFloatColor(1.0,ofNoise(x,y,z*ofGetElapsedTimef()*0.005)*1.5 ));
	starMesh.addVertex(*this);

//    ofSetColor();
//    glBegin(GL_POINTS);
//    glVertex3f(x,y,z);
//    glEnd();
    
    if (connect != NULL){
		lineMesh.addColor(ofFloatColor(0.8,constAlpha));
		lineMesh.addColor(ofFloatColor(0.8,constAlpha));
//        ofSetColor();
//        ofSetLineWidth(2.0);
		lineMesh.addVertex(*this);
		lineMesh.addVertex(*connect);
//        ofLine(*this, *connect);
    }
    
//    ofPopStyle();
}