//
//  CloudsHUDController.h
//  CloudsPointcloudViewer
//
//  Created by James George on 12/14/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxSvg.h"
#include "ofxUI.h"
#include "ofxFTGL.h"

#include "CloudsHUDLayer.h"
#include "CloudsHUDHomeButton.h"
#include "CloudsHUDLabel.h"

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

	void buildLayerSets();
    void calculateFontSizes();
    int getFontSizeForMesh( SVGMesh* textMesh );
	
	void clipBegan(CloudsClip& clip);
	
	void animateOn(CloudsHUDLayerSet layer = CLOUDS_HUD_FULL);
	void animateOff();
	
	ofImage testImage;
	
	map<CloudsHUDLayerSet, vector<CloudsHUDLayer*> > layerSets;
	vector<CloudsHUDLayer*> allLayers;
	
	void saveGuiSettings();
	void toggleGuis();
	
	ofxUISuperCanvas *hudGui;
	CloudsHUDHomeButton home;
	
  protected:
    bool    bIsHudOpen;
    
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
    
//    vector<CloudsHUDLabel*>         hudLabelList;
    map<string, CloudsHUDLabel*>    hudLabelMap;
    
};



