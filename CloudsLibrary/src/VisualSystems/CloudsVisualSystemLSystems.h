//
//  CloudsVisualSystemLSys.h
//  VSLSys
//
//  Created by Patricio Gonzalez Vivo on 5/25/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "LSystemReconstructor.h"

class CloudsVisualSystemLSystems : public CloudsVisualSystem {
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
    
    void selfMouseDragged(ofMouseEventArgs& data);
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
    void    reBuildLSys();
    
    LSystemReconstructor    lsysr;
    
    string  axiom,rule1,rule2;
    float   angle,lsysScale,lsysDepth;
    
    float   originalAlpha,traceAlpha;
    float   dotSize,particlesAlpha;
};
