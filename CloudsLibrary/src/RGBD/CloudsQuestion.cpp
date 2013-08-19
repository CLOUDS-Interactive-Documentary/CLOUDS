//
//  CloudsQuestion.cpp
//  CLOUDS
//
//  Created by James George on 6/20/13.
//
//

#include "CloudsQuestion.h"

CloudsQuestion::CloudsQuestion(){
	hovering = false;
	isSetup = false;
	radius = 10;
	cam = NULL;
	introQuestion = false;
	charsPerSecond = 20;
	//secondsToWriteQuestion = 2;
	secondsToConsiderSelected = 3;
	font = NULL;
}

void CloudsQuestion::setup(){
	if(!isSetup){
		ofRegisterMouseEvents(this);
		isSetup = true;
		

		geometry.addVertex(ofVec3f(-.5,-.5,0) * 4) ;
		geometry.addVertex(ofVec3f(.5,-.5,0)* 4);
		geometry.addVertex(ofVec3f(.5,.5,0)* 4);
		geometry.addVertex(ofVec3f(-.5,.5,0)* 4);
		geometry.setMode(OF_PRIMITIVE_LINE_LOOP);
	}
}

void CloudsQuestion::update(){
	
	ofVec3f screenPoint = cam->worldToScreen(position);
	currentScreenPoint = ofVec2f(screenPoint.x,screenPoint.y);
	
	ofVec3f screenPointTop = cam->worldToScreen(position + ofVec3f(0,radius,0));
	screenRadius = abs( screenPointTop.y - currentScreenPoint.y );
	
}

void CloudsQuestion::draw(){
	ofPushStyle();

	if(hovering){
		ofFill();
	}
	else{
		ofNoFill();
	}
	
	ofPushMatrix();
	ofTranslate(position);
	ofRotate(ofGetFrameNum()*4, 0, 1, 0);
//	ofScale(radius, radius);
	geometry.draw();
	//ofCircle(0,0,radius);

	ofPopMatrix();

	ofPopStyle();
}

void CloudsQuestion::startHovering(){
	if(!hovering){
		hovering = true;
		hoveringStartTime = ofGetElapsedTimef();
	}
}

void CloudsQuestion::stopHovering(){
	hovering = false;
}

bool CloudsQuestion::isSelected(){
	return hovering && ofGetElapsedTimef() - hoveringStartTime > secondsToConsiderSelected;
}
void CloudsQuestion::drawOverlay(){
	if(hovering){

		float secondsToWriteQuestion = question.size() / charsPerSecond;
		int charactersToType = ofMap(ofGetElapsedTimef() - hoveringStartTime, 0, secondsToWriteQuestion, 0, question.size(), true);

		string substring = question.substr(0, charactersToType);
//		cout << "hovering overlay should draw " << charactersToType << " of (" << question << ")" << endl;
		if(font != NULL){
			font->drawString(substring, currentScreenPoint.x, currentScreenPoint.y);
		}
		else{
			ofDrawBitmapString(substring, currentScreenPoint);
		}

//		ofVec3f currentScreenPoint3d(currentScreenPoint.x,currentScreenPoint.y,0);
		
		ofPushStyle();
		ofMesh progress;
		progress.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
		
		progress.addVertex(ofVec3f(currentScreenPoint.x,currentScreenPoint.y+5,0));
		progress.addVertex(ofVec3f(currentScreenPoint.x,currentScreenPoint.y+10,0));
		
		float width = font->stringWidth(question);
		float percentToSelection = ofMap(ofGetElapsedTimef() - hoveringStartTime, 0, secondsToConsiderSelected, 0, 1.0, true);
		
		progress.addVertex(ofVec3f(currentScreenPoint.x + width*percentToSelection + 5, currentScreenPoint.y+5,0));
		progress.addVertex(ofVec3f(currentScreenPoint.x + width*percentToSelection, currentScreenPoint.y+10,0));

		progress.addColor(ofFloatColor::white * .7);
		progress.addColor(ofFloatColor::white * .7);
		
		float oscatten = sin(ofGetElapsedTimef()*10)*.5+.5;
		ofFloatColor flash = ofFloatColor::white.getLerped(ofFloatColor::crimson, percentToSelection*oscatten);
		progress.addColor(flash);
		progress.addColor(flash);
		progress.draw();
		
		ofSetColor(ofFloatColor::crimson);
		ofSetLineWidth(3);
		ofLine(currentScreenPoint.x + width, currentScreenPoint.y+5,
			   currentScreenPoint.x + width, currentScreenPoint.y+10);
		
		ofPopStyle();
	}
}

void CloudsQuestion::mousePressed(ofMouseEventArgs& args){
		
}

void CloudsQuestion::mouseMoved(ofMouseEventArgs& args){
//	if(cam == NULL){
//		return;
//	}

//	hovering = currentScreenPoint.distance( ofVec2f(args.x,args.y) ) < screenRadius;
}

void CloudsQuestion::mouseReleased(ofMouseEventArgs& args){
	
}

void CloudsQuestion::mouseDragged(ofMouseEventArgs& args){
	
}
