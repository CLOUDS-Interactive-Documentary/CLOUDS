
#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsStoryEngine.h"
#include "CloudsRGBDCombinedRender.h"
#include "CloudsVisualSystem.h"

#include "ofxGameCamera.h"
#include "ofxUI.h"
//#include "CloudsCamera.h"

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
	
	void exit(ofEventArgs & args);
	
	bool simplePlaybackMode;
	
  protected:

	CloudsStoryEngine* storyEngine;
	CloudsClip currentClip;
	
	//RGBD STUFF
	CloudsRGBDCombinedRender combinedRenderer;
	CloudsVisualSystemRGBD rgbdVisualSystem;
	string combinedMoviesFolder;
	
	bool eventsRegistered;
	void storyBegan(CloudsStoryEventArgs& args);
	void clipBegan(CloudsStoryEventArgs& args);
	void clipEnded(CloudsStoryEventArgs& args);
	void storyEnded(CloudsStoryEventArgs& args);

	void visualSystemBegan(CloudsVisualSystemEventArgs& args);
	void visualSystemEnded(CloudsVisualSystemEventArgs& args);
	
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
