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
    RTcmixParseScoreFile("cmixinit.sco");
    first_vec=1; // we haven't had audio yet
      
    loadRTcmixFiles();
    
    MASTERAMP = 1.0;
    MASTERTEMPO = 120;
    mbank = "luke";
    AUTORUN = 0;
    DOCLEAR = true;
    RTCMIX_PRINT = false;
    cleartime = ofGetElapsedTimef();

    // load samples
    loadRTcmixSamples();
    
    // display
    pushInterface();
    
    // test scale
    /*for(int k=60;k<75;k++)
    {
        int q = scale(k, 9);
        cout << k << ": " << ptos(k) << " maps to " << q << ": " << ptos(q) << endl;
    }*/
        
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    
    if(DOCLEAR) {
    
        ofClear(0,0,0);
        // draw some shit
        ofSetColor(255,255,0); 
        ofDrawBitmapString("Luke's CLOUDS Music Tester", 20, 20); 
    
        // buttons and labels
        startbutton.draw();
        stopbutton.draw();
        autobutton.draw();
        ofSetColor(255,255,255,255);
        theFont.drawString("orchestra:", 50,80);
        for(int i = 0;i<orchbutton.size();i++)
        {
            orchbutton[i].draw();
        }
        theFont.drawString("harmony:", 250,130);
        for(int i = 0;i<harmonybutton.size();i++)
        {
            harmonybutton[i].draw();
        }
        theFont.drawString("rhythm:", 250,180);
        for(int i = 0;i<rhythmbutton.size();i++)
        {
            rhythmbutton[i].draw();
        }
        theFont.drawString("preset:", 250,230);
        for(int i = 0;i<presetbutton.size();i++)
        {
            presetbutton[i].draw();
        }

        theFont.drawString("volume (up/down keys):" + ofToString(MASTERAMP), 300,300);
        theFont.drawString("tempo (left/right keys):" + ofToString(MASTERTEMPO), 300,325);

        if(ofGetElapsedTimef()-cleartime>0.1) DOCLEAR = false;
    }
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
            STEREO(i*0.1, 0., 1., 0.05, i/11.0, "BD");
        }
        
        // launch initial effects chain (reverb)
        REVERB(5.0); // gimme some reverb
    }
    
    // not using right now
    if (check_bang() == 1) {
        if(AUTORUN==0)
        {
            allownote = 1;
            startbutton.state = false;
            stopbutton.state = true;
        }
        else
        {
            for(int j = 0;j<orchbutton.size();j++)
            {
                orchbutton[j].state = false;
            }
            morch.clear();
            for(int j = 0;j<3;j++)
            {
                int p = ofRandom(0,orchestra.size());
                orchbutton[p].state = true;
                morch.push_back(orchestra[p]);
            }
            mharmony = ofRandom(0, pitches.size());
            mrhythm = ofRandom(0, rhythms.size());
            for(int j = 0;j<harmonybutton.size();j++)
            {
                harmonybutton[j].state = false;
            }
            harmonybutton[mharmony].state = true;
            for(int j = 0;j<rhythmbutton.size();j++)
            {
                rhythmbutton[j].state = false;
            }
            rhythmbutton[mrhythm].state = true;
            startMusic(morch, mharmony, mrhythm, 5*60.);
            DOCLEAR = true;
        }
        if(DEBUG) cout << "BANG: " << ofGetElapsedTimef() << endl;
    }
    
    if(RTCMIX_PRINT)
    {
    char *pbuf = get_print();
    char *pbufptr = pbuf;
    while (strlen(pbufptr) > 0) {
        cout << pbufptr << endl;
        pbufptr += (strlen(pbufptr) + 1);
    }
    
    reset_print();
    }
    

}



//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    DOCLEAR = true;
    cleartime = ofGetElapsedTimef();
    if (key == 'l'){
        loadRTcmixFiles();
        pushInterface();
    }
    if (key == 'p') {
        RTCMIX_PRINT = !RTCMIX_PRINT;
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
    if(key == OF_KEY_LEFT)
    {
        MASTERTEMPO-=1;
    }
    else if(key == OF_KEY_RIGHT)
    {
        MASTERTEMPO+=1;
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
    
    DOCLEAR = true;
    cleartime = ofGetElapsedTimef();
    
    p = startbutton.test(x,y);
    if(p) {
        stopbutton.state = false;
        startbutton.state = true;
        startMusic(morch, mharmony, mrhythm, 5*60.);
    }
    p = stopbutton.test(x, y);
    if(p) {
        startbutton.state = false;
        stopbutton.state = true;
        stopMusic();
    }
    p = autobutton.test(x, y);
    if(p) {
        AUTORUN = 1-AUTORUN;
        autobutton.state = AUTORUN>0;
    }
    for(int i = 0;i<orchbutton.size();i++)
    {
        p = orchbutton[i].test(x, y);
        if(p)
        {
            orchbutton[i].state = !orchbutton[i].state;
            if(orchbutton[i].state==true) // add to orchestration list
            {
                int matched = 0;
                for(int j = 0;j<morch.size();j++)
                {
                    if(morch[j].compare(orchestra[i])==0) // already there
                    {
                        matched = 1;
                    }
                }
                if(matched==0) // add
                {
                    morch.push_back(orchestra[i]);
                }
            
            }
            else // remove from orchestration list
            {
                int matched = -1;
                for(int j = 0;j<morch.size();j++)
                {
                    if(morch[j].compare(orchestra[i])==0) // already there
                    {
                        matched = j;
                    }
                }
                if(matched>-1)
                {
                    morch.erase(morch.begin()+matched);
                }
                
            }
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
    for(int i = 0;i<presetbutton.size();i++)
    {
        p = presetbutton[i].test(x, y);
        if(p)
        {
            morch.clear();
            for(int h = 0;h<orchbutton.size();h++)
            {
                orchbutton[h].state = false;
            }
            for(int j = 0;j<presets[i].instruments.size();j++)
            {
                morch.push_back(presets[i].instruments[j]);
                for(int k = 0;k<orchbutton.size();k++)
                {
                    if(orchbutton[k].label.compare(presets[i].instruments[j])==0)
                    {
                        orchbutton[k].state = true;
                    }
                }
            }
            harmonybutton[mharmony].state = false;
            mharmony = presets[i].harmony;
            harmonybutton[mharmony].state = true;
            rhythmbutton[mrhythm].state = false;
            mrhythm = presets[i].rhythm;
            rhythmbutton[mrhythm].state = true;
            MASTERTEMPO = presets[i].tempo;
            mbank = presets[i].bank;
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

void testApp::pushInterface()
{
    char sb [2];
    string s;
    startbutton.init("start", &theFont, 300, 450, 0, 255, 0);
    stopbutton.init("stop", &theFont, 700, 450, 255, 0, 0);
    stopbutton.state = true;
    autobutton.init("auto", &theFont, 500, 450, 96, 96, 255);
    // orchestra buttons
    orchbutton.clear();
    for(int i = 0;i<orchestra.size();i++)
    {
        lukeButton c;
        s = orchestra[i];
        c.init(s, &theFont, 50, 100 + i*theFont.getLineHeight(), 192, 192, 64);
        orchbutton.push_back(c);
    }
    orchbutton[0].state = true;
    morch.clear();
    morch.push_back(orchestra[0]);
    // harmony buttons
    harmonybutton.clear();
    for(int i = 0;i<pitches.size();i++)
    {   
        lukeButton c;
        sprintf(sb, "%.2i", i+1);
        s = sb;
        c.init(s, &theFont, 250 + i*30, 150, 192, 64, 192);
        harmonybutton.push_back(c);
    }
    harmonybutton[0].state = true;
    mharmony = 0;
    // rhythm buttons
    rhythmbutton.clear();
    for(int i = 0;i<rhythms.size();i++)
    {   
        lukeButton c;
        sprintf(sb, "%.2i", i+1);
        s = sb;
        c.init(s, &theFont, 250 + i*30, 200, 64, 192, 192);
        rhythmbutton.push_back(c);
    }
    rhythmbutton[0].state = true;
    mrhythm = 0;
    // preset buttons
    presetbutton.clear();
    for(int i = 0;i<presets.size();i++)
    {   
        lukeButton c;
        sprintf(sb, "%.2i", i+1);
        s = sb;
        c.init(s, &theFont, 250 + i*30, 250, 255, 255, 255);
        presetbutton.push_back(c);
    }

}

void testApp::loadRTcmixFiles()
{
    registerOrchs();
    cout << "orchestration:" << endl;
    for(int i = 0;i<orchestra.size();i++)
    {
        cout << orchestra[i] << endl;
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

    loadpresets("presets.txt", presets);
    cout << "presets:" << endl;
    for(int i = 0;i<presets.size();i++)
    {
        for(int j = 0;j<presets[i].instruments.size();j++)
        {
            cout << presets[i].instruments[j] << " ";
        }
        cout << presets[i].harmony << " ";
        cout << presets[i].rhythm << " ";
        cout << presets[i].tempo << " ";
        cout << endl;
    }


}

void testApp::loadRTcmixSamples()
{
    LOADSOUND("RTCMIX/samps/BD.aif", "BD");
    LOADSOUND("RTCMIX/samps/SD.aif", "SD");
    LOADSOUND("RTCMIX/samps/CH.aif", "CH");
    LOADSOUND("RTCMIX/samps/OH.aif", "OH");
    LOADSOUND("RTCMIX/samps/BD2.aif", "BD2");
    LOADSOUND("RTCMIX/samps/VD1.aif", "VD1");
    LOADSOUND("RTCMIX/samps/VD2.aif", "VD2");
    LOADSOUND("RTCMIX/samps/VD3.aif", "VD3");
    LOADSOUND("RTCMIX/samps/VD4.aif", "VD4");
    tl1 = LOADSOUND("RTCMIX/samps/testloop1.aif", "testloop1");
    tl2 = LOADSOUND("RTCMIX/samps/testloop2.aif", "testloop2");
    tl3 = LOADSOUND("RTCMIX/samps/testloop3.aif", "testloop3");
    bl1 = LOADSOUND("RTCMIX/samps/loop1Cm120bpm.aif", "bassloop1");
    
    //some path, may be absolute or relative to bin/data
    string path = ofToDataPath("RTCMIX/samps/loops/");
    ofDirectory dir(path);
    //only show png files
    dir.allowExt("wav");
    dir.allowExt("aif");
    //populate the directory object
    dir.listDir();

    //go through and print out all the paths
    for(int i = 0; i < dir.numFiles(); i++){
        lukeSample foo;
        foo.filename = dir.getPath(i);
        foo.handle = dir.getName(i);
        foo.length = LOADSOUND(foo.filename, foo.handle);
        foo.bank = ofSplitString(foo.handle, "_")[0];
        string len = ofSplitString(foo.handle, "_")[2];
        len = ofSplitString(len, ".")[0];
        foo.numbeats = ofToInt(len);
        cout << foo.handle << " " << foo.bank << " " << foo.numbeats << endl;
        looperSamples.push_back(foo);
    }
    
    
}

void testApp::registerOrchs()
{
    orchestra.clear();
    orchestra.push_back("slowwaves");
    orchestra.push_back("modalbeats");
    orchestra.push_back("helmholtz");
    orchestra.push_back("meshbeats");
    orchestra.push_back("filternoise");
    orchestra.push_back("lowwavepulse");
    orchestra.push_back("slowwaveshi");
    orchestra.push_back("slowmeshbeats");
    orchestra.push_back("slowwave");
    orchestra.push_back("strumsine");
    orchestra.push_back("waveguide");
    orchestra.push_back("waveguidebeats");
    orchestra.push_back("phatbeatz");
    orchestra.push_back("vermontbeatz");
    orchestra.push_back("waveshipatterned");
    orchestra.push_back("testloop3");
    orchestra.push_back("kissmyarp");
    orchestra.push_back("testloop1");
    orchestra.push_back("reichomatic");
    orchestra.push_back("glassomatic");
    orchestra.push_back("kissmyarpfast");
    orchestra.push_back("kissmyarpsynch");
}

void testApp::startMusic(vector<string> mo, int mh, int mr, float musicdur)
{
    
    float t, beatoffset;
    float i, j;
	
    // some timing shit...
    t = ofGetElapsedTimef();
    float tempo = (15./MASTERTEMPO);
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
    
    vector<string> ilist = mo; // list of instruments
    
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
    
    // WAVESHIPATTERNED
    if (find(ilist.begin(), ilist.end(), "waveshipatterned") != ilist.end())
    {
        int pick = 0;
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            if(rhythms[mr].beats[bcount]>0.) {
            float d0 = ofRandom(0.1, 0.5);
            float of1 = d0*ofRandom(0.3, 0.7);
            float d1 = d0+of1;
            float of2 = d1*ofRandom(0.3, 0.7);
            float d2 = d1+of1+of2;
            float freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote+12., pitches[mh].scale));
            WAVETABLE(i, d0, 0.02, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            WAVETABLE(i+of1, d1, 0.02, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            WAVETABLE(i+of2, d2, 0.02, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            pick = (pick+1) % pitches[mh].notes.size();
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }

    
    // KISS MY ARP
    if (find(ilist.begin(), ilist.end(), "kissmyarp") != ilist.end())
    {
        int pick = 0;
        for(i = 0;i<musicdur;i+=tempo*2)
        {
            int oct = ofRandom(0., 1.)*12;
            int pitch = pitches[mh].notes[pick] % 12;
            pitch+=pitches[mh].basenote;
            pitch+=oct;
            pitch = scale(pitch, pitches[mh].scale);
            // cout << "doing pitch: " << ptos(pitch) << endl;
            float freq = mtof(pitch);
            WAVETABLE(i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            WAVETABLE(i+tempo*6, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            pick = (pick+1) % pitches[mh].notes.size();
        }
    }

    // KISS MY ARP SYNCH
    if (find(ilist.begin(), ilist.end(), "kissmyarpsynch") != ilist.end())
    {
        int pick = 0;
        for(i = 0;i<musicdur;i+=tempo*2)
        {
            if(rhythms[mr].beats[bcount]>0.) {
            int oct = ofRandom(0., 1.)*12;
            int pitch = pitches[mh].notes[pick] % 12;
            pitch+=pitches[mh].basenote;
            pitch+=oct;
            pitch = scale(pitch, pitches[mh].scale);
            // cout << "doing pitch: " << ptos(pitch) << endl;
            float freq = mtof(pitch);
            WAVETABLE(i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            WAVETABLE(i+tempo*6, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            pick = (pick+1) % pitches[mh].notes.size();
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }

    // KISS MY ARP FAST
    if (find(ilist.begin(), ilist.end(), "kissmyarpfast") != ilist.end())
    {
        int pick = 0;
        for(i = 0;i<musicdur;i+=tempo)
        {
            int oct = ofRandom(0., 2.)*12;
            int pitch = pitches[mh].notes[pick] % 12;
            pitch+=pitches[mh].basenote;
            pitch+=oct;
            pitch = scale(pitch, pitches[mh].scale);
            // cout << "doing pitch: " << ptos(pitch) << endl;
            float freq = mtof(pitch);
            WAVETABLE(i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            WAVETABLE(i+tempo*3, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            pick = (pick+1) % pitches[mh].notes.size();
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
    
    // PHATBEATZ
    if (find(ilist.begin(), ilist.end(), "phatbeatz") != ilist.end())
    {
        int pick;
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                
                float t_amp = rhythms[mr].beats[bcount]*ofRandom(0.1, 0.2);
                pick = (int)ofRandom(0, 3);
                if(pick<2) STEREO(i, 0., 0.5, t_amp, 0.5, "BD"); else STEREO(i, 0., 0.5, t_amp, 0.5, "SD");
            }
            pick = ofRandom(0,4);
            if(pick<2) STEREO(i, 0., 0.5, ofRandom(0.05, 0.2), 0.5, "CH");
            else if(pick==2) STEREO(i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "OH");
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }
    
    // VERMONTBEATZ
    if (find(ilist.begin(), ilist.end(), "vermontbeatz") != ilist.end())
    {
        int pick;
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            if(rhythms[mr].beats[bcount]>0.) { // this is a note!
                float t_amp = rhythms[mr].beats[bcount]*ofRandom(0.1, 0.2);
                STEREO(i, 0., 0.5, t_amp, 0.5, "BD2");
            }
            else
            {
                pick = ofRandom(0,5);
                if(pick==1) STEREO(i, 0., 0.5, ofRandom(0.05, 0.2), 0.5, "VD1");
                else if(pick==2) STEREO(i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "VD2");
                else if(pick==3) STEREO(i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "VD3");
                else if(pick==4) STEREO(i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "VD4");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }
    
    // TESTLOOP1
    if (find(ilist.begin(), ilist.end(), "testloop1") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*64.)
        {
            SOUNDLOOP(i, tl1, tempo*64., 0.25, "testloop1");
        }
    }

    // TESTLOOP2
    if (find(ilist.begin(), ilist.end(), "testloop2") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*64.)
        {
            SOUNDLOOP(i, tl2, tempo*64., 0.25, "testloop2");
        }
    }
    
    // TESTLOOP3
    if (find(ilist.begin(), ilist.end(), "testloop3") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*64.)
        {
            SOUNDLOOP(i, tl3, tempo*64., 0.25, "testloop3");
        }
    }

    // BASSLOOP1
    if (find(ilist.begin(), ilist.end(), "bassloop1") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*64)
        {
            SOUNDLOOP(i, bl1, tempo*64., 0.25, "bassloop1");
        }
    }
    
    // REICHOMATIC
    if (find(ilist.begin(), ilist.end(), "reichomatic") != ilist.end())
    {
        cout << "Sample number: " << looperSamples.size() << endl;
        for(i = 0;i<looperSamples.size();i++)
        {
            if(looperSamples[i].bank==mbank)
            {
                cout << "Sample: " << looperSamples[i].handle << endl;
                for(j = 0;j<musicdur;j+=tempo*looperSamples[i].numbeats*4)
                {
                    int p = ofRandom(0, 3);
                    if(p<1)
                    {
                        cout << "Playing: " << looperSamples[i].handle << " of length " << looperSamples[i].length << " at " << j << endl;
                        SOUNDLOOPMONO(j, looperSamples[i].length, tempo*looperSamples[i].numbeats*4, 0.25, looperSamples[i].handle, ofRandom(0.,1));
                    }
                }
            }
        }
    }
    
    // GLASSOMATIC
    if (find(ilist.begin(), ilist.end(), "glassomatic") != ilist.end())
    {
        cout << "Sample number: " << looperSamples.size() << endl;
        for(i = 0;i<looperSamples.size();i++)
        {
            if(looperSamples[i].bank==mbank)
            {
                for(j = 0;j<musicdur;j+=tempo*looperSamples[i].numbeats*4)
                {
                    SOUNDLOOPMONO(j, looperSamples[i].length, tempo*looperSamples[i].numbeats*4, 0.25, looperSamples[i].handle, (float)i/looperSamples.size()-1);
                }
            }
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
    return(mx>x&&mx<x+sw&&my>y-sh&&my<y);
}
