#pragma once

#include "ofMain.h"

/**
 * The CLOUDS Visual System super class
 * All visual systems will extend from this class
 *
 * It works pretty much the same as testApp, with the addition of a few
 * methods for interacting with play time duration, on screen start and stop, and accessing CLOUDS global state
 */
class CloudsVisualSystem {
  public:
	CloudsVisualSystem();
	~CloudsVisualSystem();
	
	//return TRUE for keywords that would allow this system to be shown
	//for example, if I'm the "computation" visual system, I'll return true to
	//a set of keywords related to computation. It's helpful to store this
	//in an XML file and load it on set up.
	virtual bool isReleventToKeyword(string keyword);
	
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
	
	//called when showing the visual system, and to end it
	virtual void begin() = 0;
	virtual void end() = 0;
	
	//how much time left to show this visual system?
	//once seconds is set to zero the end() event will be called by the controller
	float getSecondsRemaining();
	void setSecondsRemaining(float seconds);
	
  protected:

	bool isPlaying;
	float secondsRemaining;
};
