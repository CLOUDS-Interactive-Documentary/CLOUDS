
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

//--------------------------------------------------------------------
void CloudsSound::update(){
	
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

    float musicdur = args.chosenClip.getDuration();

    mcolor = ofRandom(0, colors.size());
    mharmony = ofRandom(0, pitches.size());
    mrhythm = ofRandom(0, rhythms.size());
    
    cout << "PLAYING MUSIC: " << mcolor << " " << mharmony << " " << mrhythm << " " << musicdur << endl;
    startMusic(mcolor, mharmony, mrhythm, musicdur);


    
}

//--------------------------------------------------------------------
void CloudsSound::questionAsked(CloudsQuestionEventArgs& args){
	
}

//--------------------------------------------------------------------
void CloudsSound::topicChanged(string& topic){
	
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
        args.buffer[i] = (float)s_audio_outbuf[i]/MAXAMP; // transfer to the float *output buf
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
            WAVETABLE(i*0.1, 0.1, 0.05, mtof(48.+(i*5)+7), ofRandom(1.0), "wf_organ", "amp_sharpadsr");
            STRUM(i*0.1, 1.0, 0.1, mtof(48.+(i*5)), 1.0, 1.0, ofRandom(1.0));
            //STEREO(i*0.1, 0., 0.2, 0.05, i/11.0, "BD");
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
    
    loadpresets("presets.txt", presets);
    cout << "presets:" << endl;
    for(int i = 0;i<presets.size();i++)
    {
        cout << presets[i].color << " ";
        cout << presets[i].harmony << " ";
        cout << presets[i].rhythm << " ";
        cout << presets[i].tempo << " ";
        cout << endl;
    }
    
    
}

void CloudsSound::loadRTcmixSamples()
{
    LOADSOUND("samps/BD.aif", "BD");
    LOADSOUND("samps/SD.aif", "SD");
    LOADSOUND("samps/CH.aif", "CH");
    LOADSOUND("samps/OH.aif", "OH");
}

void CloudsSound::startMusic(int mc, int mh, int mr, float musicdur)
{
    
    float t, beatoffset;
    float i, j;
	
    // some timing shit...
    t = ofGetElapsedTimef();
    float tempo = MASTERTEMPO;
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

