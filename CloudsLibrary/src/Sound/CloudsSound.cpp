
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
	
		theFont.loadFont(ofToDataPath("Times New Roman.ttf"), 32);

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
		
		ofAddListener(ofEvents().audioRequested, this, &CloudsSound::audioRequested);

		// launch initial setup score
		RTcmixInit();
		first_vec=1; // we haven't had audio yet
		
		// stash previous scaled mouse positions
		osx = 0.5;
		osy = 0.5;
		
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
	cout << "SOUND: current topic >> " << args.currentTopic << endl;
	cout << "SOUND: keywords >> ";
    for(int i=0;i<args.chosenClip.getKeywords().size();i++)
    {
        cout << i << ": " << args.chosenClip.getKeywords()[i] << " ";
    }
    cout << endl;
	cout << "SOUND:center >> " << args.chosenClip.cluster.Centre << endl;
	cout << "SOUND:hexcolor >> " << args.chosenClip.cluster.hexColor << ": " << returnColor(args.chosenClip.cluster.hexColor) << endl;
	cout << "SOUND:duration in seconds >> " << args.chosenClip.getDuration() << endl;

	float t, beatoffset;
    float musicdur = args.chosenClip.getDuration();
	
    // some timing shit...
    t = ofGetElapsedTimef();
    float tempo = 0.125;
    int bcount = 0;
    beatoffset = tempo-fmod(t,tempo); // use for accurate ahead-of-time quantization for rhythmic triggering
	
    int bpattern[] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
    int notes[] = {0, 3, 5, 7, 0, 7, 9, 10, 9, 10, 12, 3, 15, 12, 19, 14};
    
    // beats
    for(float i = 0;i<musicdur;i+=tempo)
    {
        if(bpattern[bcount]==1) {
            
            float t_amp = 1.0-fabs((i/musicdur)-0.5)*2.;
            
            MMODALBAR(i, 1., t_amp*0.2, mtof(scale(int(ofRandom(0.,36.)+40.), 2)), ofRandom(0.1,0.9), ofRandom(0.,1.), int(ofRandom(8)));
        }
        bcount = (bcount+1)%(sizeof(bpattern)/sizeof(bpattern[0]));
    }
    for(float i = 0;i<musicdur;i+=tempo*floor(ofRandom(4, 16)))
    {
        int pick = (int)ofRandom(0, sizeof(notes)/sizeof(notes[0]));
            WAVETABLE(i, ofRandom(3., 10.), 0.025, mtof(scale(notes[pick]+55., 2)), ofRandom(0.,1.), "themellowwave", "themellowamp");
        WAVETABLE(i, ofRandom(3., 10.), 0.025, mtof(scale(notes[pick]+55., 2))*0.99, ofRandom(0.,1.), "themellowwave", "themellowamp");
    }
    
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
