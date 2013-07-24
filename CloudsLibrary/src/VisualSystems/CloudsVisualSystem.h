#pragma once

#include "ofMain.h"
#include "ofxCameraSaveLoad.h"
#include "ofxTimeline.h"
#include "ofxUI.h"
#include "ofxTLCameraTrack.h"

//tenuous
#include "ofxExtras.h"
#include "ofxGenerative.h"

/**
 * The CLOUDS Visual System super class
 * All visual systems will extend from this class
 *
 * It works pretty much the same as testApp, with the addition of a few
 * methods for interacting with play time duration, on screen start and stop, and accessing CLOUDS global state
 */

//TODO: fading, info routing
class CloudsRGBDCombinedRenderer;

class CloudsVisualSystem {
  public:
	   
	CloudsVisualSystem();
	~CloudsVisualSystem();
	
	static ofFbo& getStaticRenderTarget(); //default
	
	ofFbo& getSharedRenderTarget();
	ofImage& getCursor();
	
	ofFbo* sharedRenderTarget;
	
	//SUB CLASSES USE THESE METHODS:
    virtual void selfSetup();
    virtual void selfSetupGuis();
    
    virtual void selfUpdate();
    virtual void selfDrawBackground();
    virtual void selfDrawDebug();
    virtual void selfSceneTransformation();
    virtual void selfDraw();
	virtual void selfDrawOverlay();
	virtual void selfPostDraw();
	virtual void selfPresetLoaded(string presetPath);
	
    virtual void selfExit();
    virtual void selfBegin();
	virtual void selfEnd();
	
    virtual void selfKeyPressed(ofKeyEventArgs & args);
    virtual void selfKeyReleased(ofKeyEventArgs & args);
    
    virtual void selfMouseDragged(ofMouseEventArgs& data);
    virtual void selfMouseMoved(ofMouseEventArgs& data);
    virtual void selfMousePressed(ofMouseEventArgs& data);
    virtual void selfMouseReleased(ofMouseEventArgs& data);
    
    virtual void selfSetupGui();
    virtual void selfGuiEvent(ofxUIEventArgs &e);
	
    virtual void selfSetupSystemGui();
    virtual void guiSystemEvent(ofxUIEventArgs &e);
	
    virtual void selfSetupRenderGui();
    virtual void guiRenderEvent(ofxUIEventArgs &e);
    
    virtual void selfSetupTimeline();
    virtual void selfSetupTimelineGui();
    virtual void selfTimelineGuiEvent(ofxUIEventArgs &e);
	//END SUBCLASS METHODS
	
	virtual string getSystemName();
	
	//Data Folder Path
    string getVisualSystemDataPath();
	ofxTimeline* getTimeline();
	
	//APP CYCLE EVENTS
	//pre allocate any assets that will cause freezes
	virtual void setup();
	
	//these events are registered only when running the simulation
	virtual void update(ofEventArgs & args);
	virtual void draw(ofEventArgs & args);
	
	//application exit, clean up and don't crash
	virtual void exit(ofEventArgs & args);

	//INTERACTION EVENTS -- registered only
	virtual void keyPressed(ofKeyEventArgs & args);
	virtual void keyReleased(ofKeyEventArgs & args);
	
	virtual void mouseDragged(ofMouseEventArgs & args);
	virtual void mouseMoved(ofMouseEventArgs & args);
	virtual void mousePressed(ofMouseEventArgs & args);
	virtual void mouseReleased(ofMouseEventArgs & args);

	//these two methods are called by the controller class,
	//they register events and result in a calls to begin/end on the subclass
	void playSystem();
	void stopSystem();
	
	void setRenderer(CloudsRGBDCombinedRenderer& newRenderer);

	void setupSpeaker(string speakerFirstName,
					  string speakerLastName,
					  string quoteName);
	
	void speakerEnded();
	


	//how much time left to show this visual system?
	//once seconds is set to zero the end() event will be called by the controller
	float getSecondsRemaining();
	void setSecondsRemaining(float seconds);
		
	//this will always match what you offered in relevant keywords
	void setCurrentKeyword(string theme);
	string getCurrentKeyword();
	
	//set before calling begin so the class can react to the topic
	void setCurrentTopic(string keyword);
	string getCurrentTopic();

    //Drawing Helpers
    void drawDebug();
    void drawAxis(float size, float color);
    void drawGrid(float x, float y, float w, float h, float color);
    void billBoard(ofVec3f globalCamPosition, ofVec3f globelObjectPosition);
    void drawTexturedQuad();
    void drawNormalizedTexturedQuad();
    void drawBackground();
    void ofLayerGradient(const ofColor& start, const ofColor& end);
	
    //Core Param Setup
    void setupAppParams();
    void setupDebugParams();
    void setupCameraParams();
    void setupLightingParams();
    void setupMaterialParams();
    void setupTimeLineParams();
	
    //Core UI[S]
    void setupCoreGuis();
    
    void setupGui();
    void guiEvent(ofxUIEventArgs &e);
    
    void setupSystemGui();
    void setupRenderGui();
	
    void setupBackgroundGui();
    void guiBackgroundEvent(ofxUIEventArgs &e);
    
    void setupLightingGui();
    void guiLightingEvent(ofxUIEventArgs &e);
    
    void setupCameraGui();
    void guiCameraEvent(ofxUIEventArgs &e);
	
	void setupPresetGui();
	void guiPresetEvent(ofxUIEventArgs &e);
	
    void setupMaterial(string name, ofMaterial *m);
    void guiMaterialEvent(ofxUIEventArgs &e);
	
    void setupPointLight(string name);
    void setupSpotLight(string name);
    void setupBeamLight(string name);
    void setupGenericLightProperties(ofxUISuperCanvas *g, ofxLight *l);
    void guiLightEvent(ofxUIEventArgs &e);
	
    void setupTimeline();
    void timelineBangEvent(ofxTLBangEventArgs& args);
    void setupTimelineGui();
    void guiTimelineEvent(ofxUIEventArgs &e);
    void setTimelineTrackCreation(bool state);
    void setTimelineTrackDeletion(bool state);
    void guiAllEvents(ofxUIEventArgs &e);
    void updateTimelineUIParams();
    void saveTimelineUIMappings(string path);
    void loadTimelineUIMappings(string path);
    
    //Lighting Helpers
    void lightsBegin();
    void lightsEnd();
    
    //UI Helpers
	vector<string> getPresets();
	
    void loadGUIS();
	void loadPresetGUISFromName(string presetName);
    void loadPresetGUISFromPath(string presetPath);
    void saveGUIS();
    void savePresetGUIS(string presetName);
    void showGUIS();
    void hideGUIS();
    void toggleGUIS();
    void toggleGuiAndPosition(ofxUISuperCanvas *g);
    void deleteGUIS();
    
	void setCurrentCamera( ofCamera& cam );
	void setCurrentCamera( ofCamera* swappedInCam );
	ofCamera& getCameraRef();
	ofCamera* getCurrentCamera();
	
	
	//LB
	ofVec3f getCameraPosition();
	ofVec3f positionOffset;
	


	bool cursorIsOverGUI();
	
	void setDrawToScreen( bool state );
	bool getDrawToScreen();
	
	
	
  protected:
		
	//UI
    ofxUISuperCanvas *gui;
    ofxUISuperCanvas *sysGui;
    ofxUISuperCanvas *rdrGui;
    ofxUISuperCanvas *bgGui;
    ofxUISuperCanvas *lgtGui;
    ofxUISuperCanvas *camGui;
    ofxUISuperCanvas *presetGui;
    ofxUISuperCanvas *tlGui;
	
    vector<ofxUISuperCanvas *> guis;
    map<string, ofxUICanvas *> guimap;
	
    //Background Props
    float bgAspectRatio;
    int gradientMode;
	ofColor *bgColor;
	ofx1DExtruder *bgHue;
	ofx1DExtruder *bgSat;
	ofx1DExtruder *bgBri;
	ofColor *bgColor2;
	ofx1DExtruder *bgHue2;
	ofx1DExtruder *bgSat2;
	ofx1DExtruder *bgBri2;
    
    ofxUISlider *hueSlider;
    ofxUISlider *satSlider;
    ofxUISlider *briSlider;
	ofxUIRadio *presetRadio;
	
    //ANIMATORS
    vector<ofx1DExtruder *> extruders;
	
    //MATERIAL
    ofMaterial *mat;
    map<string, ofMaterial *> materials;
    map<string, ofxUISuperCanvas *> materialGuis;
	
    //LIGHTING
    float *globalAmbientColor;
    bool bSmoothLighting;
    bool bEnableLights;
	
    //LIGHTS
    map<string, ofxLight *> lights;
    map<string, ofxUISuperCanvas *> lightGuis;
	
    //APP
    bool bRenderSystem;
    bool bUpdateSystem;
    bool bDebug;
    float debugGridSize;
	bool bClearBackground;
	bool bDrawToScreen;
	
    //CAM
    float camDistance;
    float camFOV;
    ofxViewType view;
	ofCamera* currentCamera;
    ofEasyCam cam;
    ofx1DExtruder *xRot;
    ofx1DExtruder *yRot;
    ofx1DExtruder *zRot;
	
    //TIMELINE
    void resetTimeline();
	void bindWidgetToTimeline(ofxUIWidget* widget);
    void unBindWidgetFromTimeline(ofxUIWidget* widget);
    ofxTimeline* timeline;
    ofxTLCameraTrack* cameraTrack;
	ofxTLFlags* introOutroTrack;
	
    map<ofxTLBangs*, ofxUIButton*>	tlButtonMap;
    map<ofxUIToggle*, ofxTLSwitches*>	tlToggleMap;
    map<ofxUISlider*, ofxTLCurves*>	tlSliderMap;
    map<ofxUINumberDialer*, ofxTLCurves*> tlDialerMap;

	bool bTimelineIsIndefinite;
	float getIntroDuration();
	float getOutroDuration();

    float timelineDuration;
	bool bUseCameraTrack;
    bool bEnableTimeline;
	
    bool bDeleteTimelineTrack;
    bool bShowTimeline;
    bool bEnableTimelineTrackCreation;

	
	//these variables are set by the playback controller when displaying
	//ways to interact with the pointcloud data
	CloudsRGBDCombinedRenderer* sharedRenderer;
	//set to true if the pointcloud renderer has valid speaker
	bool hasSpeaker;
	
	//speaker and quote info, constantly updated
	string speakerFirstName;
	string speakerLastName;
	string quoteName;
	
	//keyword is the topic of conversation
	string currentTopic;
	//theme is the topic chosen
	string currentKeyword;
	
	bool isPlaying;
	float secondsRemaining;
};
