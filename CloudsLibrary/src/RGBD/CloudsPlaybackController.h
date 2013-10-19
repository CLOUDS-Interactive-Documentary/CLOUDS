
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


class CloudsPlaybackControllerEvent : public ofEventArgs {
    
public:
	
	//	Tween * tween;
	string message;
	string name;
	float value, span, startTime, endTime;
    
    CloudsPlaybackControllerEvent() {
    }
	static ofEvent <CloudsPlaybackControllerEvent> events;
};

class CloudsPlaybackControllerTween {
public:
	CloudsPlaybackControllerTween(){};
	~CloudsPlaybackControllerTween(){};
	
	void setup( string _name, float _startTime, float _span, float _startVal, float _endVal, float *_value = NULL ){
		name = _name;
		
		startTime = _startTime;
		span = _span;
		endTime = startTime + span;
		
		startVal = _startVal;
		endVal = _endVal;
		
		value = _value;
		
		bStarted = false;
		bEnded = false;
		autoDelete = true;
	}
	
	void update( float elapsedTime=ofGetElapsedTimef() ){
		val = ofMap(elapsedTime, startTime, endTime, startVal, endVal, true );
		if(value != NULL){
			*value = val;
		}
		
		

		//start event
		if(!bStarted && elapsedTime >= startTime){
			bStarted = true;
			
			static CloudsPlaybackControllerEvent startEvent;
			startEvent.message = "started";
			startEvent.name = name;
			startEvent.value = startVal;
			startEvent.span = span;
			startEvent.startTime = startTime;
			startEvent.endTime = endTime;
			if(value != NULL)	*value = startVal;
			ofNotifyEvent( CloudsPlaybackControllerEvent::events, startEvent );
		}
		
		//update event
		if(bStarted && !bEnded){
			
			static CloudsPlaybackControllerEvent updateEvent;
			updateEvent.message = "updated";
			updateEvent.name = name;
			updateEvent.value = val;
			updateEvent.span = span;
			updateEvent.startTime = startTime;
			updateEvent.endTime = endTime;
			ofNotifyEvent( CloudsPlaybackControllerEvent::events, updateEvent );
		}
		
		//end event
		if(!bEnded && elapsedTime >= endTime){
			bEnded = true;
			
			static CloudsPlaybackControllerEvent endEvent;
			endEvent.message = "ended";
			endEvent.name = name;
			endEvent.value = endVal;
			endEvent.span = span;
			endEvent.startTime = startTime;
			endEvent.endTime = endTime;
			if(value != NULL)	*value = endVal;
			ofNotifyEvent( CloudsPlaybackControllerEvent::events, endEvent );
			
		}
	}
	
	bool bStarted, bEnded, autoDelete;
	float startTime, span, endTime, startVal, endVal, val;
	float *value;
	
	string name;
};


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
	void updateVisualSystemFade();
	
	void CloudsPlaybackControllerEventHandler( CloudsPlaybackControllerEvent &e );
	
	void addControllerTween( string name, float startTime, float span, float startVal, float endVal, float *value );

	vector<CloudsPlaybackControllerTween> controllerTweens;
	
	string fadeOutRGBD;
	string fadeInRGBD;
	
	string fadeOutVisualSystem;
	string fadeInVisualSystem;
	
	string nextPresetName;
	
	vector<string> scratchTracks;
	ofSoundPlayer scratchPlayer;
	int currentScratch;
	float currentVolume;
	float targetScratchVolume;
	void setUseScratch(bool useScratch);
	CloudsVisualSystem* currentVisualSystem;
	CloudsVisualSystemPreset currentVisualSystemPreset;
//    void setRandomQuestion(CloudsClip& clip);
	
	vector<CloudsClip> fakeQuestions;
  protected:

	

	CloudsStoryEngine* storyEngine;
	CloudsClip currentClip;
	CloudsAct* currentAct;
//    CloudsRun* currentRun;
	
	//RGBD STUFF
	CloudsVisualSystemRGBD rgbdVisualSystem;
	CloudsIntroSequence introSequence;
	CloudsVisualSystemClusterMap clusterMapVisualSystem;
	
	float cursorMovedTime;
	
	string combinedMoviesFolder;
	string currentTopic;
	
	bool eventsRegistered;
	void actCreated(CloudsActEventArgs& args);
	
	string prerolledClipID;
	void prerollClip(CloudsClip& clip, float toTime);
	void playClip(CloudsClip& clip);

	//VISUAL SYSTEMS
	//
	bool showingIntro;
	bool showingVisualSystem;
	bool showingClusterMap;
	//if there is a system playing this wil be non-null
//	CloudsVisualSystem* currentVisualSystem;
	CloudsVisualSystem* nextSystem;
		
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
