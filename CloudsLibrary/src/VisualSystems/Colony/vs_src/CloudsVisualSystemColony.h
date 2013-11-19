//
//  CloudsVisualSystemColony.h
//  VSColony
//
//  Created by Patricio Gonzalez Vivo on 6/26/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"

//#include "cCell.h"
#include "gCell.h"

class CloudsVisualSystemColony : public CloudsVisualSystem {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfAutoMode();
    void selfUpdate();
    void selfDrawBackground();
    void selfDraw();
    void selfDrawDebug();
    void selfSceneTransformation();
    
//    void draw(ofEventArgs & args);
    
    void selfExit();
    void selfBegin();
    void selfEnd();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void mouseDragged(ofMouseEventArgs & args);
    void mouseMoved(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
   
    
private:
    
    ofFbo       foodFbo;
    ofShader    noiseShader;
	ofImage noise;
    
    colonyPartitionMap pMap;
    
    vector< cellPtr > cells;
    vector< cellPtr > newborns;
    
    float   noiseZoom;
    int newbornCount;
    
    /*  PARAMS */
    int numInitialCells;
};
