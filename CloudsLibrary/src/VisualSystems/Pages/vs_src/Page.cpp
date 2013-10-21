//
//  Page.cpp
//  FloatingPage
//
//  Created by Elie Zananiri on 2012-11-01.
//
//

#include "Page.h"

#include "ofBitmapFont.h"

float fillAlpha = 1.0;
float meshAlpha = 0.0;
float offsetAmount = 0.0;
float alignAmount = 0.0;
float twirlAmount = 0.1;
float tiltAmount = 0.0;
float flipAmount = 0.0;
float swayAmount = 1.0;
bool bendTail = false;
bool bendWings = true;
bool bendFresh = false;
float topBendAmount = 0.0;
float bottomBendAmount = 0.5;
float tornadoAmount = 0.0;
float speedAmount = 0.0;
float expandAmount = 1.0;
float liftAmount = 1.0;

float Page::sharedFlipAngle = 0;
int Page::sharedLastFlipFrame = 0;

//--------------------------------------------------------------
Page::Page()
{
    animateOffset = ofRandom(50);

    alignAngle = ofRandom(M_TWO_PI);

    bGoingUp = true;
    startPos.set(0, 20, 0);
    pos.set(startPos);
    posInc = 0;
    posSpeed = 0.01;
    rainSpeed.set(0, -5, 0);
    tornadoAngle = ofRandom(M_TWO_PI);

    twirlAngle = 0;
    tiltAngle = 0;
    
    localFlipAngle = 0;
    flipAngle = 0;
    flipSpeed = 0;

    vertAngle = 0;
    vertOffsetY = 0;
    vertBendPct = 0;

    topBendPct = 0;
    bottomBendPct = 0;

    swayInc = 0;
    swayAngle = -M_PI/2;
    swaySpeed = 0.05;
    swayRadius = 50;

    rebuild();
}

//--------------------------------------------------------------
void Page::rebuild()
{
    path.clear();

    path.moveTo(ofPoint(-pageSize, 0, -pageSize));
    path.lineTo(ofPoint(-pageSize, 0,  pageSize));
    path.lineTo(ofPoint( pageSize, 0,  pageSize));
    path.lineTo(ofPoint( pageSize, 0, -pageSize));

    remesh(PageOverwriteNone);
}

//--------------------------------------------------------------
void Page::rebuild(float bendPct)
{
    path.clear();

    float bendY = pageSize * bendPct;
    float bendZ = ABS(pageSize * bendPct * 2);

    path.moveTo(ofPoint(-pageSize, 0, -pageSize));
    path.lineTo(ofPoint(-pageSize, 0, pageSize - bendZ));
    path.bezierTo(ofPoint(-pageSize, 0, pageSize - bendZ * 0.5), ofPoint(-pageSize, bendY * 0.5, pageSize), ofPoint(-pageSize, bendY, pageSize));
    path.lineTo(ofPoint(pageSize, bendY, pageSize));
    path.bezierTo(ofPoint(pageSize, bendY * 0.5, pageSize), ofPoint(pageSize, 0, pageSize - bendZ * 0.5), ofPoint(pageSize, 0, pageSize - bendZ));
    path.lineTo(ofPoint(pageSize, 0, -pageSize));
    path.lineTo(ofPoint(-pageSize, 0, -pageSize));

    remesh(PageOverwriteAll);
}

//--------------------------------------------------------------
void Page::rebuild(float bendTopPct, float bendBottomPct)
{
    path.clear();

    // try to keep the page area the same as it bends...

    float bendTopY = pageSize * 0.5 * bendTopPct;
    float bendTopZ = -pageSize * (1 - ABS(bendTopPct) * 0.5);
    
    float bendBottomY = pageSize * 0.5 * bendBottomPct;
    float bendBottomZ = pageSize * (1 - ABS(bendBottomPct) * 0.5);

    path.moveTo(ofPoint(-pageSize, bendTopY, bendTopZ));

    path.bezierTo(ofPoint(-pageSize, bendTopY * 0.5, bendTopZ),
                  ofPoint(-pageSize, 0, bendTopZ * 0.5),
                  ofPoint(-pageSize, 0, 0));
    path.bezierTo(ofPoint(-pageSize, 0, bendBottomZ * 0.5),
                  ofPoint(-pageSize, bendBottomY * 0.5, bendBottomZ),
                  ofPoint(-pageSize, bendBottomY, bendBottomZ));

    path.lineTo(ofPoint(pageSize, bendBottomY, bendBottomZ));

    path.bezierTo(ofPoint(pageSize, bendBottomY * 0.5, bendBottomZ),
                  ofPoint(pageSize, 0, bendBottomZ * 0.5),
                  ofPoint(pageSize, 0, 0));
    path.bezierTo(ofPoint(pageSize, 0, bendTopZ * 0.5),
                  ofPoint(pageSize, bendTopY * 0.5, bendTopZ),
                  ofPoint(pageSize, bendTopY, bendTopZ));

    remesh(PageOverwriteFirst);
}

//--------------------------------------------------------------
void Page::remesh(enum PageOverwriteMode mode)
{
    boundsMin.set(FLT_MAX, FLT_MAX, FLT_MAX);
    boundsMax.set(FLT_MIN, FLT_MIN, FLT_MIN);
    
    mesh = path.getTessellation();

    // add a bunch of dummy normals
    for (int i=0; i < mesh.getNumVertices(); i++) {
        mesh.addNormal(ofVec3f());
    }

    // calculate the real normal for each vertex
    for (int i=0; i < mesh.getNumIndices(); i += 3) {
        ofVec3f a = mesh.getVertex(mesh.getIndex(i + 0));
        ofVec3f b = mesh.getVertex(mesh.getIndex(i + 1));
        ofVec3f c = mesh.getVertex(mesh.getIndex(i + 2));

        ofVec3f ab = b - a;
        ofVec3f ac = c - a;

        ofVec3f n = ab.cross(ac).normalized();  // gotta flip it, guess i'm winding backwards...
        if ((mode == PageOverwriteAll) ||
            (i < mesh.getNumVertices() / 2 && mode == PageOverwriteFirst) ||
            (i > mesh.getNumVertices() / 2 && mode == PageOverwriteLast)) {
            // overwrite the normals
            mesh.setNormal(mesh.getIndex(i + 0), n);
            mesh.setNormal(mesh.getIndex(i + 1), n);
            mesh.setNormal(mesh.getIndex(i + 2), n);
        }
        else {
            // don't overwrite the normals
            if (mesh.getNormal(mesh.getIndex(i + 0)).length() == 0) mesh.setNormal(mesh.getIndex(i + 0), n);
            if (mesh.getNormal(mesh.getIndex(i + 1)).length() == 0) mesh.setNormal(mesh.getIndex(i + 1), n);
            if (mesh.getNormal(mesh.getIndex(i + 2)).length() == 0) mesh.setNormal(mesh.getIndex(i + 2), n);
        }
        
        // set the bounds
        boundsMin.x = MIN(boundsMin.x, MIN(a.x, MIN(b.x, c.x)));
        boundsMax.x = MAX(boundsMax.x, MAX(a.x, MAX(b.x, c.x)));
        boundsMin.y = MIN(boundsMin.y, MIN(a.y, MIN(b.y, c.y)));
        boundsMax.y = MAX(boundsMax.y, MAX(a.y, MAX(b.y, c.y)));
        boundsMin.z = MIN(boundsMin.z, MIN(a.z, MIN(b.z, c.z)));
        boundsMax.z = MAX(boundsMax.z, MAX(a.z, MAX(b.z, c.z)));
    }
}

//--------------------------------------------------------------
void Page::update()
{
    animateCounter = (ofGetElapsedTimef() * 30.0) + animateOffset * offsetAmount;

    rotInc = sin(animateCounter * 0.1);

    // twirl and tilt
    twirlAngle = rotInc * 5 * twirlAmount;
    tiltAngle = sin(rotInc * 0.01) * 100 * tiltAmount;

    // flip around, always in the same direction
    bool bFlipDecelerating = false;
    if (flipAmount > 0.05) {
        float offsetSpeed = animateOffset * .02 * offsetAmount * 0.3;
        float newSpeed = MIN(0.4, flipAmount);
        flipSpeed = MAX(flipSpeed, newSpeed + offsetSpeed);
    }
    else if (localFlipAngle != 0) {
        bFlipDecelerating = true;
        
        flipSpeed = MAX(flipSpeed * 0.9, 0.1);
        
        int currRotations = localFlipAngle / M_TWO_PI;
        int nextRotations = (localFlipAngle + flipSpeed) / M_TWO_PI;
        if (currRotations != nextRotations) {
            localFlipAngle = 0;
            flipSpeed = 0;
        }
    }
    localFlipAngle += flipSpeed;

    // flip the global value if it hasn't already been set this frame
    if (sharedLastFlipFrame < ofGetFrameNum()) {
        sharedFlipAngle = localFlipAngle;
        sharedLastFlipFrame = ofGetFrameNum();

        flipAngle = sharedFlipAngle;
    }
    else if (bFlipDecelerating) {
        // don't sync if you're decelerating
        flipAngle = localFlipAngle;
    }
    else if (localFlipAngle != 0) {
        // sync the flips
        flipAngle = ofLerp(sharedFlipAngle, localFlipAngle, offsetAmount);
    }
    else {
        flipAngle = 0;
    }

    // sway back and forth
    swayInc += swaySpeed;
    swayAngle = (sin(swayInc) * swayAmount) - M_PI / 2;
    swayPos.x = cos(swayAngle) * swayRadius;
    swayPos.y = sin(swayAngle) * swayRadius + swayRadius;
    swayPos *= swayAmount;

    if (bendTail) {
        // bend the bottom of the page
        vertBendPct = sin(animateCounter * 0.1) * bottomBendAmount;
    }
    else if (bendWings) {
        // bend more 
        topBendPct = sin(animateCounter * 0.1) * topBendAmount;
        bottomBendPct = sin(animateCounter * 0.1) * bottomBendAmount;
    }
    else if (bendFresh) {
        // funky fresh bending
        topBendPct = sin(animateCounter * 0.1) * topBendAmount;
        bottomBendPct = -rotInc * 2 * bottomBendAmount;
    }

    ofVec3f posFromCenter(pos.x, 0, pos.z);
    alignPivot = posFromCenter;

    if (tornadoAmount > 0) {
        float tornadoRadius = MIN(pos.y, groundSize);
        float distFromCenter = posFromCenter.length();
        if ((int)distFromCenter < 1) {
            // move away from the center
            pos.x += cos(tornadoAngle);
            pos.z += sin(tornadoAngle);
        }
        else {
            // tornado!
            ofPoint center(0, pos.y, 0);
            tornadoOffset = pos.getPerpendicular(center);
            tornadoOffset.normalize();
            tornadoOffset *= (pos.y * 0.1);

            if (bGoingUp) {
                // ease up the angle
                tornadoOffset.y += ofMap(pos.y, startPos.y, startPos.y + 10, tornadoAngle / 10, tornadoAngle, true);
                if (tornadoRadius >= groundSize) {
                    bGoingUp = false;
                }
            }
            else {
                // ease down the angle
                tornadoOffset.y -= ofMap(pos.y, startPos.y + 10, startPos.y, tornadoAngle, tornadoAngle / 10, true);
                tornadoOffset -= (posFromCenter * 0.01);
                if (tornadoRadius < startPos.y) {
                    bGoingUp = true;
                }
            }

            tornadoOffset.y *= liftAmount;
            pos += tornadoOffset * speedAmount;
            
            ofVec3f newPosFromCenter(pos.x, 0, pos.z);
            float newDistFromCenter = newPosFromCenter.length();
            float deltaDistFromCenter = newDistFromCenter - distFromCenter;
            newPosFromCenter.scale(distFromCenter + deltaDistFromCenter * expandAmount);
            pos.x = newPosFromCenter.x;
            pos.z = newPosFromCenter.z;
        }
    }

    if (bendTail) {
        rebuild(vertBendPct);
    }
    else if (bendWings || bendFresh) {
        rebuild(topBendPct, bottomBendPct);
    }
    else {
        rebuild();
    }
}

//--------------------------------------------------------------
void Page::draw()
{
    preDraw();
    fillDraw();
    meshDraw();
    postDraw();
}

//--------------------------------------------------------------
void Page::preDraw()
{
    ofPushMatrix();
    
    ofPoint currPos;
    currPos.x = ofLerp(startPos.x, pos.x, tornadoAmount);
    currPos.y = ofLerp(startPos.y, pos.y, tornadoAmount);
    currPos.z = ofLerp(startPos.z, pos.z, tornadoAmount);
    ofTranslate(currPos);
    
    // sway
    ofTranslate(swayPos);
    ofRotate(RAD_TO_DEG * (swayAngle + M_PI / 2), 0, 0, 1);
    
    // flip
    ofRotate(RAD_TO_DEG * flipAngle, 0, 0, 1);
    
    // vert bend
    ofTranslate(0, vertOffsetY);
    ofRotate(vertAngle, 1, 0, 0);
    
    // twirl
    ofRotate(RAD_TO_DEG * twirlAngle, 0, 1, 0);
    
    // tilt
    ofRotate(RAD_TO_DEG * tiltAngle, 1, 0, 1);
    
    // align
    ofRotate(RAD_TO_DEG * alignAngle * alignAmount, alignPivot.x, alignPivot.y, alignPivot.z);
}

//--------------------------------------------------------------
void Page::fillDraw()
{
    if (fillAlpha > 0.0) {
        ofSetColor(255, fillAlpha * 255);
        mesh.draw();
    }
}

//--------------------------------------------------------------
void Page::meshDraw()
{
    if (meshAlpha > 0.0) {
        float scaledAlpha = meshAlpha * 255;
        
        // mesh
        ofSetColor(128, scaledAlpha);
        mesh.drawWireframe();
        
        // normals
        for (int i=0; i < mesh.getNumIndices(); i++) {
            ofVec3f coord = mesh.getVertex(mesh.getIndex(i));
            ofVec3f norm = mesh.getNormal(mesh.getIndex(i));
            
            ofSetColor(255, 0, 255, scaledAlpha);
            ofLine(coord, coord + (norm * 5));
            ofSetColor(255, 255, 0, scaledAlpha);
            ofPushMatrix();
            ofTranslate(coord);
            ofRotate(90, 0, 1, 0);
            ofCircle(0, 0, 0, 0.1);
            ofPopMatrix();
        }
        
        // bounding box
        ofSetColor(0, 255, 255, scaledAlpha);
        ofLine(boundsMin.x, boundsMin.y, boundsMin.z, boundsMin.x, boundsMin.y, boundsMax.z);
        ofLine(boundsMin.x, boundsMin.y, boundsMax.z, boundsMax.x, boundsMin.y, boundsMax.z);
        ofLine(boundsMax.x, boundsMin.y, boundsMax.z, boundsMax.x, boundsMax.y, boundsMax.z);
        ofLine(boundsMax.x, boundsMax.y, boundsMax.z, boundsMax.x, boundsMax.y, boundsMin.z);
        ofLine(boundsMax.x, boundsMax.y, boundsMin.z, boundsMin.x, boundsMax.y, boundsMin.z);
        ofLine(boundsMin.x, boundsMax.y, boundsMin.z, boundsMin.x, boundsMin.y, boundsMin.z);
        ofLine(boundsMin.x, boundsMin.y, boundsMin.z, boundsMax.x, boundsMin.y, boundsMin.z);
        ofLine(boundsMax.x, boundsMin.y, boundsMin.z, boundsMax.x, boundsMin.y, boundsMax.z);
        ofLine(boundsMax.x, boundsMax.y, boundsMin.z, boundsMax.x, boundsMin.y, boundsMin.z);
        ofLine(boundsMin.x, boundsMax.y, boundsMax.z, boundsMin.x, boundsMin.y, boundsMax.z);
        ofLine(boundsMin.x, boundsMax.y, boundsMin.z, boundsMin.x, boundsMax.y, boundsMax.z);
        ofLine(boundsMin.x, boundsMax.y, boundsMax.z, boundsMax.x, boundsMax.y, boundsMax.z);
        
        // min and max coords
        ofSetColor(255, scaledAlpha);
        stringDraw("(" + ofToString(boundsMin.x, 3) + ", " + ofToString(boundsMin.y, 3) + ", " + ofToString(boundsMin.z, 3) + ")", boundsMin.x, boundsMin.y, boundsMin.z);
        stringDraw("(" + ofToString(boundsMax.x, 3) + ", " + ofToString(boundsMin.y, 3) + ", " + ofToString(boundsMin.z, 3) + ")", boundsMax.x, boundsMin.y, boundsMin.z);
        stringDraw("(" + ofToString(boundsMin.x, 3) + ", " + ofToString(boundsMax.y, 3) + ", " + ofToString(boundsMin.z, 3) + ")", boundsMin.x, boundsMax.y, boundsMin.z);
        stringDraw("(" + ofToString(boundsMin.x, 3) + ", " + ofToString(boundsMin.y, 3) + ", " + ofToString(boundsMax.z, 3) + ")", boundsMin.x, boundsMin.y, boundsMax.z);
        stringDraw("(" + ofToString(boundsMax.x, 3) + ", " + ofToString(boundsMax.y, 3) + ", " + ofToString(boundsMin.z, 3) + ")", boundsMax.x, boundsMax.y, boundsMin.z);
        stringDraw("(" + ofToString(boundsMax.x, 3) + ", " + ofToString(boundsMin.y, 3) + ", " + ofToString(boundsMax.z, 3) + ")", boundsMax.x, boundsMin.y, boundsMax.z);
        stringDraw("(" + ofToString(boundsMin.x, 3) + ", " + ofToString(boundsMax.y, 3) + ", " + ofToString(boundsMax.z, 3) + ")", boundsMin.x, boundsMax.y, boundsMax.z);
        stringDraw("(" + ofToString(boundsMax.x, 3) + ", " + ofToString(boundsMax.y, 3) + ", " + ofToString(boundsMax.z, 3) + ")", boundsMax.x, boundsMax.y, boundsMax.z);
        
        // axis
        ofSetColor(255, 0, 0, scaledAlpha);
        ofLine(0, 0, 0, 10, 0, 0);
        stringDraw("x", 11, 0, 0);
        ofSetColor(0, 255, 0, scaledAlpha);
        ofLine(0, 0, 0, 0, 10, 0);
        stringDraw("y", 0, 11, 0);
        ofSetColor(0, 0, 255, scaledAlpha);
        ofLine(0, 0, 0, 0, 0, 10);
        stringDraw("z", 0, 0, 11);
    }
}

//--------------------------------------------------------------
// Adapted from ofGLRenderer::drawString(string textString, float x, float y, float z, ofDrawBitmapMode mode)
void Page::stringDraw(string text, float x, float y, float z)
{
	// this is copied from the ofTrueTypeFont
	//GLboolean blend_enabled = glIsEnabled(GL_BLEND); //TODO: this is not used?
	GLint blend_src, blend_dst;
	glGetIntegerv( GL_BLEND_SRC, &blend_src );
	glGetIntegerv( GL_BLEND_DST, &blend_dst );
    
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
	int len = (int)text.length();
	//float yOffset = 0;
	float fontSize = 8.0f;
	bool bOrigin = false;
    
	float sx = 0;
	float sy = -fontSize;
    
	///////////////////////////
	// APPLY TRANSFORM / VIEW
	///////////////////////////
	//
    
	bool hasModelView = false;
	bool hasProjection = false;
	bool hasViewport = false;
    
	ofRectangle rViewport;
	
    //our aim here is to draw to screen
    //at the viewport position related
    //to the world position x,y,z
    
    //gluProject method
    GLdouble modelview[16], projection[16];
    GLint view[4];
    double dScreenX, dScreenY, dScreenZ;
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, view);
    view[0] = 0; view[1] = 0; //we're already drawing within viewport
    gluProject(x, y, z, modelview, projection, view, &dScreenX, &dScreenY, &dScreenZ);
    
    if (dScreenZ >= 1)
        return;
    
    rViewport = ofGetCurrentViewport();
    
    hasProjection = true;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    hasModelView = true;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glTranslatef(-1, -1, 0);
    glScalef(2/rViewport.width, 2/rViewport.height, 1);
    
    glTranslatef(dScreenX, dScreenY, 0);
    
    // EZ: Make sure to flip here.
    glScalef(1, -1, 1);
    
	// (c) enable texture once before we start drawing each char (no point turning it on and off constantly)
	//We do this because its way faster
	ofDrawBitmapCharacterStart(text.size());
    
	for(int c = 0; c < len; c++){
		if(text[c] == '\n'){
            
			sy += bOrigin ? -1 : 1 * (fontSize*1.7);
			sx = 0;
            
			//glRasterPos2f(x,y + (int)yOffset);
		} else if (text[c] >= 32){
			// < 32 = control characters - don't draw
			// solves a bug with control characters
			// getting drawn when they ought to not be
			ofDrawBitmapCharacter(text[c], (int)sx, (int)sy);
            
			sx += fontSize;
		}
	}
	//We do this because its way faster
	ofDrawBitmapCharacterEnd();
    
    
	if (hasModelView)
		glPopMatrix();
    
	if (hasProjection)
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
    
	if (hasViewport)
		ofPopView();
    
	glBlendFunc(blend_src, blend_dst);
}

//--------------------------------------------------------------
void Page::postDraw()
{
    ofPopMatrix();
}
