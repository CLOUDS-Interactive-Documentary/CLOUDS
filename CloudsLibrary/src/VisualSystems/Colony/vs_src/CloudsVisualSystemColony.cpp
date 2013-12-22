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
    numInitialCells = 200; //FIXME : Magic number
    //    noiseShader.load("", getVisualSystemDataPath()+"shaders/liquidNoise.fs");
    vbo.setMode(OF_PRIMITIVE_LINES);
    
    string path = getVisualSystemDataPath()+"shaders/";
    cellShader.setGeometryOutputCount(45); //FIXME: Debug
    cellShader.setGeometryInputType(GL_LINES);
//    balls.load(path + "balls.vert", path + "balls.frag", path + "balls.geom");
    levelSet.load(path + "levelSet.vs", path + "levelSet.fs");
    cellShader.load(path + "cells.vs", path+"cells.fs", path+"cells.gs");
    cellShader.setGeometryOutputType(GL_TRIANGLES);

}

void CloudsVisualSystemColony::selfSetupSystemGui()
{
    sysGui->addSlider("Separate",0.0,100, &params.amtSeparate);
    sysGui->addSlider("Cohere",0.0,100, &params.amtCohere);
    sysGui->addSlider("Align",0.0,100, &params.amtAlign);
    sysGui->addSlider("Friction",0.0,1.0, &params.dynamicFrictionCoeff);
    sysGui->addSlider("Turbulence Amount",0.0,100.0, &params.amtTurbulence);
    sysGui->addSlider("Turbulence Speed",0.0,100.0, &params.spdTurbulence);
    sysGui->addSlider("Fertility Rate", 0.0, 1.0, &params.fertilityRate);
    sysGui->addRangeSlider("Lifespan Range", 5, 5000, &params.lifespanMin, &params.lifespanMax);
    
    sysGui->addSpacer("Immutables");
    sysGui->addButton("Reset", &reset);
}

void CloudsVisualSystemColony::selfUpdate()
{
    cout << "cells.size(): " << cells.size() << " FPS: " << ofGetFrameRate() << endl;
	
    pMap.clear();
    vbo.clear();
    pMap.put(cells);
    for (int i = 0; i < cells.size(); i++) {
        
        neighbor_iterator iter = pMap.getNeighbours(coord2i(cells[i]->getPosition()));
        
        cells[i]->doResetForces(); //TODO: Decide where all the operations should be. This isn't a good place.
        cells[i]->doScanAndFlock(iter);
        cells[i]->doAddTurbulence();
        cells[i]->doFeedCellNoise();
        
        cells[i]->update();
        
        if (cells[i] -> isFertile() && cells[i]->isReadyToReplicate()){
            cellPtr newCell = cells[i]->doGetReplicated();
            cells.push_back(newCell);
        }
        if (cells[i]->isDead()){
            cells.erase(cells.begin() + i);
        }
        vbo.addColor(ofColor(255, 255, 255, 100));
        vbo.addColor(ofColor(255, 255, 255, 100));
        vbo.addVertex(cells[i]->getPosition());
        //FIXME: This might reverse the angle
        vbo.addVertex(ofVec2f(cells[i]->getSize(),(cells[i]->getVelocity()).angleRad(ofVec2f(1,0))));
    }
}

void CloudsVisualSystemColony::selfDrawBackground()
{
	
    ofPushStyle();
	ofEnableAlphaBlending();
    
    cellShader.begin();
    cellShader.setUniform2f("screenResolution", ofGetWidth(), ofGetHeight());
    vbo.draw();
    cellShader.end();

	ofPopStyle();
}

void CloudsVisualSystemColony::selfDraw(){

}

//void CloudsVisualSystemColony::selfPostDraw(){
//    levelSet.begin();
//    levelSet.setUniformTexture("tex", getSharedRenderTarget().getTextureReference(),0);
//    getSharedRenderTarget().draw(0, 0,
//                                 getSharedRenderTarget().getWidth(),
//                                 getSharedRenderTarget().getHeight()
//                                 );
//    levelSet.end();
//}

void CloudsVisualSystemColony::selfBegin()
{
    for (int i = 0; i < numInitialCells; i++) {
        cellPtr newCell = cellPtr(new colonyCell(ofPoint( ofRandomWidth(), ofRandomHeight(), i * 0.01), params));
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

void CloudsVisualSystemColony::selfExit(){
    cellShader.unload();
    levelSet.unload();
    vbo.clear();
    //TODO: Destroy everything in gCell;
}


void CloudsVisualSystemColony::selfSetupGuis(){}
void CloudsVisualSystemColony::selfAutoMode(){}
void CloudsVisualSystemColony::selfSetupRenderGui(){}
void CloudsVisualSystemColony::guiSystemEvent(ofxUIEventArgs &e){}
void CloudsVisualSystemColony::guiRenderEvent(ofxUIEventArgs &e){}
void CloudsVisualSystemColony::selfKeyPressed(ofKeyEventArgs & args){}
void CloudsVisualSystemColony::selfDrawDebug(){}
void CloudsVisualSystemColony::selfSceneTransformation(){}
void CloudsVisualSystemColony::selfKeyReleased(ofKeyEventArgs & args){}
void CloudsVisualSystemColony::mouseDragged(ofMouseEventArgs& data){}
void CloudsVisualSystemColony::mouseMoved(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::mousePressed(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::mouseReleased(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::selfSetupGui(){}
void CloudsVisualSystemColony::selfGuiEvent(ofxUIEventArgs &e){}