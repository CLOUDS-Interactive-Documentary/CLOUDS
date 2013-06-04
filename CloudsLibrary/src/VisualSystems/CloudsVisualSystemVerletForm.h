
#pragma once

#include "ofMain.h"

#include "CloudsVisualSystemRezanator.h"
#include "MSAPhysics3D.h"

using namespace msa::physics;

class CloudsVisualSystemVerletForm : public CloudsVisualSystemRezanator {
  public:
	
	CloudsVisualSystemVerletForm();
	
	string getSystemName();
	
    void selfSetup();
    void selfSetupGuis();
    
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
	
	float clothWidth;
	float clothHeight;
	int gridSizeX;
	int gridSizeY;
	
	void generateMesh();
	
	float springStrength;
	float springDampening;
	
	ofVboMesh mesh;
	World3D physics;
	
	vector< vector<Particle3D*> > particles;
	
};