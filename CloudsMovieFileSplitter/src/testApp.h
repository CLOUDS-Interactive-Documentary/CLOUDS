#pragma once

#include "ofMain.h"
#include "CloudsFCPParser.h"
#include "ofxXmlSettings.h"

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
    void parseClipItem(ofxXmlSettings& fcpXML, string currentName);
    
    ofxXmlSettings fcpXML;
    CloudsFCPParser parser;

};
