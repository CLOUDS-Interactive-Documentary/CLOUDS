
#include "CloudsSound.h"
#include "lukeFuncs.h"

CloudsSound::CloudsSound(){
	storyEngine = NULL;
	eventsRegistered = false;
}

//--------------------------------------------------------------------
void CloudsSound::setup(CloudsStoryEngine& storyEngine){
	this->storyEngine = &storyEngine;
	
	if(!eventsRegistered){
		ofAddListener(storyEngine.getEvents().actCreated, this, &CloudsSound::actCreated);
		ofAddListener(ofEvents().exit, this, &CloudsSound::exit);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
	
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
        
        // load samples
        loadRTcmixSamples();

        // load data files
        loadRTcmixFiles();
        
		targetAmp = 1.0;
		
        MASTERAMP = 1.0;
        MASTERTEMPO = 0.125;
        AUTORUN = 0;
        DOCLEAR = true;
        cleartime = ofGetElapsedTimef();
        
        
		ofAddListener(ofEvents().audioRequested, this, &CloudsSound::audioRequested);

		eventsRegistered = true;
	}
}

//--------------------------------------------------------------------
void CloudsSound::exit(ofEventArgs & args){
	if(eventsRegistered){
		
		eventsRegistered = false;
		
		ofRemoveListener(storyEngine->getEvents().actCreated, this, &CloudsSound::actCreated);
		ofRemoveListener(ofEvents().audioRequested, this, &CloudsSound::audioRequested);
		
		if(currentAct != NULL){
			currentAct->unregisterEvents( this );
		}
//		ofRemoveListener(storyEngine->getEvents().clipBegan, this, &CloudsSound::clipBegan);
//		ofRemoveListener(storyEngine->getEvents().clipEnded, this, &CloudsSound::clipEnded);
//		ofRemoveListener(storyEngine->getEvents().topicChanged, this, &CloudsSound::topicChanged);
		
		ofRemoveListener(ofEvents().exit, this, &CloudsSound::exit);		
		ofUnregisterMouseEvents(this);
		ofUnregisterKeyEvents(this);		
	}
}

void CloudsSound::setMasterAmp(float amp){
	targetAmp = amp;
}

//--------------------------------------------------------------------
void CloudsSound::update(){
	MASTERAMP += (targetAmp - MASTERAMP) * .05;
}

//--------------------------------------------------------------------
void CloudsSound::drawDebug(){
	
	ofPushStyle();
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
	
	ofPopStyle();
}

//--------------------------------------------------------------------
void CloudsSound::actCreated(CloudsActEventArgs& args){
	
	currentAct = args.act;
	currentAct->registerEvents(this);
}

//--------------------------------------------------------------------
void CloudsSound::actBegan(CloudsActEventArgs& args){
	//Happens at the very beginning of a sequence
}

void CloudsSound::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	
}

void CloudsSound::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	
}

//--------------------------------------------------------------------
void CloudsSound::clipBegan(CloudsClipEventArgs& args){
	///TEMPORARY FOR SCRATCH TRACKS
	//return;
	/*
	cout << "SOUND: current topic >> " << args.currentTopic << endl;
	cout << "SOUND: keywords >> ";
    for(int i=0;i<args.chosenClip.getKeywords().size();i++)
    {
        cout << i << ": " << args.chosenClip.getKeywords()[i] << " ";
    }
    for(int i=0;i<args.chosenClip.getSpecialKeywords().size();i++)
    {
        cout << i << ": " << args.chosenClip.getSpecialKeywords()[i] << " ";
    }
    for(int i=0;i<args.currentDichotomiesBalance.size();i++)
    {
        cout << i << ": " << args.currentDichotomiesBalance[i].left << " " << args.currentDichotomiesBalance[i].right << " " << args.currentDichotomiesBalance[i].balance << endl;
    }
    cout << endl;
	cout << "SOUND:center >> " << args.chosenClip.cluster.Centre << endl;
	cout << "SOUND:hexcolor >> " << args.chosenClip.cluster.hexColor << ": " << returnColor(args.chosenClip.cluster.hexColor) << endl;
	cout << "SOUND:duration in seconds >> " << args.chosenClip.getDuration() << endl;
    */
}

//--------------------------------------------------------------------
void CloudsSound::questionAsked(CloudsQuestionEventArgs& args){
	
}

//--------------------------------------------------------------------
void CloudsSound::topicChanged(CloudsTopicEventArgs& args){
	cout << "topic changed to " << args.topic << " for " << args.duration << " seconds" << endl;
    
    float musicdur = args.duration;
    
    int preset = ofRandom(0, presets.size());
    
    morch = presets[preset].instruments;
//    morch.clear();
//    for(int i = 0;i<presets[preset].instruments.size();i++)
//    {
//        morch.push_back(presets[preset].instruments[i]);
//    }
    mharmony = presets[preset].harmony;
    mrhythm = presets[preset].rhythm;
    MASTERTEMPO = presets[preset].tempo;
    
    startMusic(morch, mharmony, mrhythm, musicdur);
    
}

//--------------------------------------------------------------------
void CloudsSound::preRollRequested(CloudsPreRollEventArgs& args){
	
}

//--------------------------------------------------------------------
void CloudsSound::actEnded(CloudsActEventArgs& args){
	args.act->unregisterEvents(this);
}

//--------------------------------------------------------------------
void CloudsSound::keyPressed(ofKeyEventArgs & args){
	
}

//--------------------------------------------------------------------
void CloudsSound::keyReleased(ofKeyEventArgs & args){

    if (args.key == OF_KEY_DOWN)
    {
        MASTERAMP-=0.1;
        if(MASTERAMP<0.) MASTERAMP=0.;
    }
    if (args.key == OF_KEY_UP)
    {
        MASTERAMP+=0.1;
        if(MASTERAMP>2.) MASTERAMP=2.;
    }

}

//--------------------------------------------------------------------
void CloudsSound::mouseDragged(ofMouseEventArgs & args){
	
}

void CloudsSound::mouseMoved(ofMouseEventArgs & args){
	
}
void CloudsSound::mousePressed(ofMouseEventArgs & args){
}

void CloudsSound::audioRequested(ofAudioEventArgs& args){
    
    pullTraverse(NULL, s_audio_outbuf); // grab audio from RTcmix

    // fill up the audio buffer
    for (int i = 0; i < args.bufferSize * args.nChannels; i++)
    {
        args.buffer[i] = MASTERAMP*(float)s_audio_outbuf[i]/MAXAMP; // transfer to the float *output buf
    }
    
    // fire first audio-generating info upon confirming audio is up and running
    if (first_vec == 1)
    {
        first_vec = 0;
        // launch initial effects chain (reverb)
        //REVERB(60*60*12); // gimme twelve hours of reverb, baby.
		
        // play pretty intro melody
        for(int i = 0;i<12;i++)
        {
            WAVETABLE(i*0.1, 0.1, 0.05, mtof(48.+(i*5)+7), ofRandom(1.0), "wf_organ", "amp_sharpadsr");
            STRUM(i*0.1, 1.0, 0.1, mtof(48.+(i*5)), 1.0, 1.0, ofRandom(1.0));
            STEREO(i*0.1, 0., 0.2, 0.05, i/11.0, "BD");
        }
        // launch initial effects chain (reverb)
        REVERB(5.0); // gimme some reverb
    }
    
    // not using right now
    if (check_bang() == 1) {
        allownote = 1;
        if(DEBUG) cout << "BANG: " << ofGetElapsedTimef() << endl;
    }
}

void CloudsSound::mouseReleased(ofMouseEventArgs & args){
	
}


// UTILITY STUFF (non-sound):

// utility routine to find where we are clicking
int CloudsSound::returnQuadrant(int x, int y)
{
    int foo = 3;
    if(x < ofGetWidth()/2) foo--;
    if(y < ofGetHeight()/2) foo-=2;
    
    return(foo);
    
}

// translate color strings into numbers for score system
int CloudsSound::returnColor(string c)
{
    //cout << "color: " << c << endl;
    int outc = -1;
    if(c.compare("3753a9")==0) outc = 0;
    else if(c.compare("377ea9")==0) outc = 1;
    else if(c.compare("37a953")==0) outc = 2;
    else if(c.compare("37a97e")==0) outc = 3;
    else if(c.compare("37a9a9")==0) outc = 4;
    else if(c.compare("4537a9")==0) outc = 5;
    else if(c.compare("45a937")==0) outc = 6;
    else if(c.compare("7037a9")==0) outc = 7;
    else if(c.compare("70a937")==0) outc = 8;
    else if(c.compare("9b37a9")==0) outc = 9;
    else if(c.compare("9ba937")==0) outc = 10;
    else if(c.compare("a93737")==0) outc = 11;
    else if(c.compare("a93762")==0) outc = 12;
    else if(c.compare("a9378d")==0) outc = 13;
    else if(c.compare("a96237")==0) outc = 14;
    else if(c.compare("a98d37")==0) outc = 15;
    return(outc);
}


void CloudsSound::loadRTcmixFiles()
{
    cout << "==================" << endl;
    cout << "LOADING SOUND DATA" << endl;
    cout << "==================" << endl;

    registerOrchs();
    cout << "ORCHESTRA:" << endl;
    for(int i = 0;i<orchestra.size();i++)
    {
        cout << orchestra[i] << endl;
    }
    
    loadrhythms("rhythms.txt", rhythms);
    cout << "RHYTHMS:" << endl;
    for(int i = 0;i<rhythms.size();i++)
    {
        for(int j = 0;j<rhythms[i].beats.size();j++)
        {
            cout << rhythms[i].beats[j] << " ";
        }
        cout << endl;
    }
    
    loadpitches("pitches.txt", pitches);
    cout << "PITCHES:" << endl;
    for(int i = 0;i<pitches.size();i++)
    {
        for(int j = 0;j<pitches[i].notes.size();j++)
        {
            cout << pitches[i].notes[j] << " ";
        }
        cout << endl;
    }
    
    loadpresets("presets.txt", presets);
    cout << "PRESETS:" << endl;
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

    cout << "====" << endl;
    cout << "DONE" << endl;
    cout << "====" << endl;

}

void CloudsSound::loadRTcmixSamples()
{
    cout << "==============" << endl;
    cout << "LOADING SOUNDS" << endl;
    cout << "==============" << endl;
    string spath = getDataPath() + "sound/samps/";
    LOADSOUND(spath + "BD.aif", "BD");
    LOADSOUND(spath + "SD.aif", "SD");
    LOADSOUND(spath + "CH.aif", "CH");
    LOADSOUND(spath + "OH.aif", "OH");

    LOADSOUND(spath + "BD2.aif", "BD2");
    LOADSOUND(spath + "VD1.aif", "VD1");
    LOADSOUND(spath + "VD2.aif", "VD2");
    LOADSOUND(spath + "VD3.aif", "VD3");
    LOADSOUND(spath + "VD4.aif", "VD4");
    tl1 = LOADSOUND(spath + "testloop1.aif", "testloop1");
    tl2 = LOADSOUND(spath + "testloop2.aif", "testloop2");
    tl3 = LOADSOUND(spath + "testloop3.aif", "testloop3");
    
    //some path, may be absolute or relative to bin/data
    spath = spath+"loops/";
    ofDirectory dir(spath);
    //only show sound files
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
        //cout << foo.handle << " " << foo.bank << " " << foo.numbeats << " " << foo.length << endl;
        looperSamples.push_back(foo);
    }
    
    cout << "====" << endl;
    cout << "DONE" << endl;
    cout << "====" << endl;
    


}


void CloudsSound::registerOrchs()
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


void CloudsSound::startMusic(vector<string> mo, int mh, int mr, float musicdur)
{
    
    float t, beatoffset;
    float i, j;
	
    // some timing shit...
    t = ofGetElapsedTimef();
    float tempo = MASTERTEMPO;
    int bcount = 0;
    beatoffset = tempo-fmod(t,tempo); // use for accurate ahead-of-time quantization for rhythmic triggering
	
    flush_sched(); // kill previous music
    
    vector<string> ilist = mo; // list of instruments
    
    
    cout << "===============" << endl;
    cout << "MAKING MUSIC!!!" << endl;
    cout << musicdur << " " << "seconds" << endl;
    cout << "harmony: " << mh << ", rhythm: " << mr << endl;
    cout << "orchestration:" << endl;
    for(int i = 0;i<ilist.size();i++)
    {
        cout << ilist[i] << endl;
    }
    cout << "===============" << endl;
    
    // REVERB
    REVERB(musicdur+7.0); // gimme some reverb
    
    //
    // =========================
    // BEGIN ORCHESTRATION BLOCK
    // =========================
    //
    
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

void CloudsSound::stopMusic()
{
    flush_sched();
}

