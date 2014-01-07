//
//  PanelTenPrint.cpp
//  Code
//
//  Created by James George on 1/3/14.
//
//

#include "PanelTenPrint.h"
#include "CloudsGlobal.h"

PanelTenPrint::PanelTenPrint(){
	charsPerSecond = 1;
	maxChars = 0;
}

void PanelTenPrint::selfSetup(){
	
//	curString = ofBufferFromFile(GetCloudsVisualSystemDataPath("Code") + "newline.txt").getText();
	pairs.push_back( make_pair("\\", "/"));
	pairs.push_back( make_pair(".", " ") );
	pairs.push_back( make_pair("_", "|") );
//	pairs.push_back( make_pair("o", " ") );

	pair<string,string>& curpair = pairs[ ofRandom(pairs.size()) ];
				
												   
	float lineHeight = sharedFont->stringHeight("/");
	for(int l = lineHeight; l < drawRect.height; l += lineHeight){
		int charWidth = sharedFont->stringWidth("/");
		string curline;
		for(int c = 0; c < drawRect.width; c+=charWidth){
			curline += (ofRandomuf() > .5 ? curpair.first : curpair.second );
		}
		lines.push_back(curline);
	}	
}

void PanelTenPrint::selfUpdate(){
//	curString += ofRandomuf() > .5 ? "\\" : "/";
//	curbuf.append();
//	cout << (ofRandomuf() > .5 ? "\\" : "/");
//	if(ofGetFrameNum() % 100 == 0){
//		cout << endl;
//	}
	maxChars = offset*20.;
}

void PanelTenPrint::selfDraw(){
	int line = 0;
	float lineHeight = sharedFont->stringHeight("/");
	int charsConsumed = 0;
	for(int l = lineHeight; l < drawRect.height; l += lineHeight){
		string& curLine = lines[line%lines.size()];
		if(charsConsumed + curLine.length() > maxChars){
			sharedFont->drawString(curLine.substr(0, maxChars - charsConsumed), drawRect.x + *marginLeft, drawRect.y+l);
			break;
		}
		else{
			sharedFont->drawString(curLine, drawRect.x + *marginLeft, drawRect.y+l);
		}
		charsConsumed += curLine.length();
		line++;
	}
}
