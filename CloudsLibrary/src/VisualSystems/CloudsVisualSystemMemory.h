//
//  CloudsVisualSystemMemory.h
//  VSMemory
//
//  Created by Patricio Gonzalez Vivo on 6/20/13.
//
//

#pragma once

#include "CloudsVisualSystemRezanator.h"

class Block : public ofRectangle {
public:
    ofFloatColor    color;
    char            value;
    bool            bSelected;
    
    void draw(){
        ofPushStyle();
        
        ofFill();
        ofSetColor(color);
        ofRect(*this);
        
        ofNoFill();
        if (bSelected){
            ofSetColor(0,70,200);
        } else {
            ofSetColor(100,200);
        }
        ofRect(*this);
        
//        ofSetColor(255);
//        ofDrawBitmapString( ofToString(value) , x+width*0.5-4,y+height*0.5+4);
        
        ofPopStyle();
    }
};


class CloudsVisualSystemMemory : public CloudsVisualSystemRezanator {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfAutoMode();
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    
    void draw(ofEventArgs & args);
    
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
    unsigned char * data;
    vector<Block> blocks;
    
    void    generateBlocks( unsigned char * _data, int _nElements );

    float   blockWidth, blockHeight, blockScale;
    float   margin;
    
    int     blocksTotal;
};