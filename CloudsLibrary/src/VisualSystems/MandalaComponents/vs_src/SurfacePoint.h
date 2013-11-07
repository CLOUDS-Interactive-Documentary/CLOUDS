//
//  SurfacePoint.h
//  MandalaComponents
//
//  Created by laserstorm on 11/6/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxSimpleSurface.h"


class SurfacePoint{
public:
	
	SurfacePoint();
	~SurfacePoint();
	
	void setSurface( ofxSimpleSurface* s );
	
	void setTargetPosition( ofVec3f* v);
	void setTargetNormal( ofVec3f* v);
	
	void update();
	
	void operator = (SurfacePoint sp);
	
	//private:
	ofVec3f position, normal;
	ofVec2f uv;
	
	ofVec3f* outpos;
	ofVec3f* outnorm;
	ofxSimpleSurface* surface;
};
