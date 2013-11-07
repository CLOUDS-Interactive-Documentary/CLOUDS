//
//  MandalaComponent.h
//  MandalaComponents
//
//  Created by laserstorm on 11/6/13.
//
//

#pragma once

#include "ofMain.h"
#include "SurfacePoint.h"


class MandalaComponent : public ofNode{
public:
	
    static int componentCount;
	MandalaComponent( ofxSimpleSurface* _s=NULL,  ofVboMesh* _m=NULL, string _name = "_comp");
	~MandalaComponent();
	
	void setup( ofxSimpleSurface* _s=NULL );
	
	void setSurface( ofxSimpleSurface* _s );
	
	void setMesh( ofVboMesh* _m );
	
	void setUV( float u, float v);
	void setUV( ofVec2f uv);
	ofVec2f& getUV();
	
	
//	void setScale(float x, float y, float z);
//	void setScale( ofVec3f _scale );
	
	ofVec3f& getScale(){	return scl;}
	
	void update();
	
	void draw();
	void drawWireframe();
	void drawPoints();
	
	string getUniqueName(string _name = "comp_");
	
	
	//private:
	string name;
	
	ofVboMesh* m;
	
	ofxSimpleSurface* s;
	SurfacePoint pOnS;
	ofVec3f scl;
	float offset;
	
	bool bDraw;
};