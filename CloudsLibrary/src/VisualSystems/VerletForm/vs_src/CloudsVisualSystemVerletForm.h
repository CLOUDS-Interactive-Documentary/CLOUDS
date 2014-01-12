
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
	ofColor color,colorGoal;
	ofFloatColor cDiff,cSpec;

	int cnt,cntGoal;
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
	signed int stateCnt;
	int gridx;
	int gridy;
	int colID;
	int vertID;

	bool isEdge;
	float speed;
	ofVec3f orig,goal,gridV;
	ofVec3f vD,v;
	Particle3D* p;
} MWParticle;

typedef struct{
	int type;
	float h[7];
} MWTerrain;


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

    ofVec3f currentRotAngle;
	float currentCamDistance;
	//this makes it work with clouds!
	ofCamera& getCameraRef(){
			return cam;
	}

  protected:

	bool doAutoGenerate,doLoRes;

    ofCamera cam;
    ofVec3f camC,camRot,camRotGoal;
    ofVec3f grav,gravGoal;
    float camSpeed;
    int gravCnt,gravCntGoal;

    ofVec3f camCenterOffs,mousePos;
    ofVec3f modelRot;
    float modelRotD[3],modelRotMax,modelTrans;
	bool camEnabled,colorLightEnabled;

	ofFloatColor cWhite,cBlack,cGray;
	ofColor cWhiteRGB,cBlackRGB,cGrayRGB;

	static const bool MWDEBUG=true;
	static const int FREE=0,FIXEDSTATIC=2,FIXEDMOVING=1;
	static const int GRIDRECT=1,GRIDCIRC=0,GRIDCYL=2;
	static const int LIGHTS=4;

	bool lineMesh;

  	vector<MWParticle> pp;
	vector<MWParticle> ppActive;
	float fpsMod,stickyNum;
	int activityCnt;
	float clothWidth;
	float clothHeight;
	float colorIndex,colorMod,colorStrategyF;
	int colorStrategy;

	ofxUILabel* bbLabel[4];
	ofVec3f bbMin,bbMax,bbC,meshCentroid;
	float bbDim,bbMult,bbMultTmp;

	bool gridDoStitch;
	int gridSize;
	int gridType;
	int fixCnt;
	float gridSizeF,gridTypeF;
	float vnormalMod;
	float activityMod;
	

	void mwUpdate();
	void mwUpdateCamera();
	void mwMeshBB();
	void mwLights();
	void mwNewLightColor();

	void mwFix(MWParticle &pt,bool fix);


	ofVec3f mwNewMove(MWParticle& pt);
	void mwNewGravity();
	void mwNewLightColor(AuxLight &a);

	ofVec3f mwOutlineShape(ofVec3f &v);
	bool mwIsEdge(MWParticle &pt);
	void mwNewActivity(MWParticle& pt,signed int state);


	void mwMakeParticle(int x,int y,ofVec3f &o);
	MWParticle &mwGetParticle(bool fromEdge);
	void mwPreset();

	void mwGenerate();
	void mwCreateLights();
	void mwGridSticky();

	string hexStr(ofColor &c);

	ofColor rndColor();
	void shiftHue(ofColor &cc,float mod);
	float wraplerp(float a, float b, float t, float w);


	bool rndBool(float prob);
	float rndSigned(float a,float b);
	float bezierPoint(float a, float b, float c, float d, float t);
	float bezierPoint(float a[], float t);
	ofVec3f terrainMod(ofVec3f &v);

	ofxUISuperCanvas* clothGui;
	ofxUISuperCanvas* auxLightGuis[LIGHTS];
	
	
	void clearElements();
	
	bool shouldRegenerateMesh;
	unsigned int lastGenerated;

	void generateMesh();

	float springStrength;
	float springDampening;
	float springExtend;
	
	AuxLight auxLights[LIGHTS];
	
	ofVboMesh mesh;
	World3D physics;
	map<ofIndexType, MWParticle> meshIndexToMWParticle;
	map<ofIndexType, Particle3D*> meshIndexToParticle;
	map<Particle3D*, vector<ofIndexType> > particleToMeshIndices;
	
	void updateNormals();
	


	vector<vector<Particle3D*> > particles;
	
	//color generators
	void initColors(int row,int cnt);
	vector<ofColor> colors;

	ofPixels colorPalettes;
};
