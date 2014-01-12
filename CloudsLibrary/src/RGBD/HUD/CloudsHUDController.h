//
//  CloudsHUDController.h
//  CloudsPointcloudViewer
//
//  Created by James George on 12/14/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxFTGL.h"
#include "ofxAVFVideoPlayer.h"

#include "CloudsHUDLayer.h"
#include "CloudsHUDHomeButton.h"
#include "CloudsHUDLabel.h"
#include "CloudsEvents.h"

typedef enum {
	CLOUDS_HUD_FULL = 0,
	CLOUDS_HUD_QUESTION,
	CLOUDS_HUD_LOWER_THIRD,
	CLOUDS_HUD_PROJECT_EXAMPLE,
	CLOUDS_HUD_MAP
} CloudsHUDLayerSet;

class CloudsClip;
class CloudsHUDController {
  public:
	
	CloudsHUDController();
	
	void setup();
	void update();
	void draw();
    void drawOverlay(ofVec2f overlaySize);

	void setHomeEnabled(bool enable);
	
	void buildLayerSets();
    void calculateFontSizes();
    int getFontSizeForMesh( SVGMesh* textMesh );
    
    ofVec2f getSize();
	
	void animateOn(CloudsHUDLayerSet layer = CLOUDS_HUD_FULL);
	void animateOff(CloudsHUDLayerSet layer = CLOUDS_HUD_FULL);
	void respondToClip(CloudsClip& clip);
	
	map<CloudsHUDLayerSet, vector<CloudsHUDLayer*> > layerSets;
	vector<CloudsHUDLayer*> allLayers;
	
	void saveGuiSettings();
	void toggleGuis();
	
	void questionHoverOn(string question);
	void questionHoverOff();
	
	ofxUISuperCanvas *hudGui;
	CloudsHUDHomeButton home;
	
	void actBegan(CloudsActEventArgs& args);
	void actEnded(CloudsActEventArgs& args);
	void clipBegan(CloudsClipEventArgs& args);
	void visualSystemBegan(CloudsVisualSystemEventArgs& args);
	void visualSystemEnded(CloudsVisualSystemEventArgs& args);
	void questionAsked(CloudsQuestionEventArgs& args);
	void topicChanged(CloudsTopicEventArgs& args);
	void preRollRequested(CloudsPreRollEventArgs& args);

  protected:
	
    void populateLowerThird( string firstName="", string lastName="", string title="", string location="", string textbox="", bool forceOn=false );
    void populateProjectExample(string videoPath="", string textLeft="", string textRight="", string textTop="", bool forceOn=false);
    void populateQuestion( string question="", bool forceOn=false);
    void populateMap( string leftBox="", string rightBox="", bool forceOn=false);
    
    ofxAVFVideoPlayer       videoPlayer;
    ofRectangle             svgVideoBounds, videoBounds;
    
	bool	bDrawHome;
    bool    bIsHudOpen;
    bool    bDrawHud;
    bool    bSkipAVideoFrame;
    
	
	
    void drawLayer(CloudsHUDLayerSet layer);
    ofxFTGLSimpleLayout*    getLayoutForLayer( string layerName, string fontPath );
    
    vector<ofxFTGLFont*>    tempFontList;
    ofxFTGLSimpleLayout     *BylineBodyCopyTextBox,
                            *BylineFirstNameTextBox,
                            *BylineLastNameTextBox,
                            *BylineTopicTextBoxBottom,
                            *BylineTopicTextBoxTop,
                            *ResetButtonTextBox,
                            *QuestionTextBox,
                            *TopicTextBoxLeft,
                            *TopicTextBoxRight,
                            *ProjectExampleTextboxLeft,
                            *ProjectExampleTextboxRight,
                            *ProjectExampleTextBoxTop;
	
    map<CloudsHUDLayerSet,bool>		hudOpenMap;
    map<string, CloudsHUDLabel*>    hudLabelMap;
    ofRectangle hudBounds;
    float scaleAmt;
    float overlayScaleAmt;
};



