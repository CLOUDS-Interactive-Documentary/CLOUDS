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
    void draw3D(ofCamera* cam);
#endif

	void setHomeEnabled(bool enable);
    bool isHomeEnabled();
    void setHudEnabled(bool enable);
    bool isHudEnabled();
	
	void buildLayerSets();
    void calculateFontSizes();
    int getFontSizeForMesh( SVGMesh* textMesh );
	
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
	void questionProposed(CloudsQuestionEventArgs& args);
	void questionSelected(CloudsQuestionEventArgs& args);    
	void topicChanged(CloudsTopicEventArgs& args);
	void preRollRequested(CloudsPreRollEventArgs& args);
    
    ofVec2f getSize(bool bScaled = true);
    ofVec2f getCenter(bool bScaled = true);
    
#ifdef OCULUS_RIFT
    float layerDistance[CLOUDS_HUD_LAYER_COUNT];
    float layerRotationH[CLOUDS_HUD_LAYER_COUNT];
    float layerRotationV[CLOUDS_HUD_LAYER_COUNT];
    CloudsHUDBillboard layerBillboard[CLOUDS_HUD_LAYER_COUNT];
#endif

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
#ifdef OCULUS_RIFT
    void drawLayer3D(CloudsHUDLayerSet layer, ofCamera* cam);
#endif
    ofxFTGLSimpleLayout*    getLayoutForLayer( string layerName, string fontPath );
    ofxFTGLSimpleLayout*    getLayoutForLayer( string layerName, string fontPath, bool caps );
    ofxFTGLFont*            getFontForLayer( string layerName, string fontPath, int kerning );

    
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



