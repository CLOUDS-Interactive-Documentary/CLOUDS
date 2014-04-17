

#pragma once

#include "ofMain.h"
#include "CloudsClip.h"
#include "ofxUI.h"
#include "ofxFTGL.h"

class CloudsQuestion {
  public:
	
	CloudsQuestion();
		
	CloudsClip clip; //must have a question
	string topic;
	string question;
	
	ofCamera* cam;
	ofxFTGLSimpleLayout* font;
	
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
	
	//HACK!!!!
	vector<string> testFiles;
	vector<ofImage> testImages;
	int lastQuestionSwitchedFrame;
	int currentTestFileIndex;
	float testImageSize;
	////UNHACK
	
	//hack for now to make oculus q bigger
	float enlarge;
	
	void draw();
	void drawOverlay(bool anchorToScreen = false);
	void update();
	
	void enableHover();
	void disableHover();
	
	
	void startHovering();
	void stopHovering();
	bool isSelected();
	bool hovering;
	void destroy();
	bool isDestroyed;
	float destroyFadeoutTime;
	float destroyedStartFadeTime;
	float radius;
	bool lockHover;
	
	ofVec3f position;
	ofQuaternion currentRot;
	
	void mouseMoved(ofMouseEventArgs& args);
	void mousePressed(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);

	ofMesh geometry;
	ofMesh progressRing;
	
	ofMesh dottedCircle;
	ofVec2f currentScreenPoint;

	//intro question variables
	bool introQuestion;
	void orientToCenter();
	
  protected:
	
	bool hoveringEnabled;
	float hoveringStartTime;
	
	bool isSetup;

	float screenRadius;
};