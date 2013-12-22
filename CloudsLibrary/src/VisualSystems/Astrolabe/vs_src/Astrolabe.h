//
//  Astrolabe.h
//  Astrolabe
//
//  Created by laserstorm on 12/20/13.
//
//

#pragma once
#include "ofMain.h"
#include "ofxTween.h"

class TweenInfo{
public:
	TweenInfo(){};
	~TweenInfo(){};
	
	void set( float _step, float _duration, float _delay )
	{
		step = _step;
		duration = _duration;
		delay = _delay;
	}
	
	float duration;
	float delay;
	float step;
};

class Astrolabe : public ofNode{
public:
	Astrolabe();
	~Astrolabe();
	
	void clear();
	
	void setupMesh( float _lowRadian, float _hiRadian, float _innerRadius, float _outerRadius, float _thickness);
	
	void updateRotations();
	
	void addRotationTween( string axis, float startVal, float step, float duration, float delay, float increment );
	
	void draw();
	
	void setVbo( ofVbo& _vbo, int _numVertices );
	
	void buildTheMesh();
	
	ofVec3f normalFromThreePoints(ofVec3f p0, ofVec3f p1, ofVec3f p2);
	
	
	
//protected:
	ofxEasing* ease;
	ofxEasingQuint 	easingquint;
	ofxEasingBounce easebounce;
	ofxEasingSine easesine;
	
	
	ofVec3f eularAngels;
	map<string, ofxTween> rot;
	map<string, TweenInfo> tweenInfo;
	
	ofColor color;
	float r0, r1;//radians
	float innerRadius, outerRadius, thickness;
	vector <ofVec3f> profile;
	
	ofVbo* adoptedVbo;
	ofVbo vbo;
	int numVertices;
	vector<ofIndexType> indices;
	vector<ofVec3f> vertices;
	vector<ofVec3f> normals;
	
	//shader deformation variables
	float thicknessOffset;
};

