
#include "PanelGraph.h"
#include "CloudsGlobal.h"
#include "ofxRegex.h"
#include "ofxTween.h"
#include "ofRange.h"

PanelGraph::PanelGraph(){
	pixelStep = 10.;
}

void PanelGraph::selfSetup(){
	
}

void PanelGraph::selfDraw(){
	
	map<unsigned char, ofPolyline> charPolies;
	for(int i = drawRect.x; i < drawRect.getMaxX(); i+=pixelStep){
		int curline = (i + int(offset)) % lines.size();
		map<unsigned char,int>::iterator it;
		ofRange logs;
		for(it = syntaxLines[curline].charCounts.begin(); it != syntaxLines[curline].charCounts.end(); it++){
			float logGraphPoint = log(ofMap(it->second, 0, maxCharsOnLine, 0, 1.));
			logs.min = MIN(logs.min,logGraphPoint);
			logs.max = MAX(logs.max,logGraphPoint);
			charPolies[it->first].addVertex( ofPoint(i,ofMap(logGraphPoint,0,drawRect.height,drawRect.getMaxY(), drawRect.getMinY())) );
		}
	}
	
	int col = 0;
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
