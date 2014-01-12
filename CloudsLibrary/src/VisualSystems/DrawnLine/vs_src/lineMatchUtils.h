

#pragma once


#include "ofMain.h"
#include "statistics.h"



typedef struct {
    ofMatrix4x4 mat;
    ofMatrix4x4 mat2;
} transformation;

typedef  struct {
    ofPoint matchA;
    ofPoint matchB;
    int idOfMatch;
    transformation trans;
    float startTime;
    ofPolyline matchLine;
    int bestAngle;
    ofPoint offset;
    ofRectangle bounds;
} matchStruct;

typedef struct {
    int id;
    float distance;
    int bestAngle;
} distanceResult;



typedef struct {
    int whichLine;
    int whichAngle;
    ofPolyline * line;
    float distance;
} polyPtr;


typedef struct lineSet{
    
    vector < ofPolyline > lines;
    vector < ofPolyline > normalizeLines;
    vector < ofPolyline * > normalizedDiffAngles;
    
    ofPolyline normalized;
    
    float distance;
    int idMe;
    bool operator< (const lineSet & input) const{
        return distance < input.distance;
    }
    
    
} ;


bool sortFunc(const distanceResult & a, const distanceResult & b);
bool sortFuncPP(const polyPtr & a, const polyPtr & b);
void distancePP( ofPolyline & a, polyPtr & pp, int nPts );
transformation normalizeLineSetGetTrans (ofPolyline & input);
void normalizeLineSet(lineSet & lineSet, ofPolyline & input);
ofPolyline returnNormalizedLine (ofPolyline & input);
