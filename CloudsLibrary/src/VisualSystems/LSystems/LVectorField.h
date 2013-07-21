#pragma once

#include "ofMain.h"

class LVectorField : public ofRectangle {
public:
    
    LVectorField();
    virtual ~LVectorField();
		
    void    setupField(int _cols, int _rows, int _width, int _height);   // pass in internal dimensions and outer dimensions
	
    int     size();
    void    clear();
    
    void    propagate();
    void    fadeField(float _fadeAmount);
    void    randomizeField(float _scale);
    void    noiseField(float _scale = 1.0f, float _speed = 0.01, float _turbulence = 3, bool _signed = true);
    
    void    draw();

    ofPoint& operator [](int _index);
    
    int     getIndexFor(float _x, float _y);
    ofPoint	getForceFromPos(ofPoint pos);
    ofPoint	getForceFromPos(float xpos, float ypos);

    void    addOutwardCircle(float x, float y, float radius, float strength);
    void    addInwardCircle(float x, float y, float radius, float strength);
    void    addClockwiseCircle(float x, float y, float radius, float strength);
    void    addCounterClockwiseCircle(float x, float y, float radius, float strength);
    void    addVectorCircle(float x, float y, float vx, float vy, float radius, float strength);

private:
    ofPoint *buffer[2];
    
    int     cols, rows, nTotal;
    int     activeBuffer;
};
