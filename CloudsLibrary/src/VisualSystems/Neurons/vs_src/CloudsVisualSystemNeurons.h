#pragma once

/**
     growing, jiggling neurons.
     quatumblable.
 
	 @author jtnimoy
*/

#include "CloudsVisualSystem.h"
#include "CloudsPathCamera.h"

#define _C CloudsVisualSystemNeurons

namespace jtn{
    
#pragma mark
#pragma mark TreeNode
    
	class TreeNode:public ofPoint{
	public:
        
		ofPoint future;
        
        /**
            construct raw.
         */
        
		TreeNode();
        
        /**
            construct me by reading in a properly cue'd ifstream.
         */
        TreeNode(ifstream &fin);
		~TreeNode();
		TreeNode*parent;
		vector<TreeNode*>children;
		void update();
        void updateScreenSpace(ofCamera &cam);
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
        void serialize(ofstream &fout);
        void updateMaxDepth();
        ofVec3f screenSpace;
        bool isPartOfCamPath;
        static void clearPathFlags();
	};

//--------------------------------------------------------
#pragma mark
#pragma mark vecD

/**
    equiv to ofVec3d, where d means double. because i was getting floating point weirdness
 */

class PointD{
public:
    double x,y,z;
    PointD();
    PointD(double xx,double yy,double zz);
    PointD(ofVec3f copyable);
    PointD operator-(PointD that);
    PointD operator/(PointD that);
    operator string();
};

    
//--------------------------------------------------------
#pragma mark
#pragma mark Box
    
    class Box{
    protected:
        jtn::PointD _upper;
        jtn::PointD _lower;
    public:
        Box();
        /**
            populate upper and lower with most unlikely
            condendors for looping comparison
         */
        void setOppositeExtremes();
        
        /**
            stretch the box out by 1 value
         */
        void stretch(ofVec3f that);
        
        /**
            normalize a value scaled relative to this bounding box
         */
        jtn::PointD getNormalized(jtn::PointD that);
        jtn::PointD upper();
        jtn::PointD lower();
    };
};

//--------------------------------------------------------
#pragma mark

class _C:public CloudsVisualSystem{
 public:
	void selfSetup();
    string getSystemName();
    void selfUpdate();
    void selfDrawBackground();
    void selfDraw();
	void selfSetupGuis();
	void selfPresetLoaded(string presetPath);
    void selfSetDefaults();
    
    ofxUISuperCanvas* nCamGui;
    
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
    ofxUIButton *loadButton;
    
    ofxUIButton *generateCamPath;
    ofxUIButton *generateRandCam;
   // ofxUISlider *camDuration;
    ofxUIButton *tumbleCam;
    
    static bool renderNeurons;
    
    
    bool quatCam;
    bool flythrough;
    bool bounce;
    float camDuration;
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
    
    
    static float colorMix;
    static bool renderCamPath;
    
    void selfGuiEvent(ofxUIEventArgs &e);
   // void guiCameraEvent(ofxUIEventArgs &e);
    
    void reset(bool createRootNodes = true);
    
    /**
        write serialized neurons to a presets file.
        pointers are flattened to global array indices
        equivalent to i in _N::all[i]
     */
    void writeToFile(string dirname);
    
    /**
        read in a file from disk
        and un-flatten array indices
        back into _N *pointers
     */
    void readFromFile(string dirname);
    
    /**
        animated camera path.
        thanks @lars
     */
    CloudsPathCamera cloudsPathCam;
    /**
        generate a path along a dendrite chain
     */
    void generateFlythrough();
    
    /**
        make a series of random positions and
        add them to the camera path.
     
        hopefully more fun than flythrough
     */
    void generateRandCamBounce();
    
    /**
        overloaded to include path travel
     */
    ofCamera& getCameraRef();
    
    /**
        compute lower and upper bounds on all 3 axes
     */
    void updateBoundingBox();
    /**
        storing bounding box info here
     */
    static jtn::Box boundingBox;
    
};

