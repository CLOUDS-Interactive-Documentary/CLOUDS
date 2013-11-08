//
//  TUOrbital.cpp
//  OpenP5TextUniverse
//
//  Created by Elie Zananiri on 2013-10-29.
//
//

#include "TUOrbital.h"
#include "ofxBillboard.h"

int TUOrbital::billboardType = 0;

ofxFTGLSimpleLayout TUOrbital::font;
ofxFTGLTextAlignment TUOrbital::textAlign = FTGL_ALIGN_LEFT;
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
    this->size = size;
    this->radius = radius;
    
    text = "";
    bRenderText = true;
    
    parent = NULL;
}

//--------------------------------------------------------------
TUOrbital::TUOrbital(TUOrbital * parent, string& text)
{
    this->size = parent->size / 3 * 2;
    this->radius = parent->radius / 2;
    
    this->text = text;
    bRenderText = true;
    
    this->parent = parent;
}

//--------------------------------------------------------------
TUOrbital::~TUOrbital()
{
    for (int i = 0; i < children.size(); i++) {
        delete children[i];
    }
    children.clear();
    
    parent = NULL;
}

//--------------------------------------------------------------
void TUOrbital::update(float x, float y, float z)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;
    
    // Calculate spherical distribution for children.
    float cRadius = radius / 3;
    float dphi = PI * (3 - sqrtf(5));
    float phi = 0;
    float dz = 2.0 / float(children.size());
    float zz = 1 - dz / 2.0;
    
    // Position each child on the sphere.
    for (int i = 0; i < children.size(); i++) {
        float r = cRadius * sqrtf(1 - zz * zz);
        
        float childX = pos.x + r * cosf(phi);
        float childY = pos.y + r * sinf(phi);
        float childZ = pos.z + cRadius * zz;
        
        zz = zz - dz;
        phi = phi + dphi;
        
        // Update recursively.
        children[i]->update(childX, childY, childZ);
    }
}

//--------------------------------------------------------------
void TUOrbital::draw(ofCamera& cam)
{
    ofPushMatrix();
    {
        for (int i = 0; i < children.size(); i++) {
            if (lineWidth > 0 && lineColor.a > 0) {
                ofSetColor(lineColor);
                ofLine(pos, children[i]->pos);
            }
            
            // Draw Recursively.
            children[i]->draw(cam);
        }
        
        if (billboardType == 0) {  // SCREEN
            ofxBillboardBeginSphericalCheat(pos);
        }
        else if (billboardType == 1 && parent != NULL) {  // NODES
            ofxBillboardBeginSphericalObvious(parent->pos, pos);
        }
        else {  // ORIGIN
            ofxBillboardBeginSphericalObvious(ofVec3f::zero(), pos);
        }
    
        ofScale(1, -1, 1);
        
        if (bRenderText && textColor.a > 0) {
            ofSetColor(textColor);
            if (bAllCaps) {
                font.drawString(ofToUpper(text), (size * nodeScalar), 0);
            float x;
            if (font.getAlignment() == FTGL_ALIGN_RIGHT) {
                x = -1 * (size * nodeScalar + font.getLineLength());
            }
            else if (font.getAlignment() == FTGL_ALIGN_CENTER) {
                x = -0.5 * font.getLineLength();
            }
            else {
                x = size * nodeScalar;
            }
            float y = (size * nodeScalar) * 0.5;
            font.drawString((bAllCaps? ofToUpper(text):text), x, y);
        }
        
        if (nodeColor.a > 0) {
            ofSetColor(nodeColor);
            ofRect(-(size * nodeScalar) / 2.0f, -(size * nodeScalar) / 2.0f, (size * nodeScalar), (size * nodeScalar));
        }
        
        ofxBillboardEnd();
    }
    ofPopMatrix();
}
