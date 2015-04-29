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
	CLOUDS_HUD_HOME = 0,
	CLOUDS_HUD_QUESTION,
	CLOUDS_HUD_LOWER_THIRD,
	CLOUDS_HUD_PROJECT_EXAMPLE,
	CLOUDS_HUD_PAUSE,
	CLOUDS_HUD_NEXT,
	CLOUDS_HUD_RESEARCH_LIST,
	CLOUDS_HUD_RESEARCH_NAV,
	CLOUDS_HUD_RESEARCH_SHUFFLE,
	CLOUDS_HUD_ABOUT,

    CLOUDS_HUD_ALL
} CloudsHUDLayerSet;

typedef enum{
    CLOUDS_HUD_RESEARCH_TAB_TOPICS = 0,
    CLOUDS_HUD_RESEARCH_TAB_PEOPLE,
    CLOUDS_HUD_RESEARCH_TAB_VISUALS,
} CloudsHUDResearchTab;


class CloudsHUDResearchButton {
  public:
    CloudsHUDResearchButton(){
        top = 0;
        visible = false;
        hovered = false;
        pressed = false;
        clicked = false;
    }
    
    float top;
    bool visible;
    bool hovered;
    bool pressed;
    bool clicked;
    ofRectangle selectRect;
    ofRectangle selectRectScaled;
    string tag;
    string label;
};

class CloudsHUDResearchList{
 public:
    vector<CloudsHUDResearchButton> buttons;
    float totalScrollHeight;
    float scrollPosition;
};

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

    void setHudEnabled(bool enable);
    bool isHudEnabled();
	
	//mouse for clicking reset
	bool isResetHit();
	bool isNextHit();
	bool isExploreMapHit();
	bool isSeeMorePersonHit();
    
    bool selectedMapTab();
    bool selectedPeopleTab();
    bool selectedVisualsTab();
    
	void mouseMoved(ofMouseEventArgs& args);
	void mousePressed(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);

	void clearQuestion();
    void clearClip();
    void clearVisualSystem();
    
    bool didPause();
    bool didUnpause();    
    bool isPaused();
    
    void showAbout();
    void hideAbout();
    
	void animateOn(CloudsHUDLayerSet layer);
	void animateOff(CloudsHUDLayerSet layer);
    void animateOff();
    
	void respondToClip(CloudsClip* clip);
	void respondToSystem(const CloudsVisualSystemPreset& preset);
	
	map<CloudsHUDLayerSet, CloudsHUDLayer* > layers;
    
	vector<CloudsHUDLayer*> allLayers;
	
	void questionHoverOn(const string& question, bool animate = true);
	
	ofxUISuperCanvas *hudGui;
	CloudsHUDHomeButton home;
	
	void actBegan(CloudsActEventArgs& args);
	void actEnded(CloudsActEventArgs& args);
	void clipBegan(CloudsClipEventArgs& args);
    void clipEnded(); //not called by event system
	void visualSystemBegan(CloudsVisualSystemEventArgs& args);
	void visualSystemEnded(CloudsVisualSystemEventArgs& args);
	void questionProposed(CloudsQuestionEventArgs& args);
	void questionSelected(CloudsQuestionEventArgs& args);    
	void topicChanged(CloudsTopicEventArgs& args);
	void preRollRequested(CloudsPreRollEventArgs& args);
    
    ofVec2f getSize(bool bScaled = true);
    ofVec2f getCenter(bool bScaled = true);

	int videoFrameCounter;
	bool isPlaying;
    
#ifdef OCULUS_RIFT
    map<CloudsHUDLayerSet, float> layerDistance;
    map<CloudsHUDLayerSet, float> layerRotationH;
    map<CloudsHUDLayerSet, float> layerRotationV;
    map<CloudsHUDLayerSet, CloudsHUDBillboard> layerBillboard;
#endif

    void pause();
    void unpause();
    
    void setTopics(const set<string>& topics);
    bool isItemSelected();
    bool isItemConfirmed();
    void clearSelection();
    
    string getSelectedItem();
    
    void populateSpeakers();
    
  protected:
	
    void populateLowerThird(const string& firstName,
                            const string& lastName,
                            const string& title,
                            const string& location,
                            const string& textbox);
    
    void populateVisualSystem(const string& creatorsName,
                              const string& systemName);
    
    void populateProjectExample(const string& videoPath,
                                const string& textLeft,
                                const string& textRight,
                                const string& textTop,
                                bool forceOn = false);
    
    void populateQuestion(const string& question,
                          bool forceOn=false,
                          bool animate = true);
    
    
   	void buildLayerSets();
    void calculateFontSizes();
    int getFontSizeForMesh( SVGMesh* textMesh );
 
	ofVideoPlayer videoPlayer;
    ofRectangle   svgVideoBounds, videoBounds;
    
    
    /////////////// SCROLL VARIABLES
    //////////////
    ofRectangle   researchScrollBounds;
    ofRectangle   researchScrollBoundsScaled;
    ofRectangle   researchScrollUpBounds;
    ofRectangle   researchScrollDownBounds;
    ofRectangle   researchScrollUpBoundsScaled;
    ofRectangle   researchScrollDownBoundsScaled;
    bool          bIsScrollUpHover;
    bool          bIsScrollDownHover;
    bool          bIsScrollUpPressed;
    bool          bIsScrollDownPressed;
    bool          bIsHoldScrolling;

    float         scrollPressedTime;
    
    float         scrollIncrement;
    
    void          updateScroll();
    //////////////
    
    void updateResearchNavigation();
    
    //temporary way to confirm selection in research mode
    ofRectangle fakeConfirmSelectionBounds;
    bool fakeConfirmHovered;
    bool fakeConfirmPressed;
    bool fakeConfirmClicked;
    ////////////////////
    
    
	ofMesh resetTriangle;
    
    bool    bDrawHud;
    bool    bSkipAVideoFrame;
    bool	bActJustStarted;
    
    bool    bVisualSystemDisplayed;
    bool    bClipIsPlaying;
    bool    bJustPaused;
    bool    bJustUnpaused;
    bool    bQuestionDisplayed;
    bool    bProjectExampleDisplayed;
    
    void drawLayer(CloudsHUDLayerSet layer);
#ifdef OCULUS_RIFT
    void drawLayer3D(CloudsHUDLayerSet layer, ofCamera* cam, ofVec2f& offset);
#endif
    
    CloudsHUDResearchTab currentTab;
    void beginListStencil();
    void endListStencil();
    
    void drawList();
    
    CloudsHUDLabel*    getLabelForLayer(const string& layerName, const string& fontPath, int kerning = 35, bool caps = false,  bool useLayout = false);

    
    vector<ofxFTGLFont*>    tempFontList;
    CloudsHUDLabel* ResearchTopicListLabel;
	
    map<CloudsHUDLayerSet,bool>		hudOpenMap;
    map<string, CloudsHUDLabel*>    hudLabelMap;
    map<CloudsHUDLayerSet, vector<CloudsHUDLabel*> > hudLayerLabels;

    ofRectangle bioBounds;
    ofRectangle hudBounds;
    float scaleAmt;
    ofVec2f scaleOffset;
    int margin;
    
    map<CloudsHUDResearchTab, CloudsHUDResearchList> researchLists;
    CloudsHUDResearchList* currentResearchList;
    
    ofRectangle     getScaledRectangle(const ofRectangle& rect);
    ofRectangle     defaultBioBounds;

    CloudsClip*     currentClip;
    CloudsSpeaker   currentSpeaker;
};



