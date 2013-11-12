/*
 *  GestureRendererTraveling.mm
 *  yellowtail
 *
 *  Created by Max Hawkins on 12/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "GestureRendererTraveling.h"


void GestureRendererTraveling::renderGesture(Gesture *gesture, int maxWidth, int maxHeight) {
    ofColor fillColor = gesture->getFillColor();
    ofColor strokeColor = gesture->getStrokeColor();
    
    glColor4ub(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    renderFill(gesture, maxWidth, maxHeight);
    
    glColor4ub(strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a);
    renderOutline(gesture, maxWidth, maxHeight);
}

void GestureRendererTraveling::renderFill(Gesture *gesture, int maxWidth, int maxHeight){
    int w = maxWidth;
    int h = maxHeight;
    
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
    
        if (!gesture->hasCross){
            
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
                
                vertex(xp0,yp0); vertex(xp1,yp1); vertex(xp5,yp5);
                vertex(xp5,yp5); vertex(xp4,yp4); vertex(xp0,yp0);
                vertex(xp4,yp4); vertex(xp5,yp5); vertex(xp6,yp6);
                vertex(xp6,yp6); vertex(xp7,yp7); vertex(xp4,yp4);
                vertex(xp7,yp7); vertex(xp6,yp6); vertex(xp2,yp2);
                vertex(xp2,yp2); vertex(xp3,yp3); vertex(xp7,yp7);
            }
            
        } else {
            
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
                
                vertex(xp0,yp0); vertex(xp1,yp1); vertex(xp5,yp5);
                vertex(xp5,yp5); vertex(xp4,yp4); vertex(xp0,yp0);
                vertex(xp4,yp4); vertex(xp5,yp5); vertex(xp6,yp6);
                vertex(xp6,yp6); vertex(xp7,yp7); vertex(xp4,yp4);
                vertex(xp7,yp7); vertex(xp6,yp6); vertex(xp2,yp2);
                vertex(xp2,yp2); vertex(xp3,yp3); vertex(xp7,yp7);
                
                CrossLocation cross = gesture->crosses[i];
                if ((cross & CrossLocationLeft) || (cross & CrossLocationRight)){
                    vertex(xp0+w,yp0); vertex(xp1+w,yp1); vertex(xp5+w,yp5);
                    vertex(xp5+w,yp5); vertex(xp4+w,yp4); vertex(xp0+w,yp0);
                    vertex(xp4+w,yp4); vertex(xp5+w,yp5); vertex(xp6+w,yp6);
                    vertex(xp6+w,yp6); vertex(xp7+w,yp7); vertex(xp4+w,yp4);
                    vertex(xp7+w,yp7); vertex(xp6+w,yp6); vertex(xp2+w,yp2);
                    vertex(xp2+w,yp2); vertex(xp3+w,yp3); vertex(xp7+w,yp7);
                    
                    vertex(xp0-w,yp0); vertex(xp1-w,yp1); vertex(xp5-w,yp5);
                    vertex(xp5-w,yp5); vertex(xp4-w,yp4); vertex(xp0-w,yp0);
                    vertex(xp4-w,yp4); vertex(xp5-w,yp5); vertex(xp6-w,yp6);
                    vertex(xp6-w,yp6); vertex(xp7-w,yp7); vertex(xp4-w,yp4);
                    vertex(xp7-w,yp7); vertex(xp6-w,yp6); vertex(xp2-w,yp2);
                    vertex(xp2-w,yp2); vertex(xp3-w,yp3); vertex(xp7-w,yp7);
                }
                if ((cross & CrossLocationTop) || (cross & CrossLocationBottom)){
                    vertex(xp0,yp0+h); vertex(xp1,yp1+h); vertex(xp5,yp5+h);
                    vertex(xp5,yp5+h); vertex(xp4,yp4+h); vertex(xp0,yp0+h);
                    vertex(xp4,yp4+h); vertex(xp5,yp5+h); vertex(xp6,yp6+h);
                    vertex(xp6,yp6+h); vertex(xp7,yp7+h); vertex(xp4,yp4+h);
                    vertex(xp7,yp7+h); vertex(xp6,yp6+h); vertex(xp2,yp2+h);
                    vertex(xp2,yp2+h); vertex(xp3,yp3+h); vertex(xp7,yp7+h);
                    
                    vertex(xp0,yp0-h); vertex(xp1,yp1-h); vertex(xp5,yp5-h);
                    vertex(xp5,yp5-h); vertex(xp4,yp4-h); vertex(xp0,yp0-h);
                    vertex(xp4,yp4-h); vertex(xp5,yp5-h); vertex(xp6,yp6-h);
                    vertex(xp6,yp6-h); vertex(xp7,yp7-h); vertex(xp4,yp4-h);
                    vertex(xp7,yp7-h); vertex(xp6,yp6-h); vertex(xp2,yp2-h);
                    vertex(xp2,yp2-h); vertex(xp3,yp3-h); vertex(xp7,yp7-h);
                }
                
            }
        }

        renderVertices(GL_TRIANGLES);
    }
}

//--------------------------------------------------------------
void GestureRendererTraveling::renderOutline(Gesture *gesture, int maxWidth, int maxHeight) {
    int w = maxWidth;
    int h = maxHeight;
    
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
        
        if (!gesture->hasCross){
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
            // last point. gesture is the also the generic case, without the bez midpoint,.
            {
                p = &(gesture->polygons[nPm1]);
                xp = p->xpoints;
                yp = p->ypoints;
                vertexFp(xp,yp,0); 
                vertexFp(xp,yp,3);
                vertexFp(xp,yp,1); 
                vertexFp(xp,yp,2);
            }
            
        } else {
            
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
                
                CrossLocation cross = gesture->crosses[i];
                if (cross != CrossLocationNone){
                    if ((cross & CrossLocationLeft) || (cross & CrossLocationRight)){
                        vertex(xp0+w,yp0); vertex(xp4+w,yp4); 
                        vertex(xp4+w,yp4); vertex(xp7+w,yp7); 
                        vertex(xp7+w,yp7); vertex(xp3+w,yp3);
                        vertex(xp1+w,yp1); vertex(xp5+w,yp5); 
                        vertex(xp5+w,yp5); vertex(xp6+w,yp6); 
                        vertex(xp6+w,yp6); vertex(xp2+w,yp2); 
                        
                        vertex(xp0-w,yp0); vertex(xp4-w,yp4); 
                        vertex(xp4-w,yp4); vertex(xp7-w,yp7); 
                        vertex(xp7-w,yp7); vertex(xp3-w,yp3);
                        vertex(xp1-w,yp1); vertex(xp5-w,yp5); 
                        vertex(xp5-w,yp5); vertex(xp6-w,yp6); 
                        vertex(xp6-w,yp6); vertex(xp2-w,yp2); 
                    }
                    if ((cross & CrossLocationTop) || (cross & CrossLocationBottom)){
                        vertex(xp0,yp0+h); vertex(xp4,yp4+h); 
                        vertex(xp4,yp4+h); vertex(xp7,yp7+h); 
                        vertex(xp7,yp7+h); vertex(xp3,yp3+h);
                        vertex(xp1,yp1+h); vertex(xp5,yp5+h); 
                        vertex(xp5,yp5+h); vertex(xp6,yp6+h); 
                        vertex(xp6,yp6+h); vertex(xp2,yp2+h); 
                        
                        vertex(xp0,yp0-h); vertex(xp4,yp4-h); 
                        vertex(xp4,yp4-h); vertex(xp7,yp7-h); 
                        vertex(xp7,yp7-h); vertex(xp3,yp3-h);
                        vertex(xp1,yp1-h); vertex(xp5,yp5-h); 
                        vertex(xp5,yp5-h); vertex(xp6,yp6-h); 
                        vertex(xp6,yp6-h); vertex(xp2,yp2-h); 
                    }
                }
            }
            {
                // final point
                p = &(gesture->polygons[nPm1]);
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
                
                vertex(xp0,yp0); 
                vertex(xp3,yp3);
                vertex(xp1,yp1); 
                vertex(xp2,yp2);
                
                CrossLocation cross = (gesture->crosses[nPm1]);
                if (cross != CrossLocationNone){
                    if ((cross & CrossLocationLeft) || (cross & CrossLocationRight)){
                        vertex(xp0+w,yp0); 
                        vertex(xp3+w,yp3);
                        vertex(xp1+w,yp1); 
                        vertex(xp2+w,yp2);
                        
                        vertex(xp0-w,yp0); 
                        vertex(xp3-w,yp3);
                        vertex(xp1-w,yp1); 
                        vertex(xp2-w,yp2);
                    }
                    if ((cross & CrossLocationTop) || (cross & CrossLocationBottom)){
                        vertex(xp0,yp0+h); 
                        vertex(xp3,yp3+h);
                        vertex(xp1,yp1+h); 
                        vertex(xp2,yp2+h);
                        
                        vertex(xp0,yp0-h); 
                        vertex(xp3,yp3-h);
                        vertex(xp1,yp1-h); 
                        vertex(xp2,yp2-h);
                    }
                }
            }
            
        }
        
        renderVertices(GL_LINES);
    }
}
