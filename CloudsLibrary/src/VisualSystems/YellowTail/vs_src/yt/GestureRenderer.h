/*
 *  GestureRenderer.h
 *  yellowtail
 *
 *  Created by Max Hawkins on 12/15/10.
 *  Copyright 2010 Carnegie Mellon University. All rights reserved.
 *
 */

#define MAX_VERTICES 65536

#include "ofMain.h"
#include "YtPolygon.h"
#include "Gesture.h"
#include "Color.h"

typedef unsigned int GLenum;


class Gesture;

#ifndef GESTURE_RENDERER
#define GESTURE_RENDERER

class GestureRenderer {
public:
	static void renderGesture(Gesture *gesture, int maxWidth, int maxHeight);
protected:
	// I laugh at thread safety
	static float	vertexData[MAX_VERTICES];
	static int		vertexCount;
	
	static void renderVertices(GLenum renderMode);
	
	static void vertex   (float x, float y);
	static void vertex	(float x, float y, float dx, float dy);
	static void vertexP  (YtPolygon *p, int which);
	static void vertexP  (YtPolygon *p, int which, float dx, float dy);
	static void vertexFp (float *xpoints, float *ypoints, int which);
	static void vertexFp (float *xpoints, float *ypoints, int which, float dx, float dy);
};

#endif
