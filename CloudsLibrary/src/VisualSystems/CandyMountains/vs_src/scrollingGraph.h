//
//  scrollingGraph.h
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-01-06.
//
//
#include "ofMain.h"

class scrollingGraph {
public:
    void setup(int _size, float init, float min, float max);
    void addValue(float val);
    void draw(float height);
    
    vector<float> valHistory;
    float minVal, maxVal;
    int size;
};