
#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsStoryEngine.h"
#include "CloudsRGBDCombinedRenderer.h"
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
	void setup(CloudsStoryEngine& storyEngine);
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
	void topicChanged(string& args);
	
	void exit(ofEventArgs & args);
	
  protected:

	CloudsStoryEngine* storyEngine;
	CloudsClip currentClip;
	CloudsAct* currentAct;
	
	//RGBD STUFF
	CloudsRGBDCombinedRenderer combinedRenderer;
	CloudsVisualSystemRGBD rgbdVisualSystem;
	string combinedMoviesFolder;
	string currentTopic;
	
	bool eventsRegistered;
	void actCreated(CloudsActEventArgs& args);
	
	
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


};
