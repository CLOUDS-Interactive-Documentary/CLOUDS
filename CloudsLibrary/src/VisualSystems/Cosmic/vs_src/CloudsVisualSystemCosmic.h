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
#define COSMIC_SHADER_TYPE ofShader
//#define COSMIC_SHADER_TYPE ofxAutoReloadedShader

#include "CloudsVisualSystem.h"
#include "ofxColorPalettes.h"
#include "ofxAutoReloadedShader.h"

//TODO: rename this to your own visual system
class CloudsVisualSystemCosmic : public CloudsVisualSystem {
  public:
	CloudsVisualSystemCosmic();

    string getSystemName(){
		return "Cosmic";
	}
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void selfSetupCameraGui();
    void guiRenderEvent(ofxUIEventArgs &e);

	void selfSetup();
	void selfBegin();
	void selfPresetLoaded(string presetPath);
    void selfSceneTransformation();
	void selfUpdate();
    void selfDraw();

    // draw any debug stuff here
	void selfDrawDebug();
	
	void selfDrawBackground();
	void selfEnd();
    void selfExit();
    void selfSetDefaults();
    
    void selfKeyPressed(ofKeyEventArgs & args);

    int ID(int x, int y); 
    void setupFboViewerGui(string name, ofFbo *fbo);
    void addSphereShaderGui(string name);
    void addHomeShaderGui(string name);
    void addElectroShaderGui(string name);    
    void addAttractorShaderGui(string name);
    void addCurlNoiseShaderGui(string name); 
    
    void setupFloorVbo(); 
    void clearFbo(ofFbo &fbo);
    
    void updateRadiusShader();
    void applyHomeShader();
    void applyElectroShader();
    void applyAttractorShader();
    void applySphereShader();
    void applyCurlNoiseShader(); 
    
    void updateAcceleration();
    void updateVelocity();
    void updatePosition();

    void drawParticles();
    void drawFloor();
    
    void drawSphereDebug();
    void drawAttractorDebug();    
    
    void addGui(ofxUISuperCanvas *g);
    void selfSetupGuis();    
    
    void selfSetupTimeline();
    
protected:
	void clear();
	bool vbosAllocated;

    int numTris;
    int numIndi;
    int rows;
    int cols;
    int size;
    
    float *pos;
    float *vel;
    
    ofVbo vboFloor;
    
    ofFbo homeFbo;
    ofFbo radiFbo;
    
    ofFbo posFboSrc;
    ofFbo posFboDst;
    
    ofFbo velFboSrc;
    ofFbo velFboDst;
    
    ofFbo accFboSrc;
    ofFbo accFboDst;
    
    ofFbo electroFbo;

    COSMIC_SHADER_TYPE accShader;
    COSMIC_SHADER_TYPE velShader;
    COSMIC_SHADER_TYPE posShader;
    COSMIC_SHADER_TYPE rdrShader;
    COSMIC_SHADER_TYPE radShader;
    
    bool bUpdateRadius;
    bool bUpdateAcceleration;
    bool bUpdateVelocity;
    bool bUpdatePosition;
    
    float timeStep;
    float accLimit;
    float velLimit;
    float damping;
    float time;
    
    float radiusMultiplier;
    float particleAlpha;

    ofxColorPalettes *colorPalettes;
    float colorIndex;
    ofFloatColor *floorColors;    
    
    //Sphere Shader
    COSMIC_SHADER_TYPE sphereShader;
    vector<bool *> sphereActive;
    vector<float *> sphereRadius;
    vector<float *> sphereAccLimit;
    vector<ofVec3f *> spherePosition;
    
    //Home Shader
    //ofxAutoReloadedShader homeShader;
    ofShader homeShader;
    bool bHomingActive;
    float homeForceLimit;
    
    //Electro Shader
    COSMIC_SHADER_TYPE electroShader;
    bool bElectroActive;
    float electroForceLimit;
    
    //Attractor Shader
    COSMIC_SHADER_TYPE attractorShader;
    vector<bool *> attractorActive;
    vector<float *> attractorPower;
    vector<float *> attractorExpPower;
    vector<float *> attractorAccLimit;
    vector<ofVec3f *> attractorPosition;
    
    //Floor Shader
    COSMIC_SHADER_TYPE floorShader;
    int floorIndexSize;
    float shadowScale;
    float shadowOpacity;
    
    //Curl Noise Shader
    COSMIC_SHADER_TYPE noiseShader;
    bool bNoiseActive;
    float noiseForceLimit;
    float noiseScale;
    
    ofRange clipPlanes;
};
