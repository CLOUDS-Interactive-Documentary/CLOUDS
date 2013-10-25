//
//  CloudsPathCamera.cpp
//
//  Created by lars berg on 10/24/13.
//
//

#include "CloudsPathCamera.h"

CloudsPathCamera::CloudsPathCamera()
{
	startTime = 0;
	endTime = 30;
	bUseUpSpline = false;
	bLoop = false;
};
CloudsPathCamera::~CloudsPathCamera(){
	clear();
};

void CloudsPathCamera::clear()
{
	positionSpline.clear();
	targetSpline.clear();
	upSpline.clear();
}


void CloudsPathCamera::loadPathFromFile( string path )
{
	ofxXmlSettings xml;
	
	xml.loadFile( path );
	int numFrames = xml.getNumTags("frame");
		
	vector<ofVec3f> cameraPath( numFrames );
	vector<ofVec3f> cameraTargetPath(numFrames);
	vector<ofVec3f> cameraUp( numFrames );

	for (int i=0; i<numFrames; i++) {
		xml.pushTag("frame", i);
		
		cameraPath[i].set( xml.getValue("x", 0.f), xml.getValue("y", 0.f), xml.getValue("z", 0.f) );
		cameraTargetPath[i].set( xml.getValue("tx", 0.f), xml.getValue("ty", 0.f), xml.getValue("tz", 0.f) );
		cameraUp[i].set( xml.getValue("upx", 0.f), xml.getValue("upy", 0.f), xml.getValue("upz", 0.f) );
		
		xml.popTag();
	}
	
	clear();

	addPositionControlVertices( cameraPath );
	addTargetControlVertices( cameraTargetPath );
	addUpControlVertices( cameraUp );
	
	setLoop( false );
	useUp( true );
}

void CloudsPathCamera::addPositionControlVertex( ofVec3f v )
{
	positionSpline.addControlVertex( v );
}

void CloudsPathCamera::addTargetControlVertex( ofVec3f v )
{
	targetSpline.addControlVertex( v );
}

void CloudsPathCamera::addUpControlVertex( ofVec3f v )
{
	upSpline.addControlVertex( v );
}

void CloudsPathCamera::addPositionControlVertices( vector<ofVec3f> v )
{
	positionSpline.addControlVertices( v );
}

void CloudsPathCamera::setPositionControlVertices( vector<ofVec3f>& v )
{
	positionSpline.addControlVertices( v );
}

void CloudsPathCamera::addTargetControlVertices( vector<ofVec3f> v )
{
	targetSpline.addControlVertices( v );
}

void CloudsPathCamera::setTargetControlVertices( vector<ofVec3f>& v )
{
	targetSpline.addControlVertices( v );
}

void CloudsPathCamera::addUpControlVertices( vector<ofVec3f> v )
{
	bUseUpSpline = true;
	upSpline.addControlVertices( v );
}

void CloudsPathCamera::setUpControlVertices( vector<ofVec3f>& v )
{
	bUseUpSpline = true;
	upSpline.addControlVertices( v );
}

void CloudsPathCamera::update()
{
	float t = ofMap( ofGetElapsedTimef(), startTime, endTime,0 ,1);
	update( t );
}

void CloudsPathCamera::update( float t )
{
	if(bLoop)	t -= floor( t );
	else t = ofClamp(t, 0, 1);
	
	setPosition( positionSpline.getPoint( t ) );
	if(bUseUpSpline)	lookAt( targetSpline.getPoint( t ), upSpline.getPoint( t ) );
	else	lookAt( targetSpline.getPoint( t ) );
}

void CloudsPathCamera::setStartTime( float _startTime )
{
	startTime = _startTime;
}

void CloudsPathCamera::setEndTime(float _endTime )
{
	endTime = _endTime;
}

float& CloudsPathCamera::getStartTime()
{
	return startTime;
}

float& CloudsPathCamera::getEndTime()
{
	return endTime;
}

ofxSimpleSpline& CloudsPathCamera::getPositionSpline()
{
	return positionSpline;
}

ofxSimpleSpline& CloudsPathCamera::getTargetSpline()
{
	return targetSpline;
}

ofxSimpleSpline& CloudsPathCamera::getUpSpline()
{
	return upSpline;
}

void CloudsPathCamera::useUp( bool useUp )
{
	bUseUpSpline = useUp;
}

bool& CloudsPathCamera::getUsingUp()
{
	return bUseUpSpline;
}

void CloudsPathCamera::setLoop(bool loop)
{
	bLoop = loop;
}

bool& CloudsPathCamera::getLoop()
{
	return bLoop;
}
