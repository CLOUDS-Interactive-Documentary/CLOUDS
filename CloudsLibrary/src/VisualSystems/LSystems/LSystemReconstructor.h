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
    float startTime;
    vector<int> branchesIndex;
};

class LSystemReconstructor {
public:
    LSystemReconstructor();
    
    void setup( LSystem &_lsys, int _deep );
    
    float getPct();
    
    void start();
    void pause();
    void stop();
    
    void update();
    void draw();
    
    ofMesh  mesh;
    ofPolyline activeNodes;
    
    float tNoise;
    float aNoise;
    float bornRandom;
    float speed;
    
private:
    vector<ofPolyline>  lines;
    vector<LNode> nodes;
    
    void    lineTo(ofPoint &_pnt);
    void    addNode(ofPoint &_pnt);
    int     isNode(ofPoint &_pnt);
    
    void    renderBranch(int _index, float _relativeTime, float _speed);
    void    addLineToMesh(ofMesh &_mesh, ofPoint &A, ofPoint &B, ofFloatColor &c);
    
    float   time, lastTime;
    float   pct;
    bool    bGrow;
};

#endif /* defined(__lSystem__LSystemReconstructor__) */
