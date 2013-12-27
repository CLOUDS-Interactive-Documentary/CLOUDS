
#include "PanelGraph.h"
#include "CloudsGlobal.h"
#include "ofxRegex.h"
#include "ofxTween.h"


//static ofColor matchColors[MATCH_TYPES] = {
//	ofColor(0, 255, 0),
//	ofColor(255, 0, 0),
//	ofColor(0, 0, 255),
//	ofColor(135, 96, 82),
//	ofColor(222, 69, 219)
//};

PanelGraph::PanelGraph(){
	
}

void PanelGraph::selfSetup(){
	
}

void PanelGraph::selfDraw(){
	
	map<unsigned char, ofPolyline> charPolies;
	for(int i = drawRect.x; i < drawRect.getMaxX(); i++){
		int curline = (i + int(offset*30)) % lines.size();
		map<unsigned char,int>::iterator it;
		for(it = syntaxLines[curline].charCounts.begin(); it != syntaxLines[curline].charCounts.end(); it++){
			charPolies[it->first].addVertex(ofPoint(i,ofMap(it->second, 0, maxCharsOnLine, drawRect.getMaxY(), drawRect.getMinY())));
		}
	}
	
	int col=0;
	int numLines = charPolies.size();
	map<unsigned char, ofPolyline>::iterator it;
	for(it = charPolies.begin(); it != charPolies.end(); it++){
		ofColor c = ofColor::fromHsb( (128.*col)/numLines, 255, 255);
		c.a = 100;
		ofSetColor(c);
		it->second.draw();
		col++;
	}
	
	
}
