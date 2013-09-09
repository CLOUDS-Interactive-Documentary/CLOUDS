#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"

#include "ofxAVFVideoPlayer.h"
typedef struct{
	ofRange depthRange;
	ofVec2f depthFOV;
	ofVec2f depthPP;
} Intrinsics;

class CloudsVisualSystemRGBDVideo : public CloudsVisualSystem {
public:
	
	CloudsVisualSystemRGBDVideo();
	
	string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
	void selfPresetLoaded(string presetPath);
	
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
	
	ofCamera& getCameraRef(){
		return cloudsCamera;
	}
  protected:

	ofxAVFVideoPlayer player;
	ofShader rgbdPixelToPixelShader;
	Intrinsics videoIntrinsics;
	ofVboMesh pointCloud;
	
	float pointoffset;
	
	void reloadShader();
	
	bool playMovie(string path);
	string loadMoviePath;
	

	bool movieLoaded;
	
	ofxUITextInput* videoPathField;
};