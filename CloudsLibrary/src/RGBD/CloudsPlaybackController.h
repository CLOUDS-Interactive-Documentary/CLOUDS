
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

#include "CloudsSound.h"
#include "CloudsMixer.h"
#include "CloudsSecondaryDisplayOSCSender.h"

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
//	void setStoryEngine(CloudsStoryEngine& storyEngine);	
	void playAct(CloudsAct* act);
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
	
	//bool revertToIntroAfter1Act;//demo hack
	//bool actFinished;
	
	vector<CloudsClip> fakeQuestions;

  protected:

	//*** CORE CLOUDS STUFF
	CloudsFCPParser parser;
	CloudsVisualSystemManager visualSystems;
	CloudsStoryEngine storyEngine;
	CloudsMixer mixer;
	CloudsSound sound;
	CloudsSecondaryDisplayOSCSender oscSender;
	CloudsHUDController hud;

	//STATE STUFF
	CloudsRun run;
	CloudsAct* currentAct;
	CloudsClip currentClip;
	int numClipsPlayed;
	string currentTopic;
	bool shouldPlayAct;
	
	//RGBD STUFF
	ofPtr<CloudsVisualSystemRGBD> rgbdVisualSystem;
	//if there is a system playing this wil be non-null
	ofPtr<CloudsVisualSystem> nextSystem;
	ofPtr<CloudsIntroSequence> introSequence;
	ofPtr<CloudsVisualSystemClusterMap> clusterMapVisualSystem;
	
	string nextPresetName;
	ofPtr<CloudsVisualSystem> currentVisualSystem;
	CloudsVisualSystemPreset currentVisualSystemPreset;
	
	CloudsTransitionController transitionController;
	void updateTransition();

	float crossfadeValue;
	float cursorMovedTime;
	bool showingCursor;
	string combinedMoviesFolder;
	
	bool eventsRegistered;
	void actCreated(CloudsActEventArgs& args);
	
	string prerolledClipID;
	void prerollClip(CloudsClip& clip, float toTime);
	void playClip(CloudsClip& clip);

	//VISUAL SYSTEMS
	//
	void showIntro();
	void showIntro(vector<CloudsClip>& possibleStartQuestions);

	bool showingIntro;
	bool showingVisualSystem;
	bool showingClusterMap;
	
	void clearAct(bool destroy = true);
			
	//play a visuals sytem, if no parameter is passed one is chosen automatically based on the current discussion topic
	void showVisualSystem(CloudsVisualSystemPreset& nextVisualSystem);
	//remove the current visual system
	void hideVisualSystem();
	void showRGBDVisualSystem();
	void playNextVisualSystem();
	
};
