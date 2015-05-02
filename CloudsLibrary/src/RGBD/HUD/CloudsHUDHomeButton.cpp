//
//  CloudsHUDHomeButton.cpp
//  CloudsHUDDesigner
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsHUDHomeButton.h"
#include "CloudsGlobal.h"

CloudsHUDHomeButton::CloudsHUDHomeButton(){
    currentState = CLOUDS_HUD_HOVER_IDLE;
	maxHoverTime = 10.;
    targetFps = 24.;
    playhead = 0;
    hoverStartTime = ofGetElapsedTimef();
    bShowIdle = false;
    
    bIsHovering = false;
    bWasActivated = false;
}

void CloudsHUDHomeButton::setup(){
    loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/01_IntroHover/", rolloverPix[CLOUDS_HUD_HOVER_ROLLOVER] );
    loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/02a_IntroLoopBack/", rolloverPix[CLOUDS_HUD_HOVER_OUTRO]  );
    loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/02b_IntroToActive/", rolloverPix[CLOUDS_HUD_HOVER_ACTIVE_INTRO]  );
    loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/03_Active/", rolloverPix[CLOUDS_HUD_HOVER_ACTIVE_LOOP] );
    update();

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
    
    if( bIsHovering ){
        float elapsed = ofGetElapsedTimef() - hoverStartTime;
        float elapsedFrames = targetFps * elapsed;
        int lastPlayhead = playhead;
        
        playhead = elapsedFrames;
        
//        cout << "playhead :: " << floor(playhead) << " - " << (int)currentState << endl;
        
        if( currentState == CLOUDS_HUD_HOVER_ROLLOVER ){
            if( playhead >= rolloverPix[CLOUDS_HUD_HOVER_ROLLOVER].size() ){
                currentState = CLOUDS_HUD_HOVER_ACTIVE_INTRO;
                hoverStartTime = ofGetElapsedTimef();
                playhead = 0;
            }
        }
        else if( currentState == CLOUDS_HUD_HOVER_OUTRO ){
            
        }
        else if( currentState == CLOUDS_HUD_HOVER_ACTIVE_INTRO ){
            if( playhead >= rolloverPix[CLOUDS_HUD_HOVER_ACTIVE_INTRO].size() ){
                currentState = CLOUDS_HUD_HOVER_ACTIVE_LOOP;
                hoverStartTime = ofGetElapsedTimef();
                playhead = 0;
                //Hover activate disabled
//                bWasActivated = true;
            }
        }
        else if( currentState == CLOUDS_HUD_HOVER_ACTIVE_LOOP ){
            if( playhead >= rolloverPix[CLOUDS_HUD_HOVER_ACTIVE_LOOP].size() ){
                hoverStartTime = ofGetElapsedTimef();
            }
        }
        else{
            playhead = ofClamp( playhead, 0, rolloverPix[currentState].size()-1);
 
        }
        
        if(lastPlayhead != playhead){
            currentImage.setFromPixels(rolloverPix[currentState][floor(playhead)]);
            currentImage.update();
            
        }
    }
    else{
        currentImage.setFromPixels(rolloverPix[CLOUDS_HUD_HOVER_ROLLOVER][0]);
        currentImage.update();
    }
}

void CloudsHUDHomeButton::activate(){
    
    bWasActivated = true;
}

bool CloudsHUDHomeButton::wasActivated(){
    bool ret = bWasActivated;
    bWasActivated = false;
    return ret;
}

void CloudsHUDHomeButton::rollover(){
    currentState = CLOUDS_HUD_HOVER_ROLLOVER;
    hoverStartTime = ofGetElapsedTimef();
}

void CloudsHUDHomeButton::rollout(){
    currentState = CLOUDS_HUD_HOVER_IDLE;
    playhead = 0;
}

bool CloudsHUDHomeButton::hitTest(float xPos, float yPos){
//    cout << "Bounds are " << ofVec4f(bounds.x, bounds.y, bounds.width, bounds.height) << " scale is " << hudScale << " : test " << ofVec2f(xPos * hudScale,yPos * hudScale) << endl;
    return interactiveBounds.inside( xPos, yPos );
}

bool CloudsHUDHomeButton::hitTest(ofPoint mousePos){
    return hitTest( mousePos.x, mousePos.y );
}

void CloudsHUDHomeButton::draw(){
    if(currentImage.isAllocated()){
        currentImage.draw(bounds);
    }
}
