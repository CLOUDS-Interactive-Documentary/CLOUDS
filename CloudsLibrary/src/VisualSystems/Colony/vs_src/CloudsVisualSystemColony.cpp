#include "CloudsVisualSystemColony.h"
#define INV_2SQRT2 0.35355339059

string CloudsVisualSystemColony::getSystemName()
{
	return "Colony";
}

void CloudsVisualSystemColony::selfSetup()
{
    numInitialCells = 100; //FIXME : Magic number
    //    noiseShader.load("", getVisualSystemDataPath()+"shaders/liquidNoise.fs");
    vbo.setMode(OF_PRIMITIVE_POINTS);
    
	ofDisableArbTex();
    ofLoadImage(sprite, getVisualSystemDataPath() + "sprites/marker_dot.png");
    ofEnableArbTex();
    
	loadShader();

}

void CloudsVisualSystemColony::loadShader(){
    string path = getVisualSystemDataPath() + "shaders/";
	levelSet.load(path + "levelSet.vs", path + "levelSet.fs");
//    cellShader.load(path + "cells.vs", path + "cells.fs"); //not using the gs
    billboard.load(path + "billboard.vs", path + "billboard.fs");
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
    
    sysGui->addSlider("Nutrient Amount", 150, 500, &params.nutrientAmount);
    sysGui->addSlider("Nutrient Change Ratio", 0, 500, &params.nutrientTimeCoef);
    sysGui->addSlider("Nutrient Contrast", 0, 4.0, &params.nutrientFalloff);
    
    sysGui->addRangeSlider("Max Speed", 0.0, 10.0, &params.maxSpeed_min, &params.maxSpeed_max);
    sysGui->addRangeSlider("Max Size", 0.0, 30.0, &params.maxSize_min, &params.maxSize_max);
    
    sysGui->addSpacer("Immutables");
    sysGui->addButton("Reset", &reset);
}

void CloudsVisualSystemColony::selfUpdate()
{
//    //Video
//    if(!fbo.isAllocated() ||
//       fbo.getWidth() != getSharedRenderTarget().getWidth() ||
//       fbo.getHeight() != getSharedRenderTarget().getHeight())
//    {
//        reallocateFramebuffers();
//    }
//    
    //Data
    //cout << "cells.size(): " << cells.size() << " FPS: " << ofGetFrameRate() << endl;
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
        vbo.addVertex(cells[i]->getPosition());
        //HACK: Normal gives more data to the shader.
        vbo.addNormal(ofVec3f(cells[i]->getSize(),0.,0.));
    }
    
}

void CloudsVisualSystemColony::selfDrawBackground()
{
//    fbo.begin();
//    ofClear(0,0,0,0);
    ofPushStyle();
	ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    glDisable(GL_DEPTH_TEST);
    ofEnablePointSprites();
    

    billboard.begin();
    sprite.bind();
    
    vbo.draw();
    
    sprite.unbind();
    billboard.end();
    
    ofDisablePointSprites();
    ofDisableBlendMode();
	ofPopStyle();
	
//    fbo.end();
}

void CloudsVisualSystemColony::selfDraw(){

}

void CloudsVisualSystemColony::selfPostDraw(){

    levelSet.begin();
//    fbo.draw(0, 0,
//			 getSharedRenderTarget().getWidth(),
//             getSharedRenderTarget().getHeight());
	CloudsVisualSystem::selfPostDraw();
    levelSet.end();
}

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

//JG use main render target
void CloudsVisualSystemColony::reallocateFramebuffers(){
//    fbo.allocate(getSharedRenderTarget().getWidth(),
//                       getSharedRenderTarget().getHeight(),
//                       GL_RGBA);
//    fbo.begin();
//    ofClear(0,0,0,0);
//    fbo.end();
}

void CloudsVisualSystemColony::selfSetupGuis(){}
void CloudsVisualSystemColony::selfAutoMode(){}
void CloudsVisualSystemColony::selfSetupRenderGui(){}
void CloudsVisualSystemColony::guiSystemEvent(ofxUIEventArgs &e){}
void CloudsVisualSystemColony::guiRenderEvent(ofxUIEventArgs &e){}

void CloudsVisualSystemColony::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		loadShader();
	}
}

void CloudsVisualSystemColony::selfDrawDebug(){}
void CloudsVisualSystemColony::selfSceneTransformation(){}
void CloudsVisualSystemColony::selfKeyReleased(ofKeyEventArgs & args){}
void CloudsVisualSystemColony::mouseDragged(ofMouseEventArgs& data){}
void CloudsVisualSystemColony::mouseMoved(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::mousePressed(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::mouseReleased(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::selfSetupGui(){}
void CloudsVisualSystemColony::selfGuiEvent(ofxUIEventArgs &e){}