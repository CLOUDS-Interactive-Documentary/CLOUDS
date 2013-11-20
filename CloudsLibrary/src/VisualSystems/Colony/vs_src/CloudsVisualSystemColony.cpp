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
    numInitialCells = 50; //FIXME : Magic number
    //    noiseShader.load("", getVisualSystemDataPath()+"shaders/liquidNoise.fs");
}
void CloudsVisualSystemColony::selfSetupSystemGui()
{
    //    sysGui->addSlider("Separate",0.0,200, &flockSeparate);
    //    sysGui->addSlider("Cohere",0.0,200, &flockCohere);
    //    sysGui->addSlider("Align",0.0,200, &flockAlign);
}
void CloudsVisualSystemColony::selfSetupRenderGui(){}
void CloudsVisualSystemColony::guiSystemEvent(ofxUIEventArgs &e){}
void CloudsVisualSystemColony::guiRenderEvent(ofxUIEventArgs &e){}
void CloudsVisualSystemColony::selfKeyPressed(ofKeyEventArgs & args){}
void CloudsVisualSystemColony::selfUpdate()
{
    cout << "cells.size(): " << cells.size() << " FPS: " << ofGetFrameRate() << endl;
    pMap.clear();
    
    pMap.put(cells);
    for (int i = 0; i < cells.size(); i++) {
        neighbor_iterator iter = pMap.getNeighbours(coord2i(cells[i]->getPosition()));
        cells[i]->doScanAndFlock(iter);
        cells[i]->doFeedCellNoise();
        cells[i]->update();
//        cells[i]->doApplyBorders(10);
        //        cells[i]->doWrapXY();
        
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
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    for (int i = 0 ; i < cells.size(); i++){
        cells[i]->draw();
    }
}
void CloudsVisualSystemColony::selfDraw(){
    //    ofEnableSmoothing();
    //    ofEnableAlphaBlending();
    //    for (int i = 0 ; i < cells.size(); i++){
    //        cells[i]->draw();
    //    }
    
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