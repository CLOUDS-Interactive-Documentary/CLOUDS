//
//  CloudsSDLayer.cpp
//  CloudsSDDesigner
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsSDLayer.h"
#include "ofxXmlSettings.h"

CloudsSDLayer::CloudsSDLayer(){
	startTime = 0;
	duration = 0;
	delayTime = 0;
	percentComplete = ofVec2f(0,0);
	animating = false;
	maxUpdateInterval = 1./60.;
	lastUpdateTime = 0;
	startPercent.x = .8;
	startPercent.y = .0;
}

void CloudsSDLayer::parse(string svgFilePath){
	
	svg.load(svgFilePath);
	
	//find text boxes
	ofxXmlSettings xml;
	xml.loadFile(svgFilePath);
	xml.pushTag("svg");
	int numGTags = xml.getNumTags("g");
	for(int i = 0; i < numGTags; i++){
		if(!xml.attributeExists("g","id",i)){
			continue;
		}
		if(xml.attributeExists("g", "display", i) && xml.getAttribute("g", "display", "", i) == "none"){
			continue;
		}
		string id = ofToLower(xml.getAttribute("g", "id", "", i));
		if(id.find("textbox") != string::npos){
			cout << "Found Text Box " << id << endl;
			xml.pushTag("g",i);
			int numRects = xml.getNumTags("rect");
			if(numRects == 1){
				TextBounds tb;
				tb.gid = id;
				tb.rect = ofRectangle(xml.getAttribute("rect", "x", 0.),
									  xml.getAttribute("rect", "y", 0.),
									  xml.getAttribute("rect", "width", 0.),
									  xml.getAttribute("rect", "height", 0.));
				textBoxes.push_back(tb);
                //				cout << "Found rectangle " << tb.rect.x << " " << tb.rect.y << " " << tb.rect.width << " " << tb.rect.height << endl;
			}
			else{
				ofLogError("CloudsSDLayer::parse") << "Wrong number of rectangles in " << id << endl;
			}
			xml.popTag();
		}
		
	}
}

void CloudsSDLayer::start(){
	startTime = ofGetElapsedTimef();
	animating = true;
}

void CloudsSDLayer::update(){
	float time = ofGetElapsedTimef();
	if(animating && time - lastUpdateTime > maxUpdateInterval){
		float totalpercent = ofxTween::map(time,
										   delayTime+startTime,
										   delayTime+startTime+duration,
										   0.,1.0,true,ofxEasingExpo(),
										   ofxTween::easeOut );
		
		percentComplete.x = ofMap(totalpercent,startPercent.x,1.,0,1.0,true);
		percentComplete.y = ofMap(totalpercent,startPercent.y,1.,0,1.0,true);
        
		drawRect = ofRectangle(startPoint.x,startPoint.y,0,0);
		ofVec2f p;
		p.x = ofLerp(startPoint.x, endPoint.x, percentComplete.x);
		p.y = ofLerp(startPoint.y, endPoint.y, percentComplete.y);
		
		drawRect.growToInclude(p);
		animating = percentComplete.x != 1.0 || percentComplete.y != 1.0;
		lastUpdateTime = time;
	}
    
}

void CloudsSDLayer::draw(){
	
	ofPushStyle();
    //	ofSetColor(255, 0, 0, 10);
    //
    //	ofRect(drawRect);
	glEnable(GL_SCISSOR_TEST);
	//invert and flip
	float screenHeight = ofGetHeight();
	glScissor(drawRect.x, screenHeight - drawRect.y - drawRect.height,
			  drawRect.width, drawRect.height);
	
	svg.draw();
	glDisable(GL_SCISSOR_TEST);
    
	ofPopStyle();
    
}
