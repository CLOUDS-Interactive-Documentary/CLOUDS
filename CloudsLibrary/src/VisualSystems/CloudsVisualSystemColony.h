//
//  CloudsVisualSystemColony.h
//  VSColony
//
//  Created by Patricio Gonzalez Vivo on 6/26/13.
//
//

#pragma once

#include "CloudsVisualSystemRezanator.h"

#include "cCell.h"
        
class CloudsVisualSystemColony : public CloudsVisualSystemRezanator {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfAutoMode();
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    
    void draw(ofEventArgs & args);
    
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
    
    vector< colonyCell* > cells;
    vector< colonyCell* > newborns;
    
    float   noiseZoom;
    int newbornCount;
};
