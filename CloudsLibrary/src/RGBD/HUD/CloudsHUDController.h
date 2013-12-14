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

typedef struct {
	ofxSVG svg;
	float startTime;
	float duration;
} SVGLayer;

class CloudsHUDController {
public:
	
	CloudsHUDController();
	
	void setup();
	void draw();

	ofImage testImage;
	SVGLayer testLayer;
	
	
	void saveGuiSettings();
	void toggleGuis();
	ofxUISuperCanvas *hudGui;

};


