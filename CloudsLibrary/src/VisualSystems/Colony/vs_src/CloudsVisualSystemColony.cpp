#include "CloudsVisualSystemColony.h"
#define INV_2SQRT2 0.35355339059

string CloudsVisualSystemColony::getSystemName()
{
	return "Colony";
}

void CloudsVisualSystemColony::selfSetup()
{
    numInitialCells = 100;
    noiseShader.load("", getVisualSystemDataPath()+"shaders/liquidNoise.fs");
    vbo.setMode(OF_PRIMITIVE_POINTS);
    
	ofDisableArbTex();
    ofLoadImage(sprite, getVisualSystemDataPath() + "sprites/marker_dot.png");
    ofEnableArbTex();
    
    ofLoadImage(grunge, getVisualSystemDataPath() + "textures/dirt.jpg");
	loadShader();
    
}

void CloudsVisualSystemColony::loadShader(){
    string path = getVisualSystemDataPath() + "shaders/";
	levelSet.load(path + "levelSet.vs", path + "levelSet.fs");
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
    sysGui->addSlider("Initial Cells", 0, 1000, &numInitialCells);
    sysGui->addButton("Reset", &reset);
}

void CloudsVisualSystemColony::selfUpdate()
{
    //Video
    if ( !areFbosAllocatedAndSized() ){ reallocateFramebuffers(); }
    
    //Data
    //cout << "cells.size(): " << cells.size() << " FPS: " << ofGetFrameRate() << endl;
    pMap.clear();
    vbo.clear();
    pMap.put(cells);
    
    updateFoodTexture();
    
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
    
    
    //Main view
    fbo_main.begin();
    {
        ofPushStyle();
        ofEnablePointSprites();
        ofEnableAlphaBlending();
        
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        glDisable(GL_DEPTH_TEST);
        ofClear(0,0,0,0);
        
        billboard.begin();
        sprite.bind();
        vbo.draw();
        sprite.unbind();
        billboard.end();

        ofDisableBlendMode();
        ofDisablePointSprites();
        ofPopStyle();
    }
    fbo_main.end();
    
    
}

void CloudsVisualSystemColony::selfDrawBackground()
{
//    ofEnableBlendMode(OF_BLENDMODE_ADD);
//    grunge.bind();
//    grunge.draw(0,0);
//    grunge.unbind();
//    ofDisableBlendMode();

    
    ofEnableAlphaBlending();
    levelSet.begin();
    fbo_main.draw(0, 0, getSharedRenderTarget().getWidth(),
                  getSharedRenderTarget().getHeight());
    levelSet.end();
}

void CloudsVisualSystemColony::selfDraw(){

}


void CloudsVisualSystemColony::updateFoodTexture(){
    noiseShader.begin();
    noiseShader.setUniform1i("complexity", 1);
    noiseShader.setUniform1f("time", ofGetElapsedTimeMillis()/100.0);
    noiseShader.setUniform1f("zoom", 40.);
    noiseShader.setUniform2f("resolution", getSharedRenderTarget().getWidth(), getSharedRenderTarget().getHeight());
    ofRect(0, 0, getSharedRenderTarget().getWidth(),getSharedRenderTarget().getHeight());
    noiseShader.end();
}


void CloudsVisualSystemColony::selfBegin()
{
    populate();
}

void CloudsVisualSystemColony::selfEnd()
{
    clear();
    //TODO: Destroy everything in gCell;
}

void CloudsVisualSystemColony::selfExit(){
    clear();
    cellShader.unload();
    levelSet.unload();
}

void CloudsVisualSystemColony::selfPresetLoaded(string presetPath){
    clear();
    //TODO: use timeline->getCurrentTimeXX()
    populate();
}

void CloudsVisualSystemColony::clear(){
    for (int i = cells.size()-1; i >= 0; i--){
        cells.erase(cells.begin()+i);
    }
    cells.clear();
    vbo.clear();
}

void CloudsVisualSystemColony::populate(){
    for (int i = 0; i < (int) numInitialCells; i++) {
        cellPtr newCell = cellPtr(new colonyCell(ofPoint( ofRandomWidth(), ofRandomHeight(), i * 0.01), params));
        cells.push_back(newCell);
    }
}

bool CloudsVisualSystemColony::areFbosAllocatedAndSized(){
    return fbo_main.isAllocated()
    && foodTexture.isAllocated()
    && fbo_main.getWidth() == getSharedRenderTarget().getWidth()
    && fbo_main.getHeight() == getSharedRenderTarget().getHeight()
    && foodTexture.getWidth() == getSharedRenderTarget().getWidth()
    && foodTexture.getHeight() == getSharedRenderTarget().getHeight();
}

void CloudsVisualSystemColony::reallocateFramebuffers(){
    int w = getSharedRenderTarget().getWidth();
    int h = getSharedRenderTarget().getHeight();
    
    fbo_main.allocate(w,h,GL_RGBA);
    foodTexture.allocate(w/4., h/4., GL_RGB);
    
    fbo_main.begin();
    ofClear(0,0,0,0);
    fbo_main.end();
    
    foodTexture.begin();
    ofClear(0, 0, 0);
    foodTexture.end();
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