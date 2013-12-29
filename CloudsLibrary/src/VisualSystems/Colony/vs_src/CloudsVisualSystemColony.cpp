#include "CloudsVisualSystemColony.h"
#define INV_2SQRT2 0.35355339059

string CloudsVisualSystemColony::getSystemName()
{
	return "Colony";
}

void CloudsVisualSystemColony::selfSetup()
{
    numInitialCells = 100;
    kernel_maxValue = 0.8;
    vbo.setMode(OF_PRIMITIVE_POINTS);
    
	ofDisableArbTex();
    ofLoadImage(sprite, getVisualSystemDataPath() + "sprites/marker_dot.png");
    ofEnableArbTex();
    
    grunge.setCompression(OF_COMPRESS_ARB);
    ofLoadImage(grunge, getVisualSystemDataPath() + "textures/dirt_square.jpg");

	loadShaders();
 
}

void CloudsVisualSystemColony::loadShaders(){
    string path = getVisualSystemDataPath() + "shaders/";
	levelSet.load(path + "levelSet.vs", path + "levelSet.fs");
    billboard.load(path + "billboard.vs", path + "billboard.fs");
}

void CloudsVisualSystemColony::selfSetupSystemGui()
{

    sysGui->addToggle("Level Set Mode", &levelSetMode);
    
    sysGui->addSpacer("Immutables");
    sysGui->addIntSlider("Initial Cells", 0, 1000, &numInitialCells);
    
   }

void CloudsVisualSystemColony::selfSetupGuis(){
    float length = (gui->getGlobalCanvasWidth()-gui->getWidgetSpacing()*5)/3.;
    float dim =gui->getGlobalSliderHeight();
    
    guiDynamics = new ofxUISuperCanvas("DYNAMICS", gui);
	guiDynamics->copyCanvasStyle(gui);
	guiDynamics->copyCanvasProperties(gui);
	guiDynamics->setName("DYNAMICS");
	guiDynamics->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    guiDynamics->addSlider("Separate",0.0,100, &params.amtSeparate);
    guiDynamics->addSlider("Cohere",0.0,100, &params.amtCohere);
    guiDynamics->addSlider("Align",0.0,100, &params.amtAlign);
    guiDynamics->addSlider("Friction",0.0,1.0, &params.dynamicFrictionCoeff);
    guiDynamics->addSlider("Turbulence Amount",0.0,100.0, &params.amtTurbulence);
    guiDynamics->addSlider("Turbulence Speed",0.0,100.0, &params.spdTurbulence);
    guiDynamics->addSlider("Fertility Rate", 0.0, 1.0, &params.fertilityRate);
    guiDynamics->addRangeSlider("Lifespan Range", 5, 5000, &params.lifespanMin, &params.lifespanMax);
    guiDynamics->addSlider("Nutrient Amount", 150, 500, &params.nutrientAmount);
    guiDynamics->addSlider("Nutrient Change Ratio", 0, 500, &params.nutrientTimeCoef);
    guiDynamics->addSlider("Nutrient Contrast", 0, 4.0, &params.nutrientFalloff);
    guiDynamics->addRangeSlider("Max Speed", 0.0, 10.0, &params.maxSpeed_min, &params.maxSpeed_max);
    guiDynamics->addRangeSlider("Max Force", 0.0, 10.0, &params.maxForce_min, &params.maxForce_max);
    guiDynamics->addRangeSlider("Max Size", 0.0, 30.0, &params.maxSize_min, &params.maxSize_max);
    
    ofAddListener(guiDynamics->newGUIEvent, this, &CloudsVisualSystemColony::selfGuiEvent);
    
    
    guiLooks = new ofxUISuperCanvas("LOOK", gui);
    guiLooks->copyCanvasStyle(gui);
    guiLooks->copyCanvasProperties(gui);
    guiLooks->setName("LOOKS");
    guiLooks->setWidgetFontSize(OFX_UI_FONT_SMALL);
    guiLooks->addSlider("Cell Floor Translusence", 0., 1., &translucenseCell);
    guiLooks->addSlider("Dish Floor Translusence", 0., 1., &translucenseDish);
    
    //    guiLooks->addWidgetDown(new ofxUILabel("V SLIDERS", OFX_UI_FONT_MEDIUM));
    guiLooks->addSlider("R", 0, 1., &(kernelColor_high.x));
    guiLooks->addSlider("G", 0, 1., &(kernelColor_high.y));
    guiLooks->addSlider("B", 0, 1., &(kernelColor_high.z));
    guiLooks->addSlider("A", 0, 1., &(kernelColor_high.w));
    guiLooks->addSlider("R", 0, 1., &(kernelColor_low.x));
    guiLooks->addSlider("G", 0, 1., &(kernelColor_low.y));
    guiLooks->addSlider("B", 0, 1., &(kernelColor_low.z));
    guiLooks->addSlider("A", 0, 1., &(kernelColor_low.w));
    
    //    guiLooks->add

    guis.push_back(guiDynamics);
    guis.push_back(guiLooks);
    guimap[guiDynamics->getName()] = guiDynamics;
    guimap[guiLooks->getName()] = guiLooks;

}


void CloudsVisualSystemColony::selfUpdate()
{
    //Video
    if ( !areFbosAllocatedAndSized() ){ reallocateFramebuffers(); }
    
    //Data
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
        
        billboard.setUniform1f("kernel_maxValue", kernel_maxValue);
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

    
    ofEnableAlphaBlending();
    levelSet.begin();
    levelSet.setUniformTexture("grunge", grunge, 1);
    levelSet.setUniform1f("time", ofGetElapsedTimeMillis()/100.0);
    levelSet.setUniform1i("levelSet", levelSetMode);
    levelSet.setUniform2f("resolution", getSharedRenderTarget().getWidth(), getSharedRenderTarget().getHeight());
    levelSet.setUniform2f("imgRes", grunge.getWidth(), grunge.getHeight());
    levelSet.setUniform1f("translucenseCell", translucenseCell);
    levelSet.setUniform1f("translucenseDish", translucenseDish);
    levelSet.setUniform4fv("kernelColor_high", kernelColor_high.getPtr());
    levelSet.setUniform4fv("kernelColor_low", kernelColor_low.getPtr());
    levelSet.setUniform1f("kernel_maxValue", kernel_maxValue);
    fbo_main.draw(0, 0, getSharedRenderTarget().getWidth(),
                  getSharedRenderTarget().getHeight());
    levelSet.end();
}

void CloudsVisualSystemColony::selfDraw(){

}


void CloudsVisualSystemColony::updateFoodTexture(){
}


void CloudsVisualSystemColony::selfBegin()
{
    populate();
}

void CloudsVisualSystemColony::selfEnd()
{
    clear();
}

void CloudsVisualSystemColony::selfExit(){
    clear();
    levelSet.unload();
    //TODO: is this necessary?
    delete guiLooks;
    delete guiDynamics;
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
    && fbo_main.getWidth() == getSharedRenderTarget().getWidth()
    && fbo_main.getHeight() == getSharedRenderTarget().getHeight();
}

void CloudsVisualSystemColony::reallocateFramebuffers(){
    int w = getSharedRenderTarget().getWidth();
    int h = getSharedRenderTarget().getHeight();
    
    fbo_main.allocate(w,h,GL_RGBA);
    
    fbo_main.begin();
    ofClear(0,0,0,0);
    fbo_main.end();
}


void CloudsVisualSystemColony::selfAutoMode(){}
void CloudsVisualSystemColony::selfSetupRenderGui(){}
void CloudsVisualSystemColony::guiSystemEvent(ofxUIEventArgs &e){}
void CloudsVisualSystemColony::guiRenderEvent(ofxUIEventArgs &e){}

void CloudsVisualSystemColony::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		levelSetMode = !levelSetMode;
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