
#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsPortalEvents.h"
#include "CloudsStoryEngine.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsVisualSystem.h"
#include "CloudsAct.h"

#include "CloudsVisualSystemManager.h"

#include "CloudsHUDController.h"

#include "CloudsSound.h"
#include "CloudsMixer.h"

#ifdef CLOUDS_INSTALLATION
#include "CloudsSecondaryDisplayOSCSender.h"
#endif

#include "CloudsTransitionController.h"

#include "CloudsPortal.h"

#ifdef VHX_MEDIA
#include "ofxReachability.h"
#include "CloudsVHXAuth.h"
#endif

class CloudsIntroSequence;
class CloudsVisualSystemRGBD;
class CloudsVisualSystemTwitter;
class CloudsVisualSystemClusterMap;
class CloudsVisualSystemVisuals;

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
	void preDraw(ofEventArgs& args);
	void draw(ofEventArgs& args);
	
	void keyPressed(ofKeyEventArgs & args);
	void keyReleased(ofKeyEventArgs & args);
	
	void mouseDragged(ofMouseEventArgs & args);
	void mouseMoved(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
    void mouseScrolled(ofMouseEventArgs & args);
    void exit(ofEventArgs & args);

#ifdef VHX_MEDIA
    void networkConnected();
    void networkDisconnected();
    
    void requestTokenComplete(CloudsVHXEventArgs& args);
    void refreshTokenComplete(CloudsVHXEventArgs& args);
    void requestCodeComplete(CloudsVHXEventArgs& args);
    void linkCodeComplete(CloudsVHXEventArgs& args);
    void verifyPackageComplete(CloudsVHXEventArgs& args);
    
    void codeExpired(CloudsVHXEventArgs& args);
    void packageExpired(CloudsVHXEventArgs& args);
#endif
	
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

    void finishSetup(); //called at the end of the threaded function
    bool loading;
    bool loadFinished;
    
    void threadedFunction();
    
	
	
    
  protected:
    
	vector<CloudsClip*> startingNodes;
	//*** CORE CLOUDS STUFF
	CloudsFCPParser parser;
	CloudsVisualSystemManager visualSystems;
	CloudsStoryEngine storyEngine;
	CloudsSound sound;
#ifdef CLOUDS_INSTALLATION
	CloudsSecondaryDisplayOSCSender oscSender;
#endif
	CloudsHUDController hud;
	CloudsTransitionController transitionController;
    CloudsMixer mixer;

    #ifdef VHX_MEDIA
    ofxReachability reachability;
    CloudsVHXAuth vhxAuth;
    #endif


	//STATE STUFF
	CloudsRun run;
	CloudsAct* currentAct;
	CloudsClip* currentClip;
    
	//if there is a system playing this wil be non-null
	CloudsIntroSequence* introSequence;
	CloudsVisualSystemClusterMap* clusterMap;
    CloudsVisualSystemTwitter* peopleMap;
    CloudsVisualSystemVisuals* visualsMap;
	CloudsVisualSystem* interludeSystem;
    CloudsVisualSystem* currentVisualSystem;
    
	CloudsVisualSystemPreset nextVisualSystemPreset;
	CloudsVisualSystemPreset currentVisualSystemPreset;
    
	int numClipsPlayed;
    int numActsCreated;
    string currentTopic;
	
	bool shouldLoadAct;
	bool shouldPlayAct;
    bool shouldClearAct;
    bool shouldPlayClusterMap;
    bool resumingActFromIntro;

    string videoBufferingStatus;
    void drawRenderTarget();
    //void drawInterludeInterface();
	void drawInterludePanel(ofRectangle rect, string promptText, bool hovering, int tracking );
#ifdef KINECT_INPUT
    void drawKinectFeedback();
#endif

    void createInterludeSoundQueue();
    void prepareForReturnableResearchTransition();
    void populateResearchWithLatest();
    
	//RGBD STUFF
	CloudsVisualSystemRGBD* rgbdVisualSystem;
    bool cachedTransition;
    CloudsVisualSystem::RGBDTransitionType cachedTransitionType;
	vector<string> backgroundPresets;
	vector<string> pointcloudPresets;
	string basePreset;
	void populateRGBDPresets();
    void populateResearch();
    float interludeStartTime;
    
	void updateTransition();
    void updateHUD();
	bool updateInterludeInterface(); //true if we should stop interlude
	bool forceInterludeReset;
    ///////////////
    
	//loader screen
	bool loadingAct;
    //bool justOpened;
	int currentPresetIndex;
    void checkForUpdates();
    void updateCheckCompleted(ofHttpResponse& response);
    bool runningLatestVerion;
    string newVersionDownloadURL;
    map<string,string> versionMaps;
    
    string currentClipName;
    float actCreatedTime;
	float crossfadeValue;
	string combinedMoviesFolder;
	void drawDebugOverlay();
	void clearRenderTarget();

	bool eventsRegistered;
	void actCreated(CloudsActEventArgs& args);
	bool returnToIntro;

//	bool badIdle;
//	float badIdleStartTime;
    bool userReset;
    
	string prerolledClipID;
	void prerollClip(CloudsClip* clip, float toTime);
	void playClip(CloudsClip* clip);

	//VISUAL SYSTEMS
	//
	void showIntro();
    void loadIntroPreset();
    
	bool showingIntro;
	bool showingVisualSystem;
	bool showingClusterMap;
    bool showingClusterMapNavigation;
    bool showedClusterMapNavigation;
    bool showingInterlude;
    bool showingVisualLoop;
    bool actJustBegan;
    bool showingExploreMap;
    bool showingExplorePeople;
    bool showingExploreVisuals;
    bool showingResearchMode;
    bool researchModeTopic;
    bool researchModePerson;
    bool researchModeVisual;
    
	bool bQuestionAsked;
	bool forceCredits;
    bool bVHXRentalExpired;
    bool bShowingAct;
    bool bBufferingVideo;
    
    void drawVideoStatus();
    
    string exploreMapSelectedTopic;
    string explorePeopleSelectedSpeakerID;
    string exploreVisualsSelectedSystem;
	void clearAct();
    
	//remove the current visual system
	void hideVisualSystem();
	void showRGBDVisualSystem();
	void playNextVisualSystem();
    void showClusterMap();
    void showInterlude();
    void cleanupInterlude();
    void transitionBackToResearch();
    
    void showExploreMap();
    void showExplorePeople();
    void showExploreVisuals();

    bool showingResearchScreen;
    bool canReturnToAct;
    CloudsTransitionState resumeState;
    bool resumingAct;
    bool keyedToNext;
    float lastTimeRecheckedPurchase;
    
    //INTERLUDE INTERFACE
	void resetInterludeVariables();
    ofxFTGLSimpleLayout interludeInterfaceFont;
	float interludeExitBarWidth;
	bool interludeHoveringContinue;
	bool interludeHoveringReset;
	float interludeBarHoverStartTime;
	float interludeBarHoverHoldTime;
	float interludeBarHoverPercentComplete;
	bool interludeContinueSelected;
	bool interludeResetSelected;
	bool interludeTimedOut;
	float interludeArcRadius;
	float interludeArcBaseAlpha;
	float interludeForceOnTimer;

};





