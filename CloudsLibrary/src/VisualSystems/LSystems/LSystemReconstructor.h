//
//  LSystemReconstructor.h
//  lSystem
//
//  Created by Patricio Gonzalez Vivo on 5/14/13.
//
//

#ifndef __lSystem__LSystemReconstructor__
#define __lSystem__LSystemReconstructor__

#include "ofMain.h"
#include "LSystem.h"
#include "LNode.h"

class LSystemReconstructor {
public:
    LSystemReconstructor();
    
    void    setup( LSystem &_lsys, int _deep );
    void    init();
    
    ofRectangle getActiveNodesAre();
    
    void    update();
    
    ofMesh      originalMesh;
    ofMesh      growMesh;
    vector<ofPoint>  activeNodes;
    
    float   tNoise;
    float   aNoise;
    float   bornRandom;
    float   speed;
    
    float   breakness;
    
    bool    bGrow;
    bool    bFlow;
    
private:
    vector<ofPolyline>  lines;
    vector<LNode>       nodes;
    
    void    lineTo(ofPoint &_pnt);
    void    addNode(ofPoint &_pnt);
    int     isNode(ofPoint &_pnt);
    
    void    renderBranch(ofMesh &_mesh, int _index, float _relativeTime, float _speed);
    
    float   time, timeLast;
};

#endif
