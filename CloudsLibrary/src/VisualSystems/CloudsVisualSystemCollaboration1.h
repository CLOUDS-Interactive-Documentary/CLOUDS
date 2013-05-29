#pragma once

/**
	 introductory example of subclassing Rezanator.
	 spinning square is drawn with 1 spinning slider.
	 
	 @author jtnimoy
*/

#include "CloudsVisualSystemRezanator.h"

#define _C CloudsVisualSystemCollaboration1

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

class _C:public CloudsVisualSystemRezanator{
 public:
	void selfSetup();
    string getSystemName();
    void selfUpdate();
    void selfDrawBackground();
    void selfDraw();
	void selfSetupGuis(); 
	ofxUISlider *spinSlider;
	ofxUISlider *dotSizeSlider;
    ofxUISlider *nucleusSize;
    ofxUISlider *axonThicknessSlider;
    ofxUISlider *alphaSlider;
    ofxUISlider *swaySlider;
    ofxUISlider *nodeMaxSlider;
    ofxUIButton *resetButton;
    ofxUISlider *rootCountSlider;
	float rotation;
	vector<jtn::TreeNode*> rootNodes;
	static float dotSize;
    static float axonThickness;
    static float alpha;
    static float sway;
    static int nodeMax;
    static int rootCount;
    void selfGuiEvent(ofxUIEventArgs &e);
    void reset();
};

