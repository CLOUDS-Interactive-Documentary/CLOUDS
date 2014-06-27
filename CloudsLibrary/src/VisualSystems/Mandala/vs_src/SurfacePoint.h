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
	
	ofVec3f position, normal;
	
	ofVec3f* outpos;
	ofVec3f* outnorm;
	ofxSimpleSurface* surface;
	//ofVec2f& getUV(){
	//	return uv;
	//}
	//void setUV(const ofVec2f& uv){
	//	this->uv.set(uv);
	//}
//private:
	ofVec2f uv;

};
