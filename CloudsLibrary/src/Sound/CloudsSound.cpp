
#include "CloudsSound.h"

CloudsSound::CloudsSound(){
	storyEngine = NULL;
	eventsRegistered = false;
	maxSpeakerVolume = 1;
}

//--------------------------------------------------------------------
// this setup gets the audio shit together for the entire playback engine
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
        ofSoundStreamSetup(nchans, 0, sr, framesize, nbufs);
        ofSoundStreamStart();
        
        // launch initial setup score
        RTcmixParseScoreFile("cmixinit.sco");
        first_vec = 1; // we haven't had audio yet
        
        // load samples
        loadRTcmixSamples();

        // load data files
        loadRTcmixFiles();
        
		targetAmp = .7; // wonder what this is?
		
        MASTERAMP = 1;
        MASTERTEMPO = 120;
        AUTORUN = 0;
        DOCLEAR = true;
        RTCMIX_PRINT = false;
        
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
	
}

//--------------------------------------------------------------------
void CloudsSound::actCreated(CloudsActEventArgs& args){
	currentAct = args.act;
	currentAct->registerEvents(this);
}

//--------------------------------------------------------------------
void CloudsSound::actBegan(CloudsActEventArgs& args){
	//Happens at the very beginning of a sequence

    
    int rigged = 0; // set to '1' for rigged orchestration (set below)
    float clipdur = 0;
    float totalduration = args.act->getTimeline().getDurationInSeconds();
    int mharmony, mrhythm, mtempo;
    
    // launch music FX chain
    startMusicFX(0, totalduration);
    
    // iterate through clips
    int numclips = args.act->getAllClips().size();
    
    cout << "===============" << endl;
    cout << "MAKING MUSIC!!!" << endl;
    cout << "===============" << endl;
    

    // STUPID MAPPING TEST
    for(int i = 0;i<numclips;i++)
    {
        CloudsClip &theclip = args.act->getAllClips()[i];
        float starttime = args.act->getClipStartTime(theclip);
        if(i==numclips-1) // last clip
        {
            clipdur = theclip.getDuration();
        }
        else
        {
            CloudsClip &nextclip = args.act->getAllClips()[i+1];
            float nextstart = args.act->getClipStartTime(nextclip);
            clipdur = nextstart-starttime;
        }
        cout << i << ": " << theclip.getLinkName() << ": " << clipdur << ":" << endl;
        cout << "   starting at: " << starttime << endl;
        vector<CloudsDichotomy> foo = args.act->getDichotomiesForClip(theclip);
        vector<int> dichos;
        for(int j = 0;j<foo.size();j++)
        {
            cout << "   " << foo[j].left << " versus " << foo[j].right << " is " << foo[j].balance << endl;
            dichos.push_back(foo[j].balance);
        }
        
        vector<int> valid_presets; // make a vector of presets that match the dichotomy setting
        for(int j = 0;j<presets.size();j++)
        {
            int pscore = 0;
            for(int k=0;k<8;k++)
            {
                if(dichos[k]<=presets[j].dichomax[k]&&dichos[k]>=presets[j].dichomin[k])
                {
                    pscore++;
                }
            }
            // if(pscore==8) valid_presets.push_back(j);
        }
		
        if(valid_presets.size()==0)
        {
            valid_presets.push_back(12);
        }
        
		if(valid_presets.size() > 0){
			int thepreset = valid_presets[ofRandom(valid_presets.size())];
		
			mharmony = presets[thepreset].harmony;
			mrhythm = presets[thepreset].rhythm;
			//mtempo = presets[thepreset].tempo;
			mtempo = 120;
			for(int j = 0;j<presets[thepreset].instruments.size();j++)
			{
				startMusic(starttime, presets[thepreset].instruments[j], presets[thepreset].arg_a[j], presets[thepreset].arg_b[j], mharmony, mrhythm, clipdur, mtempo);
			}
		}

   
    }
    
    if(rigged)
    {
        flush_sched();
        startMusic(0, "reichomatic", "NULL", "NULL", 0, 0, totalduration, 120);
    }
    
    cout << "====================" << endl;
    cout << "DONE MAKING MUSIC!!!" << endl;
    cout << "====================" << endl;

    
}

void CloudsSound::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	
}

void CloudsSound::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	
}

//--------------------------------------------------------------------
void CloudsSound::clipBegan(CloudsClipEventArgs& args){

}

//--------------------------------------------------------------------
void CloudsSound::questionAsked(CloudsQuestionEventArgs& args){
	
}

//--------------------------------------------------------------------
void CloudsSound::topicChanged(CloudsTopicEventArgs& args){
}

//--------------------------------------------------------------------
void CloudsSound::preRollRequested(CloudsPreRollEventArgs& args){
	
}

//--------------------------------------------------------------------
void CloudsSound::actEnded(CloudsActEventArgs& args){
	args.act->unregisterEvents(this);
    flush_sched();
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

// =========================
// =========================
// MAIN AUDIO CALLBACK BLOCK
// =========================
// =========================
void CloudsSound::audioRequested(ofAudioEventArgs& args){
    
    pullTraverse(NULL, s_audio_outbuf); // grab audio from RTcmix

    // fill up the audio buffer
    for (int i = 0; i < args.bufferSize * args.nChannels; i++)
    {
        args.buffer[i] = MASTERAMP*(float)s_audio_outbuf[i]/MAXAMP; // transfer to the float *output buf
    }
    
    // fire first audio-generating info upon confirming audio is up and running
	//JG COMMENTED FOR DEMO
    if (first_vec == 1)
    {
        first_vec = 0;
		/*
        // play pretty intro melody
        for(int i = 0;i<12;i++)
        {
            WAVETABLE(i*0.1, 0.1, 0.05, mtof(48.+(i*5)+7), ofRandom(1.0), "wf_organ", "amp_sharpadsr");
            STRUM(i*0.1, 1.0, 0.1, mtof(48.+(i*5)), 1.0, 1.0, ofRandom(1.0));
            STEREO(i*0.1, 0., 0.2, 0.05, i/11.0, "BD");
        }
        // launch initial effects chain (reverb)
        REVERB(0, 5.0); // gimme some reverb
         */
    }
    
    // not using right now
    if (check_bang() == 1) {
        allownote = 1;
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

void CloudsSound::mouseReleased(ofMouseEventArgs & args){
	
}


// UTILITY STUFF (non-sound):

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

