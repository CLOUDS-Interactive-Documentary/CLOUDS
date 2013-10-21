#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"

/** 
 * COMPUTATION visual system
 * shows ticker tape strands running in fields
 * that can be zoomed in and diverted
 */

class CloudsVisualSystemDataCascade : public CloudsVisualSystem {
  public:

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
	
	void selfPresetLoaded(string presetPath);
	
	virtual ofCamera& getCameraRef(){
		if(doRGBD){
			return cloudsCamera;
		}
		return CloudsVisualSystem::getCameraRef();
	}
	
  protected:

	int width, height;
	ofVboMesh mesh;
	vector<int> startIndeces;

	void regenerateParticles();
		
	ofImage speedTexture;
	ofImage shiftTexture;
	
	void reloadShaders();
	ofShader updateShader;
	ofShader drawShader;
	
	ofVboMesh offsetMesh;
	ofFbo sourceOffset,targetOffset;
		
	float pathDeviation;
	
	float fogMinDepth;
	float fogMaxDepth;
	float fogDensity;
	
	bool doRGBD;
	
	bool drawDouble;
	float doubleOffset;
	float doubleCheat;
	
	bool wrappedOnly;
	float cylinderWarpFade;
	float cylinderZ;
	float cylinderRadius;

	float speed;
	float scale;
	float pointSize;
	float deviation;
	float colorflip;
	ofVec3f vertexOffset;
	bool debug;
	bool regenerate;
};