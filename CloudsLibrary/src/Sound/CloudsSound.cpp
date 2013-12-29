
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
	
        // TODO: use CloudsMixer parameters
        // RTcmix audio stuff
        sr = 44100;
        nbufs = 2; // you can use more for more processing but latency will suffer
        nchans = 2; // stereo
        framesize = 512; // sigvs
        s_audio_outbuf = (short*)malloc(nchans*framesize*sizeof(short)); // audio buffer (interleaved)
        
        // initialize RTcmix
        rtcmixmain();
        maxmsp_rtsetparams(sr, nchans, framesize, NULL, NULL);
        
        // launch initial setup score
        RTcmixParseScoreFile("cmixinit.sco");
        first_vec = true; // we haven't had audio yet
        rtc_playing = false; // engine is idle
        
        // load samples
        loadRTcmixSamples();

        // load data files
        loadRTcmixFiles();
        
        // precompute music data
        for(int i = 0;i<pitches.size();i++)
        {
            precomputemarkov(pitches[i]);
        }
        
		instGain = 5.0;
		
        MASTERTEMPO = 120;
        AUTORUN = 0;
        DOCLEAR = true;
        
		ofAddListener(GetCloudsAudioEvents()->musicAudioRequested, this, &CloudsSound::audioRequested);

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

//--------------------------------------------------------------------
void CloudsSound::update(){
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
    float starttime = 0;
    float totalduration = args.act->getTimeline().getDurationInSeconds();
    int mharmony, mrhythm, mtempo;
    bool allowchange = true;
    bool isHighEnergy = false;
	
    // launch music FX chain
    startMusicFX(0, totalduration);
    
    // iterate through clips
    int numclips = args.act->getAllClips().size();
    
    if(LUKEDEBUG) cout << "===============" << endl;
    if(LUKEDEBUG) cout << "MAKING MUSIC!!!" << endl;
    if(LUKEDEBUG) cout << "===============" << endl;
    
    // loop through clips
    // first clip: check on soundQuestionKey
    //  if found in preset, use that preset
    //  if no, check dichotomies (only using non-disabled clips)
    
    // main loop
    // stay where you are unless energy shift
    // if energy shift, check dichotomies (use only non-disabled clips)
    vector<int> valid_presets; // make a vector of presets that match the dichotomy setting
    int thepreset; // which preset did we choose?
    
    if(!rigged) {
        // STUPID MAPPING TEST
        for(int i = 0;i<numclips;i++)
        {
            CloudsClip &theclip = args.act->getAllClips()[i];
            starttime = args.act->getClipStartTime(theclip);
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
            if(LUKEDEBUG) cout << i << ": " << theclip.getLinkName() << ": " << clipdur << ":" << endl;
            if(LUKEDEBUG) cout << "   starting at: " << starttime << endl;
            vector<CloudsDichotomy> foo = args.act->getDichotomiesForClip(theclip);
            vector<int> dichos;
            for(int j = 0;j<foo.size();j++)
            {
                dichos.push_back(foo[j].balance);
            }
            
            if(LUKEDEBUG) cout << "	current energy is " << (isHighEnergy ? "HIGH" : "LOW") << endl;
            
            if(args.act->isClipEnergyShift(theclip)) allowchange = true;
            if(i==1) allowchange=true;
            
            if(allowchange)
            {
                valid_presets.clear();
                //Populate valid presets
                for(int j = 0;j<presets.size();j++)
                {
                    // CHECK FOR RIGGED
                    if(i==0 && presets[j].start_question==args.soundQuestionKey) {
                        //if(presets[j].slotnumber < 250) // temporary
                        //{
                            if(LUKEDEBUG) cout << "   rigged preset: " << presets[j].slotnumber << endl;
                            valid_presets.clear();
                            valid_presets.push_back(j);
                            break;
                        //}
                    }
                    
                    //if the energy state is the same and it's not the first clip, don't allow this preset
                    if(presets[j].disabled || (presets[j].highEnergy == isHighEnergy && i != 0) ){
                        //go to next presets
                        continue;
                    }
                    
                    int pscore = 0;
                    for(int k=0;k<8;k++)
                    {
                        if(dichos[k]<=presets[j].dichomax[k]&&dichos[k]>=presets[j].dichomin[k])
                        {
                            pscore++;
                        }
                    }
                    
                    //if all 8 dichos matched
                    if(pscore==8){
                        valid_presets.push_back(j);	
                    }
                }
            
                if(valid_presets.size()==0)
                {
                    valid_presets.push_back(0);
                }
            }
            
            // MAKE THE MUSIC
            if(valid_presets.size() > 0){
                if(allowchange){
                    thepreset = valid_presets[ ofRandom(valid_presets.size()) ];
                    isHighEnergy = presets[ thepreset ].highEnergy;
                }
            
                mharmony = presets[thepreset].harmony;
                mrhythm = presets[thepreset].rhythm;
                mtempo = presets[thepreset].tempo;
                if(LUKEDEBUG) cout << "   preset: " << presets[thepreset].slotnumber;
                for(int j = 0;j<presets[thepreset].instruments.size();j++)
                {
                    startMusic(starttime, presets[thepreset].instruments[j], presets[thepreset].arg_a[j], presets[thepreset].arg_b[j], mharmony, mrhythm, clipdur, mtempo, presets[thepreset].m_amp[j], presets[thepreset].m_rev[j], j);
                }
            }

            allowchange = false;
        }
    }
    if(rigged)
    {
        //flush_sched();
        startMusic(0, "slowwaves", "markov", "NULL", 0, 0, totalduration, 120, 0.5, 0.5, 0);
    }
    
    if(LUKEDEBUG) cout << "====================" << endl;
    if(LUKEDEBUG) cout << "DONE MAKING MUSIC!!!" << endl;
    if(LUKEDEBUG) cout << "====================" << endl;

    
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
    stopMusic();
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
        args.buffer[i] = (float)s_audio_outbuf[i]/MAXAMP; // transfer to the float *output buf
    }

    
    // not using right now
    if (check_bang() == 1) {
        if(LUKEDEBUG) cout << "BANG: " << ofGetElapsedTimef() << endl;
    }

    if(LUKEDEBUG)
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
