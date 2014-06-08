#pragma once

#include "ofMain.h"

#include "ofxPostProcessing.h"
#include "ofxUI.h"
#include "ofxMeshUtils.h"


#include "audioAnalytics.h"
#include "colorSchemeDesigner.h"
#include "terrainManager.h"
#include "cameraManager.h"
#include "lightManager.h"
#include "postProcManager.h"

#include "decayer.h"

class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    void setupGUI();
    void guiEvent(ofxUIEventArgs &e);

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
	
	audioAnalytics aa;
    colorSchemeDesigner cs;
    terrainManager tm;
    cameraManager cm;
    lightManager lm;
    postProcManager pm;
    
    int state;

    ofxUICanvas *gui;
    
    bool useLights;
    bool updateCam;
    
};
