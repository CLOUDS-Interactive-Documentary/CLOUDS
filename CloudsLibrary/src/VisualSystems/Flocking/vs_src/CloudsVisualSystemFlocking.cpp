//
//  CloudsVisualSystemFlocking.cpp
//

#include "CloudsVisualSystemFlocking.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events


void CloudsVisualSystemFlocking::selfSetupGui(){
//	guis.push_back(customGui);
//	guimap[customGui->getName()] = customGui;
    
    setupFboViewerGui("POS", &posFboSrc);
    setupFboViewerGui("VEL", &velFboSrc);
    setupFboViewerGui("ACC", &accFboSrc);
}

void CloudsVisualSystemFlocking::setupFboViewerGui(string name, ofFbo *fbo)
{
    ofxUISuperCanvas *fboGui = new ofxUISuperCanvas(ofToUpper(name), gui);
    fboGui->copyCanvasStyle(gui);
    fboGui->copyCanvasProperties(gui);
    fboGui->setName(name+"Settings");
    fboGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    fboGui->addSpacer();
    fboGui->addBaseDraws(name, fbo);
    fboGui->autoSizeToFitWidgets();
    addGui(fboGui);
}

void CloudsVisualSystemFlocking::addGui(ofxUICanvas *g)
{
    guis.push_back((ofxUISuperCanvas *)g);
    guimap[g->getName()] = g;
}

void CloudsVisualSystemFlocking::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemFlocking::selfSetupSystemGui(){

    sysGui->addToggle("UPDATE ACC", &bUpdateAcc);
    sysGui->addToggle("UPDATE VEL", &bUpdateVel);
    sysGui->addToggle("UPDATE POS", &bUpdatePos);
    sysGui->addButton("RANDOMIZE", false);
    sysGui->addButton("ADD PARTICLE", false);
    sysGui->addSpacer();
    
    sysGui->addRangeSlider("THRESHOLDS",  0.0, 1.0, ps->threshLow, ps->threshHigh);
    sysGui->addSlider("BORDER", 0, debugGridSize, &debugGridSize);
    sysGui->addSlider("ZONE RADIUS", 0.0, debugGridSize*2., ps->zoneRadius);
    sysGui->addRangeSlider("VELOCITY MIN/MAX", 0.0, 25.0, ps->velLimitLow, ps->velLimit);
    sysGui->addSlider("ACCELERATION MAXIMUM", 0.0, 25.0, ps->accLimit);
    sysGui->addSlider("DAMPING", 0.0, 1.0, ps->damping);
    sysGui->addSlider("CENTER CONSTANT", 0.0, .10, ps->centerForceConstant);
    sysGui->addSlider("ATTRACT CONSTANT", 0.0, .10, ps->attractForceConstant);
    sysGui->addSlider("ALIGN CONSTANT", 0.0, 0.1, ps->alignForceConstant);
    sysGui->addSlider("REPEL CONSTANT", 0.0, .10, ps->repelForceConstant);
    sysGui->addSlider("COHESION CONSTANT", 0.0, .10, ps->cohesionForceConstant);
    sysGui->addSlider("PERLIN CONSTANT", 0.0, .10, ps->perlinForceConstant);
    sysGui->addSlider("NOISE SCALE", 0.0, .10, ps->noiseScale)->setLabelPrecision(4);
    
    sysGui->addToggle("DRAW ZONES", ps->drawDebug);
    sysGui->addToggle("DRAW VELOCITIES", ps->drawVelocities);
    sysGui->addToggle("DRAW ACCELERATIONS", ps->drawAccelerations);
    sysGui->addButton("KILL PARTICLES", false);
    sysGui->addButton("PRINT SETTINGS", false);	
}

void CloudsVisualSystemFlocking::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.getName();
    if(name == "ADD PARTICLE")
    {
        if(e.getButton()->getValue())
        {
            for(int i = 0; i < 100; i++)
            {
                ofxBoidParticle *bp = new ofxBoidParticle(ofVec3f(ofRandom(-debugGridSize, debugGridSize), ofRandom(-debugGridSize, debugGridSize), ofRandom(-debugGridSize, debugGridSize)));
                bp->setParticlesRef(&ps->getParticles());
                ps->addParticle(bp);
            }
        }
    }
    else if(name == "BORDER")
    {
        *(ps->bWidth) = debugGridSize;
        *(ps->bHeight) = debugGridSize;
        *(ps->bDepth) = debugGridSize;
    }
    else if(name == "ZONE RADIUS")
    {
        (*ps->zoneRadiusHalf) = (*ps->zoneRadius)/2.0;
        (*ps->zoneRadiusSqrd) = powf((*ps->zoneRadius), 2.0);
    }
    else if(name == "KILL PARTICLES")
    {
        if(e.getButton()->getValue())
        {
            ps->clear();
        }
    }
    else if(name == "PRINT SETTINGS")
    {
        if(e.getButton()->getValue())
        {
            ps->printSettings();
        }
    }
    else if(name == "RANDOMIZE")
    {
        if(e.getButton()->getValue())
        {
            randomize();
        }
    }
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemFlocking::selfSetupRenderGui(){
    rdrGui->addSlider("POINT SIZE", 0.0, 40.0, &pointSize);
    rdrGui->addSlider("RED", 0.0, 1.0, &redColor);
    rdrGui->addSlider("GREEN", 0.0, 1.0, &greenColor);
    rdrGui->addSlider("BLUE", 0.0, 1.0, &blueColor);
}

void CloudsVisualSystemFlocking::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemFlocking::selfSetup()
{    
    debugGridSize = 4096;
    ps = new ofxBoidSystem();
    pointSize = 5.0;
    
    redColor = 1.0;
    greenColor = 0.0;
    blueColor = 0.0;
    bUpdateVel = true;
    bUpdateAcc = true;
    bUpdatePos = true;

    resolution = 72;
    rows = resolution;
    cols = resolution;
    
    ofDisableArbTex();
    ofLoadImage(glow, getVisualSystemDataPath()+"images/glow.png");
    spriteSize = glow.getWidth();
    cout << spriteSize << endl;
    ofEnableArbTex();
    
    loadShaders();
    setupFbos();
    setupVbo();
}

void CloudsVisualSystemFlocking::loadShaders()
{
    posShader.load(getVisualSystemDataPath()+"shaders/posShader");
    velShader.load(getVisualSystemDataPath()+"shaders/velShader");
    accShader.load(getVisualSystemDataPath()+"shaders/accShader");
    rdrShader.load(getVisualSystemDataPath()+"shaders/renderShader");
}


void CloudsVisualSystemFlocking::setupFbos()
{
    posFboDst.allocate(cols, rows, GL_RGBA32F);
    posFboDst.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    posFboSrc.allocate(cols, rows, GL_RGBA32F);
    posFboSrc.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    velFboDst.allocate(cols, rows, GL_RGBA32F);
    velFboDst.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    velFboSrc.allocate(cols, rows, GL_RGBA32F);
    velFboSrc.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    accFboDst.allocate(cols, rows, GL_RGBA32F);
    accFboDst.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    accFboSrc.allocate(cols, rows, GL_RGBA32F);
    accFboSrc.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    
    numfloat = rows*cols*4;
    pos = new float[numfloat];
    vel = new float[numfloat];
    randomize();
}

int CloudsVisualSystemFlocking::ID(int x, int y)
{
    return y*cols + x;
}

void CloudsVisualSystemFlocking::setupVbo()
{
    size = resolution*resolution;
    ofVec3f * verts = new ofVec3f[size];
    ofVec2f * texCoords = new ofVec2f[size];
    
    float offSetX = -(resolution)/2.0;
    float offSetY = -(resolution)/2.0;
    
    for(float y = 0; y < rows; y++)
    {
        for(float x = 0; x < cols; x++)
        {
            verts[ID(x,y)].set(offSetX+x, offSetY+y, 0);
            texCoords[ID(x,y)].set(x, y);
        }
    }
    
    vbo.setVertexData(verts, size, GL_DYNAMIC_DRAW);
    vbo.setTexCoordData(texCoords, size, GL_DYNAMIC_DRAW);
}



// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemFlocking::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemFlocking::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemFlocking::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemFlocking::selfUpdate(){
    if(bUpdateAcc) updateAcceleration();
    if(bUpdateVel) updateVelocity();
    if(bUpdatePos) updatePosition();
}

void CloudsVisualSystemFlocking::updateAcceleration()
{
    accFboDst.begin();
    ofClear(0, 0);
    accShader.begin();
    
    accShader.setUniformTexture("posData",
                                posFboSrc.getTextureReference(),
                                posFboSrc.getTextureReference().getTextureData().textureID);
    
    accShader.setUniformTexture("velData",
                                velFboSrc.getTextureReference(),
                                velFboSrc.getTextureReference().getTextureData().textureID);
    
    accShader.setUniform1f("resolution", resolution);
    accShader.setUniform1f("limit", *ps->accLimit);
    accShader.setUniform1f("time", ofGetElapsedTimef());
    
    accShader.setUniform1f("zoneRadius", (*ps->zoneRadius));
    accShader.setUniform1f("threshLow",  (*ps->threshLow));
    accShader.setUniform1f("threshHigh", (*ps->threshHigh));
    accShader.setUniform1f("centerForceConstant", (*ps->centerForceConstant));
    accShader.setUniform1f("repelForceConstant", (*ps->repelForceConstant));
    accShader.setUniform1f("alignForceConstant", (*ps->alignForceConstant));
    accShader.setUniform1f("attractForceConstant", (*ps->attractForceConstant));
    accShader.setUniform1f("cohesionForceConstant", (*ps->cohesionForceConstant));
    accShader.setUniform1f("accLimit", (*ps->accLimit));
    
    accFboSrc.draw(0,0);
    accShader.end();
    accFboDst.end();
    swap(accFboDst, accFboSrc);
}

void CloudsVisualSystemFlocking::updateVelocity()
{
    velFboDst.begin();
    ofClear(0, 255);
    
    velShader.begin();
    
    velShader.setUniformTexture("posData",
                                posFboSrc.getTextureReference(),
                                posFboSrc.getTextureReference().getTextureData().textureID);
    
    velShader.setUniformTexture("velData",
                                velFboSrc.getTextureReference(),
                                velFboSrc.getTextureReference().getTextureData().textureID);
    
    velShader.setUniformTexture("accData",
                                accFboSrc.getTextureReference(),
                                accFboSrc.getTextureReference().getTextureData().textureID);
    
    velShader.setUniform1f("noiseScale", (*ps->noiseScale));
    velShader.setUniform1f("perlinForceConstant", (*ps->perlinForceConstant));
    velShader.setUniform1f("damping", (*ps->damping));
    velShader.setUniform1f("time", ofGetElapsedTimef());
    velShader.setUniform1f("timestep", 1.0);
    velShader.setUniform1f("velLimit", (*ps->velLimit));
    velShader.setUniform1f("velLimitLow", (*ps->velLimitLow));
    
    velFboSrc.draw(0,0);
    
    velShader.end();
    
    velFboDst.end();
    swap(velFboDst, velFboSrc);
}


void CloudsVisualSystemFlocking::updatePosition()
{
    posFboDst.begin();
    ofClear(0, 255);
    
    posShader.begin();
    
    posShader.setUniformTexture("posData",
                                posFboSrc.getTextureReference(),
                                posFboSrc.getTextureReference().getTextureData().textureID);
    
    posShader.setUniformTexture("velData",
                                velFboSrc.getTextureReference(),
                                velFboSrc.getTextureReference().getTextureData().textureID);
    
    posShader.setUniform1f("border", debugGridSize);
    posShader.setUniform1f("timestep", 1.0);
    
    posFboSrc.draw(0,0);
    
    posShader.end();
    
    posFboDst.end();
    swap(posFboDst, posFboSrc);
}


// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemFlocking::selfDraw(){
    glDepthMask(false);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofEnablePointSprites();
    ofSetColor(0,255);
    rdrShader.begin();
    rdrShader.setUniform3f("u_color", redColor, greenColor, blueColor);
    rdrShader.setUniformTexture("posData", posFboSrc.getTextureReference(), posFboSrc.getTextureReference().getTextureData().textureID);
    rdrShader.setUniformTexture("accData", accFboSrc.getTextureReference(), accFboSrc.getTextureReference().getTextureData().textureID);

    glow.bind();
    glPointSize(pointSize);
    vbo.draw(GL_POINTS, 0, size);
    glow.unbind();
    
    rdrShader.end();
    ofDisablePointSprites();
    glDepthMask(true);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
}

// draw any debug stuff here
void CloudsVisualSystemFlocking::selfDrawDebug(){
    float size2 = debugGridSize*2.;
    ofSetRectMode(OF_RECTMODE_CENTER);
    
    ofRect(0,0,-debugGridSize, size2,size2);
    ofRect(0,0,debugGridSize, size2,size2);
    
    ofSetRectMode(OF_RECTMODE_CORNER);
    
    ofLine(debugGridSize, debugGridSize, -debugGridSize, debugGridSize, debugGridSize, debugGridSize);
    ofLine(debugGridSize, -debugGridSize, -debugGridSize, debugGridSize, -debugGridSize, debugGridSize);
    ofLine(-debugGridSize, debugGridSize, -debugGridSize, -debugGridSize, debugGridSize, debugGridSize);
    ofLine(-debugGridSize, -debugGridSize, -debugGridSize, -debugGridSize, -debugGridSize, debugGridSize);
    	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemFlocking::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemFlocking::selfEnd(){
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemFlocking::selfExit(){
    delete[] pos;
    delete[] vel;
    delete ps;
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemFlocking::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemFlocking::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemFlocking::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemFlocking::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemFlocking::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemFlocking::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemFlocking::randomize()
{
    float bounds = debugGridSize;
    for(int i = 0; i < numfloat; i+=4)
    {
        pos[i + 0] = ofRandom(-bounds,bounds);
        pos[i + 1] = ofRandom(-bounds,bounds);
        pos[i + 2] = ofRandom(-bounds,bounds);
        pos[i + 3] = 1.0;
        
        vel[i + 0] = ofRandomf();
        vel[i + 1] = ofRandomf();
        vel[i + 2] = ofRandomf();
        vel[i + 3] = 1.0;
    }
    
    posFboSrc.getTextureReference().loadData(pos, cols, rows, GL_RGBA);
    posFboDst.getTextureReference().loadData(pos, cols, rows, GL_RGBA);
    
    velFboSrc.getTextureReference().loadData(vel, cols, rows, GL_RGBA);
    velFboDst.getTextureReference().loadData(vel, cols, rows, GL_RGBA);
}