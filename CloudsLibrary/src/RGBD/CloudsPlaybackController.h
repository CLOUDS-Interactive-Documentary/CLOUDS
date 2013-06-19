
#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsStoryEngine.h"
#include "CloudsRGBDCombinedRender.h"
#include "CloudsVisualSystem.h"

#include "ofxGameCamera.h"
#include "ofxUI.h"
#include "CloudsCamera.h"

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
	void update();
	void draw();
	
	void keyPressed(ofKeyEventArgs & args);
	void keyReleased(ofKeyEventArgs & args);
	
	void mouseDragged(ofMouseEventArgs & args);
	void mouseMoved(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
	
	void exit(ofEventArgs & args);
	
  protected:

	//A ROLL STUFF
	//

//	ofxGameCamera camera;
	CloudsCamera cloudsCam;
	
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

	void visualSystemBegan(CloudsStoryEventArgs& args);
	void visualSystemEnded(CloudsStoryEventArgs& args);
	
	void playClip(CloudsClip& clip);
	
	//VISUAL SYSTEMS
	//
	bool showingVisualSystem;
	vector<CloudsVisualSystem*> visualSystems;
	map<string, CloudsVisualSystem*> nameToVisualSystem;
	//if there is a system playing this wil be non-null
	CloudsVisualSystem* currentVisualSystem;
	//this instantiates and registers all the visual systems, called once at setup
	void populateVisualSystems();
	//call once for each new system you instantiate
	void registerVisualSystem(CloudsVisualSystem* system);
	CloudsVisualSystem* visualSystemWithName(string systemName);
	
	//play a visuals sytem, if no parameter is passed one is chosen automatically based on the current discussion topic
	void showVisualSystem();
	void showVisualSystem(CloudsVisualSystem* nextVisualSystem, string keyTheme);
	//remove the current visual system
	void hideVisualSystem();
	
	//GUI
	//
	ofxUISuperCanvas* visualSystemControls;
	
	//WIDGETS
	ofxUIRadio *visualSystemRadio;
	
	ofxUICanvas* keyThemesPanel;
	ofxUIRadio *keyThemesRadio;
	ofxUIButton *playButton;
	
	
	float timeToTest;
	bool triggerVisualSystem;
	void guiEvent(ofxUIEventArgs &e);
};
