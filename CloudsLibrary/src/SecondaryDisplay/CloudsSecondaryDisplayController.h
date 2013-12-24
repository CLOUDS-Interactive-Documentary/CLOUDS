//
//  CloudsSecondaryDisplayController.h
//  CloudsSecondaryDisplay
//
//  Created by James George on 12/23/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

#include "CloudsVisualSystemClusterMap.h"
#include "CloudsRun.h"
#include "CloudsStoryEngine.h"
#include "CloudsAct.h"
#include "CloudsFCPParser.h"
#include "ofxOsc.h"
#include "CloudsSpeaker.h"

class CloudsSecondaryDisplayController {
  public:
	CloudsSecondaryDisplayController();
	
	void setup();
	void update();

	void drawOverlay();


  protected:

	CloudsVisualSystemClusterMap clusterMap;
	CloudsRun run;
	CloudsFCPParser parser;
	CloudsStoryEngine storyEngine;
	CloudsVisualSystemManager visualSystems;

	bool hasSpeaker;
	CloudsSpeaker currentSpeaker;
	CloudsClip currentClip;
	CloudsProjectExample currentExample;

	ofxFTGLSimpleLayout exampleType;
	

	ofVideoPlayer archivePlayer;

	ofxOscReceiver receiver;

};