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

#include "ofxObjLoader.h"
#include "CloudsVisualSystem.h"
#include "CloudsOrthoCamera.h"
#include "CloudsPathCamera.h"

//TODO: rename this to your own visual system
class CloudsVisualSystem3DModel : public CloudsVisualSystem {
  public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "3DModelVisualSystem";
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
	
	void updateModelTransform();
	void setupBoundingBoxVbo();
	void setupGridVbos();
	void loadShaders();
	void addToShaderMap( string name, ofShader* shader );
	
	void loadModel( string fileName, bool bSmoothMesh = false );
	void calcBoundingBox();
	void drawBoundingBox();
	string vec3ToString( ofVec3f v, int precision=3 );
	ofVec3f normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2);
	void smoothMesh( ofMesh& facetedMesh, ofMesh& targetMesh, int precision=3 );
	void facetMesh( ofMesh& smoothedMesh, ofMesh& targetMesh );
	
	void resizeTheArrowMesh( float radius, float height, float arrowBase = .25 );
	void loadCameraLineModel( ofVbo& vbo, string loc );
	void setupMultipleCameras( ofVec3f targetPos, float distance=500.f );
	void aimMultipleViews( ofVec3f targetPos );
	void drawMultipleViewCameras(float cameraScale, ofCamera* cam = NULL );
		
	void drawScene( CloudsOrthoCamera* cam = NULL, ofRectangle viewRect=ofGetCurrentViewport() );
	
	void drawScenePerspective( ofRectangle viewRect=ofGetCurrentViewport() );
	void drawScenePlan( ofRectangle viewRect=ofGetCurrentViewport() );
	void drawSceneFront( ofRectangle viewRect=ofGetCurrentViewport() );
	void drawSceneLeft( ofRectangle viewRect=ofGetCurrentViewport() );
	
	void drawSceneCamera( ofCamera* cam );
	void drawSceneGeometry(ofCamera* cam);
	

    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
	ofCamera& getCameraRef(){
		return perspCam;
	}

	//
//	ofCamera& getCameraRef(){
//		if(videoLoaded){
//			return cloudsCamera;
//		}
//		return CloudsVisualSystem::getCameraRef();
//	}

protected:
    
    //  my Stuff
	ofxUISuperCanvas* customGui;
	ofxUISuperCanvas* modelUIGui;
	ofxUISuperCanvas* gridGui;
	ofxUISuperCanvas* cameraViewsGui;
	ofxUISuperCanvas* cameraPathsGui;
	ofxUISuperCanvas* fogGui;
	ofxUISuperCanvas* transformGui;
	
	bool videoLoaded;
	ofImage someImage;
	ofShader pointcloudShader;
	ofVboMesh simplePointcloud;
	
	ofImage colorMap;
	
	vector<string> shaderNames;
	map<string, ofShader*> shaderMap;
	ofShader* activeShader;
	ofShader normalShader;
	ofShader facingRatioShader;
	ofShader phongShader;
	ofShader XRayShader;
	ofShader simpleShader;
	
	float gridLineWidth, gridScale, majorGridLineWidth, gridDim, fogFalloffDistance, fogFalloffExpo, fogFalloffScale, gridAlphaScale, gridMajorScale;
	int numGridVertices, numGridMajorVertices;
	ofVbo grid;
	ofShader gridShader;
	ofVbo gridMajor;
	ofColor gridColor, gridMajorColor;
	float gridAlpha, gridMajorAlpha, gridBrightness, gridMajorBrightness;
	
	ofVbo model;
	ofShader* modelShader;
	ofVec3f modelPos, modelScl;
	ofQuaternion modelRot;
	ofNode modelTransform;
	ofVec3f minBound, maxBound, boundCenter;
	ofVbo boundBoxVbo;
	float boundBoxLineWidth, wireframeLinewidth;
	float maxDim, modelScale;
	bool bSmoothModel, bWireframe;
	ofColor modelColor;
	float specularExpo;
	float specularScale;
	
	
	vector <string> objFiles;
	vector <string> cameraPaths;
	
	ofVbo modelNormalInfo;
	ofShader modelNormalInfoShader;
	
    ofVboMesh modelMesh;
	float discardThreshold;
	
	
	//3d modeling-ish widgetty type things
	
	//xyz arrows
	ofVboMesh arrowMesh;
	ofVec3f arrowScale;
	float arrowRadius, arrowHeight, arrowPointHeight;

	//multiple view cameras
	ofVbo cameraLines;
	int cameraLinesNumVertices;
	float cameraLineWidth, cameraLineScale;
	string singleViewName;
	ofCamera* currentSingleCam;
	
	ofNode persptiveCameraNode;
	CloudsOrthoCamera perspCam;
	ofColor perspCamColor;
	
	ofNode leftCameraNode;
	ofColor leftCamColor;
	CloudsOrthoCamera leftCam;
	
	ofNode planCameraNode;
	ofColor planCamColor;
	CloudsOrthoCamera planCam;
	
	ofNode frontCameraNode;
	ofColor frontCamColor;
	CloudsOrthoCamera frontCam;
	
	bool bFourView;
	
	bool bLeftCamIsActive, bFrontCamIsActive, bPlanCamIsActive, bPerspCamIsActive;
	
	bool bDrawBoundingBox, bDrawArrows, bDrawCameras, bDrawGrid;
	
	
	ofVec3f positionOffset;//Eular angles
	ofVec3f globalRotation;//Eular angles
	ofVec3f globalRotationVelocity;//multiplied against elapsed time
	ofVec3f accumulatedRotation;
	
	CloudsPathCamera pathCamera;
	float pathCameraPosition;
	bool bUseDuration, bDrawCameraPath;
	
	bool bRepositionModel;
	
	bool bCenterModel, bAutoScale,bDoNotScaleModel;

};
