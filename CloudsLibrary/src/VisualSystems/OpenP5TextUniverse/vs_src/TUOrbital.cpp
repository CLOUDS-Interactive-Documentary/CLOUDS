//
//  TUOrbital.cpp
//  OpenP5TextUniverse
//
//  Created by Elie Zananiri on 2013-10-29.
//
//

#include "TUOrbital.h"

float TUOrbital::focusX;
float TUOrbital::focusY;
float TUOrbital::focusZ;

ofxFTGLSimpleLayout TUOrbital::font;

//--------------------------------------------------------------
TUOrbital::TUOrbital(float size, float radius)
{
    text = "";
    
    this->size = size;
    this->radius = radius;
    
    bSelected = bClicked = bRenderText = true;
}

//--------------------------------------------------------------
TUOrbital::TUOrbital(TUOrbital& parent, string text)
{
    
    this->text = text;
    
    this->size = parent.size / 3 * 2;
    this->radius = parent.radius / 2;
    
    bSelected = bClicked = bRenderText = true;
}

//--------------------------------------------------------------
void TUOrbital::update(float x, float y, float z, bool bSelected)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;
    
    if (bSelected) {
        this->bSelected = true;
    }
    
    // calculating sphere distribution
    
    // constants
    float dphi = PI * (3 - sqrtf(5));
    float phi = 0;
    float dz = 2.0 / float(children.size());
    float zz = 1 - dz / 2.0;
    
    // child downscaling
    float cRadius = radius / 3;
    
    // determining positions children
    for (int i = 0; i < children.size(); i++) {
        float r = cRadius * sqrtf(1 - zz * zz);
        
        float childX = r * cosf(phi);
        float childY = r * sinf(phi);
        float childZ = cRadius * zz;
        
        zz = zz - dz;
        phi = phi + dphi;
        
        // recursive update
        children[i].update(childX, childY, childZ, bSelected);
    }
}

//--------------------------------------------------------------
void TUOrbital::draw(ofCamera& cam, bool bMouseDragged)
{
    ofPushMatrix();
    {
        ofTranslate(pos);
        
        ofSetColor(255);
        
        for (int i = 0; i < children.size(); i++) {
            ofLine(ofVec3f::zero(), children[i].pos);
            
            // recursive draw
            children[i].draw(cam, bMouseDragged);
        }
        
        // orient to camera position
        ofVec3f rotations = cam.getOrientationEuler();
        ofRotateX(rotations.x);
        ofRotateY(rotations.y);
        ofRotateZ(rotations.z);
        ofScale(1, -1, 1);
        
        if (bRenderText && bClicked) {
            font.drawString(text, size, 0);
        }
        
        ofRect(-size / 2.0f, -size / 2.0f, size, size);
        
        if (!bMouseDragged) {
            if (isMouseover(cam)) {
                
                if (ofGetMousePressed()) {
                    bClicked = true;
                    bSelected = true;
                    
                    GLfloat modelview[16];
                    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
                    focusX = modelview[12];
                    focusY = modelview[13];
                    focusZ = modelview[14];
                }
            }
            else {
                //bSelected = false;
            }
            
            if (ofGetKeyPressed(OF_KEY_RETURN)) {
                bClicked = false;
                bSelected = false;
            }
        }
    }
    ofPopMatrix();
    
}

//--------------------------------------------------------------
bool TUOrbital::isMouseover(ofCamera& cam)
{
    ofVec2f topLeft = cam.worldToScreen(ofVec3f(-size / 2.0f, -size / 2.0f, 0));
    ofVec2f bottomRight = cam.worldToScreen(ofVec3f(size / 2.0f, size / 2.0f, 0));
    
    if (ofGetMouseX() > topLeft.x && ofGetMouseX() < bottomRight.x) {
        if (ofGetMouseY() > topLeft.y && ofGetMouseY() < bottomRight.y) {
            return true;
        }
    }
    return false;
}
