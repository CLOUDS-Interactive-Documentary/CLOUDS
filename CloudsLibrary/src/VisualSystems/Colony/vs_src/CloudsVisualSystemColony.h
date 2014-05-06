/* 
 WOW SUCH VISUAL
 VERY SYSTEM
 MUCH COPYRIGHT
 */


#pragma once

#include "CloudsVisualSystem.h"

#include "gCell.h"
#include "CloudsGlobal.h"
#include "CloudsAudioEvents.h"
//#include "ofxTonic.h"
#include "TonicSample.h"

//using namespace Tonic;

class CloudsVisualSystemColony : public CloudsVisualSystem {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetDefaults();
    void selfSetupGuis();
    
    void selfAutoMode();
    void selfUpdate();
    void selfDrawBackground();
    void selfDraw();
    void selfDrawDebug();
    void selfSceneTransformation();
    
    void selfExit();
    void selfBegin();
    void selfEnd();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void mouseDragged(ofMouseEventArgs & args);
    void mouseMoved(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void selfPresetLoaded(string presetPath);
    
    
private:
    
    /* SUCH PARAMS */
    cellParams params;
    bool levelSetMode, levelSetBG;
    float levelSetShadowAmt;
    int numInitialCells;
    
    float translucenseCell, translucenseDish;
    ofVec4f kernelColor_high, kernelColor_low;
    float kernelShadowAmt;

    float stippleScale;
    ofVec4f stippleColor;


    /* MUCH PRIVATE */
    float kernelMaxValue;
    
    /* MUCH GEOMETRY */
    ofVboMesh   vbo;
    
    /* WOW VIDEO */
    ofFbo fbo_main;
    ofTexture sprite, backgroundTexture;
    ofShader    levelSet;
    ofShader    billboard;
    string backgroundFilename;
    vector<string> backgroundFilenames;
    
    /* LOL DATA */
    colonyPartitionMap pMap;
    vector< cellPtr > cells;
    
    /* SO GUI */
    ofxUISuperCanvas* guiDynamics;
    ofxUISuperCanvas* guiLooks;
    ofxUISuperCanvas* guiBackground;
    
    /* ALL THE UTILITY */
    void updateFoodTexture();
    bool areFbosAllocatedAndSized();
    void loadShaders();
    void loadTexture(string);
    void loadTextureAndUpdateUI(string);
    void reallocateFramebuffers();
    
    void clear();
    void populate();
    
    /* VERY DEBUG */
    ofImage img_debug;
    
    /* SOUND LVL=100 */
    ofxUISuperCanvas* soundGui;
<<<<<<< HEAD
	vector<string> soundFiles;
    //string soundFiles[3] = {"granular_water2.aif",
    //    "granular_water2_slow.aif",
    //    "Grains1_slow_low.aif"};
    bool playSample[3];
    Tonic::ControlTrigger soundTriggers[3];
    Tonic::ofxTonicSynth synth;
    Tonic::Generator buildSynth();
    Tonic::ControlParameter volumeControl;
=======

    vector<TonicSample> tonicSamples;
    
//    string soundFiles[3] = {"granular_water2.aif",
//        "granular_water2_slow.aif",
//        "Grains1_slow_low.aif"};
//    bool playSample[3];
//    ControlTrigger soundTriggers[3];
    ofxTonicSynth synth;
    Generator buildSynth();
    ControlParameter volumeControl;
>>>>>>> master
    float gain;
	void audioRequested(ofAudioEventArgs& args);
};
