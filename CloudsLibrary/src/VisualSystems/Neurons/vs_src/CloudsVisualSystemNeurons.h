#pragma once

/**
     growing, jiggling neurons.
     quatumblable.
 
	 @author jtnimoy
*/

#include "CloudsVisualSystem.h"

#define _C CloudsVisualSystemNeurons

namespace jtn{
	class TreeNode:public ofPoint{
	public:
		ofPoint future;
		TreeNode();
		~TreeNode();
		TreeNode*parent;
		vector<TreeNode*>children;
		void update();
		void draw();
		static vector<TreeNode*> terminals;
		int ident;
		float r,g,b,a;
		ofPoint rot;
		int generation;
		static vector<TreeNode*> all;
		int age;
		bool isTerminal();
		static GLuint drawMode;
        ofPoint direction;
        static int maxDepth;
	};
};

//--------------------------------------------------------

class _C:public CloudsVisualSystem{
 public:
	void selfSetup();
    string getSystemName();
    void selfUpdate();
    void selfDrawBackground();
    void selfDraw();
	void selfSetupGuis();
	void selfPresetLoaded(string presetPath);
    
    
	ofxUISlider *spinSlider;
	ofxUISlider *dotSizeSlider;
    ofxUISlider *nucleusSize;
    ofxUISlider *axonThicknessSlider;
    ofxUISlider *alphaSlider;
    ofxUISlider *swaySlider;
    ofxUISlider *nodeMaxSlider;
    ofxUISlider *rootCountSlider;
    ofxUISlider *danceAmpSlider;
    ofxUISlider *danceFreqSlider;
    ofxUISlider *danceOffsetSlider;

    
    ofxUIButton *resetButton;
    ofxUIButton *saveButton;
	float rotation;
	vector<jtn::TreeNode*> rootNodes;
	static float dotSize;
    static float axonThickness;
    static float alpha;
    static float sway;
    static int nodeMax;
    static int rootCount;
    static float danceAmp;
    static float danceFreq;
    static float danceOffset;
    void selfGuiEvent(ofxUIEventArgs &e);
    void reset();
    void writeToDisk(string dirname);
};

