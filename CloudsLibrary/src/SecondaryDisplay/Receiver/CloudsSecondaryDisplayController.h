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
#include "ofxSvg.h"
#include "CloudsSpeaker.h"
#include "CloudsSDLayer.h"

typedef enum {
	CLOUDS_SD_FULL = 0,
	CLOUDS_SD_QUESTION,
	CLOUDS_SD_LOWER_THIRD,
	CLOUDS_SD_PROJECT_EXAMPLE,
	CLOUDS_SD_MAP
} CloudsSDLayerSet;


class CloudsSecondaryDisplayController {
  public:
	CloudsSecondaryDisplayController();
	
	void setup();
    void buildSDLayerSets();
	void update();
	void drawOverlay();
    
//    void buildSDLayerSets();
    void updateSDLayers();
    
    void animateOn(CloudsSDLayerSet layer = CLOUDS_SD_FULL);
	void animateOff();
    
    map<CloudsSDLayerSet, vector<CloudsSDLayer*> > sdLayerSets;
	vector<CloudsSDLayer*> allSDLayers;
    
    void saveGuiSettings();
	void toggleGuis();
	
	ofxUISuperCanvas *SDGui;
    //	CloudsSDHomeButton home;
    
    void draw();
    void drawSDLayer(CloudsSDLayerSet sdLayer);

  protected:
	CloudsVisualSystemClusterMap clusterMap;
	CloudsRun run;
	CloudsFCPParser parser;
	CloudsStoryEngine storyEngine;
	CloudsVisualSystemManager visualSystems;

	bool playingMovie;
	bool hasSpeaker;
	CloudsSpeaker currentSpeaker;
	CloudsClip currentClip;
	CloudsProjectExample currentExample;

	ofxFTGLSimpleLayout exampleType;

	ofVideoPlayer archivePlayer;

	ofxOscReceiver receiver;

};
