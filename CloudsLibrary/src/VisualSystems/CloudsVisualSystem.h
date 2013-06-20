#pragma once

#include "ofMain.h"
#include "CloudsRGBDCombinedRender.h"
//#include "CloudsCamera.h"

/**
 * The CLOUDS Visual System super class
 * All visual systems will extend from this class
 *
 * It works pretty much the same as testApp, with the addition of a few
 * methods for interacting with play time duration, on screen start and stop, and accessing CLOUDS global state
 */

//TODO: fading, info routing

class CloudsVisualSystem {
  public:
	   
	CloudsVisualSystem();
	~CloudsVisualSystem();
	
	virtual string getSystemName() = 0;
	
	//Data Folder Path
    string getVisualSystemDataPath();
    static string getDataPath();
	
	//return TRUE for keywords that would allow this system to be shown
	//for example, if I'm the "computation" visual system, I'll return true to
	//a set of keywords related to computation. It's helpful to store this
	//in an XML file and load it on set up.
	bool isReleventToKeyword(string keyword);
	
	//APP CYCLE EVENTS
	//pre allocate any assets that will cause freezes
	virtual void setup();
	
	//these events are registered only when running the simulation
	virtual void update(ofEventArgs & args) = 0;
	virtual void draw(ofEventArgs & args) = 0;
	
	//application exit, clean up and don't crash
	virtual void exit(ofEventArgs & args) = 0;

	//INTERACTION EVENTS -- registered only
	virtual void keyPressed(ofKeyEventArgs & args) = 0;
	virtual void keyReleased(ofKeyEventArgs & args) = 0;
	
	virtual void mouseDragged(ofMouseEventArgs & args) = 0;
	virtual void mouseMoved(ofMouseEventArgs & args) = 0;
	virtual void mousePressed(ofMouseEventArgs & args) = 0;
	virtual void mouseReleased(ofMouseEventArgs & args) = 0;

	//these two methods are called by the controller class,
	//they register events and result in a calls to begin/end on the subclass
	void playSystem();
	void stopSystem();
	
	void setRenderer(CloudsRGBDCombinedRender& newRenderer);


	void setupSpeaker(string speakerFirstName,
					  string speakerLastName,
					  string quoteName);
	
	void speakerEnded();
	
	//REZA: Needs to be replaced by the Rezonator merge
	virtual vector<string> getPresets() = 0;
	virtual void loadPresetGUISFromName(string presetName) = 0;
	virtual void setCurrentCamera(ofCamera& cam) = 0;

	//SET and CALLED FROM CONTROLLER
	
	//how much time left to show this visual system?
	//once seconds is set to zero the end() event will be called by the controller
	float getSecondsRemaining();
	void setSecondsRemaining(float seconds);
		
	//this will always match what you offered in relevant keywords
	void setCurrentKeyword(string theme);
	string getCurrentKeyword();
	
	//set before calling begin so the class can react to the topic
	void setCurrentTopic(string keyword);
	string getCurrentTopic();

	vector<string>& getRelevantKeywords();
	
  protected:
		
	//called when showing the visual system, and to end it
	virtual void begin() = 0;
	virtual void end() = 0;

	//the sub class must populate this in setup() if it's to be called
	vector<string> relevantKeywords;

	//these variables are set by the playback controller when displaying
//	CloudsCamera* sharedCamera;
	//ways to interact with the pointcloud data
	CloudsRGBDCombinedRender* sharedRenderer;
	//set to true if the pointcloud renderer has valid speaker
	bool hasSpeaker;
	
	//speaker and quote info, constantly updated
	string speakerFirstName;
	string speakerLastName;
	string quoteName;
	
	//keyword is the topic of conversation
	string currentTopic;
	//theme is the topic chosen
	string currentKeyword;
	
	bool isPlaying;
	float secondsRemaining;
};
