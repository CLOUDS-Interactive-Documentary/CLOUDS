#include "testApp.h"
#include "lukeFuncs.h"

//--------------------------------------------------------------
void testApp::setup(){

    // OF shit
    theFont.loadFont(ofToDataPath("Courier New Bold.ttf"), 12);
    ofBackground(0,0,0,255);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofSetBackgroundAuto(true);
    
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
    RTcmixParseScoreFile("cmixinit.sco");
    first_vec=1; // we haven't had audio yet
      
    loadRTcmixFiles();
    
    MASTERAMP = 1.0;

    char sb [2];
    string s;
    startbutton.init("start", &theFont, 200, 450, 0, 255, 0);
    stopbutton.init("stop", &theFont, 600, 450, 255, 0, 0);
    stopbutton.state = true;
    for(int i = 0;i<16;i++)
    {   
        lukeButton c;
        sprintf(sb, "%.2i", i+1);
        s = sb;
        c.init(s, &theFont, 50 + i*30, 100, 192, 192, 64);
        colorbutton.push_back(c);
    }
    colorbutton[0].state = true;
    mcolor = 0;
    for(int i = 0;i<16;i++)
    {   
        lukeButton c;
        sprintf(sb, "%.2i", i+1);
        s = sb;
        c.init(s, &theFont, 50 + i*30, 150, 192, 64, 192);
        harmonybutton.push_back(c);
    }
    harmonybutton[0].state = true;
    mharmony = 0;
    for(int i = 0;i<16;i++)
    {   
        lukeButton c;
        sprintf(sb, "%.2i", i+1);
        s = sb;
        c.init(s, &theFont, 50 + i*30, 200, 64, 192, 192);
        rhythmbutton.push_back(c);
    }
    rhythmbutton[0].state = true;
    mrhythm = 0;

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    
    // draw some shit
    ofSetColor(255,255,0); 
    ofDrawBitmapString("Luke's Music Tester", 20, 20); 
    
    // buttons and labels
    startbutton.draw();
    stopbutton.draw();
    ofSetColor(255,255,255,255);
    theFont.drawString("color:", 50,80);        
    for(int i = 0;i<colorbutton.size();i++)
    {
        colorbutton[i].draw();
    }
    theFont.drawString("harmony:", 50,130);        
    for(int i = 0;i<colorbutton.size();i++)
    {
        harmonybutton[i].draw();
    }
    theFont.drawString("rhythm:", 50,180);        
    for(int i = 0;i<colorbutton.size();i++)
    {
        rhythmbutton[i].draw();
    }

    theFont.drawString("volume:" + ofToString(MASTERAMP), 300,300);        
}

// RTcmix audio callback (we overrode and put in the pullTraverse()
void testApp::audioRequested(float * output, int bufferSize, int nChannels) {
    
    pullTraverse(NULL, s_audio_outbuf); // grab audio from RTcmix
    
    // fill up the audio buffer
    for (int i = 0; i < bufferSize*nChannels; i++)
    {
        output[i] = MASTERAMP*(float)s_audio_outbuf[i]/MAXAMP; // transfer to the float *output buf
    }
    
    // fire first audio-generating info upon confirming audio is up and running
    if (first_vec == 1)
    {
        first_vec = 0;
        
        // play pretty intro melody
        for(int i = 0;i<12;i++)
        {
            WAVETABLE(i*0.1, 0.1, 0.05, mtof(48.+(i*5)+7), ofRandom(1.0), "wf_organ", "amp_sharpadsr");
            STRUM(i*0.1, 1.0, 0.1, mtof(48.+(i*5)), 1.0, 1.0, ofRandom(1.0));
        }
        // launch initial effects chain (reverb)
        REVERB(5.0); // gimme some reverb
    }
    
    // not using right now
    if (check_bang() == 1) {
        allownote = 1;
        startbutton.state = false;
        stopbutton.state = true;
        if(DEBUG) cout << "BANG: " << ofGetElapsedTimef() << endl;
    }
}



//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    if (key == 'l'){
        loadRTcmixFiles();
    }
    if (key == OF_KEY_DOWN)
    {
        MASTERAMP-=0.1;
        if(MASTERAMP<0.) MASTERAMP=0.;
    }
    if (key == OF_KEY_UP)
    {
        MASTERAMP+=0.1;
        if(MASTERAMP>2.) MASTERAMP=2.;
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

    bool p;
    
    p = startbutton.test(x,y);
    if(p) {
        stopbutton.state = false;
        startbutton.state = true;
        startMusic(mcolor, mharmony, mrhythm, 5*60.);
    }
    p = stopbutton.test(x, y);
    if(p) {
        startbutton.state = false;
        stopbutton.state = true;
        stopMusic();
    }
    for(int i = 0;i<colorbutton.size();i++)
    {
        p = colorbutton[i].test(x, y);
        if(p)
        {
            for(int j = 0;j<colorbutton.size();j++)
            {
                colorbutton[j].state = false;
            }
            colorbutton[i].state = true;
            mcolor = i;
        }
    }
    for(int i = 0;i<harmonybutton.size();i++)
    {
        p = harmonybutton[i].test(x, y);
        if(p)
        {
            for(int j = 0;j<harmonybutton.size();j++)
            {
                harmonybutton[j].state = false;
            }
            harmonybutton[i].state = true;
            mharmony = i;
        }
    }
    for(int i = 0;i<rhythmbutton.size();i++)
    {
        p = rhythmbutton[i].test(x, y);
        if(p)
        {
            for(int j = 0;j<rhythmbutton.size();j++)
            {
                rhythmbutton[j].state = false;
            }
            rhythmbutton[i].state = true;
            mrhythm = i;
        }
    }
    
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

void testApp::loadRTcmixFiles()
{
    loadcolors("colors.txt", colors);
    cout << "colors:" << endl;
    for(int i = 0;i<colors.size();i++)
    {
        for(int j = 0;j<colors[i].instruments.size();j++)
        {
            cout << colors[i].instruments[j] << " ";
        }
        cout << endl;
    }
    
    loadrhythms("rhythms.txt", rhythms);
    cout << "rhythms:" << endl;
    for(int i = 0;i<rhythms.size();i++)
    {
        for(int j = 0;j<rhythms[i].beats.size();j++)
        {
            cout << rhythms[i].beats[j] << " ";
        }
        cout << endl;
    }
    
    loadpitches("pitches.txt", pitches);
    cout << "pitches:" << endl;
    for(int i = 0;i<pitches.size();i++)
    {
        for(int j = 0;j<pitches[i].notes.size();j++)
        {
            cout << pitches[i].notes[j] << " ";
        }
        cout << endl;
    }
    

}

void testApp::startMusic(int mc, int mh, int mr, float musicdur)
{
    
    float t, beatoffset;
    float i, j;
	
    // some timing shit...
    t = ofGetElapsedTimef();
    float tempo = 0.125;
    int bcount = 0;
    beatoffset = tempo-fmod(t,tempo); // use for accurate ahead-of-time quantization for rhythmic triggering
	
    flush_sched(); // kill previous music

    // REVERB
    REVERB(musicdur+7.0); // gimme some reverb

    //
    // =========================
    // BEGIN ORCHESTRATION BLOCK
    // =========================
    //
    
    vector<string> ilist = colors[mc].instruments; // list of instruments
    
    // MODALBEATS
    if (find(ilist.begin(), ilist.end(), "modalbeats") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo)
        {
            if(rhythms[mr].beats[bcount]>0.) {
            
                float t_amp = (1.0-fabs((i/musicdur)-0.5)*2.)*rhythms[mr].beats[bcount];
                float pick = (int)ofRandom(0, pitches[mh].notes.size());
                float t_freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale));
            
                MMODALBAR(i, 1., t_amp*0.2, t_freq, ofRandom(0.1,0.9), ofRandom(0.,1.), int(ofRandom(8))    );
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    
    }

    // WAVEGUIDEBEATS
    if (find(ilist.begin(), ilist.end(), "waveguidebeats") != ilist.end())
    {
        int preset = 3;
        for(i = 0;i<musicdur;i+=tempo)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                
                float t_amp = rhythms[mr].beats[bcount];
                float pick = (int)ofRandom(0, pitches[mh].notes.size());
                float t_freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale));
                
                MBANDEDWG(i, ofRandom(0.05, 0.5), t_amp*ofRandom(0.05, 0.15), t_freq, ofRandom(0.,1.), ofRandom(0.,1.)>0.5, ofRandom(0.7, 1.0), preset, ofRandom(0.8, 1.), 0.99, 0., ofRandom(0.,1.), "vel_strike");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
        
    }

    // LOWWAVEPULSE
    if (find(ilist.begin(), ilist.end(), "lowwavepulse") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo)
        {
            float p = 0.7;
            if(rhythms[mr].beats[bcount]>0.) {
                
                float t_amp = rhythms[mr].beats[bcount];
                float pick = (int)ofRandom(0, pitches[mh].notes.size());
                float t_freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale))*0.5;
                
                WAVETABLE(i, ofRandom(0.1, 0.3), 0.05, t_freq, p, "wf_organ", "amp_sharpadsr");
                WAVETABLE(i, ofRandom(0.1, 0.3), 0.05, t_freq*1.5, 1.-p, "wf_organ", "amp_sharpadsr");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
            p = 1.0-p;
        }
        
    }

    // MESHBEATS
    if (find(ilist.begin(), ilist.end(), "meshbeats") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo)
        {
                float t_amp = (1.0-fabs((i/musicdur)-0.5)*2.)*0.38;
                int nx = ofRandom(2,12);
                int ny = ofRandom(2,12);
                MMESH2D(i, 1., t_amp*0.5, nx, ny, ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.));
        }
        
    }

    // SLOWMESHBEATS
    if (find(ilist.begin(), ilist.end(), "slowmeshbeats") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            float t_amp = (1.0-fabs((i/musicdur)-0.5)*2.)*0.38;
            int nx = ofRandom(2,12);
            int ny = ofRandom(2,12);
            MMESH2D(i, 1., t_amp*0.5, nx, ny, ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.));
        }
        
    }

    // SLOWWAVES
    if (find(ilist.begin(), ilist.end(), "slowwaves") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(4, 16)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale));
            WAVETABLE(i, ofRandom(3., 10.), 0.025, freq, ofRandom(0.,1.), "wf_slowwaves", "amp_triangle");
            WAVETABLE(i, ofRandom(3., 10.), 0.025, freq*0.99, ofRandom(0.,1.), "wf_slowwaves", "amp_triangle");
        }

    }

    // SLOWWAVESHI
    if (find(ilist.begin(), ilist.end(), "slowwaveshi") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float d0 = ofRandom(2., 6.);
            float of1 = d0*ofRandom(0.3, 0.7);
            float d1 = d0+of1;
            float of2 = d1*ofRandom(0.3, 0.7);
            float d2 = d1+of1+of2;
            float freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote+12., pitches[mh].scale));
            WAVETABLE(i, d0, 0.02, freq, ofRandom(0.,1.), "wf_slowwaveshi", "amp_triangle");
            WAVETABLE(i+of1, d1, 0.02, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_slowwaveshi", "amp_triangle");
            WAVETABLE(i+of2, d2, 0.02, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_slowwaveshi", "amp_triangle");
        }
        
    }

    // HELMHOLTZ
    if (find(ilist.begin(), ilist.end(), "helmholtz") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(4, 16)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale))*2.;
            MBLOWBOTL(i, ofRandom(1., 3.), 0.25, freq, ofRandom(0.05, 0.2), ofRandom(0.5, 0.9), ofRandom(0.,1.), "amp_sharp", "amp_triangle");
            MBLOWBOTL(i+tempo*floor(ofRandom(0, 4)), ofRandom(1., 3.), 0.25, freq*1.5, ofRandom(0.01, 0.08), ofRandom(0.5, 0.9), ofRandom(0.,1.), "amp_sharp", "amp_triangle");
        }
        
    }

    // FILTERNOISE
    if (find(ilist.begin(), ilist.end(), "filternoise") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale))*2.;
            FNOISE3(i, ofRandom(1., 3.), 0.25, 1.0, ofRandom(0.,1.), freq, freq*2.0, freq*3.0, 90., "amp_triangle");
        }
        
    }

    // STRUMSINE
    if (find(ilist.begin(), ilist.end(), "strumsine") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float pitch = scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale);
            WAVETABLE(i, 2., 0.025, mtof(pitch), ofRandom(1.0), "wf_organ", "amp_sharpadsr");
            for(j=0;j<tempo;j+=(tempo/floor(ofRandom(4,8))))
            {
                    
                STRUM(i+j, 1.0, 0.05, mtof(pitch), ofRandom(1.0, 5.0), ofRandom(1.0, 5.0), ofRandom(1.0));
                int tr = ofRandom(0, 5);
                if(tr==0) pitch+=7;
            }
        }
        PANECHO(0., 0., musicdur+5., 1., tempo*4., tempo*6., 0.7, 7.);
        
    }
    
    // WAVEGUIDE
    if (find(ilist.begin(), ilist.end(), "waveguide") != ilist.end())
    {
        int preset = 3;
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(6, 24)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float pitch = scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale);
            MBANDEDWG(i, ofRandom(7., 15.0), ofRandom(0.05, 0.15), mtof(pitch), ofRandom(0.,1.), ofRandom(0.,1.)>0.5, ofRandom(0.7, 1.0), preset, ofRandom(0.8, 1.), 0.99, 0., ofRandom(0.,1.), "vel_strike");
        
        }
    }
    
    //
    // =======================
    // END ORCHESTRATION BLOCK
    // =======================
    //

    // schedule end event at time + ring-down
    SCHEDULEBANG(musicdur+7.0);
    
}

void testApp::stopMusic()
{
    flush_sched();
}


//
// luke's shitty UI stuff
//

lukeButton::lukeButton()
{
    label = " ";
    x = 0;
    y = 0;
    state = false;
    r = 0;
    g = 0;
    b = 0;
    myFont = NULL;
}

void lukeButton::init(string c_label, ofTrueTypeFont *f, int c_x, int c_y, int c_r, int c_g, int c_b)
{
    label = c_label;
    myFont = f;
    x = c_x;
    y = c_y;
    r = c_r;
    g = c_g;
    b = c_b;
    state = false;
}

void lukeButton::draw()
{
    if (myFont != NULL){
        ofSetColor(r,g,b,state?255:50);        
        float sw = myFont->stringWidth(label);
        float sh = myFont->getLineHeight();
        ofRect(x,y-sh,sw,sh);
        ofSetColor(255,255,255,255);
        myFont->drawString(label, x,y);
    }
}

bool lukeButton::test(int mx, int my)
{
    float sw = myFont->stringWidth(label);
    float sh = myFont->getLineHeight();
    return(mx>x&&mx<x+sw&&my>y-sh&&my<y+sh);
}
