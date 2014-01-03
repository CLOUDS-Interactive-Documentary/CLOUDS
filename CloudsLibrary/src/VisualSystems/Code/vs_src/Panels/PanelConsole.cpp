//
//  PanelTenPrint.cpp
//  Code
//
//  Created by James George on 1/3/14.
//
//

#include "PanelConsole.h"

PanelConsole::PanelConsole(){
	charsPerSecond = 1;
	maxChars = 0;
}

void PanelConsole::selfSetup(){
	
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

void PanelConsole::selfUpdate(){
	maxChars = offset*20.;
}

void PanelConsole::selfDraw(){
	int line = 0;
	lineHeight = sharedFont->stringHeight("/");
	int charsConsumed = 0;
	for(int l = lineHeight*1.5; l < drawRect.height; l += lineHeight*1.5){
		int curline = (line + int(offset)) % lines.size();
		sharedFont->drawString( lines[curline], drawRect.x, drawRect.y + l);
		line++;
	}
}
