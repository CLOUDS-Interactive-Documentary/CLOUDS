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
    
    void setupPhysics();
    void setupGrid();
    
    void updatePhysics();
    void drawPhysics();
  
    int width,height;
    msa::physics::World2D physics;


};