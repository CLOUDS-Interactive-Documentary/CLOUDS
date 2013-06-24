#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "CloudsVisualSystemRezanator.h"


typedef struct{
	ofVec3f pos;
	int index;
	int frame;
	bool dead;
} Particle;

class CloudsVisualSystemVectorFlow : public CloudsVisualSystemRezanator {
public:
	
	CloudsVisualSystemVectorFlow();
	
	string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
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
	
protected:
	
	vector<Particle> particles;
	
	int maxVertices;
	ofVboMesh particleMesh;
	ofVboMesh lines;
	int trailLength;
	
	void addParticle();
	void initParticle(Particle& p);
	
	ofVec3f getDirection(float x, float y);
	float getMagnitude(float x, float y);
	
	void initFlowField();
	bool regenerateFlow;
	float step;
	float chaos;
	int width, height;
	float speed;
	float maxLength;

	float fieldAlpha;
};