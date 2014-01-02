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
#include "CloudsHUDLabel.h"

class CloudsSecondaryDisplayController {
  public:
	CloudsSecondaryDisplayController();
	
	void setup();
	void update();
	void drawOverlay();
    
    void saveGuiSettings();
	void toggleGuis();
	
	ofxUISuperCanvas *SDGui;
    void draw();
    
    bool debug, color;

  protected:
	
	CloudsVisualSystemClusterMap clusterMap;
	//CloudsRun run;
	CloudsFCPParser parser;
	//CloudsStoryEngine storyEngine;
	//CloudsVisualSystemManager visualSystems;

	void loadSVGs();
    void hideQuestionBox();
    void showQuestionBox();
    ofxFTGLSimpleLayout* getLayoutForLayer( SVGMesh* textMesh, string font);
    int getFontSizeForMesh( SVGMesh* textMesh, string font);
    void drawTextToMesh(ofxFTGLSimpleLayout* font, string text, SVGMesh* mesh);
    void onActBegan();
    void onActEnded();

	//TODO: make separate layer sets for Project Example vs Person
	vector<CloudsSVGMesh> testAllLayout;
    
    CloudsSVGMesh bioLayout, projectLayout, questionLayout;
	
	bool playingMovie;
	bool hasSpeaker;
	CloudsSpeaker currentSpeaker;
	CloudsClip currentClip;
	CloudsProjectExample currentExample;

	ofxFTGLSimpleLayout  *h1, *h2, *h3, *h4, *h5, *p;

	ofVideoPlayer archivePlayer;

	ofxOscReceiver receiver;

	ofFbo displayTarget;
    
    float pFontSize, h3FontSize;
    
    string displayMode, lastQuestion;
    
    //colors
    ofColor lightBlue, darkBlue;
    
    vector<ofxFTGLFont*>    tempFontListThin, tempFontListBook;
    
    SVGMesh *meshQuestion,
            *meshBioLastName,
            *meshBioFirstName,
            *meshBioLocation,
            *meshBioTitle,
            *meshBioDescription,
            *meshProjectVideo,
            *meshProjectTitle,
            *meshProjectArtist,
            *meshProjectDescription;
    
    int stringCounter;

    map<string, CloudsHUDLabel*>    hudLabelMap;


};
