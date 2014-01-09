#pragma once

#include "ofMain.h"
#include "ofxCameraSaveLoad.h"
#include "ofxTimeline.h"
#include "ofxUI.h"
#include "ofxTLCameraTrack.h"
#include "CloudsRGBDCamera.h"
#include "CloudsInputEvents.h"
#include "ofxLight.h"
#include "ofxGenerative.h"
//#include "ofxMaterial.h"

#ifdef OCULUS_RIFT
#include "ofxOculusRift.h"
#endif

/**
 * The CLOUDS Visual System super class
 * All visual systems will extend from this class
 *
 * It works pretty much the same as testApp, with the addition of a few
 * methods for interacting with play time duration, on screen start and stop, and accessing CLOUDS global state
 */

class CloudsRGBDVideoPlayer;

enum ofxViewType
{
    OFX_VIEW_TOP = 0,
    OFX_VIEW_BOTTOM,
    OFX_VIEW_FRONT,
    OFX_VIEW_BACK,
    OFX_VIEW_RIGHT,
    OFX_VIEW_LEFT,
    OFX_VIEW_3D
};

enum CloudsDrawCursorMode
{
    DRAW_CURSOR_NONE = 0,
    DRAW_CURSOR_PRIMARY,
    DRAW_CURSOR_ALL
};

class CloudsVisualSystem {
  public:
	   
	CloudsVisualSystem();
	virtual ~CloudsVisualSystem();
	
	enum RGBDTransitionType
	{
	  TWO_DIMENSIONAL = 0,
	  FLY_THROUGH = 1,
	  WHIP_PAN = 2,
	  RGBD = 3
	};
	
	
	static ofFbo& getStaticRenderTarget(); //default
	static void forceScreenResolution(int screenWidth, int screenHeight);
	static void setNumSamples(int samples = 0);
	static void loadBackgroundShader();
	static CloudsRGBDVideoPlayer& getRGBDVideoPlayer();
	static void getBackgroundMesh(ofMesh& mesh, ofImage& image, float width, float height);
    static void get2dMesh(ofMesh& mesh, float width, float height);
	
#ifdef OCULUS_RIFT
	static ofxOculusRift& getOculusRift();
#endif
	ofFbo& getSharedRenderTarget();
	ofImage& getCursor();
	int getCanvasWidth();
	int getCanvasHeight();
	
	//SUB CLASSES USE THESE METHODS:
	virtual void selfSetDefaults();
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
	
	virtual void selfInteractionMoved(CloudsInteractionEventArgs& args);
	virtual void selfInteractionStarted(CloudsInteractionEventArgs& args);
	virtual void selfInteractionDragged(CloudsInteractionEventArgs& args);
	virtual void selfInteractionEnded(CloudsInteractionEventArgs& args);


    virtual void selfSetupGui();
    virtual void selfGuiEvent(ofxUIEventArgs &e);
	
    virtual void selfSetupSystemGui();
    virtual void guiSystemEvent(ofxUIEventArgs &e);
	
    virtual void selfSetupRenderGui();
    virtual void guiRenderEvent(ofxUIEventArgs &e);

	virtual void selfSetupCameraGui();
    virtual void guiCameraEvent(ofxUIEventArgs &e);


    virtual void selfSetupTimeline();
    virtual void selfSetupTimelineGui();
    virtual void selfTimelineGuiEvent(ofxUIEventArgs &e);
	//END SUBCLASS METHODS
	
	virtual string getSystemName() = 0;
	
	void setupRGBDTransforms();

	//Data Folder Path
    string getVisualSystemDataPath(bool ignoredFolder = false);
	ofxTimeline* getTimeline();
	
	//APP CYCLE EVENTS
	//pre allocate any assets that will cause freezes
	void setup();
	
	//these events are registered only when running the simulation
	void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	
	//application exit, clean up and don't crash
	void exit();

	//INTERACTION EVENTS -- registered only
	void keyPressed(ofKeyEventArgs & args);
	void keyReleased(ofKeyEventArgs & args);
	
	void mouseDragged(ofMouseEventArgs & args);
	void mouseMoved(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);

	//CLOUDS INTERACTION EVENTS
	void interactionMoved(CloudsInteractionEventArgs& args);
	void interactionStarted(CloudsInteractionEventArgs& args);
	void interactionDragged(CloudsInteractionEventArgs& args);
	void interactionEnded(CloudsInteractionEventArgs& args);

	//these two methods are called by the controller class,
	//they register events and result in a calls to begin/end on the subclass
	void playSystem();
	void stopSystem();
	
	bool isSetup();
	bool isPlaying();
	

	void setupSpeaker(string speakerFirstName,
					  string speakerLastName,
					  string quoteName);

	virtual void speakerChanged(){};
	void speakerEnded();
	
	//this will always match what you offered in relevant keywords
	void setKeywords(string mainKeyword, vector<string> allKeywords);
	
    //Drawing Helpers
    void drawDebug();
    void drawAxis(float size, float color);
    void drawGrid(float x, float y, float w, float h, float color);
    void billBoard(ofVec3f globalCamPosition, ofVec3f globelObjectPosition);

    void drawBackground();
	void drawBackgroundGradient();
    void draw2dSystemPlane();
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
	
	void setupPresetGui();
	void guiPresetEvent(ofxUIEventArgs &e);
	
    void setupMaterial(string name, ofxMaterial *m);
    void guiMaterialEvent(ofxUIEventArgs &e);
	
    void setupPointLight(string name, ofxLight* light = NULL);
    void setupSpotLight(string name);
    void setupBeamLight(string name);
    void setupGenericLightProperties(ofxUISuperCanvas *g, ofxLight *l);
    void guiLightEvent(ofxUIEventArgs &e);
	
    void setupTimeline();
	//override with caution, you still need to always call this when a bang occurs
    virtual void timelineBangEvent(ofxTLBangEventArgs& args);
    void setupTimelineGui();
    void guiTimelineEvent(ofxUIEventArgs &e);
    void setTimelineTrackCreation(bool state);
    void setTimelineTrackDeletion(bool state);
    void guiAllEvents(ofxUIEventArgs &e);
    void updateTimelineUIParams();
    void saveTimelineUIMappings(string path);
    void loadTimelineUIMappings(string path);
    
#ifdef KINECT_INPUT
    void setupKinectGui();
	void guiKinectEvent(ofxUIEventArgs &e);
#endif
#ifdef OCULUS_RIFT
    void setupOculusGui();
	void guiOculusEvent(ofxUIEventArgs &e);
#endif
    
    //Lighting Helpers
    void lightsBegin();
    void lightsEnd();
    
    //UI Helpers
	vector<string> getPresets();
	string currentPresetName;
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

//	void setCurrentCamera( ofCamera& cam );
//	void setCurrentCamera( ofCamera* swappedInCam );
//	ofCamera* getCurrentCamera();
	virtual ofCamera& getCameraRef();

	
	ofVec3f translatedHeadPosition;
	float pointcloudScale;
	float pointcloudOffsetZ;
	CloudsRGBDCamera cloudsCamera;
	
	//LB
	ofVec3f getCameraPosition();
	ofVec3f positionOffset;

	RGBDTransitionType getTransitionType();
	
	bool cursorIsOverGUI();
	
	void setDrawToScreen( bool state );
	bool getDrawToScreen();

	float getCurrentAudioAmplitude();
	
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
#ifdef KINECT_INPUT
    ofxUISuperCanvas *kinectGui;
#endif
#ifdef OCULUS_RIFT
    ofxUISuperCanvas *oculusGui;
#endif
    
//    //UI Colours
//    ofxUIColor cb;
//    ofxUIColor co;
//    ofxUIColor coh;
//    ofxUIColor cf;
//    ofxUIColor cfh;
//    ofxUIColor cp;
//    ofxUIColor cpo;
	
	void stackGuiWindows();
	void drawScene();
	
    vector<ofxUISuperCanvas *> guis;
    map<string, ofxUICanvas *> guimap;
	
    //Background Props
    float bgAspectRatio;
	bool bBarGradient;
    int gradientMode;
	ofColor bgColor;
	ofColor bgColor2;

	float bgHue;
	float bgSat;
	float bgBri;
	float bgHue2;
	float bgSat2;
	float bgBri2;
	
	
    ofxUISlider *hueSlider;
    ofxUISlider *satSlider;
    ofxUISlider *briSlider;
	
	ofxUIRadio *presetRadio;
	ofxUIRadio *transitionRadio;
	
    //ANIMATORS
    vector<ofx1DExtruder *> extruders;
	
    //MATERIAL
    ofxMaterial *mat;
    map<string, ofxMaterial *> materials;
    map<string, ofxUISuperCanvas *> materialGuis;

	bool bIsPlaying;
	bool bIsSetup;
	bool bIs2D;
	
	//some crashes are being caused by update before draw
	//this makes sure the draw() command only happens after the first update
	bool updateCyclced;

    //LIGHTING
	ofFloatColor globalAmbientColorHSV;
    bool bSmoothLighting;
    bool bEnableLights;
	
    //LIGHTS
	ofxLight* light;
    map<string, ofxLight *> lights;
    map<string, ofxUISuperCanvas *> lightGuis;
	
    //APP
    bool bRenderSystem;
    bool bUpdateSystem;
    bool bDebug;
    float debugGridSize;
	bool bClearBackground;
	bool bDrawToScreen;
	bool bUseOculusRift;
    CloudsDrawCursorMode drawCursorMode;
    
    //CAM
    float camDistance;
    float camFOV;
    ofxViewType view;
//	ofCamera* currentCamera;
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

	//background stuff
	bool bMatchBackgrounds;
		
	//these variables are set by the playback controller when displaying
	//ways to interact with the pointcloud data
//	CloudsRGBDVideoPlayer* sharedRenderer;
	//set to true if the pointcloud renderer has valid speaker
	bool hasSpeaker;
	bool confirmedDataPath;
	string cachedDataPath;
	string cachedDataPathIgnore;
	
	//speaker and quote info, constantly updated
	string speakerFirstName;
	string speakerLastName;
	string quoteName;
	
	//keyword is the topic of conversation
//	string currentTopic;
	//theme is the topic chosen
	
	string mainKeyword;
	vector<string> keywords;
	
//	float secondsRemaining;
	
	void checkOpenGLError(string function);
	
};
