/*
 *  Gesture.h
 *  YellowTail
 *
 *  Copyright 2008 Carnegie Mellon University. All rights reserved.
 *
 */

#ifndef GESTURE_H
#define GESTURE_H

#include <math.h>
#include <algorithm>
#include "ofxXmlSettings.h"

#include "ofMain.h"
#include "Vec3f.h"
#include "YtPolygon.h"
#include "YellowTailColor.h"

#define INITIAL_THICKNESS 22

// this is just as big as we'd like this to get
#define GESTURE_CAPACITY 480

    
typedef enum {
	GestureStyleTraveling,
	GestureStyleInPlace
} GestureStyle;

typedef enum {
	GestureDirectionForwards,
	GestureDirectionBackwards
} GestureDirection;

typedef enum {
	CrossLocationNone     =   0,
	CrossLocationLeft     =   1 << 0,
	CrossLocationRight    =   1 << 1,
	CrossLocationTop      =   1 << 2,
	CrossLocationBottom   =   1 << 3,
} CrossLocation;

class Gesture
{
public:
	Gesture(int maxWidth, int maxHeight);
	
	void	clear();
	void	clearPolygons();
	
	void	addPoint (float x, float y);
	void	addPointAndHalfwayPoint (float x, float y, float distanceThresholdForAdding);
	float	distanceToLastPoint (float ix, float iy);
	
	void    render();
	void	compile();
	void    advance();
	void	smooth(float weight);
	void    fade();
	
	string toXML();
	void fromXMLFile(string xmlFile);
	
	CrossLocation		crosses[GESTURE_CAPACITY];
	bool    hasCross;
	
	YtPolygon polygons[GESTURE_CAPACITY];
	int		polygonCount;
	
	Vec3f	**points;
	int		pointCount;
	
	bool    isVisible();
	bool    isValid();
	
	void setStyle(GestureStyle style) { mStyle = style; }
	GestureStyle getStyle() { return mStyle; }

	void setFillColor(ofColor fillColor) { mFillColor = fillColor; }
	ofColor getFillColor() { return mFillColor; }
	
	void setStrokeColor(ofColor strokeColor) { mStrokeColor = strokeColor; }
	ofColor getStrokeColor() { return mStrokeColor; }
	
	void setDirection(GestureDirection direction) { mDirection = direction; }
	GestureDirection getDirection() { return mDirection; }
	
private:
	float	getPressureFromVelocity(float v);
	void	handleCoordinateOverflow();
	
	void    fromXMLSerializer(ofxXmlSettings xmlSerializer);
	
	float	damp;
	float	dampInv;
	float	damp1;
	
	ofColor mFillColor;
	ofColor mStrokeColor;
	GestureStyle		mStyle;
	GestureDirection    mDirection;
	
	int		maxWidth;
	int		maxHeight;
	
	float	initialX;
	float	inity;
	float	jumpDx;
	float	jumpDy;
	float	thickness;
	
	inline float GMOD (float A, float B){ return (A - (floorf(A/B)*B)); }
};    



#endif