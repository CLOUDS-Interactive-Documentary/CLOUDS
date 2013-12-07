#pragma once

#include "ofMain.h"
#include "lukeFuncs.h"

//
// luke's shitty UI stuff
//

class lukeButton {
public:
	string label;
    ofTrueTypeFont *myFont;
	int x;
	int y;
    bool state;
	int r, g, b;
    
    lukeButton();
    void init(string, ofTrueTypeFont*, int, int, int, int, int);

    void draw();
    bool test(int, int);
};


// main class:

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    
    // Luke's stuff
	// 
    void startMusic(vector<string> mo, int mh, int mr, float musicdur);
    void stopMusic();
    void loadRTcmixFiles();
    void loadRTcmixSamples();
    void registerOrchs();
    void pushInterface();
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
    int mharmony, mrhythm;
    vector<string> morch;
    string mbank;
    
    float MASTERAMP;
    float MASTERTEMPO;
    int AUTORUN;
    bool DOCLEAR;
    bool RTCMIX_PRINT;
    float cleartime;
    float tl1, tl2, tl3, bl1;
    
    vector<lukeRhythm> rhythms;
    vector<lukePitchArray> pitches;
    vector<lukePreset> presets;
    vector<string> orchestra;
    
    vector<lukeSample> looperSamples;

    // ui crap
    ofTrueTypeFont theFont;
    lukeButton startbutton, stopbutton, autobutton;
    vector<lukeButton> orchbutton;
    vector<lukeButton> colorbutton;
    vector<lukeButton> harmonybutton;
    vector<lukeButton> rhythmbutton;
    vector<lukeButton> presetbutton;
        
};
