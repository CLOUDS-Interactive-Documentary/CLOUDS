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

struct LNode : public ofPoint {
    float   startTime;
    float   pct;
    vector<int> branchesIndex;
};

class LSystemReconstructor {
public:
    LSystemReconstructor();
    
    void    setup( LSystem &_lsys, int _deep );
    void    init();
    
    float   getPct();
    void    update();
    
    ofMesh      originalMesh;
    ofMesh      growMesh;
    ofPolyline  activeNodes;
    
    float   tNoise;
    float   aNoise;
    float   bornRandom;
    float   speed;
    
    bool    bGrow;
    bool    bFlow;
    
private:
    vector<ofPolyline>  lines;
    vector<LNode> nodes;
    
    void    lineTo(ofPoint &_pnt);
    void    addNode(ofPoint &_pnt);
    int     isNode(ofPoint &_pnt);
    
    void    renderBranch(ofMesh &_mesh, int _index, float _relativeTime, float _speed);
    
    float   time, timeLast;
    float   pct;
};

#endif /* defined(__lSystem__LSystemReconstructor__) */
