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
    
    buttonSize.set(200, 200);
    
    bIsHovering = false;
    bWasOpened = false;
}

void CloudsHUDHomeButton::setup(){
    homeLocation = ofVec2f(100 - 84, ofGetHeight()-300 + 60);
    
    rolloverTextures[CLOUDS_HUD_HOVER_ROLLOVER]     = loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/01_IntroHover/" );;
    rolloverTextures[CLOUDS_HUD_HOVER_OUTRO]        = loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/02a_IntroLoopBack/" );;
    rolloverTextures[CLOUDS_HUD_HOVER_ACTIVE_INTRO] = loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/02b_IntroToActive/" );;
    rolloverTextures[CLOUDS_HUD_HOVER_ACTIVE_LOOP]  = loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/03_Active/" );;
}

vector<ofTexture*> CloudsHUDHomeButton::loadFramesDir( string dirPath ){
    ofDirectory dir( dirPath );
	dir.allowExt("png");
	dir.listDir();
    
    cout << "Loading home button textures (" << dir.numFiles() << ") [";
    
    vector<ofTexture*>  textureList;
    
    float sc = 1.0;
    ofImage loader;
    for(int i = 0; i < dir.numFiles(); i++){
        cout << ".";
        
        textureList.push_back( new ofTexture() );
        loader.loadImage( dir.getPath(i) );
        sc = buttonSize.y / loader.getHeight();
        loader.resize( (int)loader.getWidth()*sc, (int)loader.getHeight()*sc );
        
//        loader.saveImage(GetCloudsDataPath() + "HUD/HOME_SM/04_LoopOut/" + dir.getName(i) );  // save out resized images
        
        textureList[textureList.size()-1]->allocate( loader.getWidth(), loader.getHeight(), GL_RGBA );
        textureList[textureList.size()-1]->loadData( loader.getPixels(), loader.getWidth(), loader.getHeight(), GL_RGBA );
    }
    cout << "]" << endl;
    
    return textureList;
}

void CloudsHUDHomeButton::update(){
    lastFrameTime = ofGetElapsedTimef();
    
	homeLocation = ofVec2f(100 - 84, ofGetHeight()-300 + 57);
//    hitBox.set( homeLocation, buttonSize.x, buttonSize.y );
    
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
        playhead = elapsedFrames;
        
//        cout << "playhead :: " << floor(playhead) << " - " << (int)currentState << endl;
        
        if( currentState == CLOUDS_HUD_HOVER_ROLLOVER ){
            if( playhead >= rolloverTextures[CLOUDS_HUD_HOVER_ROLLOVER].size() ){
                currentState = CLOUDS_HUD_HOVER_ACTIVE_INTRO;
                hoverStartTime = ofGetElapsedTimef();
                playhead = 0;
            }
        }else if( currentState == CLOUDS_HUD_HOVER_OUTRO ){
            
        }else if( currentState == CLOUDS_HUD_HOVER_ACTIVE_INTRO ){
            if( playhead >= rolloverTextures[CLOUDS_HUD_HOVER_ACTIVE_INTRO].size() ){
                currentState = CLOUDS_HUD_HOVER_ACTIVE_LOOP;
                hoverStartTime = ofGetElapsedTimef();
                playhead = 0;
                
                bWasOpened = true;
            }
        }else if( currentState == CLOUDS_HUD_HOVER_ACTIVE_LOOP ){
            if( playhead >= rolloverTextures[CLOUDS_HUD_HOVER_ACTIVE_LOOP].size() ){
                hoverStartTime = ofGetElapsedTimef();
            }
        }
        
        
//        playhead = (int)elapsedFrames;
//        
//        if( playhead > LOOP_END ){
//            float isolatedLoop = elapsedFrames - LOOP_START;
//            playhead = LOOP_START + ((int)isolatedLoop % (int)(LOOP_END - LOOP_START));
//        }
    }
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
    return bounds.inside( xPos, yPos );
}

bool CloudsHUDHomeButton::hitTest(ofPoint mousePos){
    return hitTest( mousePos.x, mousePos.y );
}

void CloudsHUDHomeButton::draw(){
    ofPushMatrix();{
        
//        ofTranslate( homeLocation );
        /*
        if( currentState == CLOUDS_HUD_HOVER_IDLE ){
            rolloverFrames[0]->draw(0,0);
        }else if( currentState == CLOUDS_HUD_HOVER_ROLLOVER ){
            playhead = ofClamp( playhead, 0, rolloverFrames.size()-1);
            rolloverFrames[floor(playhead)]->draw(0,0);
        }
        else if( currentState == CLOUDS_HUD_HOVER_OUTRO ){
            playhead = ofClamp( playhead, 0, rolloverOutFrames.size()-1);
            rolloverOutFrames[floor(playhead)]->draw(0,0);
        }
        else if( currentState == CLOUDS_HUD_HOVER_ACTIVE_INTRO ){
            playhead = ofClamp( playhead, 0, activeIntroFrames.size()-1);
            activeIntroFrames[floor(playhead)]->draw(0,0);
        }
        else if( currentState == CLOUDS_HUD_HOVER_ACTIVE_LOOP ){
            playhead = ofClamp( playhead, 0, activeLoopFrames.size()-1);
        }*/
        
        if( currentState == CLOUDS_HUD_HOVER_IDLE ){
            rolloverTextures[CLOUDS_HUD_HOVER_ROLLOVER][0]->draw( bounds );
        }else{
            playhead = ofClamp( playhead, 0, rolloverTextures[currentState].size()-1);
            rolloverTextures[currentState][floor(playhead)]->draw( bounds );
        }
        
        
	}ofPopMatrix();
}
