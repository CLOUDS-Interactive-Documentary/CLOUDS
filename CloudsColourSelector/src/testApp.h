#pragma once

#include "ofMain.h"
#include "CloudsFCPParser.h"
#include "ofxUI.h"
#include "ofxTween.h"

class testApp : public ofBaseApp{
  public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);  
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    void checkColorDistance();
    float weightedDistance(ofVec3f& pnt1 ,ofVec3f& pnt2,ofVec3f& weights);
	void exit();
    
    
    CloudsFCPParser parser;
    ofVideoPlayer player;
    ofImage img;
    float threshold;
    float thresholdLower;
    float thresholdUpper;
    ofVec2f samplePoint;
    ofVec2f samplePointNorm;
    ofFloatColor samplePointColor;
    ofxEasingSine easing;
    float hueWeight;
    float satWeight;
    float brightWeight;
    
    float hueThreshold;
    float satThreshold;
    float brightThreshold;
    
    ofxUISuperCanvas *gui;
	void guiEvent(ofxUIEventArgs &e);
    ofShader shaderSkinDetection;
    ofShader shaderBlurX;
    ofShader shaderBlurY;
    ofFbo fboSkinDetect;
    ofFbo fboBlurOnePass;
    ofFbo fboBlurTwoPass;
    
    bool blur;
    float blurAmount;
    
    ofFbo imgFbo;
    
};
