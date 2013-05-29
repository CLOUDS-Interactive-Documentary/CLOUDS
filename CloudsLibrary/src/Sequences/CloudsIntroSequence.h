//
//  IntroSequence.h
//  ComputerTicker
//
//  Created by James George on 5/26/13.
//
//

#pragma once

#include "ofMain.h"
#include "CloudsVisualSystemRezanator.h"
#include "ofxGameCamera.h"
//#include "ofxExtrudedText.h"

class CloudsIntroSequence : public CloudsVisualSystemRezanator {
  public:
	CloudsIntroSequence();
	~CloudsIntroSequence();
	
	virtual string getSystemName();
	
    virtual void selfSetup();
    virtual void selfSetupGuis();
    
    virtual void selfUpdate();
    virtual void selfDrawBackground();
    virtual void selfDrawDebug();

    virtual void selfDraw();
    virtual void selfExit();
    virtual void selfBegin();
	virtual void selfEnd();
	
    virtual void selfKeyPressed(ofKeyEventArgs & args);
    virtual void selfKeyReleased(ofKeyEventArgs & args);
    
    virtual void selfMouseDragged(ofMouseEventArgs& data);
    virtual void selfMouseMoved(ofMouseEventArgs& data);
    virtual void selfMousePressed(ofMouseEventArgs& data);
    virtual void selfMouseReleased(ofMouseEventArgs& data);
    
    virtual void selfSetupSystemGui();
    virtual void guiSystemEvent(ofxUIEventArgs &e);
    
    virtual void selfSetupRenderGui();
    virtual void guiRenderEvent(ofxUIEventArgs &e);

  protected:
	
	ofFbo fullscreenFbo;
	
	float fontSize;
	float fontExtrusion;
	float fontScale;
	
	float currentFontSize;
	float currentFontExtrusion;
	
	float perlinOffset;
	float wireframeAlpha;
	void reloadShaders();
	
	ofMesh blocksMesh;
	ofMesh tunnelMeshTight;
	ofMesh tunnelMeshLoose;
	
	ofxGameCamera camera;
	
	ofShader tunnelShader;
	ofShader chroma;
	
	ofRange pointSize;	
	ofRange distanceRange;
	
	void drawCloudsType();

	float maxChromaDistort;
	float perlinAmplitude;
	float perlinDensity;
	float perlinSpeed;
	
//	ofxExtrudedText cloudsTypeMesh;
	ofMesh thickTypeMesh;
	ofMesh thinTypeMesh;

};