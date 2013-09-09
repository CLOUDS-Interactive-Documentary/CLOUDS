#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsStoryEngine.h"
#include "CloudsAct.h"
#include "lukeFuncs.h"

class CloudsSound {
  public:
	CloudsSound();
	
	void setup(CloudsStoryEngine& storyEngine);
	void exit(ofEventArgs & args);
	
	void update();
	void drawDebug();
	
	void setMasterAmp(float amp);
	
	void keyPressed(ofKeyEventArgs & args);
	void keyReleased(ofKeyEventArgs & args);
	
	void mouseDragged(ofMouseEventArgs & args);
	void mouseMoved(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
	
	void audioRequested(ofAudioEventArgs& args);

	void actBegan(CloudsActEventArgs& args);
	void actEnded(CloudsActEventArgs& args);
	void clipBegan(CloudsClipEventArgs& args);
	void visualSystemBegan(CloudsVisualSystemEventArgs& args);
	void visualSystemEnded(CloudsVisualSystemEventArgs& args);
	void questionAsked(CloudsQuestionEventArgs& args);	
	void topicChanged(CloudsTopicEventArgs& args);
	void preRollRequested(CloudsPreRollEventArgs& args);
	
  protected:

	CloudsStoryEngine* storyEngine;
	CloudsAct* currentAct;
	
	bool eventsRegistered;
	void actCreated(CloudsActEventArgs& args);

    // Luke's stuff
	//
    void startMusic(int mc, int mh, int mr, float musicdur);
    void stopMusic();
    void loadRTcmixFiles();
    void loadRTcmixSamples();
    void audioRequested(float * output, int bufferSize, int nChannels);
	short *s_audio_outbuf; // this is the buf filled by rtcmix (it uses short samples)
    int sr; // sampling rate
    int nbufs; // number of buffers
    int nchans; // 2 = stereo
    int framesize; // sigvs (512 seems ok)

    int returnQuadrant(int x, int y);
    int returnColor(string c);
    int first_vec;
    int osx, osy;
    int quadrant;
    float sx, sy, delta;
    float allownote;
    int mcolor, mharmony, mrhythm;

    float MASTERAMP;
    float MASTERTEMPO;
    int AUTORUN;
    bool DOCLEAR;
    float cleartime;
    float targetAmp;
	
    vector<lukeRhythm> rhythms;
    vector<lukeColor> colors;
    vector<lukePitchArray> pitches;
    vector<lukePreset> presets;

};