//
//  PanelTenPrint.cpp
//  Code
//
//  Created by James George on 1/3/14.
//
//

#include "PanelConsole.h"

static string errors[] = {
	"Undefined action save function called",
	"NULL right after instantiaton. correlating system null?",
	"Triggered visual system while still showing one",
	"Hiding visual system while none is showing",
	"doesn't have combined video",
	"Still showing last system",
	"Check OSCSettings.xml in CloudsData/SecondaryDisplay/OSC",
	"Check OSCSettings.xml in CloudsData/SecondaryDisplay/OSC",
	"Couldn't find preset with ID",
	"Could not open text file",
	"Draw threshold turned on but image not prepared",
	"/Library/Application Support/Google/Chrome/Default/History",
	"no data file",
	"TIMELINE UPDATE FOR SYSTEM",
	"Unable to parse \" << filename << \": \" << reader",
	"Failed to set font size"
};


PanelConsole::PanelConsole(){
	charsPerSecond = 1;
	maxChars = 0;
}

void PanelConsole::selfSetup(){
	
	vector<string> types;
	types.push_back("vectors");
	types.push_back("dots");
	types.push_back("errors");	
	string type = types[ ofRandom(types.size()) ];

	///VECTORS
	if(type == "vectors"){
		for(int i = 0; i < 500; i++){
			float curwidth = 0;
			stringstream ss;
			string laststr = "";
			while(curwidth < drawRect.width){
				laststr = ss.str();
				ofVec3f vec3(ofRandomuf(),ofRandomuf(),ofRandomuf());
				ss << "(" << vec3 << ")";
				curwidth = sharedFont->stringWidth( ss.str() );
			}
			lines.push_back(laststr);
		}
	}
	//DOTS	
	else if(type == "dots"){
		for(int i = 0; i < 500; i++){
			float curwidth = 0;
			float targetWidth = ofRandom(drawRect.width*.25, drawRect.width*.9);
			stringstream ss;
			string laststr = "";
			while(curwidth < targetWidth){
				laststr = ss.str();
				ss << "...";
				curwidth = sharedFont->stringWidth( ss.str() );
			}
			lines.push_back(laststr);
		}
	}
	//ERRORS
	else if(type == "errors"){
		
		vector<string> errors;
		errors.push_back("Undefined action save function called");
		errors.push_back("NULL right after instantiaton. correlating system null?");
		errors.push_back("Triggered visual system while still showing one");
		errors.push_back("Hiding visual system while none is showing");
		errors.push_back("doesn't have combined video");
		errors.push_back("Still showing last system");
		errors.push_back("Check OSCSettings.xml in CloudsData/SecondaryDisplay/OSC");
		errors.push_back("Check OSCSettings.xml in CloudsData/SecondaryDisplay/OSC");
		errors.push_back("Couldn't find preset with ID");
		errors.push_back("Could not open text file");
		errors.push_back("Draw threshold turned on but image not prepared");
		errors.push_back("/Library/Application Support/Google/Chrome/Default/History");
		errors.push_back("no data file");
		errors.push_back("TIMELINE UPDATE FOR SYSTEM");
		errors.push_back("Unable to parse \" << filename << \": \" << reader");
		errors.push_back("Failed to set font size");
		
		for(int i = 0; i < errors.size(); i++){
			string errorType = ofRandomuf() > .5 ? "ERROR" : "WARNING";
			lines.push_back("[" + errorType + " " + errors[i] + "]" );
		}
	}
	
}

void PanelConsole::selfUpdate(){
	maxChars = offset*20.;
}

void PanelConsole::selfDraw(){
	int line = 0;
	lineHeight = sharedFont->stringHeight("/");
	int charsConsumed = 0;
	for(int l = lineHeight*1.5; l < drawRect.height; l += lineHeight*1.5){
		int curline = (line + int(offset)) % lines.size();
		char timestamp[1024];
		sprintf(timestamp, "$ %02d:%04d: ", ofGetSeconds(), int(ofGetElapsedTimeMillis()));
		sharedFont->drawString( timestamp + lines[curline], drawRect.x, drawRect.y + l);
		line++;
	}
}
