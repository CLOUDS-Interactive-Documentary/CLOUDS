//
//  CloudsVisualSystemNervousSystem.h
//  CloudsVisualSystemEditor
//
//  Created by James George on 6/26/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"

class CloudsVisualSystemLaplacianTunnel : public CloudsVisualSystem {
  public:
	
	CloudsVisualSystemLaplacianTunnel();
	
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
	
	ofxUISuperCanvas* customGui;
	float exampleSlider;
	
  protected:
	
	int frameCount;
	float startTime;
	float fps;
	
	float numReplications;
	float replicationOffset;
	
	vector<int> indexCount;
	vector<ofVbo*> vbos;
	
	void clear();
};
