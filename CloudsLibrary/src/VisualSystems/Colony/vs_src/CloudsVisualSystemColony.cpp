//
//  CloudsVisualSystemColony.cpp
//  VSColony
//
//  Created by Patricio Gonzalez Vivo on 6/26/13.
//
//

#include "CloudsVisualSystemColony.h"

string CloudsVisualSystemColony::getSystemName()
{
	return "Colony";
}

void CloudsVisualSystemColony::selfSetup()
{
    
    numInitialCells = 5; //FIXME : Magic number
    
    noiseShader.load("", getVisualSystemDataPath()+"shaders/liquidNoise.fs");
    foodFbo.allocate(ofGetWidth(), ofGetHeight());
    noiseZoom = 100.0;
    
    
    //TODO : Review
	if(!noise.isAllocated() || noise.getWidth() != ofGetWidth() || noise.getHeight() != ofGetHeight()){
		noise.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_GRAYSCALE);
		
		for(int x = 0; x < noise.getWidth(); x++){
			for(int y = 0; y < noise.getHeight(); y++){
				noise.setColor(x,y,ofColor(ofNoise(x/200.,y/200.0) * 255, 255) );
			}
		}
		noise.reloadTexture();
	}
}

void CloudsVisualSystemColony::selfSetupSystemGui()
{
    sysGui->addSlider("noiseZoom", 0.0, 1000, &noiseZoom);
  //  sysGui->addSlider("NutrientLevel", 0.0, 1000, &nutrientLevel);
}

void CloudsVisualSystemColony::selfSetupRenderGui(){}
void CloudsVisualSystemColony::guiSystemEvent(ofxUIEventArgs &e){}
void CloudsVisualSystemColony::guiRenderEvent(ofxUIEventArgs &e){}
void CloudsVisualSystemColony::selfKeyPressed(ofKeyEventArgs & args){}

void CloudsVisualSystemColony::selfUpdate()
{
    cout << "cells.size(): " << cells.size() << " FPS: " << ofGetFrameRate() << endl;
    pMap.clear();
    
    int width = foodFbo.getWidth();
    int height = foodFbo.getHeight();
    

    
    //  Update cells position and life
    //
    pMap.put(cells.begin(), cells.end());
    for (int i = 0; i < cells.size(); i++) {
        neighbor_iterator iter = pMap.getNeighbours(coord2i(cells[i]->getPosition()));
        iter.initialize();
            cells[i]->doApplyFlock(iter);
            cells[i]->doApplyBorders();
            cells[i]->doFeedCellWidth( noise.getPixelsRef() );
            cells[i]->update();
        
            if (cells[i] -> isFertile() && cells[i]->isReadyToReplicate()){
                cellPtr newCell = cells[i]->doGetReplicated();
                cells.push_back(newCell);
            }
            if (cells[i]->isDead()){
                cells.erase(cells.begin() + i);
            }
        }
}

void CloudsVisualSystemColony::selfSetupGuis(){}
void CloudsVisualSystemColony::selfAutoMode(){}

void CloudsVisualSystemColony::selfDrawBackground()
{
//    foodFbo.draw(0, 0);
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    for (int i = 0 ; i < cells.size(); i++){
        cells[i]->draw();
    }
    

}


void CloudsVisualSystemColony::selfDrawDebug(){}
void CloudsVisualSystemColony::selfSceneTransformation(){}
void CloudsVisualSystemColony::selfExit(){}


void CloudsVisualSystemColony::selfBegin()
{
    for (int i = 0; i < numInitialCells; i++) {
        cellPtr newCell = cellPtr(new colonyCell(ofPoint( ofRandomWidth(), ofRandomHeight() )));
        cells.push_back(newCell);
    }
}

void CloudsVisualSystemColony::selfEnd()
{
    for (int i = cells.size()-1; i >= 0; i--){
        cells.erase(cells.begin()+i);
    }
    cells.clear();
}

void CloudsVisualSystemColony::selfKeyReleased(ofKeyEventArgs & args){}
void CloudsVisualSystemColony::mouseDragged(ofMouseEventArgs& data){}
void CloudsVisualSystemColony::mouseMoved(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::mousePressed(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::mouseReleased(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::selfSetupGui(){}
void CloudsVisualSystemColony::selfGuiEvent(ofxUIEventArgs &e){}