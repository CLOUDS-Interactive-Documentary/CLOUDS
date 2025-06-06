#include "testApp.h"
#include "lukeFuncs.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    // OF shit
    theFont.loadFont(ofToDataPath("Times New Roman.ttf"), 32);
    ofBackground(0,0,0,255);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofSetBackgroundAuto(false);
    
    
    // RTcmix audio stuff
    sr = 44100;
    nbufs = 2; // you can use more for more processing but latency will suffer    
    nchans = 2; // stereo
    framesize = 512; // sigvs  
    s_audio_outbuf = (short*)malloc(nchans*framesize*sizeof(short)); // audio buffer (interleaved)
    
    // initialize RTcmix
	rtcmixmain();
	maxmsp_rtsetparams(sr, nchans, framesize, NULL, NULL);
    
    // initialize OF audio streaming
    // ALERT: there is a piece of shit bug in this (related to jack, i think)
    // make sure to set your audio output in your audio/midi setup to 44100 24-bit.  
    // anything else will cause the thing to reset to 96k 16bit 
    // and all will go haywire and play an octave higher (or whatever)
    ofSoundStreamSetup(nchans, 0, sr, framesize, nbufs);
    ofSoundStreamStart();

    // launch initial setup score
    RTcmixInit();
    first_vec=1; // we haven't had audio yet
    
    // stash previous scaled mouse positions
    osx = 0.5;
    osy = 0.5;
    
}

//--------------------------------------------------------------
void testApp::update(){
    // whatev...
}

//--------------------------------------------------------------
void testApp::draw(){

    // which routine?
    quadrant = returnQuadrant(ofGetMouseX(), ofGetMouseY());
    
    // get mouse scalars
    if(quadrant%2==0) sx = ofMap(ofGetMouseX(), 0, ofGetWidth()/2, 0., 1.);
    else sx = ofMap(ofGetMouseX(), ofGetWidth()/2, ofGetWidth(), 0., 1.);
    if(quadrant/2==0) sy = ofMap(ofGetMouseY(), 0, ofGetHeight()/2, 0., 1.);
    else sy = ofMap(ofGetMouseY(), ofGetHeight()/2, ofGetHeight(), 0., 1.);
    // ...and scaled delta...
    delta = sqrt((osx-sx)*(osx-sx) + (osy-sy)*(osy-sy));
    osx = sx;
    osy = sy;


    // DRAW SOME STUFF
    
    // low background erase
    ofSetColor(0,0,0,10);
    ofRect(0,0,ofGetWidth(),ofGetHeight());

    // words
    ofSetColor(255,255,255,10);
    theFont.drawString("waves", (ofGetWidth()*0.25)-theFont.stringWidth("waves")/2.0,ofGetHeight()*0.25);    
    theFont.drawString("strum", (ofGetWidth()*0.75)-theFont.stringWidth("strum")/2.0,ofGetHeight()*0.25);    
    theFont.drawString("mallet", (ofGetWidth()*0.25)-theFont.stringWidth("mallet")/2.0,ofGetHeight()*0.75);    
    theFont.drawString("talk", (ofGetWidth()*0.75)-theFont.stringWidth("talk")/2.0,ofGetHeight()*0.75);    

    // lines and circles
    if(quadrant==0) ofSetColor(255, 0, 0, 60);
    else if(quadrant==1) ofSetColor(0, 255, 0, 60);
    else if(quadrant==2) ofSetColor(50, 50, 255, 60);
    else if(quadrant==3) ofSetColor(255,255,0, 60);
    ofLine(0, ofGetHeight()/2, ofGetWidth(),ofGetHeight()/2);
    ofLine(ofGetWidth()/2, 0, ofGetWidth()/2,ofGetHeight());
    ofCircle(ofGetMouseX(), ofGetMouseY(), delta*10.);

}

// RTcmix audio callback (we overrode and put in the pullTraverse()
void testApp::audioRequested(float * output, int bufferSize, int nChannels) {
    
    pullTraverse(NULL, s_audio_outbuf); // grab audio from RTcmix
    
    // fill up the audio buffer
    for (int i = 0; i < bufferSize*nChannels; i++)
    {
        output[i] = (float)s_audio_outbuf[i]/MAXAMP; // transfer to the float *output buf
    }
    
    // fire first audio-generating info upon confirming audio is up and running
    if (first_vec == 1)
    {
        first_vec = 0;
        // launch initial effects chain (reverb)
        REVERB(60*60*12); // gimme twelve hours of reverb, baby.

        // play pretty intro melody
        for(int i = 0;i<12;i++)
        {
            WAVETABLE(i*0.1, 0.1, 0.05, mtof(48.+(i*5)+7), ofRandom(1.0), "thewave", "theamp");
            STRUM(i*0.1, 1.0, 0.1, mtof(48.+(i*5)), 1.0, 1.0, ofRandom(1.0));
        }
        INPUTSOUND("SJ.aif"); // load up steve jobs, poor guy
    }
    
    // not using right now
    if (check_bang() == 1) {
        allownote = 1;
        if(DEBUG) cout << "BANG: " << ofGetElapsedTimef() << endl;
    }
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

    float t, beatoffset;
    
    // some timing shit...
    t = ofGetElapsedTimef();
    float tempo = 0.1;
    float del1, del2;
    beatoffset = tempo-fmod(t,tempo); // use for accurate ahead-of-time quantization for rhythmic triggering
    
    switch (quadrant) {
        case 0:
            // pretty waves
            WAVETABLE(0., 3., 0.05, mtof(scale(int((1.0-sy)*36.)+40., 2)), sy, "themellowwave", "themellowamp");
            break;
        case 1:
            // plucky
            STRUM(0., 1.0, 0.2, mtof(scale(int((1.0-sy)*36.)+50., 5)), ofMap(delta, 0., 1., 5., 0.1), 1., sx);
            break;
        case 2:
            // only in mousePressed(), sorry
            break;
        case 3:
            // steve jobs in a beat
            del1 = tempo*4./(float(round(sx*8.0+1.0))/8.0);
            del2 = tempo*4./(float(round((1.0-sx)*8.0+1.0))/8.0);
            SCHEDULEBANG(beatoffset);
            if(allownote==1) {
                PANECHO(0+beatoffset, sy, ofMap(delta, 0., 1., 0.0, tempo), 0.7, del1, del2, 0.4, 10.);
                allownote=0;
            }
            if(DEBUG) cout << beatoffset << " " << del1 << " " << del2 << endl;
            break;            
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
    float t, beatoffset;

    // some timing shit...
    t = ofGetElapsedTimef();
    float tempo = 0.125;
    beatoffset = tempo-fmod(t,tempo); // use for accurate ahead-of-time quantization for rhythmic triggering

   // int bpattern[] = {1,0,1,1,1,0,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,1,1};
    int bpatter[] = {1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 };
    

    switch (quadrant) {
        case 0:
            // fast attack waves
            WAVETABLE(0., 0.1, 0.2, mtof(scale(int((1.0-sy)*36.)+28., 2)), sy, "thewave", "theamp");
            break;
        case 1:
            // plucks
            STRUM(0., 1.0, 0.3, mtof(scale(int((1.0-sy)*36.)+50., 5)), 1.0, 1., sx);
            STRUM(0.125, 1.0, 0.2, mtof(scale(int((1.0-sy)*36.)+50.+4., 5)), ofMap(delta, 0., 1., 5., 0.1), 1., sx);
            STRUM(0.25, 1.0, 0.2, mtof(scale(int((1.0-sy)*36.)+50.+7., 5)), ofMap(delta, 0., 1., 5., 0.1), 1., sx);
            break;
        case 2:
            // beats
            for(int i = 0;i<32;i++)
            {
                if(bpattern[i]==1) {
            MMODALBAR(i*tempo+beatoffset, 1., 0.2, mtof(scale(int((1.0-sy)*36.)+40., 2)), ofMap(i,0,31,0.1,0.9), sx, int(ofRandom(8)));
                }
            }
            if(DEBUG) cout << sx << " " << sy << " " << delta << endl;
            break;
        case 3:
            // only in mouseDragged(), sorry
            break;            
        default:
            break;
    }

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

// utility routine to find where we are clicking
int testApp::returnQuadrant(int x, int y)
{
    int foo = 3;
    if(x<ofGetWidth()/2) foo--;
    if(y<ofGetHeight()/2) foo-=2;
    
    return(foo);
    
}
