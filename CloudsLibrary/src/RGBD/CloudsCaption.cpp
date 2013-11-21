#include "CloudsCaption.h"
#include "CloudsGlobal.h"

CloudsCaption::CloudsCaption(){
	
    isEnabled = false;
	isPlaying = false;
	charsPerSecond = 45;
    lifeDuration = 3000;
	deathDuration = 300;
    
	font = NULL;
	
	expandPercent = .1;
	selectPercent = 0;
#ifdef OCULUS_RIFT
	enlarge = .2;
#else
	enlarge = 0;
#endif
	
}

void CloudsCaption::begin(){
    cout << "BEGIN BEGIN BEGIN " << isEnabled << endl;
    if (isEnabled) {
        isPlaying = true;
        caption = ofToUpper(caption);
        birthTime = ofGetElapsedTimeMillis();
        deathTime = birthTime + lifeDuration + deathDuration;
    }
}

void CloudsCaption::update(){

}

void CloudsCaption::draw(){
//	ofPushStyle();
//    
//	if(hovering){
//		ofFill();
//	}
//	else{
//		ofNoFill();
//	}
//    
//	if(hovering){
//		ofxEasingCubic cub;
//		expandPercent = ofxTween::map(ofGetElapsedTimef() - hoveringStartTime, 0, secondsToConsiderSelected*.3, .2, 1.0, true, cub, ofxTween::easeOut);
//		selectPercent = ofxTween::map(ofGetElapsedTimef() - hoveringStartTime, 0, secondsToConsiderSelected, 0, 1.0, true, cub, ofxTween::easeOut);
//	}
//	else{
//		expandPercent += (.1 - expandPercent) * .2;
//		selectPercent += ( 0 - selectPercent) * .4;
//	}
//	
//	//make it blow up and fade out really quickly.
//	if(isDestroyed){
//		ofxEasingCubic cub;
//		expandPercent = ofxTween::map(ofGetElapsedTimef(), destroyedStartFadeTime, destroyFadeoutTime, .0, 1.0, true, cub, ofxTween::easeOut);
//        //		cout << "expand percent " << expandPercent << endl;
//	}
//	
//    //	cout << "expand percent " << expandPercent << " radius " << radius << endl;
//	
//	CloudsCaption::shader.setUniform1f("expandPercent", expandPercent+enlarge);
//	CloudsCaption::shader.setUniform1f("maxExpand", radius);
//	CloudsCaption::shader.setUniform1f("selectPercent", selectPercent);
//	CloudsCaption::shader.setUniform1f("destroyedAttenuate", isDestroyed ? 1.0 - expandPercent : 1.0);
//	
//	ofPushMatrix();
//	ofTranslate(position);
//	ofNode n;
//	n.setPosition(position);
//	n.lookAt(cam->getPosition());
//	
//	currentRot.slerp(.1, currentRot, n.getOrientationQuat());
//	
//	ofVec3f axis;
//	float angle;
//	currentRot.getRotate(angle, axis);
//	ofVec3f dial = n.getLookAtDir();
//	ofRotate(-ofGetElapsedTimef()*100,dial.x,dial.y,dial.z);
//	ofRotate(angle,axis.x,axis.y,axis.z);
//	
//    //	ofScale(radius*expandPercent, radius*expandPercent, radius*expandPercent);
//	dottedCircle.draw();
//	if(!isDestroyed){
//		progressRing.draw();
//	}
//	
//	ofPopMatrix();
//    
//	ofPopStyle();
}

void CloudsCaption::drawOverlay(){
    if (!isEnabled || !isPlaying) return;
    
    glDisable(GL_DEPTH_TEST);
    ofPushStyle();
    
    float width = font->stringWidth(caption);
    //ofVec2f screenPosition(ofGetWidth()/2 - width/2, ofGetHeight() * .66);
#ifdef OCULUS_RIFT
    // TODO: Use vars for screen position
    ofVec2f screenPosition = ofVec2f(20, 200);
#else
    ofVec2f screenPosition = ofVec2f(20, ofGetHeight() - 40);
#endif
    
    //DRAW BACKBOX
    //		ofPushStyle();
    //		ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
    //		ofSetColor(255,20);
    //		ofRect(screenPosition.x, screenPosition.y-25, width+40, 50);
    //		ofPopStyle();
    
    float secondsToWriteCaption = caption.size() / charsPerSecond;
    int charactersToType = ofMap(ofGetElapsedTimef() - birthTime, 0, secondsToWriteCaption, 0, caption.size(), true);
    string substring = caption.substr(0, charactersToType);
    //		if(font != NULL){
    //			ofPushStyle();
    //			ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
    //			ofSetColor(50);
    //			font->drawString(substring, screenPosition.x+12, screenPosition.y+2);
    //			ofSetColor(255);
    //			ofEnableBlendMode(OF_BLENDMODE_ADD);
    float alpha = ofMap(ofGetElapsedTimeMillis(), deathTime - deathDuration, deathTime, 255, 0, true);
    cout << alpha << endl;
//    ofSetColor(255, alpha);
    ofSetColor(255);
    font->drawString(substring, screenPosition.x+10, screenPosition.y);
    font->drawString(substring, screenPosition.x+10, screenPosition.y);
    //			ofPopStyle();
    //		}
    //		else{
    //			ofDrawBitmapString(substring, screenPosition);
    //		}
    
    
    ofPopStyle();
    glEnable(GL_DEPTH_TEST);
}

