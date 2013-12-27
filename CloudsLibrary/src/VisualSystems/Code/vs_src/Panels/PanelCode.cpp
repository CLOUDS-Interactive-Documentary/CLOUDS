
#include "PanelCode.h"
#include "CloudsGlobal.h"
#include "ofxRegex.h"
#include "ofxTween.h"

PanelCode::PanelCode(){

}

void PanelCode::selfSetup(){
	
	maxCharsOnLine = 0;

	//TODO: shared font
	layout.loadFont( GetCloudsDataPath() + "font/Consolas.ttf", 8);
	
}

void PanelCode::selfDraw(){
	
	float lineHeight = layout.getStringBoundingBox("Q",0,0).height;
	int line = 0;

	for(int l = lineHeight*1.5; l < drawRect.height; l += lineHeight*1.5){
		int curline = (line + int(offset)) % syntaxLines.size();
		ofPushStyle();
		ofSetColor(tint);
		
		layout.drawString( syntaxLines[curline].baseLine, drawRect.x, drawRect.y + l);
		
		for(int c = 0; c < MATCH_TYPES; c++){
			ofSetColor( matchColorTypes[c] * tint );
			layout.drawString( syntaxLines[curline].colored[c], drawRect.x, drawRect.y + l);
		}
		ofPopStyle();
		
		line++;
	}
}


