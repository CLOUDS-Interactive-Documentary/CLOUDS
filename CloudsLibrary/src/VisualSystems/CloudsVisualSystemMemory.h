//
//  CloudsVisualSystemMemory.h
//  VSMemory
//
//  Created by Patricio Gonzalez Vivo on 6/20/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"

class Block : public ofRectangle {
public:
    ofFloatColor    color,borderColor;
    int             value;
    float           border;
    bool            bSelected;
    
    bool operator > (Block &b){ return this->value > b.value; }    
    bool operator < (Block &b){ return this->value < b.value; }
    
    void draw(){
        ofPushStyle();
        
        ofFill();
        ofSetColor(color);
        ofRect(*this);
        
        ofNoFill();
        if (bSelected){
            ofSetColor(borderColor);
        } else {
            ofSetColor(ofFloatColor(0.5,borderColor.a));
        }
        ofRect(*this);
        
//        ofSetColor(255);
//        ofDrawBitmapString( ofToString(value) , x+width*0.5-4,y+height*0.5+4);
        
        ofPopStyle();
    }
};


class CloudsVisualSystemMemory : public CloudsVisualSystem {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfAutoMode();
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    
    void selfExit();
    void selfBegin();
	void selfEnd();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void mouseDragged(ofMouseEventArgs & args);
    void mouseMoved(ofMouseEventArgs & args);
    void mousePressed(ofMouseEventArgs & args);
    void mouseReleased(ofMouseEventArgs & args);
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
private:
    vector<Block> blocks;
    
    void    generate();
    void    generateFromMemory();
    void    generateFromTexture(ofTexture &_tex);
    
    void    unSelectAll();
    void    swapBlocks(int _indexA, int _indexB);
    
    void    applySort();
    void    applyDeFrag();
    void    applyRandomSort();
    void    applyBiDirectionalSort();
    
    void    applyRandomMix();
    void    applyRandomUp();
    void    applyRandomDown();
    
    ofFloatColor borderColor;
    float   margin;
    float   blockWidth, blockHeight, blockScale;
    float   randomSort, randomMix, randomUp, randomDown;
    float   noiseLerp;
    
    int     xBlocks;
    int     yBlocks;
    
    bool    bSort;
    bool    bDeFrag;
    bool    bBiDirectionalSort;
    bool    bTexture;
    
};