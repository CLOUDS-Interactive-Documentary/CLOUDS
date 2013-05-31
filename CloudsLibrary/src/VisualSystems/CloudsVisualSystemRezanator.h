#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxGenerative.h"
#include "ofxColorPalettes.h"
#include "ofxExtras.h"
#include "CloudsVisualSystem.h"
#include "ofxCameraSaveLoad.h"

#include "ofxTimeline.h"

class CloudsVisualSystemRezanator : public CloudsVisualSystem {
public:

	vector<string> getPresets();
	
	//Core Functions
    void setup();               //setup*Params -> selfSetup -> setupCoreGuis -> selfSetupGuis (Extra Guis if Needed)
    void update(ofEventArgs & args);
	void draw(ofEventArgs & args);
	void exit(ofEventArgs & args);

    //CLOUDS Specific
    void begin();
	void end();
    
    //Core Key Events
    void keyPressed(ofKeyEventArgs & args);
    void keyReleased(ofKeyEventArgs & args);
    
    //Core Mouse Events
    void mouseDragged(ofMouseEventArgs & args);
	void mouseMoved(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
        
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
    void loadGUIS();
    void loadPresetGUIS(string presetName); 
    void saveGUIS();
    void savePresetGUIS(string presetName); 
    void showGUIS();
    void hideGUIS();
    void toggleGUIS();
    void toggleGuiAndPosition(ofxUISuperCanvas *g);
    void deleteGUIS();
    
	void setCurrentCamera(ofCamera& cam);
	bool cursorIsOverGUI();
	
    //Drawing Helpers
    void drawDebug();
    void drawAxis(float size, float color);
    void drawGrid(float x, float y, float w, float h, float color);
    void billBoard(ofVec3f globalCamPosition, ofVec3f globelObjectPosition);
    void drawTexturedQuad();
    void drawNormalizedTexturedQuad();
    void drawBackground();
    void ofLayerGradient(const ofColor& start, const ofColor& end);
	
    //Rezanator SubClasses Override These Methods:
	virtual string getSystemName();

    virtual void selfSetup();
    virtual void selfSetupGuis(); 
    
    virtual void selfUpdate();
    virtual void selfDrawBackground();
    virtual void selfDrawDebug();
    virtual void selfSceneTransformation();
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
    
    virtual void selfSetupGui();
    virtual void selfGuiEvent(ofxUIEventArgs &e);

    virtual void selfSetupSystemGui();
    virtual void guiSystemEvent(ofxUIEventArgs &e);

    virtual void selfSetupRenderGui();
    virtual void guiRenderEvent(ofxUIEventArgs &e);
    
    virtual void selfSetupTimeline(); 
    virtual void selfSetupTimelineGui();
    virtual void selfTimelineGuiEvent(ofxUIEventArgs &e);    


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

    //CAM
    float camDistance;
    float camFOV;
    ofxViewType view;
	ofCamera* currentCamera;
    ofEasyCam cam;
    ofx1DExtruder *xRot;
    ofx1DExtruder *yRot;
    ofx1DExtruder *zRot;
    
    //COLORS
    ofxColorPalettes *colorPalletes;
    
    //TIMELINE
    void resetTimeline(); 
	void bindWidgetToTimeline(ofxUIWidget* widget);
    void unBindWidgetFromTimeline(ofxUIWidget* widget); 
    ofxTimeline *timeline;
    
    map<ofxTLBangs*, ofxUIButton*>	tlButtonMap;
    map<ofxUIToggle*, ofxTLSwitches*>	tlToggleMap;
    map<ofxUISlider*, ofxTLCurves*>	tlSliderMap;
    map<ofxUINumberDialer*, ofxTLCurves*> tlDialerMap;
    
    float timelineDuration; 
    bool bEnableTimeline;
    bool bDeleteTimelineTrack;
    bool bShowTimeline;
    bool bEnableTimelineTrackCreation;
};