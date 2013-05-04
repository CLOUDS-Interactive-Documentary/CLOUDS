
#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsStoryEngine.h"
#include "CloudsRGBDCombinedRender.h"
#include "CloudsVisualSystem.h"

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

	//POINTCLOUD STUFF
	//
	ofVideoPlayer player;
	CloudsStoryEngine* storyEngine;
	CloudsRGBDCombinedRender renderer;
	CloudsClip currentClip;
	
	bool playingCombinedVideo;
	bool eventsRegistered;
	void storyBegan(CloudsStoryEventArgs& args);
	void clipChanged(CloudsStoryEventArgs& args);
	void playClip(CloudsClip& clip);
	
	//VISUAL SYSTEMS
	//
	bool showingVisualSystem;
	vector<CloudsVisualSystem*> visualSystems;
	CloudsVisualSystem* currentVisualSystem;
	void populateVisualSystems();
	void showVisualSystem();
	void hideVisualSystem();
	
	//MEDIA/INTERNAL
	string relinkMovieFilepath(string filePath);

};