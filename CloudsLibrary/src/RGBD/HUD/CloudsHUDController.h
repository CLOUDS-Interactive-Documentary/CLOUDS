//
//  CloudsHUDController.h
//  CloudsPointcloudViewer
//
//  Created by James George on 12/14/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxFTGL.h"


#include "CloudsHUDHomeButton.h"
#include "CloudsHUDLabel.h"
#include "CloudsEvents.h"
#include "CloudsSpeaker.h"
#include "CloudsHUDScroller.h"

typedef enum {
    //main layers
	CLOUDS_HUD_HOME = 0,
	CLOUDS_HUD_QUESTION,
	CLOUDS_HUD_LOWER_THIRD,
	CLOUDS_HUD_PROJECT_EXAMPLE,
	CLOUDS_HUD_PAUSE,
	CLOUDS_HUD_NEXT,
    
    //research
	CLOUDS_RESEARCH,
	CLOUDS_RESEARCH_SHUFFLE,
	CLOUDS_RESEARCH_RESUME,

    //mobile research info ones
    CLOUDS_RESEARCH_TOPIC,
    CLOUDS_RESEARCH_PPL,
    CLOUDS_RESEARCH_VS,
    
    //about
    CLOUDS_ABOUT_BACKERS,
    CLOUDS_ABOUT_CAST,
    CLOUDS_ABOUT_CREDITS,
    CLOUDS_ABOUT_INFO,
    CLOUDS_ABOUT_MAIN,
    CLOUDS_ABOUT_SETTINGS,
    CLOUDS_ABOUT_SETTINGS_EXPANDED,

    CLOUDS_HUD_ALL
} CloudsHUDLayerSet;

typedef enum{
    CLOUDS_HUD_RESEARCH_TAB_TOPICS = 0,
    CLOUDS_HUD_RESEARCH_TAB_PEOPLE,
    CLOUDS_HUD_RESEARCH_TAB_VISUALS,
} CloudsHUDResearchTab;

typedef enum{
    CLOUDS_HUD_ABOUT_TAB_INFO = 0,
    CLOUDS_HUD_ABOUT_TAB_CAST,
    CLOUDS_HUD_ABOUT_TAB_CREDITS,
    CLOUDS_HUD_ABOUT_TAB_BACKERS,
} CloudsHUDAboutTab;

typedef enum{
    CLOUDS_HUD_TRIANGLE_UP = 0,
    CLOUDS_HUD_TRIANGLE_RIGHT,
    CLOUDS_HUD_TRIANGLE_DOWN,
    CLOUDS_HUD_TRIANGLE_LEFT,
    CLOUDS_HUD_TRIANGLE_SQUARE,
    CLOUDS_HUD_TRIANGLE_X,
    CLOUDS_HUD_TRIANGLE_NONE
} CloudsHUDTriangleDirection;

typedef struct{
    bool visible;
    ofVec2f pos;
    string backer;
} CloudsHUDBacker;

class CloudsHUDResearchButton {
  public:
    CloudsHUDResearchButton(){
        top = 0;
        visible = false;
        hovered = false;
        pressed = false;
        clicked = false;
        visited = false;
    }
    
    CloudsHUDResearchTab parentTab;
    float top;
    bool visible;
    bool hovered;
    bool pressed;
    bool clicked;
    bool visited; //has the viewer encountered this thing before
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
class CloudsHUDLayer;
class CloudsHUDController {
  public:
	
	CloudsHUDController();
	
	void setup();
	void update();
	void draw();
//#ifdef OCULUS_RIFT
//    void draw3D(ofCamera* cam, ofVec2f offset = ofVec2f::zero());
//#endif

    void setHudEnabled(bool enable);
    bool isHudEnabled();
	
	//mouse for clicking reset
	bool isResetHit();
	bool isResearchResetHit();
	bool isNextHit();
	bool isExploreMapHit();
	bool isSeeMorePersonHit();
    bool isResumeActHit();

    void preselectMap();
    bool selectedMapTab();
    bool selectedPeopleTab();
    bool selectedVisualsTab();
    
    bool aboutClosed();
    
	void mouseMoved(ofMouseEventArgs& args);
	void mousePressed(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	void mouseScrolled(ofMouseEventArgs& args);
    
	void clearQuestion();
    void clearClip();
    void clearVisualSystem();
    
    void pause();
    void unpause(bool force = false);
    bool didPause();
    bool didUnpause();    
    bool isPaused();
    
    void showAbout();
    void hideAbout();
    
    void enteringVisuals();
    void exitingVisuals();
    
	void animateOn(CloudsHUDLayerSet layer);
	void animateOff(CloudsHUDLayerSet layer);
    void animateOff();
    
	void respondToClip(CloudsClip* clip);
	void respondToSystem(const CloudsVisualSystemPreset& preset);
	
	map<CloudsHUDLayerSet, CloudsHUDLayer* > layers;
    
	vector<CloudsHUDLayer*> allLayers;
	
	void questionHoverOn(const string& question, bool animate = true);
    
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
    float transitionFade;
    void researchTransitionFinished();
    
//#ifdef OCULUS_RIFT
//    map<CloudsHUDLayerSet, float> layerDistance;
//    map<CloudsHUDLayerSet, float> layerRotationH;
//    map<CloudsHUDLayerSet, float> layerRotationV;
//    map<CloudsHUDLayerSet, CloudsHUDBillboard> layerBillboard;
//#endif
    
    void setTopics(const set<string>& topics);
    void populateSpeakers();
    void setVisuals(map<string, CloudsVisualSystemCredit>& visuals);
    
    void setVisitedTopics(set<string> topics);
    void setVisitedPeople(set<string> people);
    void setVisitedVisuals(set<string> visuals);
    
    void updateVisited(CloudsHUDResearchList& list, set<string> set);
    
    void setSeeMoreName(string name);
    void selectTopic(string topic);
    void selectPerson(string personID);
    void selectVisual(string visualName);
    void selectItem(CloudsHUDResearchTab tab, string itemID);

    //where the researched item may be clicked
    void setResearchClickAnchor(ofVec2f anchor);
    
    bool isItemSelected();
    bool isItemConfirmed();
    bool didItemSelectionChange();
    void clearSelection();
    
    string getSelectedItem();
    
    bool visualLevelDidChange();
    bool requestClearHistory();
    
    bool setToPretty;
    void setHasHistory(bool hasHistory);
    
  protected:
	
    void populateLowerThird(const string& firstName,
                            const string& lastName,
                            const string& title,
                            const string& location,
                            const string& textbox);
    
    void populateVisualSystem(const string& title,
                              const string& line1,
                              const string& line2);
    
    void showProjectExample(const string& videoPath,
                            bool forceOn = false);
    
    void populateQuestion(const string& question,
                          bool forceOn=false,
                          bool animate = true);
    
    
   	void buildLayerSets();
    void calculateFontSizes();
    int getFontSizeForMesh( SVGMesh* textMesh );
    void setupBacking(string labelName, CloudsHUDLayerSet layer, string backingName);
    
	ofVideoPlayer videoPlayer;
    ofRectangle   svgVideoBounds, videoBounds;
    
    vector<CloudsHUDBacker> backers;
    /////////////// SCROLL VARIABLES
    //////////////
    CloudsHUDScroller researchScroller;
    CloudsHUDScroller aboutScroller;
    void updateScroll();
    //////////////
    
    bool bPaused;

    void updateResearchNavigation();
    void updateAboutNavigation();

    bool bResearchTransitioning;
    
    void selectButton(const CloudsHUDResearchButton& button);
    void unselectButtons();
    
    //string bioText;
    ofxFTGLFont backersFont;
        
    bool    bDrawHud;
    bool    bSkipAVideoFrame;
    bool	bActJustStarted;
    
    bool    bVisualSystemDisplayed;
    bool    bClipIsPlaying;
    bool    bJustPaused;
    bool    bJustUnpaused;
    bool    bQuestionDisplayed;
    bool    bProjectExampleDisplayed;
    bool    bItemSelectionChanged;
    
//#ifdef OCULUS_RIFT
//    void drawLayer3D(CloudsHUDLayerSet layer, ofCamera* cam, ofVec2f& offset);
//#endif
    
    CloudsHUDResearchTab currentTab;
    CloudsHUDResearchTab nextTab;

    CloudsHUDAboutTab currentAboutTab;
    bool visualLevelChanged;
    bool historyCleared;
    
    void drawList();
    void drawBackersList();
    
    CloudsHUDLabel* getLabelForLayer(const string& layerName,
                                     ofBuffer& fontBuffer,
                                     int kerning = 35,
                                     bool caps = false,
                                     bool useLayout = false,
                                     int layoutFontSize = 12);
    
    void attachTriangleToLabel(CloudsHUDLabel* label, CloudsHUDLayerSet layer, string triangleLayerName, CloudsHUDTriangleDirection direction);
    
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
    
    ofImage* currentPreviewImage;
    CloudsHUDLabel* currentPreviewSelection;
    ofImage topicMapPreview;
    ofImage peopleMapPreview;
    map<CloudsHUDResearchTab, CloudsHUDResearchList> researchLists;
    CloudsHUDResearchList* currentResearchList;
    
    ofRectangle     getScaledRectangle(const ofRectangle& rect);
    ofRectangle     defaultBioBounds;

    CloudsClip*     currentClip;
    CloudsSpeaker   currentSpeaker;
    
    string filenameForLayer(CloudsHUDLayerSet layer);
#ifdef VHX_MEDIA
    map<string, CloudsMedia*> vhxProjectExampleIds;
    void parseVHXIds();

    void vhxRequestComplete(CloudsVHXEventArgs& args);

#endif
    
    CloudsMedia* waitingMedia;
};



