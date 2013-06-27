
#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"
#include "ofxOcean.h"
#include "ofxOceanRenderer.h"

class CloudsVisualSystemOcean : public CloudsVisualSystem {
  public:
	CloudsVisualSystemOcean();

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
	
  protected:
	
	ofxOcean ocean;
	ofxOceanRenderer renderer;

	bool shouldRegenerateOcean;
	void generateOcean();
	
	ofxUISuperCanvas* oceanGui;
	
	//Generator variables
	float windSpeed;
    float oceanTileSizeX;
    float oceanTileSizeY;

	
	
	float waveScale;
	float choppyScale;
	float waveSpeed;
	
};