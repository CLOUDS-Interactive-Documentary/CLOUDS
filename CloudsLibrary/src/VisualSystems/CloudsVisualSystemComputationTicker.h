#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "CloudsVisualSystem.h"

/** 
 * COMPUTATION visual system
 * shows ticker tape strands running in fields
 * that can be zoomed in and diverted
 */

class CloudsVisualSystemComputationTicker : public CloudsVisualSystem {
  public:
	
	virtual void setup();
	virtual string getSystemName();
	
	//these events are registered only when running the simulation
	virtual void update(ofEventArgs & args);
	virtual void draw(ofEventArgs & args);
	
	//INTERACTION EVENTS -- registered only
	virtual void keyPressed(ofKeyEventArgs & args);
	virtual void keyReleased(ofKeyEventArgs & args);
	
	virtual void mouseDragged(ofMouseEventArgs & args);
	virtual void mouseMoved(ofMouseEventArgs & args);
	virtual void mousePressed(ofMouseEventArgs & args);
	virtual void mouseReleased(ofMouseEventArgs & args);
		
	//application exit, clean up and don't crash
	virtual void exit(ofEventArgs & args);
	
  protected:

	//called when showing the visual system, and to end it
	virtual void begin();
	virtual void end();

	int width, height;
	ofVboMesh mesh;
	vector<int> startIndeces;

	void regenerateParticles();
		
	ofImage speedTexture;
	ofImage shiftTexture;
	
	void reloadShaders();
	ofShader updateShader;
	ofShader drawShader;
	
	ofVboMesh offsetMesh;
	ofFbo sourceOffset,targetOffset;
	
	ofFbo fbo;
	
	float pathDeviation;
	
	float speed;
	float scale;
	float pointSize;
	float deviation;
	float colorflip;
	
	bool debug;
	bool regenerate;
	ofxUICanvas* gui;
};