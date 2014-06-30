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

#ifdef CLOUDS_APP
#include "CloudsPortal.h"
#endif

#ifdef CLOUDS_INTERLUDE_NAV
#include "ofxFTGL.h"
#include "CloudsCalibrationNode.h"
#endif

#ifdef OCULUS_RIFT
#include "ofxOculusRift.h"
	#ifdef CLOUDS_HUD
		#include "CloudsHUDController.h"
	#endif
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

class CloudsVisualSystem {
  public:
	   
	CloudsVisualSystem();
	virtual ~CloudsVisualSystem();
	
	enum RGBDTransitionType
	{
		TWO_DIMENSIONAL = 0,
		FLY_THROUGH = 1,
		WHIP_PAN = 2,
		RGBD = 3,
		QUESTION = 4
	};
	
	
	static ofFbo& getStaticRenderTarget(); //default
	static void forceScreenResolution(int screenWidth, int screenHeight);
	static void setNumSamples(int samples = 0);
	static void loadBackgroundShader();
    static void loadPostShader();
	static CloudsRGBDVideoPlayer& getRGBDVideoPlayer();
	static void getBackgroundMesh(ofMesh& mesh, ofImage& image, float width, float height);
    static void get2dMesh(ofMesh& mesh, float width, float height);
	
	//sound effects
	static ofSoundPlayer* getClick();
	static ofSoundPlayer* getSelectHigh();
	static ofSoundPlayer* getSelectMid();
	static ofSoundPlayer* getSelectLow();

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
	virtual void selfPostDraw(int width, int height);
    virtual void selfDrawCursor(ofVec3f& pos, bool bDragged, CloudsCursorMode mode, float focus = 1.0f, float fadeOut = 1.0f);

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
    
    virtual void drawCursors();
	
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
    
    void setupPostGui();
    void guiPostEvent(ofxUIEventArgs &e);
    
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
    
#ifdef CLOUDS_HUD
    CloudsHUDController* hud;
    void setupHUDGui();
	void guiHUDEvent(ofxUIEventArgs &e);
#endif
	
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
	virtual ofCamera& getCameraRef();
	
	ofVec3f translatedHeadPosition;
	float pointcloudScale;
	float pointcloudOffsetZ;
	CloudsRGBDCamera cloudsCamera;
	
	//LB
	ofVec3f getCameraPosition();
	ofVec3f positionOffset;
	ofColor getBgColor(){ return bgColor; }

	RGBDTransitionType getTransitionType();
	
	bool cursorIsOverGUI();
	
	void setDrawToScreen( bool state );
	bool getDrawToScreen();

	float getCurrentAudioAmplitude();
    
#ifdef CLOUDS_APP
    bool bShowPortals;
    vector<CloudsPortal> portals;
#endif

#ifdef KINECT_INPUT
    void drawKinectDebug();
#endif
	
    CloudsCursorMode primaryCursorMode;
    CloudsCursorMode secondaryCursorMode;
    
	bool isInterlude;
	bool updateInterludeInterface();
	void drawInterludeInterface();
	
#ifdef CLOUDS_INTERLUDE_NAV
	CalibrationNode resetNode;
	CalibrationNode continueNode;
	ofxFTGLFont interludeFont;
#endif
	
	float interludeNodeSize;
	ofRange interludeActivationRange;
	float interludeNodeHoldTime;
	float interludeBasePosX;
	float interludeBasePosZ;
	int interludeFontSize;
	int currentInterludeFontSize;
	float interludeTypeScale;
	float interludeTypeYOffset;
	float interludeTypeTracking;
	
	bool bPromptForInteraction;

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
    ofxUISuperCanvas *postGui;


#if defined(MOUSE_INPUT)
#elif defined(KINECT_INPUT)
    ofxUISuperCanvas *kinectGui;
#elif defined(OCULUS_RIFT)
    ofxUISuperCanvas *oculusGui;
    ofxUISuperCanvas *hudGui;
#endif
    
	void stackGuiWindows();
	void drawScene();
	
    vector<ofxUISuperCanvas *> guis;
    std::map<string, ofxUICanvas *> guimap;
	
    //Background Props
    float bgAspectRatio;
	bool bBarGradient;
    bool bWashGradient;
    float backgroundGradientExponent;
    
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
    std::map<string, ofxMaterial *> materials;
    std::map<string, ofxUISuperCanvas *> materialGuis;

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
    std::map<string, ofxLight *> lights;
    std::map<string, ofxUISuperCanvas *> lightGuis;
	
    //APP
    bool bRenderSystem;
    bool bUpdateSystem;
    bool bDebug;
    float debugGridSize;
	bool bClearBackground;
	bool bDrawToScreen;
	bool bUseOculusRift;

    //CAM
    float camDistance;
    float camFOV;
    ofxViewType view;
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
	
    std::map<ofxTLBangs*, ofxUIButton*>	tlButtonMap;
    std::map<ofxUIToggle*, ofxTLSwitches*>	tlToggleMap;
    std::map<ofxUISlider*, ofxTLCurves*>	tlSliderMap;
    std::map<ofxUINumberDialer*, ofxTLCurves*> tlDialerMap;

    ofxUILabel* durationLabel;
    
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
	
	bool hasSpeaker;
	bool confirmedDataPath;
	string cachedDataPath;
	string cachedDataPathIgnore;
	
	//speaker and quote info, constantly updated
	string speakerFirstName;
	string speakerLastName;
	string quoteName;
	
	string mainKeyword;
	vector<string> keywords;
	
	static void checkOpenGLError(string function);
	
	//TRANSITION OPTIONS
	void loadTransitionOptions();
	void setTransitionOptionGui(string type, string screenName, ofxUIEventArgs &e);
	string getTransitionOption();
	
	//INTERACTIVE CAMERA
	bool bUseInteractiveCamera, bEnableInteractiveSpin;
	float interactiveCameraMinX, interactiveCameraMaxX, interactiveCameraMinY, interactiveCameraMaxY,interactiveCameraDamping;
    
    bool bInvertCameraSpinAxis;
    float cameraSpinDamp;
    ofVec2f interactiveCameraSpinRange;
    ofVec2f interactiveCameraSpin;
    
	ofVec2f interactiveCameraRot, previousinteractiveCameraRot;
    
    //POST FX
    bool bEnablePostFX;
    float postChromaDist;
    float postGrainDist;
    
    int bleed;
    bool bDoBloom;
    float bloomAmount;
    int bloomSamples;
	
	bool bSetManualBackgroundColors;
    
};
