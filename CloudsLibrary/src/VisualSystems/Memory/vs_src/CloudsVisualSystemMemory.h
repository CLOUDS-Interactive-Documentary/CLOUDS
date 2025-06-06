//
//  CloudsVisualSystemMemory.h
//  VSMemory
//
//  Created by Patricio Gonzalez Vivo on 6/20/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "ofxTonic.h"
#include "CloudsAudioEvents.h"

//using namespace Tonic;

class Block : public ofRectangle {
  public:
    ofFloatColor    color,borderColor;
	float			borderBase;
    int             value;
    float           border;
    bool            bSelected;
    bool			bActivated; //mouse interactivity
	ofIntRange outlineIndices;
	ofIntRange fillIndices;
	ofVboMesh* outlineMesh;
	ofVboMesh* fillMesh;
	
    bool operator > (Block &b){
		return this->value > b.value;
	}
    bool operator < (Block &b){
		return this->value < b.value;
	}
    
	void setup(){
		bActivated = false;
		bSelected = false;
		outlineIndices.min = outlineMesh->getNumVertices();
		
		ofVec3f a = ofVec3f(getMinX(),getMinY(),0);
		ofVec3f b = ofVec3f(getMaxX(),getMinY(),0);
		ofVec3f c = ofVec3f(getMaxX(),getMaxY(),0);
		ofVec3f d = ofVec3f(getMinX(),getMaxY(),0);
		
		outlineMesh->addVertex(a);
		outlineMesh->addVertex(b);
		
		outlineMesh->addVertex(b);
		outlineMesh->addVertex(c);

		outlineMesh->addVertex(c);
		outlineMesh->addVertex(d);
		
		outlineMesh->addVertex(d);
		outlineMesh->addVertex(a);
		
		outlineIndices.max = outlineMesh->getNumVertices();
		
		for(int i = 0; i < 8; i++){
			outlineMesh->addColor(ofFloatColor());
		}
		
		fillIndices.min = fillMesh->getNumVertices();
		
		fillMesh->addVertex(a);
		fillMesh->addVertex(b);
		fillMesh->addVertex(d);
		
		fillMesh->addVertex(b);
		fillMesh->addVertex(d);
		fillMesh->addVertex(c);
		
		fillIndices.max = fillMesh->getNumVertices();
		
		for(int i = 0; i < 6; i++){
			fillMesh->addColor(color);
		}
	}
		
	void update(){
		ofFloatColor curStroke;
		if(bActivated){
			curStroke = ofFloatColor(1.0, 1.0, 1.0, borderColor.a);
		}
		else if (bSelected){
            curStroke = borderColor;
        } else {
            curStroke = ofFloatColor(borderBase,borderColor.a);
        }

		for(int i = outlineIndices.min; i < outlineIndices.max; i++) {
			outlineMesh->setColor(i, curStroke);
		}
		for(int i = fillIndices.min; i < fillIndices.max; i++){
			fillMesh->setColor(i, color);
		}
			

	}

};


class CloudsVisualSystemMemory : public CloudsVisualSystem {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
	void selfSetDefaults();
	
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    
    void selfExit();
    void selfBegin();
	void selfEnd();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs & args);
    void selfMouseMoved(ofMouseEventArgs & args);
    void selfMousePressed(ofMouseEventArgs & args);
    void selfMouseReleased(ofMouseEventArgs & args);
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    
private:
    vector<Block> blocks;
	
	ofVboMesh outlineMesh;
	ofVboMesh fillMesh;

    void    generate();
    void    generateFromMemory();
    void    generateFromTexture(ofTexture &_tex);
    
    void    unSelectAll();
    void    swapBlocks(int _indexA, int _indexB, bool _colored = true);
    
    void    applySort();
    void    applyDeFrag();
    void    applyRandomSort();
    void    applyBiDirectionalSort();
    
    void    applyRandomMix();
    void    applyRandomUp();
    void    applyRandomDown();
    
	ofRange baseColorRange;
    ofFloatColor borderColor;
	float borderBase;

    float   margin;
    float   blockWidth, blockHeight, blockScale;
    float   randomSort, randomMix, randomUp, randomDown;
//    float   brightnessOffset;
    float   noiseLerp;
    
    int     xBlocks;
    int     yBlocks;
    
    bool    bSort;
    bool    bDeFrag;
    bool    bBiDirectionalSort;
    bool    bTexture;
    
    // Sound
    float fMainGain;
    Tonic::ControlParameter mainGain;
    Tonic::ofxTonicSynth synth;
    Tonic::Generator buildSynth();
	void audioRequested(ofAudioEventArgs& args);
    
};