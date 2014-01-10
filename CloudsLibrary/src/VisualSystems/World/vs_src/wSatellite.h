//
//  wSatellite.h
//  VSWorld
//
//  Created by Patricio Gonzalez Vivo on 6/27/13.
//
//

#pragma once

#include "ofMain.h"

class wSatellite : public ofPoint {
public:
	wSatellite();
    
    void    place( float _alt, ofVec3f _orbit );
    
    void    update();
    
	void	draw();
	
	//JG optimization
//	ofIndexType meshIndex;
//    vector<int> connectedIndeces;
//	vector<ofIndexType> connectedIndecesIndedeces;
	
//	ofVboMesh* satelliteMesh;
private:
    float   lat,lon,freq;
    ofPoint orbit;
    ofPoint altitud;
};