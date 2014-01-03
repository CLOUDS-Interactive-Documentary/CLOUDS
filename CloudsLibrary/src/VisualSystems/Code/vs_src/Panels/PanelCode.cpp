
#include "PanelCode.h"
#include "CloudsGlobal.h"
#include "ofxRegex.h"
#include "ofxTween.h"

PanelCode::PanelCode(){
	sharedFont = NULL;
}

void PanelCode::selfSetup(){
	
	maxCharsOnLine = 0;
}

void PanelCode::selfDraw(){

	if(sharedFont == NULL){
		return;
	}
	
	float lineHeight = sharedFont->getStringBoundingBox("Q",0,0).height;
	int line = 0;

	for(int l = lineHeight*1.5; l < drawRect.height; l += lineHeight*1.5){
		int curline = (line + int(offset)) % syntaxLines.size();
		ofPushStyle();
		ofSetColor(tint);
		
		sharedFont->drawString( syntaxLines[curline].baseLine, drawRect.x, drawRect.y + l);
		
		for(int c = 0; c < MATCH_TYPES; c++){
			ofSetColor( matchColorTypes[c] * tint );
			sharedFont->drawString( syntaxLines[curline].colored[c], drawRect.x, drawRect.y + l);
		}
		ofPopStyle();
		
		line++;
	}
}


