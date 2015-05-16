//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "CloudsVisualSystemPreset.h"

typedef struct{
    string title;
    string byLine;
    
    ofImage image;
    ofVec3f pos;
    float rotation;
    ofPolyline screenPoly;
    ofVboMesh mesh;
    ofVec3f normal;
    bool onScreen;
    bool hovered;
} VisualThumb;

class CloudsVisualSystemVisuals : public CloudsVisualSystem {
  public:
    
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "Visuals";
	}

	//These methods let us add custom GUI parameters and respond to their events
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
	//Use system gui for global or logical settings, for exmpl
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
	//use render gui for display settings, like changing colors
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);

	//This is called whenever a new preset is loaded, before selfSetup()
	//use it to ensure all your simple variables are initialized to an
	//acceptable default state
	void selfSetDefaults();
	
	// selfSetup is called when the visual system is first instantiated
	// This will be called during a "loading" screen, so any big images or
	// geometry should be loaded here
    void selfSetup();

	// selfBegin is called when the system is ready to be shown
	// this is a good time to prepare for transitions
	// but try to keep it light weight as to not cause stuttering
    void selfBegin();

	// selfPresetLoaded is called whenever a new preset is triggered
	// it'll be called right before selfBegin() and you may wish to
	// refresh anything that a preset may offset, such as stored colors or particles
	void selfPresetLoaded(string presetPath);
    
	//do things like ofRotate/ofTranslate here
	//any type of transformation that doesn't have to do with the camera
    void selfSceneTransformation();
	
	//normal update call
	void selfUpdate();

	// selfDraw draws in 3D using the default ofEasyCamera
	// you can change the camera by returning getCameraRef()
    void selfDraw();
		
    // draw any debug stuff here
	void selfDrawDebug();

	// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
	void selfDrawBackground();

    void selfDrawOverlay();
	// this is called when your system is no longer drawing.
	// Right after this selfUpdate() and selfDraw() won't be called any more
	void selfEnd();

	// this is called when you should clear all the memory and delet anything you made in setup
    void selfExit();

	//events are called when the system is active
	//Feel free to make things interactive for you, and for the user!
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
	
    void selectSystem(string systemName);
    bool selectionChanged();
    bool selectionConfirmed();
    string getSelectedSystem();
    
    //can be done on thread
    void setVisuals(map<string, CloudsVisualSystemCredit>& visuals);
    //done after load
    void pushTextures();
    
    ofCamera& getCameraRef(){
        if(bFreeCam){
            return CloudsVisualSystem::getCameraRef();
        }
        return selectCamera;
    }
    
    vector<string> getAvailableSystems();
protected:
    
	
    //vs sys standards
	ofxUISuperCanvas* cylinderGui;
    int cylinderCellsTall;
    int cylinderCellsWide;
    float cylRadius;
    float rowHeight;
    float imageScale;
    void layoutThumbnails();
    bool bFreeCam;
    float cameraBackupDistance;
    int fontSize;
    int currentFontSize;
    ofVec2f typeOffset;
    
    ofVec3f targetCameraSideDir;
    ofVec3f targetCameraUpDir;
    ofVec3f currentCameraUpDir;
    ofVec3f currentCameraSideDir;

    ofxFTGLFont visualFont;
    
    
    map<string, VisualThumb> thumbs;
    
    ofVec3f camTargetPos;
    ofVec3f camLookPos;
    string selectedSystem;
    bool bSelectionChanged;
    bool bSelectionConfirmed;
    float typeScale;
    bool bMouseEventCanceled;
    ofCamera selectCamera;
};
