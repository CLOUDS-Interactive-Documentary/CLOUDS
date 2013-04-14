/**
 * Example - Scene With Timeline
 * This example shows how to create a basic RGB+D scene
 * with a timeline to scrub through the video.
 *
 * It requiers ofxGameCamera https://github.com/Flightphase/ofxGameCamera 
 *
 * James George 2012 
 * Released under the MIT License
 *
 * The RGBDToolkit has been developed with support from the STUDIO for Creative Inquiry and Eyebeam
 */

#pragma once

#include "ofMain.h"

#include "ofxGui.h"
#include "ofxGameCamera.h"

#include "CloudsRGBDCombinedRender.h"

class testApp : public ofBaseApp{
  public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void exit();
    
    ofVideoPlayer               player;
    CloudsRGBDCombinedRender    renderer;
    ofxGameCamera               cam;

    ofxPanel        gui;
    ofxFloatSlider  xshift;
    ofxFloatSlider  yshift;
    ofxFloatSlider  xsimplify;
    ofxFloatSlider  ysimplify;
    ofxToggle       flipTexture;
    ofxButton       loadNew;

};
