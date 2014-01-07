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
    void reloadShader();
    
    void saveGuiSettings();
	void toggleGuis();
    void hideGUI();
	
	ofxUISuperCanvas *SDGui;
    void draw();
    
    bool debug, color;
	void respondToClip(CloudsClip& clip);
	CloudsFCPParser parser;
    
    float tx;
	
  protected:
	
	CloudsVisualSystemClusterMap clusterMap;

	void loadSVGs();
    void hideQuestionBox();
    void showQuestionBox();
    ofxFTGLSimpleLayout* getLayoutForLayer( SVGMesh* textMesh, string font, float kerning);
    ofxFTGLFont* getFontForLayer( SVGMesh* textMesh, string font, float kerning);

    int getFontSizeForMesh( SVGMesh* textMesh, string font);
    void drawTextToMesh(ofxFTGLSimpleLayout* font, string text, SVGMesh* mesh);
    void onActBegan();
    void onActEnded();
    void animateIn();
    void animateOut();
    void setNormalsOn(CloudsSVGMesh* m);

	//TODO: make separate layer sets for Project Example vs Person
	//vector<CloudsSVGMesh*> allLayouts;
    
    CloudsSVGMesh bioLayout, projectLayout, questionLayout;
	
	bool playingMovie;
	bool hasSpeaker;
	CloudsSpeaker currentSpeaker;
	CloudsClip currentClip;
	CloudsProjectExample currentExample;

	//ofxFTGLSimpleLayout  *h1, *h2, *h3, *h4, *h5, *p;

	ofVideoPlayer archivePlayer;

	ofxOscReceiver receiver;

	ofFbo displayTarget, gui;
    
    float playhead, animationSpeed;
    
    string displayMode, lastQuestion;
    
    //colors
    ofColor lightBlue, darkBlue;
    
    vector<ofxFTGLFont*>    tempFontListThin, tempFontListBook;
    
    SVGMesh *meshQuestion,
            *meshBioLastName,
            *meshBioFirstName,
            *meshBioLocation,
            *meshBioLocationBG,
            *meshBioTitle,
            *meshBioTitleBG,
            *meshBioDescription,
            *meshProjectVideo,
            *meshProjectTitle,
            *meshProjectArtist,
            *meshProjectDescription;
    
    ofMesh  videoBox;
    
    ofxFTGLFont             *fontBioLastName,
                            *fontBioFirstName,
                            *fontBioLocation,
                            *fontBioTitle;
    
    ofxFTGLSimpleLayout     *layoutQuestion,
                            *layoutBioDescription,
                            *layoutProjectVideo,
                            *layoutProjectTitle,
                            *layoutProjectArtist,
                            *layoutProjectDescription;
    
    float beginTime;

    map<string, CloudsHUDLabel*>    hudLabelMap;
    
    ofShader shader;
    
    ofShader lineAnimate;

    bool animatingIn, animatingOut;

};
