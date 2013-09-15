

#pragma once

#include "ofMain.h"
#include "CloudsClip.h"
#include "ofxUI.h"

class CloudsQuestion {
  public:
	
	CloudsQuestion();
		
	CloudsClip clip; //must have a question
	string topic;
	string question;
	
	ofCamera* cam;
	ofTrueTypeFont* font;
	
	bool falloff;
	float falloffDistance;
	
	float maxRadius;
	float minRadius;
	float tickRadius;
	
	float charsPerSecond;
	float secondsToConsiderSelected;
	
	void setup();
	
	static ofShader shader;
	static void startShader();
	static void endShader();
	static void reloadShader();
	static void addQuestionVariables(ofxUISuperCanvas* gui);
	
	float birthTime;
	float expandPercent;
	float selectPercent;
	
	void draw();
	void drawOverlay();
	void update();
	
	void startHovering();
	void stopHovering();
	bool isSelected();
	bool hovering;
	
	float radius;
	ofVec3f position;
	ofQuaternion currentRot;
	
	void mouseMoved(ofMouseEventArgs& args);
	void mousePressed(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);

	ofMesh geometry;
	ofMesh progressRing;
	
	ofMesh sphereGeo;
	
	ofMesh dottedCircle;
	ofVec2f currentScreenPoint;

	//intro question variables
	bool introQuestion;
	void orientToCenter();
	
	
  protected:
	
	float hoveringStartTime;
	
	bool isSetup;

	float screenRadius;
};