#pragma once

#include "MSAPhysics2D.h"
#include "ofMain.h"
#include "CloudsFCPParser.h"

typedef struct {
    
} ClipLink;

class CloudsFCPVisualizer {
  public:
    CloudsFCPVisualizer();
    CloudsFCPParser* database;
    void setup();
    
    //void setupPhysics();
    void createClusterPhysics();
    void createIterativePhysics();
    
    void updatePhysics();
    void drawPhysics();

    void exportForGraphviz();
    
    void drawGrid();
 
    map< pair<string,string>, int > sharedClips;
    
    int width,height;
    msa::physics::World2D physics;

    ofTrueTypeFont font;

};