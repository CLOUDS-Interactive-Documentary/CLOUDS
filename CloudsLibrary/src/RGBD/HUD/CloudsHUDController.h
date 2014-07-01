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

#include "CloudsHUDLayer.h"
#include "CloudsHUDHomeButton.h"
#include "CloudsHUDLabel.h"
#include "CloudsEvents.h"
#include "CloudsSpeaker.h"

typedef enum {
	CLOUDS_HUD_QUESTION        = 0x0001,
	CLOUDS_HUD_LOWER_THIRD     = 0x0010,
	CLOUDS_HUD_PROJECT_EXAMPLE = 0x0100,
	CLOUDS_HUD_MAP             = 0x1000,
    
	CLOUDS_HUD_FULL            = 0x1111,
    CLOUDS_HUD_LAYER_COUNT     = 4
} CloudsHUDLayerSet;

#ifdef OCULUS_RIFT
typedef enum {
	CLOUDS_HUD_BILLBOARD_NONE = 0,
	CLOUDS_HUD_BILLBOARD_CAMERA,
	CLOUDS_HUD_BILLBOARD_OCULUS
} CloudsHUDBillboard;
#endif

class CloudsClip;
class CloudsHUDController {
  public:
	
	CloudsHUDController();
	
	void setup();
	void update();
	void draw();
#ifdef OCULUS_RIFT
    void draw3D(ofCamera* cam, ofVec2f offset = ofVec2f::zero());
#endif

	void setHomeEnabled(bool enable);
    bool isHomeEnabled();
    void setHudEnabled(bool enable);
    bool isHudEnabled();
	
	bool isResetHit();

	void clearQuestion();
	
	void buildLayerSets();
    void calculateFontSizes();
    int getFontSizeForMesh( SVGMesh* textMesh );
	
	void animateOn(CloudsHUDLayerSet layer = CLOUDS_HUD_FULL);
	void animateOff(CloudsHUDLayerSet layer = CLOUDS_HUD_FULL);
	void respondToClip(CloudsClip* clip);
    void playCued();
	
	map<CloudsHUDLayerSet, vector<CloudsHUDLayer*> > layerSets;
	vector<CloudsHUDLayer*> allLayers;
	
	void saveGuiSettings();
	void toggleGuis();
	
	void questionHoverOn(const string& question, bool animate = true);
	void questionHoverOff();
	
	ofxUISuperCanvas *hudGui;
	CloudsHUDHomeButton home;
	
	void actBegan(CloudsActEventArgs& args);
	void actEnded(CloudsActEventArgs& args);
	void clipBegan(CloudsClipEventArgs& args);
	void visualSystemBegan(CloudsVisualSystemEventArgs& args);
	void visualSystemEnded(CloudsVisualSystemEventArgs& args);
	void questionProposed(CloudsQuestionEventArgs& args);
	void questionSelected(CloudsQuestionEventArgs& args);    
	void topicChanged(CloudsTopicEventArgs& args);
	void preRollRequested(CloudsPreRollEventArgs& args);
    
    ofVec2f getSize(bool bScaled = true);
    ofVec2f getCenter(bool bScaled = true);
    
#ifdef OCULUS_RIFT
    map<CloudsHUDLayerSet, float> layerDistance;
    map<CloudsHUDLayerSet, float> layerRotationH;
    map<CloudsHUDLayerSet, float> layerRotationV;
    map<CloudsHUDLayerSet, CloudsHUDBillboard> layerBillboard;
#endif

  protected:
	
    void populateLowerThird(const string& firstName="", const string& lastName="", const string& title="", const string& location="", const string& textbox="", bool forceOn=false );
    void populateProjectExample(const string& videoPath="", const string& textLeft="", const string& textRight="", const string& textTop="", bool forceOn=false);
    void populateQuestion(const string& question="", bool forceOn=false, bool animate = true);
    void populateMap(const string& leftBox="", const string& rightBox="", bool forceOn=false);
    
	ofVideoPlayer videoPlayer;
    ofRectangle   svgVideoBounds, videoBounds;
    
	bool	bDrawHome;
    bool    bIsHudOpen;
    bool    bDrawHud;
    bool    bSkipAVideoFrame;
    bool	bActJustStarted;
    bool    bLowerThirdCued;
    bool    bVisualSystemDisplayed;
	bool	bResetIsHovered;
	bool	bResetIsClicked;
    float   cuedClipEndTime;
	
    void drawLayer(CloudsHUDLayerSet layer);
#ifdef OCULUS_RIFT
    void drawLayer3D(CloudsHUDLayerSet layer, ofCamera* cam, ofVec2f& offset);
#endif
    ofxFTGLSimpleLayout*    getLayoutForLayer(const string& layerName, const string& fontPath);
    ofxFTGLSimpleLayout*    getLayoutForLayer(const string& layerName, const string& fontPath, bool caps);
    ofxFTGLFont*            getFontForLayer(const string& layerName, const string& fontPath, int kerning);

    
    vector<ofxFTGLFont*>    tempFontList;
    
    ofxFTGLFont             *BylineFirstNameTextBox,
                            *BylineLastNameTextBox,
                            *BylineTopicTextBoxBottom,
                            *BylineTopicTextBoxTop;
    
    ofxFTGLSimpleLayout     *BylineBodyCopyTextBox,
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
    
    int margin;
    
    ofRectangle             defaultBioBounds;
    
    CloudsSpeaker           speaker;
};



