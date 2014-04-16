
#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsStoryEngine.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsVisualSystem.h"
#include "CloudsAct.h"
#include "ofxUI.h"

#include "CloudsIntroSequence.h"
#include "CloudsVisualSystemClusterMap.h"
#include "CloudsVisualSystemRGBD.h"

#include "CloudsHUDController.h"

#include "CloudsSound.h"
#include "CloudsMixer.h"
#include "CloudsSecondaryDisplayOSCSender.h"

#include "CloudsTransitionController.h"

#include "CloudsPortal.h"


/**
 * This class controls playback of RGBD sequences
 * and decides when to show Visual Systems
 *
 */
class CloudsPlaybackController : public ofThread {
  public:
	CloudsPlaybackController();

	//set it up with an existing story engine that will register the events
	void setup();
	void loadCurrentAct(); //starts loading screen
	void updateLoadingAct(); //loads one system everytime it's called
	void playCurrentAct();//when done loading
	
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
	void questionProposed(CloudsQuestionEventArgs& args);
	void questionSelected(CloudsQuestionEventArgs& args);
	void topicChanged(CloudsTopicEventArgs& args);
	void preRollRequested(CloudsPreRollEventArgs& args);
    
    void portalHoverBegan(CloudsPortalEventArgs& args);
    void portalHoverEnded(CloudsPortalEventArgs& args);
	
	void drawContinueReset();
	
	void exit(ofEventArgs & args);
	
	vector<CloudsClip> fakeQuestions;
	
	void finishSetup(); //called at the end of the threaded function
	bool loading;
	float loadPercent;
	bool loadFinished;
	
	void threadedFunction();
  protected:
	vector<CloudsClip> startingNodes;
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
	
	bool shouldLoadAct;
	bool shouldPlayAct;
    bool shouldClearAct;
    
    void drawRenderTarget();
    void drawInterludeInterface();
    
    CloudsVisualSystem* currentVisualSystem;
    void createInterludeSoundQueue();
    int numActsCreated;
    
    bool cachedTransition;
    CloudsVisualSystem::RGBDTransitionType cachedTransitionType;
    
	//RGBD STUFF
	CloudsVisualSystemRGBD* rgbdVisualSystem;
	//if there is a system playing this wil be non-null
	CloudsIntroSequence* introSequence;
	CloudsVisualSystemClusterMap* clusterMap;
	CloudsVisualSystem* interludeSystem;
    float interludeStartTime;
	
	CloudsVisualSystemPreset nextVisualSystemPreset;	
	CloudsVisualSystemPreset currentVisualSystemPreset;
		
	CloudsTransitionController transitionController;
	void updateTransition();
	bool updateInterludeInterface(); //true if we should stop interlude
	
	//loader screen
	bool loadingAct;
	int currentPresetIndex;
	
    string currentClipName;
    float actCreatedTime;
	float crossfadeValue;
	string combinedMoviesFolder;
	void drawDebugOverlay();
	
	bool eventsRegistered;
	void actCreated(CloudsActEventArgs& args);
	bool returnToIntro;
    
	string prerolledClipID;
	void prerollClip(CloudsClip& clip, float toTime);
	void playClip(CloudsClip clip);

	//VISUAL SYSTEMS
	//
	void showIntro();

	bool showingIntro;
	bool showingVisualSystem;
	bool showingClusterMap;
    bool showingInterlude;
    bool exitedInterlude;
	bool bQuestionAsked;
	
	void clearAct();
    
	//remove the current visual system
	void hideVisualSystem();
	void showRGBDVisualSystem();
	void playNextVisualSystem();
    void showClusterMap();
    void showInterlude();
    void cleanupInterlude();

    //INTERLUDE INTERFACE
	void resetInterludeVariabls();
	ofxFTGLFont interludeInterfaceFont;
	float interludeExitBarWidth;
	bool interludeHoveringContinue;
	bool interludeHoveringReset;
	float interludeBarHoverStartTime;
	float interludeBarHoverHoldTime;
	float interludeBarHoverPercentComplete;
	bool interludeContinueSelected;
	bool interludeResetSelected;
	
//    CloudsPortal continuePortal;
//    void setupPortals();
//    ofRectangle resetRect;
//    ofRectangle resetSelectionRect;
//    float resetSelectedPercentComplete;
//    float maxResetHoverTime;
//    float startResetHoverTime;
//    float endResetHoverValue;
//    bool prevResetValue;
//	
//    bool bResetSelected;
//    bool bResetTransitionComplete;



//    void clearRestButtonParams();
//    ofVec2f lastMousePos, currMousePos;
//    unsigned long long lastMouseMoveMillis;
};





