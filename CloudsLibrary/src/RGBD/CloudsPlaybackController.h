
#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsStoryEngine.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsVisualSystem.h"
#include "CloudsAct.h"
#include "ofxGameCamera.h"
#include "ofxUI.h"

#include "CloudsIntroSequence.h"
#include "CloudsVisualSystemClusterMap.h"
#include "CloudsVisualSystemRGBD.h"
#include "CloudsHUDController.h"

#include "CloudsTransitionController.h"

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
    void setRun(CloudsRun& run);
	void showIntro(vector<CloudsClip>& possibleStartQuestions);
	
	void playAct(CloudsAct* act);
	CloudsVisualSystemClusterMap& getClusterMap();
	CloudsRGBDVideoPlayer& getSharedVideoPlayer();
	
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
	void topicChanged(CloudsTopicEventArgs& args);
	void preRollRequested(CloudsPreRollEventArgs& args);
	
	void exit(ofEventArgs & args);
	
	CloudsTransitionController transitionController;
	void updateTransition();
	
	string nextPresetName;
	ofPtr<CloudsVisualSystem> currentVisualSystem;
	CloudsVisualSystemPreset currentVisualSystemPreset;
	
	bool revertToIntroAfter1Act;//demo hack
	bool actFinished;
	
	vector<CloudsClip> fakeQuestions;

	//crossfading CloudsVisualSystems
//	float crossfadeValue, fadeStartTime, fadeEndTime, fadeDuration, fadeStartVal, fadeTargetVal;
//	bool fadingOut, fadingIn;
	
//	ofxEasingQuint fadeEase;
//	void updateVisualSystemFade();
	
//	ofCamera superCamera;
//	ofCamera* rgbdCamera;
//	ofCamera* nextCamera;
//	ofVec3f cameraStartPos, camDelta;
//	ofMatrix4x4 accumulatedTransform;
//	void mixCameras(ofCamera* targetCam, ofCamera* c0, ofCamera* c1, float x );
//	void CloudsPlaybackControllerEventHandler( CloudsPlaybackControllerEvent &e );
//	void addControllerTween( string name, float startTime, float span, float startVal, float endVal, float *value );
//	vector<CloudsPlaybackControllerTween> controllerTweens;
	
//	string fadeOutRGBD;
//	string fadeInRGBD;
//	string fadeOutVisualSystem;
//	string fadeInVisualSystem;
	
//	void playScratchTrack(string track);
//	vector<string> scratchTracks;
//	ofSoundPlayer scratchPlayer;
	
//	int currentScratch;
//	float currentVolume;
//	float scratchVolumeAttenuate;
//	float targetScratchVolume;
//	void setUseScratch(bool useScratch);
	//    void setRandomQuestion(CloudsClip& clip);
  protected:

	CloudsStoryEngine* storyEngine;
	CloudsClip currentClip;
	CloudsAct* currentAct;
	
	//RGBD STUFF
	ofPtr<CloudsVisualSystemRGBD> rgbdVisualSystem;
	//if there is a system playing this wil be non-null
	ofPtr<CloudsVisualSystem> nextSystem;
	ofPtr<CloudsIntroSequence> introSequence;
	CloudsVisualSystemClusterMap clusterMapVisualSystem;
	CloudsHUDController hud;

	float crossfadeValue;
	float cursorMovedTime;
	bool showingCursor;
	int numClipsPlayed;
	
	string combinedMoviesFolder;
	string currentTopic;
	
	bool eventsRegistered;
	void actCreated(CloudsActEventArgs& args);
	
	string prerolledClipID;
	void prerollClip(CloudsClip& clip, float toTime);
	void playClip(CloudsClip& clip);

	//VISUAL SYSTEMS
	//
	void showIntro();
	bool showingIntro;
	bool showingVisualSystem;
	bool showingClusterMap;
//	bool fadingIntro;
	void clearAct(bool destroy = true);
			
	//play a visuals sytem, if no parameter is passed one is chosen automatically based on the current discussion topic
	void showVisualSystem(CloudsVisualSystemPreset& nextVisualSystem, float transitionDuration=3);
	//remove the current visual system
	void hideVisualSystem();
	void playNextVisualSystem();
	
	float rgbdVisualSystemFadeInDuration, rgbdVisualSystemFadeOutDuration;
	
	void transitionRgbdSystemOut( float transitionDuration=0, float fadeDuration=3 );
	void transitionRgbdSystemIn( float transitionDuration=0, float fadeDuration=3 );
	bool bIsFading;
};
