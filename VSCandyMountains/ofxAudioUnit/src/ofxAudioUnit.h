#pragma once

#include "ofxAudioUnitBase.h"    // for base Audio Unit class ofxAudioUnit
#include "ofxAudioUnitDSPNode.h" // for base DSP class ofxAudioUnitDSPNode

// ofxAudioUnit subclasses for specific audio units
#include "ofxAudioUnitFilePlayer.h"
#include "ofxAudioUnitInput.h"
#include "ofxAudioUnitMixer.h"
#include "ofxAudioUnitOutput.h"
#include "ofxAudioUnitSampler.h"

#if !TARGET_OS_IPHONE
	#include "ofxAudioUnitNetReceive.h"
	#include "ofxAudioUnitNetSend.h"
	#include "ofxAudioUnitSpeechSynth.h"
	#include "ofxAudioUnitRecorder.h"
#endif

// ofxAudioUnitDSPNode subclasses for specific DSP tasks
#include "ofxAudioUnitTap.h"
#include "ofxAudioUnitFftNode.h"
