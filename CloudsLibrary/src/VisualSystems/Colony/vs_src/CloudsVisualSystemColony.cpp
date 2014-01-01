#include "CloudsVisualSystemColony.h"

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
 
    // sound
    synth.setOutputGen(buildSynth());
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
    sysGui->addIntSlider("Initial Cells", 0, 300, &numInitialCells);
    
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
    guiDynamics->addSlider("Nutrient Amount", 50, 500, &params.nutrientAmount);
    guiDynamics->addSlider("Nutrient Change Ratio", 0, 1.0, &params.nutrientTimeCoef);
    guiDynamics->addSlider("Nutrient Contrast", 0, 12.0, &params.nutrientFalloff);
    guiDynamics->addSlider("Nutrient Scale", 0.0001, 0.1, &params.nutrientScale);
    guiDynamics->addRangeSlider("Max Speed", 0.0, 10.0, &params.maxSpeed_min, &params.maxSpeed_max);
    guiDynamics->addRangeSlider("Max Force", 0.0, 10.0, &params.maxForce_min, &params.maxForce_max);
    guiDynamics->addRangeSlider("Max Size", 0.0, 30.0, &params.maxSize_min, &params.maxSize_max);
    
    ofAddListener(guiDynamics->newGUIEvent, this, &CloudsVisualSystemColony::selfGuiEvent);
    
    
    guiLooks = new ofxUISuperCanvas("LOOK", gui);
    guiLooks->copyCanvasStyle(gui);
    guiLooks->copyCanvasProperties(gui);
    guiLooks->setName("LOOKS");
    guiLooks->addToggle("Level Set BG", &levelSetBG);
    guiLooks->setWidgetFontSize(OFX_UI_FONT_SMALL);
    guiLooks->addSlider("Cell Floor Translusence", 0., 1., &translucenseCell);
    guiLooks->addSlider("Dish Floor Translusence", 0., 1., &translucenseDish);
    
    float hDim = 16;
    float vDim = 80;

    guiLooks->addWidgetDown(new ofxUILabel("STIPPLE", OFX_UI_FONT_MEDIUM));
    guiLooks->addSlider("R", 0, 1., &(stippleColor.x), hDim, vDim);
    guiLooks->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiLooks->addSlider("G", 0, 1., &(stippleColor.y), hDim, vDim);
    guiLooks->addSlider("B", 0, 1., &(stippleColor.z), hDim, vDim);
    guiLooks->addSlider("A", 0, 1., &(stippleColor.w), hDim, vDim);

    guiLooks->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    guiLooks->addSlider("Stipple Scale", 0,2, &stippleScale);
    
    
    guiLooks->addWidgetDown(new ofxUILabel("KERNEL COLOR", OFX_UI_FONT_MEDIUM));
    guiLooks->addSlider("R1", 0, 1., &(kernelColor_high.x), hDim, vDim);
    guiLooks->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    guiLooks->addSlider("G1", 0, 1., &(kernelColor_high.y), hDim, vDim);
    guiLooks->addSlider("B1", 0, 1., &(kernelColor_high.z), hDim, vDim);
    guiLooks->addSlider("A1", 0, 1., &(kernelColor_high.w), hDim, vDim);
    guiLooks->addSlider("R2", 0, 1., &(kernelColor_low.x), hDim, vDim);
    guiLooks->addSlider("G2", 0, 1., &(kernelColor_low.y), hDim, vDim);
    guiLooks->addSlider("B2", 0, 1., &(kernelColor_low.z), hDim, vDim);
    guiLooks->addSlider("A2", 0, 1., &(kernelColor_low.w), hDim, vDim);
    guiLooks->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    guis.push_back(guiDynamics);
    guis.push_back(guiLooks);
    guimap[guiDynamics->getName()] = guiDynamics;
    guimap[guiLooks->getName()] = guiLooks;
    
    // sound
    soundGui = new ofxUISuperCanvas("COLONY Sound", gui);
	soundGui->copyCanvasStyle(gui);
	soundGui->copyCanvasProperties(gui);
	soundGui->setName("COLONY Sound");
	soundGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    soundGui->addToggle(soundFiles[0], &playSample[0]);
    soundGui->addToggle(soundFiles[1], &playSample[1]);
    soundGui->addToggle(soundFiles[2], &playSample[2]);
    
	guis.push_back(soundGui);
	guimap[soundGui->getName()] = soundGui;
    ofAddListener(soundGui->newGUIEvent, this, &CloudsVisualSystemColony::selfGuiEvent);
}



//==========================================================================================



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
        //TODO: use the other two normal positions to dictate an excited state
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
    
    
    //Debug
    
    //FIXME: Should definitely be removed in the distribution mode
    if (bDebug){
        if ( (!img_debug.isAllocated())
            || (!img_debug.getWidth()==getSharedRenderTarget().getWidth())
            || (!!img_debug.getHeight()==getSharedRenderTarget().getHeight()))
        {
            img_debug.allocate(
                               getSharedRenderTarget().getWidth(),
                               getSharedRenderTarget().getHeight(),
                               OF_IMAGE_GRAYSCALE);
        }
        
        for (int i = 0; i < img_debug.getWidth(); i++){
            for (int j = 0; j < img_debug.getHeight(); j++){
                img_debug.setColor(i, j, ofColor(colonyCell::getCellNoise(i, j, params.nutrientTimeCoef, params.nutrientFalloff, params.nutrientAmount, params.nutrientScale)));
            }
        }
        img_debug.update();
    }
    
}

void CloudsVisualSystemColony::selfDrawBackground()
{
    //FIXME: This shouldn't be here, but it's the only way to draw stuff in 2d
    if(bDebug){
        img_debug.draw(0, 0, getSharedRenderTarget().getWidth(), getSharedRenderTarget().getHeight());
        ofLog(OF_LOG_ERROR, "Colony: bDebug is on. Remove this before reaching production");
    }
    
    //FIXME: This is a safety check if FBOs are not allocated, in order to avoid calling an empty one

        ofEnableAlphaBlending();
        
        levelSet.begin();
        levelSet.setUniformTexture("grunge", grunge, 1);
        levelSet.setUniform1f("time", ofGetElapsedTimeMillis()/100.0);
        levelSet.setUniform1i("levelSet", levelSetMode);
        levelSet.setUniform1i("levelSetBg", levelSetBG);
        levelSet.setUniform2f("resolution", getSharedRenderTarget().getWidth(), getSharedRenderTarget().getHeight());
        levelSet.setUniform2f("imgRes", grunge.getWidth(), grunge.getHeight());
        levelSet.setUniform1f("translucenseCell", translucenseCell);
        levelSet.setUniform1f("translucenseDish", translucenseDish);
        levelSet.setUniform4fv("kernelColor_high", kernelColor_high.getPtr());
        levelSet.setUniform4fv("kernelColor_low", kernelColor_low.getPtr());
        levelSet.setUniform1f("kernel_maxValue", kernel_maxValue);
        
        ofxLight& l = (*(*lights.begin()).second);
        levelSet.setUniform3fv("lightDirection", l.lightPos.getPtr());
        levelSet.setUniform3f("lightColor", l.lightSpecularHSV.r,l.lightSpecularHSV.g,l.lightSpecularHSV.b);
        
        levelSet.setUniform1f("stippleScale", stippleScale);
        levelSet.setUniform4fv("stippleColor", stippleColor.getPtr());
    
    if (areFbosAllocatedAndSized()){
        fbo_main.draw(0, 0, getSharedRenderTarget().getWidth(),
                      getSharedRenderTarget().getHeight());
    } else {
        ofRect(0, 0, getSharedRenderTarget().getWidth(), getSharedRenderTarget().getHeight());
        ofLog(OF_LOG_ERROR, "Colony : selfDrawBackground() being called before fbos were allocated. Drawing empty background instead");
    };
        levelSet.end();
    
}

void CloudsVisualSystemColony::selfDraw(){

}


void CloudsVisualSystemColony::updateFoodTexture(){
}


void CloudsVisualSystemColony::selfBegin()
{
    populate();

    // sound
    ofAddListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemColony::audioRequested);
    
    for (int i=0; i<3; i++)
    {
        if (playSample[i]) {
            soundTriggers[i].trigger();
        }
    }
}

void CloudsVisualSystemColony::selfEnd()
{
    clear();

    // sound
    ofRemoveListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemColony::audioRequested);
}

void CloudsVisualSystemColony::selfExit(){
    clear();
    billboard.unload();
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

void CloudsVisualSystemColony::selfDrawDebug(){

}
void CloudsVisualSystemColony::selfSceneTransformation(){}
void CloudsVisualSystemColony::selfKeyReleased(ofKeyEventArgs & args){}
void CloudsVisualSystemColony::mouseDragged(ofMouseEventArgs& data){}
void CloudsVisualSystemColony::mouseMoved(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::mousePressed(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::mouseReleased(ofMouseEventArgs &args){}
void CloudsVisualSystemColony::selfSetupGui(){}
void CloudsVisualSystemColony::selfGuiEvent(ofxUIEventArgs &e)
{
    for (int i=0; i<3; i++)
    {
        if (e.widget->getName() == soundFiles[i]) {
            ofxUIToggle* toggle = static_cast<ofxUIToggle*>(e.widget);
            playSample[i] = toggle->getValue();
            if (toggle->getValue() == true) {
                soundTriggers[i].trigger();
            }
        }
    }
}

Generator CloudsVisualSystemColony::buildSynth()
{
    string strDir = GetCloudsDataPath()+"sound/textures/";
    ofDirectory sdir(strDir);
    
    SampleTable samples[3];
    
    int nSounds = sizeof(soundFiles) / sizeof(string);
    for (int i=0; i<nSounds; i++)
    {
        string strAbsPath = sdir.getAbsolutePath() + "/" + soundFiles[i];
        samples[i] = loadAudioFile(strAbsPath);
    }
    
    Generator sampleGen1 = BufferPlayer().setBuffer(samples[0]).loop(1).trigger(soundTriggers[0]);
    Generator sampleGen2 = BufferPlayer().setBuffer(samples[1]).loop(1).trigger(soundTriggers[1]);
    Generator sampleGen3 = BufferPlayer().setBuffer(samples[2]).loop(1).trigger(soundTriggers[2]);
    
    return sampleGen1 * 0.4f + sampleGen2 * 0.4f + sampleGen3 * 0.2f;
}

void CloudsVisualSystemColony::audioRequested(ofAudioEventArgs& args)
{
    synth.fillBufferOfFloats(args.buffer, args.bufferSize, args.nChannels);
}


