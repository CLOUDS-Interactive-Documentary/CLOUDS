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
#include "ParamManager.h"
#include "LinkedList.h"
#include "Ball.h"
#include "EditLine.h"
#include "ofxSimpleSpline.h"

//TODO: rename this to your own visual system
class CloudsVisualSystemBallDroppings : public CloudsVisualSystem {
  public:
    
    CloudsVisualSystemBallDroppings();
    
	//TODO: Change this to the name of your visual system
	//This determines your d2ata path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "BallDroppings";
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
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
	

    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
	ofCamera& getCameraRef(){
		return cam;
	}

protected:
    //custom methods
    void step();
    
    void createBall(); ///< 	create a new ball at the emmitter's position
    void deletePickedVertex(); ///< delete the vertex currently under the mouse along with the line attached to it
    void undo(); ///< undo the last user interface move
    int validLine(EditLine*l); ///< checks whether a line is still part of the system
    
    //resetting methods
    
    void resetWorld(); ///< 	reset the entire app
    void resetVars();///< reset global state variables
    void resetBalls(); ///< clear only the balls
    void resetLines(); ///< clear only the lines
    void showPanel();//sets the info display panel to visible
    float oldMillis; ///< remembering what time it was
    bool mouseDrag;
    long clickCount; ///< number of times the user has pressed the mouse button
    
    
    float ball_drop_rate; ///< how fast or slow to release balls
    bool paused; ///< freezing the app like pausing the game
    float gravity; ///< strength of gravity pulling towards bottom of screen
    float ballcolor; ///< a gray tone for the color of the ball (0 is black, 1 is white)
    float linesColor;
    float emitterColor;
    float friction; ///< friction multiplier to keep the physics realistic
    float frequencyRange; ///< pitch range for ball song
    bool fullScreenMode; ///< whether or not the app is fullscreen
    
    LinkedList *balls; ///< the list of balls
    LinkedList *lines; ///< the list of lines
    LinkedList *emptyBalls; ///< registry for offstage balls waiting to be recycled
    
    //undo stuff
    
    LinkedList *undoables; ///< list of undoable actions
    int undoBusy; ///< true if the undo process is still busy undo
    
    V3 oldMouse; ///< mouse position one frame ago
    V3 lastMouseDown; ///< where the mouse was last time the button was pressed
    
    //interface for working with the line closest to the mouse
    EditLine *closestLine;
    int closestLineVertex;
    float closestLine_maxPickingDistance;
    float closestLineDistance;
    V3 closestLine_beginMove;
    int mousestate_draggingvert;
    
    //positioning for the ball emitter circle
    V3 hole;
    V3 holeLag;
    
    ofTrueTypeFont font; ///< typeface for the info display
    float displayAlpha;///< opacity of the info display
    void lineGradient(float x1,float y1,float x2,float y2,float color1,float color2);//2 color line drawing
    void updateClosestPickingLine();///< recalculates the line closest to the mouse
    
    // preset stuff
    ofxUISuperCanvas* customGui;
    float mainGain;
};
