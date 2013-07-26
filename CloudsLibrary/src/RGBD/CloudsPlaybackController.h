
#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsStoryEngine.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsVisualSystem.h"
#include "CloudsAct.h"
#include "ofxGameCamera.h"
#include "ofxUI.h"


#include "CloudsVisualSystemRGBD.h"

/**
 * This class controls playback of RGBD sequences
 * and decides when to show Visual Systems
 *
 */
class CloudsPlaybackController {
  public:
	CloudsPlaybackController();
	~CloudsPlaybackController();

	//set it up with an existing story engine that will register the events
	void setup();
	void setStoryEngine(CloudsStoryEngine& storyEngine);
	void beginWithClip(); //auto play by creating new acts
	void playAct(CloudsAct* act);

	//update and draw to the screen, this will always
	//show the main CLOUDS experience as pointclouds or visual systems
	void update(ofEventArgs& args);
	void draw(ofEventArgs& args);
	
	void keyPressed(ofKeyEventArgs & args);
	void keyReleased(ofKeyEventArgs & args);
	
	void mouseDragged(ofMouseEventArgs & args);
	void mouseMoved(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
	
	void actBegan(CloudsActEventArgs& args);
	void actEnded(CloudsActEventArgs& args);
	void clipBegan(CloudsClipEventArgs& args);
	void visualSystemBegan(CloudsVisualSystemEventArgs& args);
	void visualSystemEnded(CloudsVisualSystemEventArgs& args);
	void questionAsked(CloudsQuestionEventArgs& args);
	void topicChanged(string& args);
	void preRollRequested(CloudsPreRollEventArgs& args);
	
	void exit(ofEventArgs & args);
	
	ofFbo sharedRenderTarget;
	ofFbo nextRenderTarget;
	
	
	
	//crossfading CloudsVisualSystems
	float crossfadeValue, fadeStartTime, fadeEndTime, fadeDuration, fadeStartVal, fadeTargetVal;
	bool fadingOut, fadingIn;
	ofCamera superCamera;
	ofCamera* rgbdCamera;
	ofCamera* nextCamera;
	ofVec3f cameraStartPos, camDelta;
	ofMatrix4x4 accumulatedTransform;
	
	void mixCameras(ofCamera* targetCam, ofCamera* c0, ofCamera* c1, float x );
	ofxEasingQuint fadeEase;
	void updateVisualSystemCrossFade();
	

	
  protected:

	CloudsStoryEngine* storyEngine;
	CloudsClip currentClip;
	CloudsAct* currentAct;
	
	//RGBD STUFF
	CloudsVisualSystemRGBD rgbdVisualSystem;
	string combinedMoviesFolder;
	string currentTopic;
	
	bool eventsRegistered;
	void actCreated(CloudsActEventArgs& args);
	
	
	string prerolledClipID;
	void prerollClip(CloudsClip& clip, float toTime);
	void playClip(CloudsClip& clip);

	//VISUAL SYSTEMS
	//
	bool showingVisualSystem;
	//if there is a system playing this wil be non-null
	CloudsVisualSystem* currentVisualSystem;
	
	//play a visuals sytem, if no parameter is passed one is chosen automatically based on the current discussion topic
	void showVisualSystem(CloudsVisualSystemPreset& nextVisualSystem);
	//remove the current visual system
	void hideVisualSystem();
	
	void fadeInVisualSystem( float duration=3, float start=ofGetElapsedTimef() );
	void fadeOutVisualSystem( float duration=3, float start=ofGetElapsedTimef() );
	
	void transitionRgbdSystemOut( float transitionDuration=0, float fadeDuration=3 );
	void transitionRgbdSystemIn( float transitionDuration=0, float fadeDuration=3 );
	bool bIsFading;


};
