#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsStoryEngine.h"

class CloudsSound {
  public:
	CloudsSound();
	
	void setup(CloudsStoryEngine& storyEngine);
	void exit(ofEventArgs & args);
	
	void update();
	void drawDebug();
	
	void keyPressed(ofKeyEventArgs & args);
	void keyReleased(ofKeyEventArgs & args);
	
	void mouseDragged(ofMouseEventArgs & args);
	void mouseMoved(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
	
	void audioRequested(ofAudioEventArgs& args);
	
  protected:
	
	bool eventsRegistered;
	void storyBegan(CloudsStoryEventArgs& args);
	void clipEnded(CloudsStoryEventArgs& args);
	void clipBegan(CloudsStoryEventArgs& args);
	void storyEnded(CloudsStoryEventArgs& args);
	
	CloudsStoryEngine* storyEngine;
	
	
	// Luke's vars
	// 
	short *s_audio_outbuf; // this is the buf filled by rtcmix (it uses short samples)
    int sr; // sampling rate
    int nbufs; // number of buffers
    int nchans; // 2 = stereo
    int framesize; // sigvs (512 seems ok)
	
    int returnQuadrant(int x, int y);
    ofTrueTypeFont theFont;
    int first_vec;
    int osx, osy;
    int quadrant;
    float sx, sy, delta;
    float allownote;

};