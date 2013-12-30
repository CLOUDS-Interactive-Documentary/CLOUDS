
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
        DOCMIXPRINT = false;
        
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

    
    int rigged = 0; // set to '1' for slowwaves all the time
    float totalduration = args.act->getTimeline().getDurationInSeconds(); // how long
    bool isHighEnergy = false;
	
    vector<CloudsSoundCue>&thecues = args.act->getSoundCues(); // copy the cues
    vector<int> valid_presets; // make a vector of presets that match the dichotomy setting
    vector<int> cuedichos; // place to stash cue dichotomies

    int numcues = thecues.size(); // how many cues in this act?
    
    float pad = 5.0; // padding for FX chain
    
    //
    // GOGOGO
    //
    
    if(LUKEDEBUG) cout << "===============" << endl;
    if(LUKEDEBUG) cout << "MAKING MUSIC!!!" << endl;
    if(LUKEDEBUG) cout << "===============" << endl;

    totalduration+=pad; // pad the total
    
    if(LUKEDEBUG) cout << "TOTAL DURATION: " << totalduration << endl;

    // launch music FX chain
    startMusicFX(0, totalduration);
    
    // iterate through clips
    if(rigged) // fallback
    {
        startMusic(0, "slowwaves", "markov", "NULL", 0, 0, totalduration, 120, 0.5, 0.5, 0, "e_FADEINOUTFASTEST");
    }
    else
    {
        for(int i = 0;i<numcues;i++)
        {
            // TEST 1: CHECK FOR RIGGED PRESET NAME -- THESE CUES CAN BE "DISABLED"
            if(thecues[i].riggedPresetName!="")
            {
                for(int j = 0; j<presets.size();j++)
                {
                    if(presets[j].name==thecues[i].riggedPresetName) // match
                    {
                        valid_presets.push_back(j);
                    }
                }
            }
            // TEST 2: CHECK FOR OPENING QUESTION MATCH
            if(thecues[i].soundQuestionKey!="")
            {
                for(int j = 0; j<presets.size();j++)
                {
                    if(presets[j].start_question==thecues[i].soundQuestionKey&&presets[j].disabled==0) // match
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
                for(int j = 0;j<thecues[i].dichotomies.size();j++)
                {
                    cuedichos.push_back(thecues[i].dichotomies[j].balance);
                }

                //Populate valid presets
                valid_presets.clear();
                for(int j = 0;j<presets.size();j++)
                {
                    int pscore = 0;
                    for(int k=0;k<8;k++)
                    {
                        if(cuedichos[k]<=presets[j].dichomax[k]&&cuedichos[k]>=presets[j].dichomin[k])
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
            
            if(LUKEDEBUG) cout << "   preset: " << presets[GOPRESET].slotnumber << endl;
            schedulePreset(presets[GOPRESET], thecues[i].startTime, thecues[i].duration, thecues[i].mixLevel);

        }
    }
    
    if(LUKEDEBUG) cout << "====================" << endl;
    if(LUKEDEBUG) cout << "DONE MAKING MUSIC!!!" << endl;
    if(LUKEDEBUG) cout << "====================" << endl;

    
}

void CloudsSound::enterTunnel()
{
    string soundfile = "CLOUDS_introTunnel_light.wav"; // change to something in trax
    string ampsym = "tunnelamp"; // needs to be unique per RT instance
    float volume = 1.0; // how load does this sound play?

    if(LUKEDEBUG) cout << "sound: enterTunnel()" << endl;

    PATCHFX("STEREO", "in 0", "out 0-1"); // bypass reverb
    STREAMSOUND_DYNAMIC(0, soundfile, 1.0, ampsym, PF_TUNNEL_BUS);
}

void CloudsSound::exitTunnel()
{
    float fadedur = 5.0; // change to adjust fade time

    if(LUKEDEBUG) cout << "sound: exitTunnel()" << endl;

    PFIELD_SCHED(0., fadedur, PF_TUNNEL_BUS, "ramp_10");
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
    // stopMusic();
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
