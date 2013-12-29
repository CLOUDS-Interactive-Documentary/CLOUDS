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
//#include "CloudsRun.h"
//#include "CloudsStoryEngine.h"
//#include "CloudsAct.h"
#include "CloudsFCPParser.h"
#include "ofxOsc.h"
#include "CloudsSpeaker.h"
#include "CloudsSVGMesh.h"

class CloudsSecondaryDisplayController {
  public:
	CloudsSecondaryDisplayController();
	
	void setup();
	void update();
	void drawOverlay();
    
    void saveGuiSettings();
	void toggleGuis();
    
    void drawNextLayout();
    void drawPrevLayout();
	
	ofxUISuperCanvas *SDGui;
    void draw();

  protected:
	
	CloudsVisualSystemClusterMap clusterMap;
	//CloudsRun run;
	CloudsFCPParser parser;
	//CloudsStoryEngine storyEngine;
	//CloudsVisualSystemManager visualSystems;

	void loadSVGs();
	//TODO: make separate layer sets for Project Example vs Person
	vector<CloudsSVGMesh> testAllLayout;
    
    CloudsSVGMesh bioLayout, projectLayout, systemLayout;
	
	bool playingMovie;
	bool hasSpeaker;
	CloudsSpeaker currentSpeaker;
	CloudsClip currentClip;
	CloudsProjectExample currentExample;

	ofxFTGLSimpleLayout exampleType, h1, h2, h3, h4, h5, p;

	ofVideoPlayer archivePlayer;

	ofxOscReceiver receiver;

	ofFbo displayTarget;
    
    int pFontSize, h3FontSize;
    
    string displayMode, lastQuestion;
};
