#pragma once

#include "ofMain.h"
#include "CloudsVisualSystemClusterMap.h"

class testApp : public ofBaseApp {
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

	CloudsVisualSystemClusterMap visualSystem;
	
//	bool rendering;
//	int frameNum;
	
	/*
	ofxPanel gui;
	ofxIntSlider seed;
	ofxIntSlider heroNodes;
	ofxIntSlider numIterations;
	ofxFloatSlider heroRadius;
	ofxFloatSlider heroRadiusVariance;
	ofxIntSlider numBranches;
	ofxFloatSlider minDistance;
	ofxFloatSlider distanceRange;
	ofxFloatSlider stepSize;
	ofxFloatSlider lineThickness;
	ofxFloatSlider lineAlpha;
	ofxFloatSlider minAttractRadius;
	ofxFloatSlider minRepelRadius;
	ofxFloatSlider minFuseRadius;
	
	ofxFloatSlider lineStartTime;
	ofxFloatSlider lineEndTime;
	ofxIntSlider lineFadeVerts;
	ofxFloatSlider lineBlurAmount;
	ofxFloatSlider lineBlurFade;
	
	ofxFloatSlider maxAttractForce;
	ofxFloatSlider maxRepelForce;
	
	ofxFloatSlider maxTraverseDistance;
	ofxFloatSlider traverseNodeWeight;
	ofxFloatSlider traverseStepSize;

	ofxIntSlider numSurvivingBranches;
	ofxIntSlider numPointsAtReplicate;
	
	ofxFloatSlider replicatePointDistance;
	ofxFloatSlider replicatePointSize;
	ofxFloatSlider maxTraverseAngle;

	ofxIntSlider nodePopLength;
	*/

	/*
	int seed;
	int heroNodes;
	int numIterations;
	float heroRadius;
	float heroRadiusVariance;
	float numBranches;
	float minDistance;
	float distanceRange;
	float stepSize;
	float lineThickness;
	float lineAlpha;
	float minAttractRadius;
	float minRepelRadius;
	float minFuseRadius;
	
	float lineStartTime;
	float lineEndTime;
	int lineFadeVerts;
	float lineBlurAmount;
	float lineBlurFade;
	
	float maxAttractForce;
	float maxRepelForce;
	
	float maxTraverseDistance;
	float traverseNodeWeight;
	float traverseStepSize;
	
	int numSurvivingBranches;
	int numPointsAtReplicate;
	
	float replicatePointDistance;
	float replicatePointSize;
	float maxTraverseAngle;
	
	int nodePopLength;

	ofxTLCameraTrack camTrack;
	ofxGameCamera cam;
	ofFbo renderTarget;
	ofFbo lineBlurTarget;
	
	ofxTimeline timeline;
	ofRectangle fboRect;
	ofxTLColorTrack* lineColor;
	
	vector<Node*> nodes;
	vector<ofVec3f> fusePoints;
	ofVboMesh geometry;
	//fuzzy points
	ofVboMesh nodeCloudPoints;
	//for drawing the line
	ofVboMesh traversal;
	
	ofxTLCurves* nodeBounce;
	//for drawing the node graphics
	vector<Node*> traversedNodes;
	map<ofIndexType,ofIndexType> traversalIndexToNodeIndex;
	ofVboMesh traversedNodePoints;
	
	void traverse();
	void generate();
	ofVec3f trailHead;
	ofShader billboard;
	ofShader lineAttenuate;

	ofShader gaussianBlur;
	string renderFolder;
	ofImage nodeSprite;
	ofImage nodeSpriteBasic;
	void loadShader();
	*/
};
