
#pragma once

#include "ofMain.h"

#include "CloudsVisualSystem.h"
#include "MSAPhysics3D.h"

typedef struct{
	bool enabled;
	ofLight light;
	float currentRot;
	float spinRadius;
	ofVec3f spinAxis;
	float spinSpeed;
	ofVec3f position;
} AuxLight;

using namespace msa::physics;

typedef struct{
	int state;
	int stateCnt;
	int gridx;
	int gridy;
	ofVec3f vD;
	Particle3D* p;
} MWParticle;


class CloudsVisualSystemVerletForm : public CloudsVisualSystem {
  public:
	
	CloudsVisualSystemVerletForm();
	
	string getSystemName();
	
    void selfSetup();
    void selfSetupGuis();
    void selfPresetLoaded(string presetPath);
	
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    void selfDraw();
    void selfExit();
    void selfBegin();
	void selfEnd();
	
	
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
	
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
	
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
    void selfSetupTimeline();
    void selfSetupTimelineGui();
    void selfTimelineGuiEvent(ofxUIEventArgs &e);

	float currentRotAngle;
	float currentCamDistance;
	//this makes it work with clouds!
	ofCamera& getCameraRef(){
		return cam;
	}
	
  protected:

	ofCamera cam;
  	vector<MWParticle*> pp;
	vector<MWParticle*> ppActive;
	float fpsMod,activityCnt;

	void mwUpdate();
	void mwNewActivity(int id,int state);
	void mwFix(MWParticle &pt);
	void mwMakeParticle(Particle3D* pt,int x,int y);

	std::vector< std::vector<ofVec3f> > mwGrid(int type);
	void mwGridSticky();
	void mwGridPt (ofVec3f &o,int type);
	int mwGetRndID(int edge);

	float rndExtend ();


	ofxUISuperCanvas* clothGui;
	ofxUISuperCanvas* auxLightGuis[3];
	
	
	float clothWidth;
	float clothHeight;
	float colorIndex;
	float gridSize;
	
	void clearElements();
	
	bool shouldRegenerateMesh;
	void generateMesh();
	
	float springStrength;
	float springDampening;
	float springExtend;
	
	AuxLight auxLights[3];
	
	ofVboMesh mesh;
	World3D physics;
	map<ofIndexType, Particle3D*> meshIndexToParticle;
	map<Particle3D*, vector<ofIndexType> > particleToMeshIndices;
	
	void updateNormals();
	


	vector< vector<Particle3D*> > particles;
	
	//color generators
	vector<ofColor> initColors(int row);
	vector<ofColor> colors;

	ofPixels colorPalettes;
};