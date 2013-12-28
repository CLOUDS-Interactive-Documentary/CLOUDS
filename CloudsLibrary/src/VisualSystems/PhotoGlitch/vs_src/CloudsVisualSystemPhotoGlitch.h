//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
// Welcome to the EMPTY CloudsVisualSystem
//
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "ofxTween.h"

class PGCell
{
public:
    int idx;
    int row, origRow;
    int col, origCol;
    ofFloatColor avgColor;
    ofxTween tweenX, tweenY;
};

enum sortMode {
    SOURCE_MODE,
    TARGET_MODE
};

struct compareObj {
    PGCell * cell;
    int index;
} ;

struct glitchParams{
    bool enable = false;
    bool sortByBrightness = false;
    bool sortByHue = false;
    bool randomSort = true;
    bool reorder = false;
    bool shuffle = false;
    vector<string> targetImageNames;
    sortMode mode;
};

class PhotoGlitch
{
public:
    int ID;
    int numVerts;
    GLfloat* verts;
    GLfloat* texCoords;
    GLfloat* colors;
    int numIndices;
    GLuint* indices;
    
    ofImage tex;
    ofVbo vbo;
    
    PGCell* cells;
    
    void clear(){
        
        if (cells != NULL) {
            delete [] cells;
            cells = NULL;
        }
        
        if (verts != NULL) {
            delete [] verts;
            verts = NULL;
        }
        
        if( texCoords != NULL){
            delete [] texCoords;
            texCoords = NULL;
        }
        
        if (colors != NULL) {
            delete [] colors;
            colors = NULL;
        }
        
        if (indices != NULL) {
            delete [] indices;
            indices = NULL;
        }
        
    }
};


class CloudsVisualSystemPhotoGlitch : public CloudsVisualSystem
{
  public:    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "PhotoGlitch";
	}

	//These methods let us add custom GUI parameters and respond to their events
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
	//Use system gui for global or logical settings, for exmpl
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
	//use render gui for display settings, like changing colors
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);

	// selfSetup is called when the visual system is first instantiated
	// This will be called during a "loading" screen, so any big images or
	// geometry should be loaded here
    void selfSetup();

	// selfBegin is called when the system is ready to be shown
	// this is a good time to prepare for transitions
	// but try to keep it light weight as to not cause stuttering
    void selfBegin();

	// selfPresetLoaded is called whenever a new preset is triggered
	// it'll be called right before selfBegin() and you may wish to
	// refresh anything that a preset may offset, such as stored colors or particles
	void selfPresetLoaded(string presetPath);
    
	//do things like ofRotate/ofTranslate here
	//any type of transformation that doesn't have to do with the camera
    void selfSceneTransformation();
	
	//normal update call
	void selfUpdate();

	// selfDraw draws in 3D using the default ofEasyCamera
	// you can change the camera by returning getCameraRef()
    void selfDraw();
		
    // draw any debug stuff here
	void selfDrawDebug();

	// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
	void selfDrawBackground();

	// this is called when your system is no longer drawing.
	// Right after this selfUpdate() and selfDraw() won't be called any more
	void selfEnd();

	// this is called when you should clear all the memory and delet anything you made in setup
    void selfExit();

	//events are called when the system is active
	//Feel free to make things interactive for you, and for the user!
    void selfKeyPressed(int key);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(int x, int y, int button);
    void selfMouseMoved(int x, int y, int button);
    void selfMousePressed(int x, int y, int button);
    void selfMouseReleased(int x, int y, int button);
    
    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
//	ofCamera& getCameraRef(){
//		return myCustomCamera;
//	}
    
    void clearSource();
    void clearTarget();
    void generateSource();
    void generateTarget();
    void generate(PhotoGlitch& pg, int imageIndex,bool isSource= false);
    void beginAnimation();
    void updateAnimation();
    
    void shuffle(bool tweenCells=true);
    void sortHue(bool tweenCells=true);
    void sortBri(bool tweenCells=true);
    void reorder(bool tweenCells=true);
    void sortTarget();
    void sortTargetBrightness();
    void addTargetToUI(ofxUISuperCanvas* gui,string suffix, glitchParams& params );
    void tweenAll();
    void tween(int i, int j = -1);
    void tweenTarget(int i, int j = -1);
    void tweenFast(int i);
    void updateSequence();
    int getTargetFileName(ofxUISuperCanvas * gui, int targetId);

//    PGCell * cells;

//    vector<PhotoGlitch> photos;

  protected:
    

    ofxUISuperCanvas * customGui;
    ofxUISuperCanvas * target1Gui;
    ofxUISuperCanvas * target2Gui;
    
    //Source and targets
    PhotoGlitch target1;
    PhotoGlitch target2;
    PhotoGlitch sourcePhoto;
    
    
    //Params that control the animation. gp1 and hp2 correspond to targets 1 and 2
    glitchParams gp1;
    glitchParams gp2;
    glitchParams sourceParams;
   
    PhotoGlitch * currentTarget;
    glitchParams * currentTargetParams;
    
    int numDivRows;
    int numDivCols;
    int numCells;
//    int currentTargetIndex;    
//    int numVerts;
//    GLfloat * verts;
//    GLfloat * texCoords;
//    GLfloat * colors;
//    int numIndices;
//    GLuint * indices;
    
//    GLfloat * targetTexCoords;
//    GLfloat * targetColors;
//    int numTargetVerts;
//    GLfloat * targetVerts;
//    int numTargetIndices;
//    GLuint * targetIndices;
    
//    ofImage tex;
//    ofVbo vbo;
//    ofImage targetTex;
//    ofVbo targetVbo;
    
    ofVbo bgVbo;
    


    ofRectangle screenRect;
    
    //SRC + TRGT IMG PARAMS
    ofDirectory imagesDir;
    ofDirectory targetImagesDir;
    
    int selectedSrcImageIdx;
    int selectedTargetImageIdx;
    
    //ANIMATION PARAMS
    int tweenDuration;
    int tweenDelay;
    int delayParameter;
    int delayValue;
    
    float screenSliceWidth;
    float screenSliceHeight;
    float texSliceWidth;
    float texSliceHeight;
    float targetTexSliceWidth;
    float targetTexSliceHeight; 
    
    
    //Ma boolzz
    bool bUseColors;
    bool bUseTexture;
    bool bDrawTarget;
    bool bLoopBack;
    bool bOneCycleComplete;
    bool bRandomSort;
    bool bDrawBackground;
    bool bIsFirstTime;
    bool bCurrentlyAnimating;
    bool bStartAnimating;
    bool bDoPerpendicular;
    bool bShouldGenerate;
//    bool bShouldGenerateTargetOnly;
};
