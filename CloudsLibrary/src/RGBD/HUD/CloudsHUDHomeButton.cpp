//
//  CloudsHUDHomeButton.cpp
//  CloudsHUDDesigner
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsHUDHomeButton.h"
#include "CloudsGlobal.h"
#include "ofxTween.h"

CloudsHUDHomeButton::CloudsHUDHomeButton(){
    currentState = CLOUDS_HOME_IDLE;
	//maxHoverTime = 10.;
    targetFps = 24.;
    playhead = 0;
    //hoverStartTime = ofGetElapsedTimef();
    bShowIdle = false;
    bleepAlpha = 0;
    bHasNextState = false;
    stateChangedTime = 0;
    
    animatedHoverStartTime = 0;
    bAnimateHoverRadar = true;
    bIsHovering = false;
    bWasActivated = false;
}

void CloudsHUDHomeButton::setup(){
    loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/00_Idle/", rolloverPix[CLOUDS_HOME_IDLE] );
    loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/01_IntroHover/", rolloverPix[CLOUDS_HOME_HOVER_ON] );
    loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/02a_IntroLoopBack/", rolloverPix[CLOUDS_HOME_HOVER_LOOP_BACK]  );
    loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/02b_IntroToActive/", rolloverPix[CLOUDS_HOME_ACTIVATING]  );
    loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/03_Active/", rolloverPix[CLOUDS_HOME_ACTIVE] );
    loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/04_LoopOut/", rolloverPix[CLOUDS_HOME_DEACTIVATING] );
    
    currentImage.setFromPixels(rolloverPix[currentState][0]);
    
    currentState = CLOUDS_HOME_IDLE;

    //changeState(CLOUDS_HOME_IDLE);
    
    //update();

}

void CloudsHUDHomeButton::loadFramesDir( string dirPath, vector<ofPixels>& pixels ){

    ofDirectory dir( dirPath );
	dir.allowExt("png");
	dir.listDir();
    
    pixels.resize(dir.numFiles());
    for(int i = 0; i < dir.numFiles(); i++){
        ofLoadImage(pixels[i], dir.getPath(i));
    }
}

void CloudsHUDHomeButton::update(){
    
    bool hit = hitTest( GetCloudsInputX(), GetCloudsInputY() );
    if( !bIsHovering && hit ){
        rollover();
    }else if( bIsHovering && !hit ){
        rollout();
    }
    bIsHovering = hit;
    
    float elapsedSeconds = ofGetElapsedTimef() - stateChangedTime;
    int elapsedFrames = targetFps * elapsedSeconds;
    int lastPlayhead = playhead;
    
    lastState = currentState;
    playhead = elapsedFrames;
    //cout << "playhead " << playhead << " / " << rolloverPix[currentState].size() << endl;
    //cout << "time since state change " << stateChangedTime << endl;
    if(elapsedFrames >= rolloverPix[currentState].size()){
        //generic next state
        if(bHasNextState){
            changeState(nextState);
        }
        //specific next states
        else if(currentState == CLOUDS_HOME_HOVER_ON){
            changeState(CLOUDS_HOME_HOVER_LOOP_BACK);
        }
        else if(currentState == CLOUDS_HOME_HOVER_LOOP_BACK){
            if(bIsHovering){
                changeState(CLOUDS_HOME_HOVER_ON);
            }
            else{
                changeState(CLOUDS_HOME_IDLE);
            }
        }
        else if(currentState == CLOUDS_HOME_ACTIVATING){
            changeState(CLOUDS_HOME_ACTIVE);
        }
        else if(currentState == CLOUDS_HOME_DEACTIVATING){
            if(bIsHovering){
                changeState(CLOUDS_HOME_HOVER_LOOP_BACK);
            }
            else{
                changeState(CLOUDS_HOME_IDLE);
            }
        }
        //loop
        else {
//            queueState(currentState);
            playhead = elapsedFrames % rolloverPix[currentState].size();
            stateChangedTime = ofGetLastFrameTime();
        }
    }
    
    //cout << currentState << ": ph " << playhead << " elapsed frames " << elapsedFrames << " / " << rolloverPix[currentState].size() << endl;
    
    if(lastState != currentState || lastPlayhead != playhead){
        currentImage.setFromPixels(rolloverPix[currentState][playhead]);
        currentImage.update();
    }
    

}

void CloudsHUDHomeButton::queueState(CloudsHUDHomeState newState){
//    cout << "QUEUE STATE " << newState << endl;

    bHasNextState = true;
    nextState = newState;
}

void CloudsHUDHomeButton::changeState(CloudsHUDHomeState newState){
//    cout << "CHANGE STATE " << newState << endl;
    bHasNextState = false;
    currentState = newState;
    stateChangedTime = ofGetElapsedTimef();
    playhead = 0;
}

void CloudsHUDHomeButton::activate(){
    if(currentState == CLOUDS_HOME_IDLE ||
       currentState == CLOUDS_HOME_HOVER_ON ||
       currentState == CLOUDS_HOME_HOVER_LOOP_BACK)
    {
        queueState(CLOUDS_HOME_ACTIVATING);
    }
    else{
        queueState(CLOUDS_HOME_DEACTIVATING);
    }
    bWasActivated = true;
}

bool CloudsHUDHomeButton::wasActivated(){
    bool ret = bWasActivated;
    bWasActivated = false;
    return ret;
}

void CloudsHUDHomeButton::rollover(){
    if(currentState == CLOUDS_HOME_IDLE){
        queueState(CLOUDS_HOME_HOVER_ON);
    }

}

void CloudsHUDHomeButton::rollout(){
//    if(currentState == CLOUDS_HOME_HOVER_LOOP_BACK || currentState == CLOUDS_HOME_HOVER_ON){
//        queueState(CLOUDS_HOME_IDLE);
//    }
}

bool CloudsHUDHomeButton::hitTest(float xPos, float yPos){
    return interactiveBounds.inside( xPos, yPos );
}

bool CloudsHUDHomeButton::hitTest(ofPoint mousePos){
    return hitTest( mousePos.x, mousePos.y );
}

void CloudsHUDHomeButton::draw(){
    if(bAnimateHoverRadar){
        float bleepTime = 2.0;
        float bleepPosition = fmod(ofGetElapsedTimef() - animatedHoverStartTime, bleepTime)/bleepTime;
        
        float bleepPercent = ofxTween::map(bleepPosition, 0, 1.0, 0, 1, true, ofxEasingQuad(), ofxTween::easeOut);
        float alpha = bleepPercent * ofxTween::map(bleepPercent, .9, 1.0, 1.0, 0.0, true, ofxEasingQuad(), ofxTween::easeIn);
        bleepExpand = bleepPercent;
        bleepAlpha = alpha;
    }
    else{
        bleepAlpha *= .9;
    }
    
    if(bleepAlpha > 0.01){
        ofPushStyle();
        ofSetCircleResolution(50);
        ofEnableAlphaBlending();
        if(currentState == CLOUDS_HOME_IDLE){
            ofNoFill();
            ofSetColor(CloudsColorHover, 255 * bleepAlpha * .49 );
            ofCircle(bounds.getCenter(), bounds.width * .25 * bleepExpand);
        }
        else{
            //don't grow the circle
            bleepExpand = 1.0;
        }
        
        ofFill();
        ofSetColor(CloudsColorActive, 255 * bleepAlpha * .37);
        ofCircle(bounds.getCenter(), bounds.width * .25  * bleepExpand);

        ofPopStyle();
    }
    
    if(currentImage.isAllocated()){
        currentImage.draw(bounds);
    }
    
}
