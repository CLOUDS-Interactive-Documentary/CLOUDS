//
//  smoother.h
//  just_you_AA_NI_GLSL
//
//  Created by Jason Levine on 2013-11-12.
//
//

#pragma once
#include "ofMain.h"


//mean - Return mean of n past values.  requires vector of past values

//weighted mean - Return some pct of current value plus some percentage of last value(s).  requires vector of past values


//median - sort current value with last n values.  Return median. requires vector of past values

//decay - If value is greater than threshold, return value, otherwise return decayed version of that value that was greater than threshold.  requires threshold, decay, and value.



class smoother {
    public:
    void setNumPValues(int _numPVals) { numPVals = _numPVals; }
    void addValue(float value);
    float getMean();
    float getWMean(float curWeight, float pastWeight);
    float getMedian();
    
    int numPVals;
    vector<float> pastValues;
};
