//
//  CloudsPathCamera.h
//
//  Created by lars berg on 10/24/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxSimpleSpline.h"

class CloudsPathCamera : public ofCamera{
public:
	CloudsPathCamera();
	~CloudsPathCamera();
	
	void clear();
	
	void loadPathFromFile( string path );
	
	void addPositionControlVertex( ofVec3f v );
	void addTargetControlVertex( ofVec3f v );
	void addUpControlVertex( ofVec3f v );
	
	void addPositionControlVertices( vector<ofVec3f> v );
	void setPositionControlVertices( vector<ofVec3f>& v );
	
	void addTargetControlVertices( vector<ofVec3f> v );
	void setTargetControlVertices( vector<ofVec3f>& v );
	
	void addUpControlVertices( vector<ofVec3f> v );
	void setUpControlVertices( vector<ofVec3f>& v );
	
	void update();
	void update( float t );
	
	void drawPaths( float lineWidth = 3);
	
	void setStartTime( float _startTime );
	void setDuration(float _duration );
	
	float& getStartTime();
	float& getDuration();
	
	ofxSimpleSpline& getPositionSpline();
	ofxSimpleSpline& getTargetSpline();
	ofxSimpleSpline& getUpSpline();
	
	void useUp( bool useUp );
	bool& getUsingUp();
	void setLoop(bool loop);
	bool& getLoop();
	
protected:
	ofxSimpleSpline positionSpline, targetSpline, upSpline;
	float startTime, duration;
	
	bool bLoop, bUseUpSpline;
};
