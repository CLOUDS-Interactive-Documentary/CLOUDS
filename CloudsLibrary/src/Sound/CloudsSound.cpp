
#include "CloudsSound.h"

CloudsSound::CloudsSound(){
	
	currentAct = NULL;
	storyEngine = NULL;
	presetFlags = NULL;
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
		ofAddListener(ofEvents().update, this, &CloudsSound::update);
		ofAddListener(ofEvents().exit, this, &CloudsSound::exit);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
	
		frontPlayer = ofPtr<ofSoundPlayer>( new ofSoundPlayer() );
		backPlayer  = ofPtr<ofSoundPlayer>( new ofSoundPlayer() );

#ifdef RTCMIX
        // RTcmix audio stuff
        sr = 44100;
        nbufs = 2; // you can use more for more processing but latency will suffer
        nchans = 2; // stereo
        framesize = 512; // sigvs
        s_audio_outbuf = (short*)malloc(nchans*framesize*sizeof(short)); // audio buffer (interleaved)
        s_audio_compbuf = (short*)malloc(nchans*framesize*sizeof(short)); // audio buffer (interleaved)
		
        // initialize RTcmix
        rtcmixmain();
        maxmsp_rtsetparams(sr, nchans, framesize, NULL, NULL);
        
        // launch initial setup score
        RTcmixParseScoreFile("cmixinit.sco");


        // load samples
        loadRTcmixSamples();

        // load data files
        loadRTcmixFiles();
        
        // precompute music data
        for(int i = 0;i<pitches.size();i++)
        {
            precomputemarkov(pitches[i]);
        }
#endif
		
		first_vec = true; // we haven't had audio yet
        buzzreps = 0; // no buzzing yet

        whichdream = 0;
		instGain = 7.5;
        in_tunnel = false;
		
        MASTERTEMPO = 120;
        AUTORUN = 0;
        DOCLEAR = true;
        DOCMIXPRINT = false;
        ACTBUS = 0;
        ab.clear();
        abn.clear();
        for(int i = 0;i<PF_NUMBUSES;i++)
        {
			ab.push_back("ACTBUS"+ofToString(i));
			abn.push_back(PF_MAINACT_BUS_START+i);
        }
        
		ofAddListener(GetCloudsAudioEvents()->musicAudioRequested, this, &CloudsSound::audioRequested);

		GetCloudsAudioEvents()->doflush = false;
        GetCloudsAudioEvents()->respawn = false;
        GetCloudsAudioEvents()->setupflush = false;
        GetCloudsAudioEvents()->dodelay = false;
		
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
		
		ofRemoveListener(ofEvents().update, this, &CloudsSound::update);
		ofRemoveListener(ofEvents().exit, this, &CloudsSound::exit);
		ofUnregisterMouseEvents(this);
		ofUnregisterKeyEvents(this);		
	}
}

//--------------------------------------------------------------------
void CloudsSound::update(ofEventArgs & args){
    update();
}

void CloudsSound::update(){
    if(GetCloudsAudioEvents()->doflush)
    {
        if(LUKEDEBUG) cout << "FLUSHING SCHEDULER." << endl;
        else cout << "SOUND: MUSIC STOPPED." << endl;
#ifdef RTCMIX
        flush_sched();
#endif
        sleep(1);
        // zero output buffer (AHA!)
        bzero((void *) s_audio_outbuf, nchans*framesize*sizeof(short));
        GetCloudsAudioEvents()->setupflush = false;
        GetCloudsAudioEvents()->doflush = false;
    }
    if(GetCloudsAudioEvents()->respawn)
    {
        if(LUKEDEBUG) cout << "REDOING MUSIC." << endl;
        else cout << "SOUND: MUSIC RESPAWNED." << endl;
#ifdef RTCMIX
        flush_sched();
#endif
        sleep(1);
        // zero output buffer (AHA!)
        bzero((void *) s_audio_outbuf, nchans*framesize*sizeof(short));
        GetCloudsAudioEvents()->respawn = false;
        playCurrentCues();
    }
}

//--------------------------------------------------------------------
void CloudsSound::drawDebug(){
	
}

//--------------------------------------------------------------------
void CloudsSound::actCreated(CloudsActEventArgs& args){
	if(currentAct != NULL){
		currentAct->unregisterEvents(this);
	}
	currentAct = args.act;
	currentAct->registerEvents(this);
}

//--------------------------------------------------------------------
void CloudsSound::actBegan(CloudsActEventArgs& args){
	//Happens at the very beginning of a sequence
    ofxTimeline& actTimeLine = currentAct->getTimeline();
    
    actTimeLine.addPage("sound",true);
    presetFlags = actTimeLine.addFlags("Presets");

	//    vector<CloudsSoundCue>& thecues = args.act->getSoundCues(); // copy the cues
	
    currentCuesTotalDuration = args.act->getTimeline().getDurationInSeconds(); // how long
	currentCues = args.act->getSoundCues();
	cueFlagsAdded = false;
	
	playCurrentCues();
	
    actTimeLine.setCurrentPage(0);
    if(LUKEDEBUG) cout << "====================" << endl;
    if(LUKEDEBUG) cout << "DONE MAKING MUSIC!!!" << endl;
    if(LUKEDEBUG) cout << "====================" << endl;

}

void CloudsSound::playCurrentCues(){
	
    vector<int> valid_presets; // make a vector of presets that match the dichotomy setting
    vector<int> cuedichos; // place to stash cue dichotomies
	
    int rigged = 0; // set to '1' for slowwaves all the time
    bool isHighEnergy = false;
    int numcues = currentCues.size(); // how many cues in this act?
    
    //
    // GOGOGO
    //
    
    if(LUKEDEBUG) cout << "===============" << endl;
    if(LUKEDEBUG) cout << "MAKING MUSIC!!!" << endl;
    if(LUKEDEBUG) cout << "===============" << endl;
	
    float pad = 5.0; // padding for FX chain
    currentCuesTotalDuration += pad; // pad the total
    
    if(LUKEDEBUG) cout << "TOTAL DURATION: " << currentCuesTotalDuration << endl;
    else cout << "SOUND: MUSIC STARTED." << endl;
#ifdef RTCMIX
    // launch music FX chain
    startMusicFX(0, currentCuesTotalDuration);
#endif
	
    // iterate through clips
    if(rigged) // fallback
    {
#ifdef RTCMIX
        startMusic(0, "slowwaves", "markov", "NULL", 0, 0, currentCuesTotalDuration, 120, 0.5, 0.5, 0, "e_FADEINOUTFASTEST");
#endif
    }
    else
    {
        for(int i = 0;i<numcues;i++)
        {
            // TEST 1: CHECK FOR RIGGED PRESET NAME -- THESE CUES CAN BE "DISABLED"
            if(currentCues[i].riggedPresetName!="")
            {
                for(int j = 0; j<presets.size();j++)
                {
                    if(presets[j].name==currentCues[i].riggedPresetName) // match
                    {
                        valid_presets.push_back(j);
                    }
                }
            }
            // TEST 2: CHECK FOR OPENING QUESTION MATCH
            if(currentCues[i].soundQuestionKey!="")
            {
                for(int j = 0; j<presets.size();j++)
                {
                    if(presets[j].start_question == currentCues[i].soundQuestionKey && presets[j].disabled==0) // match
                    {
                        
                        valid_presets.push_back(j);
                    }
                }
            }
            // USE DICHOTOMIES
            else
            {
                // add up dichos for cue
                cuedichos.clear();
                for(int j = 0; j < currentCues[i].dichotomies.size(); j++)
                {
                    cuedichos.push_back(currentCues[i].dichotomies[j].balance);
                }
				
                //Populate valid presets
                valid_presets.clear();
                for(int j = 0;j<presets.size();j++)
                {
                    int pscore = 0;
                    for(int k=0;k<8;k++)
                    {
                        if(cuedichos[k] <= presets[j].dichomax[k]&&cuedichos[k]>=presets[j].dichomin[k])
                        {
                            pscore++;
                        }
                    }
                    //if all 8 dichos matched
                    if(pscore==8&&presets[j].highEnergy==isHighEnergy&&presets[j].disabled==0){
                        //if(presets[j].slotnumber<250) { // temporary
                        valid_presets.push_back(j);
                        //}
                    }
                }
				
                isHighEnergy = !isHighEnergy; // flip energy state at each dicho check
            }
            
            // emergency check
            if(valid_presets.size()==0)
            {
                valid_presets.push_back(0);
            }
            
            // MAKE THE MUSIC
            int GOPRESET = valid_presets[ ofRandom(valid_presets.size()) ];
			if(!cueFlagsAdded && presetFlags != NULL){
				presetFlags->addFlagAtTime(presets[GOPRESET].name + " : "+ ofToString(presets[GOPRESET].slotnumber), currentCues[i].startTime *1000 );

			}
            
			if(LUKEDEBUG) cout << "   preset: " << presets[GOPRESET].slotnumber << endl;
            if(LUKEDEBUG) cout << "FUCKSOUND: schedule: " << i << endl;

			schedulePreset(presets[GOPRESET], currentCues[i].startTime, currentCues[i].duration, currentCues[i].mixLevel, i+1);
			
        }
    }
	cueFlagsAdded = true;
}

void CloudsSound::enterTunnel()
{
    string soundfile = "CLOUDS_introTunnel_light.wav"; // change to something in trax
    float volume = 1.0; // how load does this sound play?
#ifdef RTCMIX
    if(LUKEDEBUG) cout << "sound: enterTunnel()" << endl;

    PATCHFX("STEREO", "in 0", "out 0-1"); // bypass reverb
    STREAMSOUND_DYNAMIC(0, soundfile, 1.0);
    SCHEDULEBANG(477.); // length of sound
#else
	//TODO: enter tunnel
	frontPlayer->loadSound(GetCloudsDataPath() + "sound/renders/tunnel.mp3");
    frontPlayer->play();
#endif
    in_tunnel = true;
    float ftime = 0.1;
    ofNotifyEvent(GetCloudsAudioEvents()->fadeAudioUp, ftime);
}

void CloudsSound::exitTunnel()
{
    float fd = 5.0; // change to adjust fade time

    if(LUKEDEBUG) cout << "sound: exitTunnel()" << endl;

    stopMusic();
    // PFIELD_SCHED(0., fd, PF_TUNNEL_BUS, "ramp_10");
    in_tunnel = false;
}

void CloudsSound::enterClusterMap()
{
    string soundfile;
    if(whichdream==0) soundfile = "cloudsdream_mix1.aif";
    if(whichdream==1) soundfile = "cloudsdream_mix2.aif";
    if(whichdream==2) soundfile = "cloudsdream_mix3.aif";
    string ampsym = "clusteramp"; // needs to be unique per RT instance
    float volume = 1.0; // how load does this sound play?
    
    if(LUKEDEBUG) cout << "sound: enterClusterMap()" << endl;
#ifdef RTCMIX
    PATCHFX("STEREO", "in 0", "out 0-1"); // bypass reverb
    STREAMSOUND_DYNAMIC(0, soundfile, 1.0);
#else
	frontPlayer->loadSound(GetCloudsDataPath() + "sound/renders/cloudsdream_mix1.mp3");
	frontPlayer->play();
#endif
	
    float ftime = 0.1;
    ofNotifyEvent(GetCloudsAudioEvents()->fadeAudioUp, ftime);
}

void CloudsSound::exitClusterMap()
{
    if(LUKEDEBUG) cout << "sound: exitClusterMap()" << endl;
    
    stopMusic();
    // PFIELD_SCHED(0., fd, PF_CLUSTERMAP_BUS, "ramp_10");
    whichdream = (whichdream+1)%3;
}


void CloudsSound::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	
}

void CloudsSound::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	
}

//--------------------------------------------------------------------
void CloudsSound::clipBegan(CloudsClipEventArgs& args){

}

//--------------------------------------------------------------------
void CloudsSound::questionProposed(CloudsQuestionEventArgs& args){

}

//--------------------------------------------------------------------
void CloudsSound::topicChanged(CloudsTopicEventArgs& args){
}

//--------------------------------------------------------------------
void CloudsSound::preRollRequested(CloudsPreRollEventArgs& args){
	
}
//--------------------------------------------------------------------
void CloudsSound::questionSelected(float fadeTime){

}
//--------------------------------------------------------------------
void CloudsSound::questionSelected(CloudsQuestionEventArgs& args){
    
}

//--------------------------------------------------------------------
void CloudsSound::actEnded(CloudsActEventArgs& args){
	if(currentAct != args.act){
		ofLogError("CloudsSound::actEnded") << "Inconsistent acts in Clouds Sound";
	}
	args.act->unregisterEvents(this);
	currentAct = NULL;

	//JG added in stop music here in case we jumped ahead.
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

void CloudsSound::doPrinting() {
    DOCMIXPRINT = !DOCMIXPRINT;
    if(DOCMIXPRINT) RTcmixParseScoreFile("print_on.sco");
    else RTcmixParseScoreFile("print_off.sco");
}

// =========================
// =========================
// MAIN AUDIO CALLBACK BLOCK
// =========================
// =========================
void CloudsSound::audioRequested(ofAudioEventArgs& args){

#ifdef RTCMIX
    int cdif = 0;
    int csum = 0;

        pullTraverse(NULL, s_audio_outbuf); // grab audio from RTcmix
        // fill up the audio buffer
        for (int i = 0; i < args.bufferSize * args.nChannels; i++)
        {
            args.buffer[i] = (float)s_audio_outbuf[i]/MAXAMP; // transfer to the float *output buf
            
            csum+=abs(s_audio_outbuf[i]);
            cdif+=(s_audio_outbuf[i]-s_audio_compbuf[i]);
            s_audio_compbuf[i] = s_audio_outbuf[i];
        }
    
    if(csum>0 && cdif==0) {
        cout << "BUZZZZZZZZZZZZZZ!!!" << endl;
        buzzreps++;
    }
    else buzzreps = 0;
    
    if(buzzreps>20) {
        GetCloudsAudioEvents()->respawn = true;
        buzzreps = 0;
    }
        
        // not using right now
        if (check_bang() == 1) {
            if(LUKEDEBUG) cout << "BANG: " << ofGetElapsedTimef() << endl;
            if(in_tunnel) enterTunnel();
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
#endif
	
}

void CloudsSound::mouseReleased(ofMouseEventArgs & args){
	
}
