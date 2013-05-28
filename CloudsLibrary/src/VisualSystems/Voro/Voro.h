//
//  ofxVoro.h
//  Funtions for wrapping Voro++ library ( http://math.lbl.gov/voro++/ )
//
//  Created by Patricio Gonzalez Vivo on 4/17/13.
//
//

#pragma once

#include "ofMain.h"

#include "voro++.h"
using namespace voro;

//  cell (voronoicell) wrappers
//
float           getCellRadius(voro::voronoicell &_con);
ofPoint         getCellCentroid(voro::voronoicell &_con, ofPoint _pos = ofPoint(0,0,0));
vector<ofPoint> getCellVerteces(voro::voronoicell &_con, ofPoint _pos = ofPoint(0,0,0));
ofMesh          getCellMesh(voro::voronoicell &_con, ofPoint _pos = ofPoint(0,0,0));

//  container wrappers
//
void            addCellSeed(voro::container &_con, ofPoint &_pnt, int _i, bool _checkInside = false);
void            addCellSeed(voro::container &_con, ofPoint *_pnt, int _i, bool _checkInside = false);
void            addCellsSeeds(voro::container &_con, vector<ofPoint> &_pnts, bool _checkInside = false);
void            addCellsSeeds(voro::container &_con, vector<ofPoint*> &_pnts, bool _checkInside = false);
void            addCellsSeeds(voro::container &_con, ofPoint *_pnts, int _nSize, bool _checkInside = false);

vector<float>   getCellsRadius(voro::container &_con);
vector<ofMesh>  getCellsFromContainer(voro::container &_con);
vector<ofPoint> getCellsPositions(voro::container &_con);
vector<ofPoint> getCellsCentroids(voro::container &_con);

bool            insideContainer(voro::container &_con, ofPoint _pnt);