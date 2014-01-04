
#pragma once

#include "ofMain.h"
#include <string>
#include <map>
#include <vector>

#include "CloudsVisualSystem.h"
#include "MSAPhysics3D.h"

typedef struct{
	bool enabled;
	ofLight light;
	ofFloatColor color;
	float currentRot;
	float spinRadius;
	ofVec3f spinAxis;
	float spinSpeed;
	ofVec3f position;
	ofVec3f dir;
} AuxLight;

using namespace msa::physics;

typedef struct{
	int id;
	int state;
	int stateCnt;
	int gridx;
	int gridy;
	bool isEdge;

	ofVec3f orig;
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


  protected:
	static const int FREE=0,FIXEDSTATIC=1,FIXEDMOVING=2;
	static const int GRIDRECT=1,GRIDCIRC=0,GRIDCYL=2;

  	vector<MWParticle> pp;
	vector<MWParticle> ppActive;
	float fpsMod,activityCnt,stickyNum;
	float clothWidth;
	float clothHeight;
	float colorIndex;
	float gridSizeF;

	float lastFixY;

	int gridSize;
	int gridType;
	

	void mwUpdate();
	void mwLights();

	ofVec3f mwOutlineShape(ofVec3f &v);
	bool mwIsEdge(MWParticle &pt);
	void mwNewActivity(MWParticle& pt,signed int state);
	void mwFix(MWParticle &pt);

	void mwMakeParticle(int x,int y,ofVec3f &o);
	MWParticle &mwGetParticle(signed int id,bool fromEdge,bool addToActive);
	void mwPreset();

	void mwGenerate();
	void mwCreateLights();
	void mwGridSticky();


	ofxUISuperCanvas* clothGui;
	ofxUISuperCanvas* auxLightGuis[4];
	
	
	void clearElements();
	
	bool shouldRegenerateMesh;
	void generateMesh();

	float springStrength;
	float springDampening;
	float springExtend;
	
	AuxLight auxLights[4];
	
	ofVboMesh mesh;
	World3D physics;
	map<ofIndexType, Particle3D*> meshIndexToParticle;
	map<Particle3D*, vector<ofIndexType> > particleToMeshIndices;
	
	void updateNormals();
	


	vector<vector<Particle3D*>> particles;
	
	//color generators
	vector<ofColor> initColors(int row);
	vector<ofColor> colors;

	ofPixels colorPalettes;
};
