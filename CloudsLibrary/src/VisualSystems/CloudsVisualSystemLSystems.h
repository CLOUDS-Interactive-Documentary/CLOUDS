//
//  CloudsVisualSystemLSys.h
//  VSLSys
//
//  Created by Patricio Gonzalez Vivo on 5/25/13.
//
//

#pragma once

#include "CloudsVisualSystemRezanator.h"
#include "LSystemReconstructor.h"

class CloudsVisualSystemLSystems : public CloudsVisualSystemRezanator {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfAutoMode();
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    void selfDraw();
    void selfExit();
    void selfBegin();
	void selfEnd();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& da  ta);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void billBoard(); 
    
protected:
    
    ofVec3f objectLookAt;
    
    void reBuildLSys();
    
    ofTexture   dot;
    
    LSystemReconstructor lsysr;
    string  axiom;
    string  rule1;
    string  rule2;
    float   angle;
    float   globalScale;
    float   lsysScale;
    float   lsysDepth;
    float   dotSize;
};
