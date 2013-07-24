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

    
    // Luke's vars
	// 
    void startMusic(int mc, int mh, int mr, float musicdur);
    void stopMusic();
    void loadRTcmixFiles();
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
    
    vector<lukeRhythm> rhythms;
    vector<lukePitchArray> pitches;
    vector<lukeColor> colors;

    // ui crap
    ofTrueTypeFont theFont;
    lukeButton startbutton, stopbutton;
    vector<lukeButton> colorbutton;
    vector<lukeButton> harmonybutton;
    vector<lukeButton> rhythmbutton;
    
};
