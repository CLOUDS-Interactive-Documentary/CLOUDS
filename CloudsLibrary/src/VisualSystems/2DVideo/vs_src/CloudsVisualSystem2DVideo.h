//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
// Welcome to the EMPTY CloudsVisualSystem
//
//
//

#pragma once

#include "CloudsVisualSystem.h"

//--------------------------------------------------------------
class CloudsVisualSystem2DVideo : public CloudsVisualSystem
{
  public:
    string getSystemName(){
		return "2DVideo";
	}
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    void selfSetup();
    void selfBegin();
	void selfPresetLoaded(string presetPath);
    void selfSceneTransformation();
	void selfUpdate();
    void selfDraw();
	void selfDrawDebug();
	void selfDrawBackground();
	void selfEnd();
    void selfExit();
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    void loadMovieAtIndex(int index);
    void loadMovieWithName(string name);
    void selfSetDefaults();
    ofCamera& getCameraRef();

	ofVec2f currentRotation;
	ofVec2f rotationRange; //make a gui
	
    void restart();
    void render();

protected:
    ofxUISuperCanvas * playerGui;
    ofQuaternion curRot;

	void loadMovieFromRadio();
	bool receivedFrame;
    ofPtr<ofVideoPlayer> player;
    vector<string> movieStrings;
    void finishLoad();
    
    int movieIndex;
    string targetMovieName;
	string loadedMoviePath;
    ofRectangle screenRect;
    ofRectangle videoRect;
    ofMesh videoMesh;
    ofFbo videoFbo;
    bool bFileLoadCompleted;
    
    float inTime, outTime;
    ofCamera  vidCam;
    
#ifdef VHX_MEDIA
    CloudsMedia* waitingMedia;
    void vhxRequestComplete(CloudsVHXEventArgs& args);
#endif
    bool bWaitForVHX;
    
};
