//
//  HistoryNode.cpp
//  EmptyVisualSystem
//
//  Created by Elie Zananiri on 2013-07-28.
//
//

#include "HistoryNode.h"

//--------------------------------------------------------------
ofTrueTypeFont HistoryNode::font;
ofColor HistoryNode::textColor(255);
float HistoryNode::textAlignX = 0;
float HistoryNode::textAlignY = 0;

ofColor HistoryNode::lineColor(128);

ofColor HistoryNode::nodeColor(128);
float HistoryNode::nodeRadius = 5;

float HistoryNode::minZ = -1000;
float HistoryNode::maxZ = 0;
float HistoryNode::minAlpha = 0;

float HistoryNode::lineWidth = 10;
float HistoryNode::levelDepth = 100;
float HistoryNode::noiseStep = 0.02;
float HistoryNode::noiseAmount = 10;

//--------------------------------------------------------------
HistoryNode::HistoryNode(const string& segment, int timestamp, int level, list<string>& childSegments, float latitude, float longitude)
: _name(segment)
, _timestamp(timestamp)
, _level(level)
, _latitude(latitude)
, _longitude(longitude)
{
    _textWidth = font.stringWidth(_name);
    _textHeight = font.stringHeight("a");  // ignore ascent/descent
        
    addChild(childSegments, level + 1);
}

//--------------------------------------------------------------
void HistoryNode::addChild(list<string>& segments, int childLevel)
{
    if (segments.size() > 0) {
        string nextSegment = segments.front();
        segments.pop_front();
        try {
            HistoryNode * nextChild = _children.at(nextSegment);
            nextChild->addChild(segments, childLevel);
        }
        catch (const std::out_of_range& e) {
            HistoryNode * nextChild = new HistoryNode(nextSegment, _timestamp, childLevel, segments, _latitude + ofRandom(-10, 10), _longitude + ofRandom(-10, 10));
            _children[nextSegment] = nextChild;
        }
    }
}

//--------------------------------------------------------------
ofPoint HistoryNode::draw()
{    
    // Add some noise to the mix.
    _t = ofGetFrameNum() * noiseStep;
	float latOffset = ofSignedNoise(_t + _latitude) * noiseAmount;
	float longOffset = ofSignedNoise(_t + _longitude) * noiseAmount;
    
    ofQuaternion latRot, longRot, spinQuat;
    latRot.makeRotate(_latitude + latOffset, 1, 0, 0);
    longRot.makeRotate(_longitude + longOffset, 0, 1, 0);
//    spinQuat.makeRotate(ofGetFrameNum(), 0, 1, 0);
    
    ofVec3f center = ofVec3f(0, 0, levelDepth * (_level + 1));
    ofVec3f worldPoint = latRot * longRot * spinQuat * center;
    
    ofPushMatrix();
    ofTranslate(worldPoint);
    {
        float z = billboard();
        alpha = ofMap(z, minZ, maxZ, minAlpha * 255, 255, true);
        
        ofSetColor(nodeColor, alpha);
        ofSphere(0, 0, 0, nodeRadius);
        
        ofScale(1, -1, 1);
        ofSetColor(textColor, alpha);
        float widthOffset = (_textWidth / 2.0f + nodeRadius * 2) * textAlignX;
        float heightOffset = (_textHeight / 2.0f + nodeRadius * 2) * textAlignY;
        font.drawStringAsShapes(_name, -_textWidth / 2.0f + widthOffset, _textHeight / 2.0f + heightOffset);
    }
    ofPopMatrix();
    
    for (auto& it : _children) {
        ofPoint childPoint = it.second->draw();
        
        // Use the node alphas when drawing the connecting lines.
        glBegin(GL_LINE_STRIP);
        ofSetColor(lineColor, alpha);
        glVertex3f(worldPoint.x, worldPoint.y, worldPoint.z);
        ofSetColor(lineColor, it.second->alpha);
        glVertex3f(childPoint.x, childPoint.y, childPoint.z);
        glEnd();
    }
        
    return worldPoint;
}

//--------------------------------------------------------------
float HistoryNode::billboard()
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
    
    // Return z-coord for setting the alpha.
    return modelview[14];
}

//--------------------------------------------------------------
void HistoryNode::print()
{
    for (int i = 0; i < _level; i++) {
        cout << "  ";
    }
    cout << _name << endl;
    
    for (auto& it : _children) {
        it.second->print();
    }
}
