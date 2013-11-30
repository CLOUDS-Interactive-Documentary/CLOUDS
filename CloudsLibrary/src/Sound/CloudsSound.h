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
	
	float maxSpeakerVolume; // set between 0. and 1.0 to modulate speaker volume
	
  protected:

	CloudsStoryEngine* storyEngine;
	CloudsAct* currentAct;
	
	bool eventsRegistered;
	void actCreated(CloudsActEventArgs& args);

    // Luke's stuff
	//
    void startMusicFX(float outskip, float musicdur);
    void startMusic(float outskip, string mo, string arg_a, string arg_b, int mh, int mr, float musicdur, float bpm);
    void stopMusic();
    void loadRTcmixFiles();
    void loadRTcmixSamples();
    void registerOrchs();
    void audioRequested(float * output, int bufferSize, int nChannels);
	short *s_audio_outbuf; // this is the buf filled by rtcmix (it uses short samples)
    int sr; // sampling rate
    int nbufs; // number of buffers
    int nchans; // 2 = stereo
    int framesize; // sigvs (512 seems ok)

    int returnColor(string c);
    int first_vec;
    float allownote;

    float MASTERAMP;
    float MASTERTEMPO;
    int AUTORUN;
    bool DOCLEAR;
    float cleartime;
    float targetAmp;
    bool RTCMIX_PRINT;
    float tl1, tl2, tl3, bl1;
	
    vector<lukeRhythm> rhythms;
    vector<lukePitchArray> pitches;
    vector<lukePreset> presets;
    vector<string> orchestra;

    vector<lukeSample> looperSamples;

};