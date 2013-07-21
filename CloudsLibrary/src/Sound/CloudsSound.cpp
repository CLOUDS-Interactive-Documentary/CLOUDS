
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
//	cout << "SOUND: current topic >> " << storyEngine->getCurrentTopic() << endl;
//	cout << "SOUND: keywords >> ";
//    for(int i=0;i<args.chosenClip.keywords.size();i++)
//    {
//        cout << i << ": " << args.chosenClip.keywords[i] << " ";
//    }
//    cout << endl;
//	cout << "SOUND:center >> " << args.chosenClip.cluster.Centre << endl;
//	cout << "SOUND:hexcolor >> " << args.chosenClip.cluster.hexColor << endl;
//	cout << "SOUND:duration in seconds >> " << args.chosenClip.getDuration() << endl;

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
	
    float t, beatoffset;
    
    // some timing shit...
    t = ofGetElapsedTimef();
    float tempo = 0.1;
    float del1, del2;
    beatoffset = tempo-fmod(t,tempo); // use for accurate ahead-of-time quantization for rhythmic triggering
    /*
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
     */
}

void CloudsSound::mouseMoved(ofMouseEventArgs & args){
	
}
void CloudsSound::mousePressed(ofMouseEventArgs & args){
	float t, beatoffset;
	
    // some timing shit...
    t = ofGetElapsedTimef();
    float tempo = 0.125;
    beatoffset = tempo-fmod(t,tempo); // use for accurate ahead-of-time quantization for rhythmic triggering
	
    int bpattern[] = {1,0,1,1,1,0,1,0,1,1,1,0,1,0,0,1,1,1,0,1,0,1,0,0,1,0,0,0,0,0,1,1};
    /*
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
     */

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
