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
    IDLE_FRAME  = 15;
    LOOP_START  = 64;
    LOOP_END    = 295;
    
	maxHoverTime = 10.;
    targetButtonHeight = 200.;
    targetFps = 24.;
    
    playhead = IDLE_FRAME;
    hoverStartTime = ofGetElapsedTimef();
    
    bIsHovering = false;
}

void CloudsHUDHomeButton::setup(){
    homeLocation = ofVec2f(100 - 84, ofGetHeight()-300 + 60);
    
//    ofDirectory dir(GetCloudsDataPath() + "HUD/HOME/");
    ofDirectory dir(GetCloudsDataPath() + "HUD/HOME_SM/");  // load small images for quicker debugging
	dir.allowExt("png");
	dir.listDir();
    
    cout << "Loading home button textures (" << dir.numFiles() << ") [";
    
    float sc = 1.0;
    
    ofImage loader;
    for(int i = 0; i < dir.numFiles(); i++){
        cout << ".";
        
        textureList.push_back( new ofTexture() );
        loader.loadImage( dir.getPath(i) );
        sc = targetButtonHeight / loader.getWidth();
        loader.resize( (int)loader.getWidth()*sc, (int)loader.getHeight()*sc );
        
//        loader.saveImage(GetCloudsDataPath() + "HUD/HOME_SM/" + dir.getName(i) );
        
        textureList[textureList.size()-1]->allocate( loader.getWidth(), loader.getHeight(), GL_RGBA );
        textureList[textureList.size()-1]->loadData( loader.getPixels(), loader.getWidth(), loader.getHeight(), GL_RGBA );
    }
    cout << "]" << endl;
    
    if( textureList.size() ){
        hitBox.set( homeLocation, textureList[0]->getWidth(), textureList[0]->getHeight() );
    }
}

void CloudsHUDHomeButton::update(){
	homeLocation = ofVec2f(100 - 84, ofGetHeight()-300 + 57);
    hitBox.set( homeLocation, textureList[0]->getWidth(), textureList[0]->getHeight() );
    
    bool hit = hitTest( GetCloudsInputX(), GetCloudsInputY() );
    
    if( !bIsHovering && hit ){
        hoverStartTime = ofGetElapsedTimef();
    }
    bIsHovering = hit;
    
    if( bIsHovering ){
        float elapsed = ofGetElapsedTimef() - hoverStartTime;
        float elapsedFrames = targetFps * elapsed;
        
        playhead = (int)elapsedFrames;
        
        if( playhead > LOOP_END ){
            float isolatedLoop = elapsedFrames - LOOP_START;
            playhead = LOOP_START + ((int)isolatedLoop % (int)(LOOP_END - LOOP_START));
        }
    }else{
        playhead = IDLE_FRAME;
    }
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
        
        textureList[floor(playhead)]->draw(0,0);
	}ofPopMatrix();
}
