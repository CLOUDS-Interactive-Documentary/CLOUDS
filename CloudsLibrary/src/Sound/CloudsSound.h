#pragma once

#include "ofMain.h"
#include "CloudsEvents.h"
#include "CloudsAudioEvents.h"
#include "CloudsStoryEngine.h"
#include "CloudsAct.h"
#include "lukeFuncs.h"

#define PF_INTRO_BUS 1
#define PF_TUNNEL_BUS 2
#define PF_LOADER_BUS 3
#define PF_OUTRO_BUS 4
#define PF_CLUSTERMAP_BUS 5
#define PF_MAINACT_BUS_START 6
#define PF_NUMBUSES 100

#define TUNNEL_TRACK_KEY "tunnel"
#define TUNNEL_TRACK_PATH (GetCloudsDataPath(true) + "sound/tunnel.mp3")

typedef struct{
    string trackKey;
	float startTime;
	float mixLevel;
} QueuedTrack;

class CloudsSound {
  public:
	CloudsSound();
	void setup();
	void setup(CloudsStoryEngine& storyEngine);
	void exit(ofEventArgs & args);
	
	void update(ofEventArgs & args);
	void update();
	void drawDebug();
	
	void keyPressed(ofKeyEventArgs & args);
	void keyReleased(ofKeyEventArgs & args);
	
	void mouseDragged(ofMouseEventArgs & args);
	void mouseMoved(ofMouseEventArgs & args);
	void mousePressed(ofMouseEventArgs & args);
	void mouseReleased(ofMouseEventArgs & args);
    void mouseScrolled(ofMouseEventArgs & args);
	
	void audioRequested(ofAudioEventArgs& args);
	void actBegan(CloudsActEventArgs& args);
	void actEnded(CloudsActEventArgs& args);
	void clipBegan(CloudsClipEventArgs& args);
	void visualSystemBegan(CloudsVisualSystemEventArgs& args);
	void visualSystemEnded(CloudsVisualSystemEventArgs& args);
	void questionProposed(CloudsQuestionEventArgs& args);
	void questionSelected(CloudsQuestionEventArgs& args);
    void questionSelected(float fadeTime);
	void topicChanged(CloudsTopicEventArgs& args);
	void preRollRequested(CloudsPreRollEventArgs& args);
    
    // new callbacks
    void enterTunnel();
    void exitTunnel();
//    void enterClusterMap();
//    void exitClusterMap();
	
	float maxSpeakerVolume; // set between 0. and 1.0 to modulate speaker volume
	
    // Luke's public stuff
    void schedulePreset(lukePreset &p, float outskip, float dur, int mixlevel, int orchstep);
    void startMusicFX(float outskip, float musicdur);
    void startMusic(float outskip, string mo, string arg_a, string arg_b, int mh, int mr, float musicdur, float bpm, float m_amp, float m_rev, int instnum, string ampenvelope);
    void stopMusic();
    void reloadPresets();
    void doPrinting();
    int ACTBUS; // needs to be public for UDP shit in the scoredesigner
    bool in_tunnel;
    bool isScoreDesigner;
    
	void threadedFunction();
		
    // public data structures
    vector<lukePreset> presets;
	
    map<string, CloudsMedia *> renders;
    float mixVolumeForTrack(string trackPath);
	void setMixVolumeForTrack(string trackPath, float level);
	void playImmediately(string trackKey);

	void saveMixLevels();
	float frontMixLevel;
	float backMixLevel;
	
  protected:

	CloudsStoryEngine* storyEngine;
	CloudsAct* currentAct;

	//only used in non RTCMIX context
	ofPtr<ofVideoPlayer> frontPlayer;
	ofPtr<ofVideoPlayer> backPlayer;
	
	string currentTrackKey;
	bool eventsRegistered;
	void actCreated(CloudsActEventArgs& args);

    // Luke's stuff
	//
    void loadRTcmixFiles();
    void loadRTcmixSamples();
    void registerOrchs();
    void audioRequested(float * output, int bufferSize, int nChannels);
	short *s_audio_outbuf; // this is the buf filled by rtcmix (it uses short samples)
	short *s_audio_compbuf; // this is the buf for comparison (it uses short samples)
    int sr; // sampling rate
    int nbufs; // number of buffers
    int nchans; // 2 = stereo
    int framesize; // sigvs (512 seems ok)

    bool first_vec;
    int whichdream;
    int buzzreps;

    float MASTERTEMPO;
    int AUTORUN;
    bool DOCLEAR;
    float cleartime;
    float instGain; // master multiplier per instrument
    bool DOCMIXPRINT;
    vector<string> ab; // act bus strings for RTCMIX
    vector<int> abn; // act bus numbers for RTCMIX
	
    vector<lukeRhythm> rhythms;
    vector<lukePitchArray> pitches;
    vector<lukeSimpleMelody> simplemelodies;

    vector<lukeSample> looperSamples;

	vector<CloudsSoundCue> currentCues;

	
	ofxTLFlags* presetFlags;
	bool cueFlagsAdded;
	float currentCuesTotalDuration;
	void playCurrentCues();
	
	//new audio fade system;
	float playerSwapTime;
	float playerFadeDuration;
	
	map<string, float> perTrackMix;
	float frontMixAttenuate;
	float backMixAttenuate;
	vector<QueuedTrack> queuedTracks;
};