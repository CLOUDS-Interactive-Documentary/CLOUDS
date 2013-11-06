//
//  TUOrbital.cpp
//  OpenP5TextUniverse
//
//  Created by Elie Zananiri on 2013-10-29.
//
//

#include "TUOrbital.h"

ofxFTGLSimpleLayout TUOrbital::font;
string TUOrbital::fontName = "Helvetica.ttf";
float TUOrbital::lineLength = 150.0f;
float TUOrbital::fontSize = 14;
float TUOrbital::fontDepth = 0;
bool TUOrbital::bAllCaps = false;
ofColor TUOrbital::textColor(255);

ofColor TUOrbital::lineColor(255);
float TUOrbital::lineWidth = 1.0f;

ofColor TUOrbital::nodeColor(255);
float TUOrbital::nodeScalar = 1.0f;

//--------------------------------------------------------------
TUOrbital::TUOrbital(float size, float radius)
{
    text = "";
    
    this->size = size;
    this->radius = radius;
    
    bRenderText = true;
}

//--------------------------------------------------------------
TUOrbital::TUOrbital(TUOrbital& parent, string text)
{
    
    this->text = text;
    
    this->size = parent.size / 3 * 2;
    this->radius = parent.radius / 2;
    
    bRenderText = true;
}

//--------------------------------------------------------------
void TUOrbital::update(float x, float y, float z)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;
    
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
        children[i].update(childX, childY, childZ);
    }
}

//--------------------------------------------------------------
void TUOrbital::draw(ofCamera& cam)
{
    ofPushMatrix();
    {
        ofTranslate(pos);
        
        for (int i = 0; i < children.size(); i++) {
            if (lineWidth > 0) {
                ofSetColor(lineColor);
                ofLine(ofVec3f::zero(), children[i].pos);
            }
            
            // recursive draw
            children[i].draw(cam);
        }
        
        billboard();
        ofScale(1, -1, 1);
        
        if (bRenderText) {
            ofSetColor(textColor);
            if (bAllCaps) {
                font.drawString(ofToUpper(text), (size * nodeScalar), 0);
            }
            else {
                font.drawString(text, (size * nodeScalar), 0);
            }
        }
        
        ofSetColor(nodeColor);
        ofRect(-(size * nodeScalar) / 2.0f, -(size * nodeScalar) / 2.0f, (size * nodeScalar), (size * nodeScalar));
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void TUOrbital::billboard()
{
	// Get the current modelview matrix.
	float modelview[16];
	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
    
	// Undo all rotations.
	// Beware all scaling is lost as well.
	for (int i = 0; i < 3; i++) {
		for (int j=0; j < 3; j++) {
			if (i == j) {
				modelview[i * 4 + j] = 1.0;
            }
			else {
				modelview[i * 4 + j] = 0.0;
            }
		}
    }
    
	// Set the modelview with no rotations.
	glLoadMatrixf(modelview);
}
