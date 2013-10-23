//
//  HistoryNode.h
//  EmptyVisualSystem
//
//  Created by Elie Zananiri on 2013-07-28.
//
//

#pragma once

#include "ofMain.h"

extern int kLevelDepth;

//--------------------------------------------------------------
//--------------------------------------------------------------
class HistoryNode
{
public:
    HistoryNode(const string& name, int timestamp, int level, list<string>& childSegments, float latitude = ofRandom(360), float longitude = ofRandom(360));
    void addChild(list<string>& segments, int childLevel);
    
    float billboard();
    ofPoint draw();
    void print();
    
    float alpha;
    
    static ofTrueTypeFont font;
    static ofColor textColor;
    static float textAlignX;
    static float textAlignY;
    
    static ofColor lineColor;
    
    static ofColor nodeColor;
    static float nodeRadius;

    static float minZ;
    static float maxZ;
    static float minAlpha;
    
    static float lineWidth;
    static float levelDepth;
    static float noiseStep;
    static float noiseAmount;
    
private:
    string _name;
    int _timestamp;
    int _level;
    map<string, HistoryNode *> _children;
    
    float _textWidth;
    float _textHeight;
    
    float _latitude;
    float _longitude;
    
    float _t;
};
