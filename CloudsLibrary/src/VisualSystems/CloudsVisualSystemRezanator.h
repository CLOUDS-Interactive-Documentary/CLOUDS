#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxGenerative.h"
#include "ofxExtras.h"
#include "CloudsVisualSystem.h"
#include "ofxCameraSaveLoad.h"

class CloudsVisualSystemRezanator : public CloudsVisualSystem {
public:

    //Data Folder Path
    string getVisualSystemDataPath();

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

    void setupMaterial(string name, ofMaterial *m);
    void guiMaterialEvent(ofxUIEventArgs &e);

    void setupPointLight(string name);
    void setupSpotLight(string name);
    void setupBeamLight(string name);
    void setupGenericLightProperties(ofxUISuperCanvas *g, ofxLight *l);
    void guiLightEvent(ofxUIEventArgs &e);
        
    //Lighting Helpers
    void lightsBegin();
    void lightsEnd();
    
    //UI Helpers
    void loadGUIS();
    void saveGUIS();
    void showGUIS();
    void hideGUIS();
    void toggleGUIS();
    void toggleGuiAndPosition(ofxUISuperCanvas *g);
    void deleteGUIS();
    
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
        
protected:

    //UI
    ofxUISuperCanvas *gui;
    ofxUISuperCanvas *sysGui;
    ofxUISuperCanvas *rdrGui;
    ofxUISuperCanvas *bgGui;
    ofxUISuperCanvas *lgtGui;
    ofxUISuperCanvas *camGui;
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
    ofEasyCam cam;
    ofx1DExtruder *xRot;
    ofx1DExtruder *yRot;
    ofx1DExtruder *zRot;

};