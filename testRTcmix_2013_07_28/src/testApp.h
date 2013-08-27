#pragma once

#define MAXAMP 32768.0 // maximum amp for oscillators (-1., 1) = 2^15
#define DEBUG false

#include "ofMain.h"
#include <globals.h>
#include <RTcmix.h>

class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    // audio functions
    double mtof(double f, double tuning);
    double ftom(double f, double tuning);
    int scale(int p);
    
    // data
    vector <float> foo_t, foo_f1, foo_a1, foo_f2, foo_a2, foo_f3, foo_a3;
    
    RTcmix *rrr;
    string libpath;
    int pmouseX;
    int pmouseY;
    double elapsed;
    int fpos;
    double viddur;
    
    // video shit
    ofVideoPlayer vidPlayer;
};
