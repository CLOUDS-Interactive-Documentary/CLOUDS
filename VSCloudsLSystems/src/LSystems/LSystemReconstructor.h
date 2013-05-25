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

class LSystemReconstructor : public ofPath {
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
    
    float speed;
    
private:
    void    addNode(ofPoint &_pnt);
    int     isNode(ofPoint &_pnt);
    void    renderBranch(int _index, float _relativeTime, float _speed);
    
    vector<LNode> nodes;
    
    float   time, lastTime;
    float   pct;
    bool    bGrow;
};

#endif /* defined(__lSystem__LSystemReconstructor__) */
