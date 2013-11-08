/*
 *  GestureRenderer.mm
 *  yellowtail
 *
 *  Created by Max Hawkins on 12/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "GestureRenderer.h"
#include "YtPolygon.h"


float	GestureRenderer::vertexData[MAX_VERTICES] = {};
int		GestureRenderer::vertexCount;


// TODO: determine if inline functions can be put in .cpp files
//--------------------------------------------------------------
void GestureRenderer::vertex(float x, float y) {
	int vc2 = vertexCount*2;
	vertexData[vc2    ] = x;
	vertexData[vc2 + 1] = y;
	vertexCount++;
}

//--------------------------------------------------------------
void GestureRenderer::vertex(float x, float y, float dx, float dy) {
	int vc2 = vertexCount*2;
	vertexData[vc2    ] = x+dx;
	vertexData[vc2 + 1] = y+dy;
	vertexCount++;
}

//--------------------------------------------------------------
void GestureRenderer::vertexFp (float *xpoints, float *ypoints, int which){
	int vc2 = vertexCount*2;
	vertexData[vc2  ] = xpoints[which];
	vertexData[vc2+1] = ypoints[which];
	vertexCount++;
}

//--------------------------------------------------------------
void GestureRenderer::vertexFp (float *xpoints, float *ypoints, int which, float dx, float dy){
	int vc2 = vertexCount*2;
	vertexData[vc2  ] = xpoints[which] +dx;
	vertexData[vc2+1] = ypoints[which] +dy;
	vertexCount++;
}

//--------------------------------------------------------------
void GestureRenderer::vertexP (YtPolygon *p, int which) {
	int vc2 = vertexCount*2;
	vertexData[vc2  ] = p->xpoints[which];
	vertexData[vc2+1] = p->ypoints[which];
	vertexCount++;
}
//--------------------------------------------------------------
void GestureRenderer::vertexP (YtPolygon *p, int which, float dx, float dy) {
	float x = p->xpoints[which];
	float y = p->ypoints[which];
	vertexData[vertexCount*2    ] = x+dx;
	vertexData[vertexCount*2 + 1] = y+dy;
	vertexCount++;
}

void GestureRenderer::renderVertices(GLenum renderMode) {
	glVertexPointer(2, GL_FLOAT, 0, vertexData);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(renderMode, 0, vertexCount);
	
}


