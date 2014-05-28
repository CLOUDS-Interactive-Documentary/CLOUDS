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
	duration = 30;
	bUseUpSpline = false;
	bLoop = true;
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
	
	ofPolyline posLine;
	ofPolyline targetLine;
	ofPolyline upLine;

	for (int i=0; i<numFrames; i++) {
		xml.pushTag("frame", i);
		
		ofVec3f p(xml.getValue("x", 0.f), xml.getValue("y", 0.f), xml.getValue("z", 0.f) );
		posLine.addVertex( p );
		targetLine.addVertex(ofVec3f(xml.getValue("tx", 0.f), xml.getValue("ty", 0.f), xml.getValue("tz", 0.f)));
		upLine.addVertex(ofVec3f(xml.getValue("upx", 0.f), xml.getValue("upy", 0.f), xml.getValue("upz", 0.f)));
		
		xml.popTag();
	}
	
	clear();
	
	xml.clear();
	
	//linearize the cv spacing for the control curve
	float spacing = .5;//this seemed to fit the typical paths we have so far
	
	addPositionControlVertices( posLine.getResampledBySpacing(spacing).getVertices() );
	addTargetControlVertices( targetLine.getResampledBySpacing(spacing).getVertices() );
	addUpControlVertices( upLine.getResampledBySpacing(spacing).getVertices() );
	
	posLine.clear();
	targetLine.clear();
	upLine.clear();
	
	startTime = ofGetElapsedTimef();
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

void CloudsPathCamera::	update()
{
    if(duration == 0){
        u = startTime;
    }
    else{
        u = ofMap( ofGetElapsedTimef(), startTime, startTime + duration,0 ,1);
    }
	update( u );
}

void CloudsPathCamera::update( float t )
{
	if(bLoop)	t -= floor( t );
	else t = ofClamp(t, 0, 1);
	
	u = t;
	
	setPosition( positionSpline.getPoint( t ) );
	if(bUseUpSpline)	lookAt( targetSpline.getPoint( t ), upSpline.getPoint( t ) );
	else	lookAt( targetSpline.getPoint( t ) );
}

void CloudsPathCamera::drawPaths( float lineWidth)
{
	ofSetLineWidth(lineWidth);
	ofSetColor(255,0,0, 255);
	positionSpline.update();
	positionSpline.draw();
	
	ofSetColor(0,0,255, 255);
	targetSpline.update();
	targetSpline.draw();
}

void CloudsPathCamera::setStartTime( float _startTime )
{
	startTime = _startTime;
}

void CloudsPathCamera::setDuration(float _duration )
{
	duration = _duration;
}

float& CloudsPathCamera::getStartTime()
{
	return startTime;
}

float& CloudsPathCamera::getDuration()
{
	return duration;
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
