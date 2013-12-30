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
}

void CloudsHUDHomeButton::setup(){
    homeLocation = ofVec2f(100 - 84, ofGetHeight()-300 + 60);
    
    rolloverFrames      = loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/01_IntroHover/" );
    rolloverOutFrames   = loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/02a_IntroLoopBack/" );
    activeIntroFrames   = loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/02b_IntroToActive/" );
    activeLoopFrames    = loadFramesDir( GetCloudsDataPath() + "HUD/HOME_SM/03_Active/" );
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
	homeLocation = ofVec2f(100 - 84, ofGetHeight()-300 + 57);
    hitBox.set( homeLocation, buttonSize.x, buttonSize.y );
    
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
        
        if( currentState == CLOUDS_HUD_HOVER_ROLLOVER ){
            
        }else if( currentState == CLOUDS_HUD_HOVER_OUTRO ){
            
        }else if( currentState == CLOUDS_HUD_HOVER_ACTIVE_INTRO ){
            
        }else if( currentState == CLOUDS_HUD_HOVER_ACTIVE ){
            
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
    
    cout << "HUD ROLLOVER " << endl;
}

void CloudsHUDHomeButton::rollout(){
    currentState = CLOUDS_HUD_HOVER_IDLE;
    playhead = 0;
    
    cout << "HUD ROLLOUT" << endl;
}

bool CloudsHUDHomeButton::hitTest(float xPos, float yPos){
    return hitBox.inside( xPos, yPos );
}

bool CloudsHUDHomeButton::hitTest(ofPoint mousePos){
    return hitTest( mousePos.x, mousePos.y );
}

void CloudsHUDHomeButton::draw(){
    ofPushMatrix();{
        
        ofTranslate( homeLocation );
        
        if( currentState == CLOUDS_HUD_HOVER_IDLE ){
            rolloverFrames[0]->draw(0,0);
        }else if( currentState == CLOUDS_HUD_HOVER_ROLLOVER ){
            playhead = floor( ofClamp( playhead, 0, rolloverFrames.size()-1) );
            rolloverFrames[playhead]->draw(0,0);
        }
        else if( currentState == CLOUDS_HUD_HOVER_OUTRO ){
            playhead = floor( ofClamp( playhead, 0, rolloverOutFrames.size()-1) );
            rolloverOutFrames[playhead]->draw(0,0);
        }
        else if( currentState == CLOUDS_HUD_HOVER_ACTIVE_INTRO ){
            playhead = floor( ofClamp( playhead, 0, activeIntroFrames.size()-1) );
            activeIntroFrames[playhead]->draw(0,0);
        }
        else if( currentState == CLOUDS_HUD_HOVER_ACTIVE ){
            playhead = floor( ofClamp( playhead, 0, activeLoopFrames.size()-1) );
            activeLoopFrames[playhead]->draw(0,0);
        }
        
	}ofPopMatrix();
}
