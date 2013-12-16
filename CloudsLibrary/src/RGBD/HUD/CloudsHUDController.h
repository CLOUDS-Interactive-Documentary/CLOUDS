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
#include "CloudsHUDLayer.h"
#include "CloudsHUDHomeButton.h"

typedef enum {
	CLOUDS_HUD_FULL = 0,
	CLOUDS_HUD_QUESTION,
	CLOUDS_HUD_LOWER_THIRD,
	CLOUDS_HUD_PROJECT_EXAMPLE
} CloudsHUDLayerSet;

class CloudsHUDController {
public:
	
	CloudsHUDController();
	
	void setup();
	void update();
	void draw();

	void buildLayerSets();
	
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
	void drawLayer(CloudsHUDLayerSet layer);
	
};



