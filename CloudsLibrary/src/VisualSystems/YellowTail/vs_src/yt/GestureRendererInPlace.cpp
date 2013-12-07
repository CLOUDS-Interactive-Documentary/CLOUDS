/*
 *  GestureRendererInPlace.cpp
 *  yellowtail
 *
 *  Created by Max Hawkins on 12/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "GestureRendererInPlace.h"




void GestureRendererInPlace::renderGesture(Gesture *gesture, int maxWidth, int maxHeight) {
    ofColor fillColor = gesture->getFillColor();
    ofColor strokeColor = gesture->getStrokeColor();
    
    glColor4ub(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    renderFill(gesture, maxWidth, maxHeight);
    
    glColor4ub(strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a);
    renderOutline(gesture, maxWidth, maxHeight);
}

void GestureRendererInPlace::renderFill(Gesture *gesture, int maxWidth, int maxHeight){  
	vertexCount = 0;
    int nP = gesture->polygonCount;
    int nPm1 = nP-1;
    
    if (nP > 0){
        YtPolygon *p;
        float *xp;
        float *yp;
        
        float xp0,yp0;
        float xp1,yp1;
        float xp2,yp2;
        float xp3,yp3;
        float xp4,yp4;
        float xp5,yp5;
        float xp6,yp6;
        float xp7,yp7;
        
        float *vp;
        for (int i=0; i<nPm1; i++){
            p = &(gesture->polygons[i]);
            xp = p->xpoints;
            yp = p->ypoints;
            
            xp0 = xp[0]; yp0 = yp[0];
            xp1 = xp[1]; yp1 = yp[1];
            xp2 = xp[2]; yp2 = yp[2];
            xp3 = xp[3]; yp3 = yp[3];
            xp4 = xp[4]; yp4 = yp[4];
            xp5 = xp[5]; yp5 = yp[5];
            xp6 = xp[6]; yp6 = yp[6];
            xp7 = xp[7]; yp7 = yp[7];
            
            vp = &vertexData[vertexCount*2];
            vertexCount += 18;
            
            *vp	= xp0;	*(++vp) = yp0;
            *(++vp)	= xp1;	*(++vp) = yp1;
            *(++vp)	= xp5;	*(++vp) = yp5;
            
            *(++vp)	= xp5;	*(++vp)	= yp5; 
            *(++vp)	= xp4;	*(++vp)	= yp4; 
            *(++vp)	= xp0;	*(++vp)	= yp0; 
            
            *(++vp)	= xp4;	*(++vp)	= yp4; 
            *(++vp)	= xp5;	*(++vp)	= yp5; 
            *(++vp)	= xp6;	*(++vp)	= yp6;
            
            *(++vp)	= xp6;	*(++vp)	= yp6; 
            *(++vp)	= xp7;	*(++vp)	= yp7; 
            *(++vp)	= xp4;	*(++vp)	= yp4; 
            
            *(++vp)	= xp7;	*(++vp)	= yp7; 
            *(++vp)	= xp6;	*(++vp)	= yp6; 
            *(++vp)	= xp2;	*(++vp)	= yp2; 
            
            *(++vp)	= xp2;	*(++vp)	= yp2; 
            *(++vp)	= xp3;	*(++vp)	= yp3; 
            *(++vp)	= xp7;	*(++vp)	= yp7;
        }
        
        renderVertices(GL_TRIANGLES);
    }
}

//--------------------------------------------------------------
void GestureRendererInPlace::renderOutline(Gesture *gesture, int maxWidth, int maxHeight) {    
	vertexCount = 0;
    
    int nP = gesture->polygonCount;
    int nPm1 = nP -1;
    if (nP > 0){
        
        YtPolygon *p;
        float *xp;
        float *yp;
        
        
        float xp0,yp0;
        float xp1,yp1;
        float xp2,yp2;
        float xp3,yp3;
        float xp4,yp4;
        float xp5,yp5;
        float xp6,yp6;
        float xp7,yp7;

        for (int i=0; i<nPm1; i++){
            p = &(gesture->polygons[i]);
            
            xp = p->xpoints;
            yp = p->ypoints;
            
            xp0 = xp[0]; yp0 = yp[0];
            xp1 = xp[1]; yp1 = yp[1];
            xp2 = xp[2]; yp2 = yp[2];
            xp3 = xp[3]; yp3 = yp[3];
            xp4 = xp[4]; yp4 = yp[4];
            xp5 = xp[5]; yp5 = yp[5];
            xp6 = xp[6]; yp6 = yp[6];
            xp7 = xp[7]; yp7 = yp[7];
            
            vertex(xp0,yp0); vertex(xp4,yp4); 
            vertex(xp4,yp4); vertex(xp7,yp7); 
            vertex(xp7,yp7); vertex(xp3,yp3);
            vertex(xp1,yp1); vertex(xp5,yp5); 
            vertex(xp5,yp5); vertex(xp6,yp6); 
            vertex(xp6,yp6); vertex(xp2,yp2); 
        }
        
        p = &(gesture->polygons[nPm1]);
        xp = p->xpoints;
        yp = p->ypoints;
        vertexFp(xp,yp,0); 
        vertexFp(xp,yp,3);
        vertexFp(xp,yp,1); 
        vertexFp(xp,yp,2);
        
        renderVertices(GL_LINES);
    }
}
