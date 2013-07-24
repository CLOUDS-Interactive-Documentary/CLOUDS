
#pragma once

#include "ofMain.h"

#include "CloudsVisualSystem.h"
#include "MSAPhysics3D.h"

using namespace msa::physics;

class CloudsVisualSystemVerletForm : public CloudsVisualSystem {
  public:
	
	CloudsVisualSystemVerletForm();
	
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
	
	ofxUISuperCanvas* clothGui;
	
	float clothWidth;
	float clothHeight;
	float colorIndex;
	float gridSize;
	
	void clearElements();
	
	bool shouldRegenerateMesh;
	void generateMesh();
	
	float springStrength;
	float springDampening;
	float springExtend;
	
	ofVboMesh mesh;
	World3D physics;
//	map<Particle3D*, ofIndexType> particleToMeshIndex;
	map<ofIndexType, Particle3D*> meshIndexToParticle;
	map<Particle3D*, vector<ofIndexType> > particleToMeshIndices;
	
	
	void updateNormals();
	
	vector< vector<Particle3D*> > particles;
	
	//color generators
	vector<ofColor> initColors(int row);
	vector<ofColor> colors;

	ofPixels colorPalettes;
};