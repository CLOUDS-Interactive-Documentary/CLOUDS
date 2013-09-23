
#pragma once

#include "ofMain.h"

#include "CloudsVisualSystem.h"

class CloudsVisualSystemYellowTail : public CloudsVisualSystem {
  public:
	
	CloudsVisualSystemYellowTail();
	
	string getSystemName();
	
    void selfSetup();
    void selfSetupGuis();
    void selfPresetLoaded(string presetPath);
	
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
    
    void selfSetupTimeline();
    void selfSetupTimelineGui();
    void selfTimelineGuiEvent(ofxUIEventArgs &e);
 
  protected:
	
	ofxUISuperCanvas* customGui;
	float someParam;
	

};