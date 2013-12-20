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
#include "ofxAVFVideoPlayer.h"
//--------------------------------------------------------------
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

    void restart();
    void render();

protected:
    ofxUISuperCanvas * playerGui;

    ofxAVFVideoPlayer player;
    vector<string> movieStrings;
    int movieIndex;
    ofRectangle screenRect;
    ofRectangle videoRect;
    bool bFileLoaded;
    float inTime, outTime;
};
