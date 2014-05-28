#include "CloudsVisualSystemCosmic.h"

void CloudsVisualSystemCosmic::setupFloorVbo()
{
    int floorSize = debugGridSize*debugGridSize*4;
    floorIndexSize = debugGridSize*debugGridSize*6;
    
    ofxColorPalette *p = colorPalettes->getPalletePointer(floor(colorIndex));
    floorColors = new ofFloatColor[floorSize];
    ofVec2f *floorTexCoords = new ofVec2f[floorSize];
    ofVec3f *floor = new ofVec3f[floorSize];
    ofIndexType* floorindex = new ofIndexType[floorIndexSize];
    int index = 0;
    int x = 0;
    int y = 0;
    
    for(int i = 0; i < floorSize; i+=4)
    {
        floor[i].set(x, y, 0.0);
        floor[i+1].set(x, y, 0.0);
        floor[i+2].set(x, y, 0.0);
        floor[i+3].set(x, y, 0.0);
        
        floorindex[index++] = i;
        floorindex[index++] = i+1;
        floorindex[index++] = i+3;
        
        floorindex[index++] = i+1;
        floorindex[index++] = i+2;
        floorindex[index++] = i+3;
        
        floorTexCoords[i].set(0,0);
        floorTexCoords[i+1].set(debugGridSize,0);
        floorTexCoords[i+2].set(debugGridSize,debugGridSize);
        floorTexCoords[i+3].set(0,debugGridSize);
        
        floorColors[i].set(p->getColor(i));
        floorColors[i+1].set(p->getColor(i));
        floorColors[i+2].set(p->getColor(i));
        floorColors[i+3].set(p->getColor(i));
        
        x++;
        if(x%(int)debugGridSize == 0 && x != 0)
        {
            x = 0;
            y++;
        }
    }
    
    vboFloor.setVertexData(floor, floorSize, GL_STATIC_DRAW);
    vboFloor.setColorData(floorColors, floorSize, GL_STATIC_DRAW);
    vboFloor.setTexCoordData(floorTexCoords, floorSize, GL_STATIC_DRAW);
    vboFloor.setIndexData(floorindex, floorIndexSize, GL_STATIC_DRAW);
    
    delete[] floorTexCoords;
    delete[] floorindex;
    delete[] floor;
    shadowScale = 4.0;
    shadowOpacity = 1.0;
}

void CloudsVisualSystemCosmic::selfSetupCameraGui(){
    camGui->addSlider("near plane", 0.01, 10, &clipPlanes.min);
	camGui->addSlider("far plane" , 1000, 100000, &clipPlanes.max);
}

void CloudsVisualSystemCosmic::selfSetDefaults(){
    primaryCursorMode = CURSOR_MODE_CAMERA;
    secondaryCursorMode = CURSOR_MODE_INACTIVE;
}
void CloudsVisualSystemCosmic::selfSetupTimeline()
{
    timeline->setBPM(120);
    timeline->setShowBPMGrid(true);
    timeline->enableSnapToBPM(true);
    timeline->enableSnapToOtherKeyframes(true);
    
//    string path = getVisualSystemDataPath()+"audio/moan.wav";
//    ofxTLAudioTrack *at = new ofxTLAudioTrack();
//
//    timeline->addTrack("audio", at);
//    bool loaded = at->loadSoundfile(path);
//    if(!loaded)
//    {
//        loaded = at->loadSoundfile(path);
//    }
}

void CloudsVisualSystemCosmic::selfSetup()
{
	vbosAllocated = true;
	
    colorPalettes = new ofxColorPalettes();
    colorPalettes->loadFromDirectory(getVisualSystemDataPath()+"colors/");
    
    debugGridSize = 64;
    float interval = 1;
    
    size= (int)debugGridSize/interval;
    rows = size;
    cols = size;
    size*=size;
    numTris = (rows-1)*(cols-1)*2;
    numIndi = numTris*3;
    
    colorIndex = 0;
    
    int numfloat = rows*cols*3;
    pos = new float[numfloat];
    vel = new float[numfloat];
    float bounds = debugGridSize/2.0;
    for(int i = 0; i < numfloat; i+=3)
    {
        pos[i + 0] = ofRandom(-bounds,bounds);
        pos[i + 1] = ofRandom(-bounds,bounds);
        pos[i + 2] = ofRandom(-bounds,bounds);
        
        vel[i + 0] = ofRandomf();
        vel[i + 1] = ofRandomf();
        vel[i + 2] = ofRandomf();
    }
    
    
    float *rad = new float[numfloat];
    for(int i = 0; i < numfloat-3; i++)
    {
        rad[i + 0] = ofRandom(0.5, 1.0);
        rad[i + 1] = ofRandom(0.5, 1.0);
        rad[i + 2] = ofRandom(0.5, 1.0);
    }
    
    homeFbo.allocate(cols, rows, GL_RGB32F);
    homeFbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    electroFbo.allocate(cols, rows, GL_RGB32F);
    electroFbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    radiFbo.allocate(cols, rows, GL_RGB32F);
    radiFbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    posFboSrc.allocate(cols, rows, GL_RGB32F);
    posFboSrc.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    posFboDst.allocate(cols, rows, GL_RGB32F);
    posFboDst.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    velFboSrc.allocate(cols, rows, GL_RGB32F);
    velFboSrc.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    velFboDst.allocate(cols, rows, GL_RGB32F);
    velFboDst.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    accFboSrc.allocate(cols, rows, GL_RGB32F);
    accFboSrc.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    accFboDst.allocate(cols, rows, GL_RGB32F);
    accFboDst.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    radiFbo.getTextureReference().loadData(rad, cols, rows, GL_RGB32F);
    homeFbo.getTextureReference().loadData(pos, cols, rows, GL_RGB);
    posFboSrc.getTextureReference().loadData(pos, cols, rows, GL_RGB);
    posFboDst.getTextureReference().loadData(pos, cols, rows, GL_RGB);
    velFboSrc.getTextureReference().loadData(vel, cols, rows, GL_RGB);
    
    accShader.load(getVisualSystemDataPath()+"shaders/accShader");
    velShader.load(getVisualSystemDataPath()+"shaders/velShader");
    posShader.load(getVisualSystemDataPath()+"shaders/posShader");
    rdrShader.load(getVisualSystemDataPath()+"shaders/renderShader");
    sphereShader.load(getVisualSystemDataPath()+"shaders/sphereShader");
    
    timeStep = 0.001;
    radiusMultiplier = 1.0;
    damping = 0.0;
    bHomingActive = true;
    homeForceLimit = 1.0;
    bElectroActive = true;
    electroForceLimit = 1.0;
    
    cout << getVisualSystemDataPath() << endl; 
    homeShader.load(getVisualSystemDataPath()+"shaders/homeShader");
    electroShader.load(getVisualSystemDataPath()+"shaders/electroShader");
    attractorShader.load(getVisualSystemDataPath()+"shaders/attractorShader");
    radShader.load(getVisualSystemDataPath()+"shaders/radShader");
    floorShader.load(getVisualSystemDataPath()+"shaders/floorShader");
    noiseShader.load(getVisualSystemDataPath()+"shaders/noiseShader");
    
    delete[] rad;
    
    setupFloorVbo();
    
    clipPlanes.min = 1.f;
    clipPlanes.max = 100000.f;
    
}

void CloudsVisualSystemCosmic::selfSetupGuis()
{
//    setupFboViewerGui("HOME", &homeFbo);
//    setupFboViewerGui("RADI", &radiFbo);
//    setupFboViewerGui("POSITION", &posFboSrc);
//    setupFboViewerGui("VELOCITY", &velFboDst);
//    setupFboViewerGui("ACCELERATION", &accFboSrc);
    
    addSphereShaderGui("SPH0");
    addSphereShaderGui("SPH1");
    addHomeShaderGui("HMG");
    addElectroShaderGui("ELE");
    
    addAttractorShaderGui("ART0");
    addAttractorShaderGui("ART1");
    addAttractorShaderGui("ART2");
}


void CloudsVisualSystemCosmic::selfSetupSystemGui()
{
    sysGui->addButton("RESET", false);
    sysGui->addButton("RANDOMIZE", false);
    sysGui->addToggle("UPDATE RADIUS", &bUpdateRadius);
    sysGui->addToggle("UPDATE ACCELERATION", &bUpdateAcceleration);
    sysGui->addToggle("UPDATE VELOCITY", &bUpdateVelocity);
    sysGui->addToggle("UPDATE POSITION", &bUpdatePosition);
    sysGui->addSlider("TIMESTEP", 0.0, 1.0, &timeStep)->setLabelPrecision(4);
    sysGui->addSlider("DAMPING", 0.0, 1.0, &damping);
    sysGui->addSlider("ACCLIMIT", 0.0, 10.0, &accLimit);
    sysGui->addSlider("VELLIMIT", 0.0, 50.0, &velLimit);
}


void CloudsVisualSystemCosmic::guiSystemEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
    if(name == "RESET")
    {
        ofxUIButton *b = e.getButton();
        if(b->getValue())
        {
            homeFbo.getTextureReference().loadData(pos, cols, rows, GL_RGB);
            posFboSrc.getTextureReference().loadData(pos, cols, rows, GL_RGB);
            posFboDst.getTextureReference().loadData(pos, cols, rows, GL_RGB);
            clearFbo(velFboSrc);
            clearFbo(velFboDst);
            clearFbo(accFboSrc);
            clearFbo(accFboDst);
        }
    }
    else if(name == "RANDOMIZE")
    {
        ofxUIButton *b = e.getButton();
        if(b->getValue())
        {
            int numTotal = cols*rows*3;
            for(int i = 0; i < numTotal; i+=3)
            {
                vel[i + 0] = ofRandom(-velLimit,velLimit);
                vel[i + 1] = ofRandom(-velLimit,velLimit);
                vel[i + 2] = ofRandom(-velLimit,velLimit);
            }
            
            velFboSrc.getTextureReference().loadData(vel, cols, rows, GL_RGB);
            velFboDst.getTextureReference().loadData(vel, cols, rows, GL_RGB);
        }
    }
}

//use render gui for display settings, like changing colors
void CloudsVisualSystemCosmic::selfSetupRenderGui()
{
    rdrGui->addSlider("RADIUS", 0.0, 60.0, &radiusMultiplier)->setIncrement(1.0);
    rdrGui->addSlider("COLOR PALETTE", 0, colorPalettes->getCount(), &colorIndex)->setIncrement(1.0);
    rdrGui->addSlider("PARTICLE ALPHA", 0.0, 1.0, &particleAlpha);
    rdrGui->addSlider("SHADOW SCALE", 0.0, 32.0, &shadowScale);
    rdrGui->addSlider("SHADOW ALPHA", 0.0, 1.0, &shadowOpacity);
}

void CloudsVisualSystemCosmic::guiRenderEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
    if(name == "COLOR PALETTE")
    {
        int floorSize = debugGridSize*debugGridSize*4;
        ofxColorPalette *p = colorPalettes->getPalletePointer(floor(colorIndex));
        for(int i = 0; i < floorSize; i+=4)
        {
            floorColors[i].set(p->getColor(i));
            floorColors[i+1].set(p->getColor(i));
            floorColors[i+2].set(p->getColor(i));
            floorColors[i+3].set(p->getColor(i));
        }
        vboFloor.setColorData(floorColors, floorSize, GL_STATIC_DRAW);
    }
}

void CloudsVisualSystemCosmic::selfPresetLoaded(string presetPath)
{
	
}

void CloudsVisualSystemCosmic::selfBegin()
{
	
}

void CloudsVisualSystemCosmic::selfSceneTransformation()
{
	
}

//TRIAL ONE -- NOTHING BUT UPDATES and RADIUS



//TRIAL TWO -- HOMING ACTIVE
//TRIAL TWO -- Then activated attractor, took off homing
//TRIAL THREE -- put electro on
//TRIAL FOUR -- turned on electro and homing
//TRIAL FIVE -- sphere shader only
//EVERYTHING
void CloudsVisualSystemCosmic::selfUpdate()
{
	getCameraRef().setNearClip( clipPlanes.min );
    getCameraRef().setFarClip( clipPlanes.max );

    time = ofGetElapsedTimef();
    
	checkOpenGLError("TEST::START UPDDATE");

    if(bUpdateRadius)
    {
        updateRadiusShader();
    }
	
	checkOpenGLError("TEST::POST RADIUS");
    if(bUpdateAcceleration)
    {
        clearFbo(accFboSrc);
        if(bHomingActive) applyHomeShader();
		checkOpenGLError("TEST::POST HOME");
        if(bElectroActive) applyElectroShader();
		checkOpenGLError("TEST::POST ElECTRO");
        applyAttractorShader();
		checkOpenGLError("TEST::POST ATTRACT");
        applySphereShader();
		checkOpenGLError("TEST::POST SPHERE");
        updateAcceleration();
		checkOpenGLError("TEST::POST ACCEL");
    }
    if(bUpdateVelocity)
    {
        updateVelocity();
        if(bNoiseActive) applyCurlNoiseShader();
		checkOpenGLError("TEST::POST CURL");
    }
    if(bUpdatePosition)
    {
        updatePosition();
		checkOpenGLError("TEST::POST UPDATE");
    }
}

void CloudsVisualSystemCosmic::selfDraw()
{
	checkOpenGLError("TEST::START DRAW?");
    drawFloor();
	checkOpenGLError("TEST::FLOOR FINISHED");
    drawParticles();
	checkOpenGLError("TEST::PARTICLES FINISHED");
}

void CloudsVisualSystemCosmic::selfDrawDebug()
{
    drawSphereDebug();
    drawAttractorDebug();
}

void CloudsVisualSystemCosmic::selfDrawBackground()
{
	
}

void CloudsVisualSystemCosmic::selfEnd()
{	
}

void CloudsVisualSystemCosmic::selfExit()
{
	clear();
}

void CloudsVisualSystemCosmic::clear(){
	if(!vbosAllocated){
		return;
	}

    delete colorPalettes;
    vboFloor.clear();
    
    delete[] floorColors;
    delete[] pos;
    delete[] vel;
    
    for(int i = 0; i < sphereActive.size(); i++)
    {
        bool *b = sphereActive[i];
        delete b;
        float *r = sphereRadius[i];
        delete r;
        float *l = sphereAccLimit[i];
        delete l;
        ofVec3f *p = spherePosition[i];
        delete p;
    }
    
    sphereActive.clear();
    sphereRadius.clear();
    sphereAccLimit.clear();
    spherePosition.clear();
    
    for(int i = 0; i < attractorActive.size(); i++)
    {
        bool *b = attractorActive[i];
        delete b;
        float *r = attractorPower[i];
        delete r;
        float *e = attractorExpPower[i];
        delete e;
        float *l = attractorAccLimit[i];
        delete l;
        ofVec3f *p = attractorPosition[i];
        delete p;
        
    }
    
    attractorActive.clear();
    attractorPower.clear();
    attractorExpPower.clear();
    attractorAccLimit.clear();
    attractorPosition.clear();
	vbosAllocated = false;
}

void CloudsVisualSystemCosmic::selfKeyPressed(ofKeyEventArgs & args)
{
    switch (args.key)
    {
        case 'A':
        {
            if(tlGui != NULL)
            {
                ofxUIToggle *t = (ofxUIToggle *) tlGui->getWidget("ANIMATE");
                t->setValue(!t->getValue());
                t->triggerSelf();
            }
        }
            break;
            
        default:
            break;
    }
}

int CloudsVisualSystemCosmic::ID(int x, int y)
{
    return y*cols + x;
}

void CloudsVisualSystemCosmic::drawSphereDebug()
{
    vector<bool *>::iterator bit = sphereActive.begin();
    vector<float *>::iterator rit = sphereRadius.begin();
    vector<float *>::iterator lit = sphereAccLimit.begin();
    vector<ofVec3f *>::iterator  pit = spherePosition.begin();
    
    for(int i = 0; i < sphereActive.size(); i++)
    {
        if(*(*bit))
        {
            ofNoFill();
            float color = 255-bgBri;
            ofSetColor(color, 25.0*(*(*lit)));
#if OF_VERSION_MINOR > 7
            ofDrawSphere(*(*pit), *(*rit));
#else
            ofSphere(*(*pit), *(*rit));
#endif
        }
        
        ++bit;
        ++rit;
        ++lit;
        ++pit;
    }
}

void CloudsVisualSystemCosmic::drawAttractorDebug()
{
    vector<bool *>::iterator bit = attractorActive.begin();
    vector<float *>::iterator lit = attractorAccLimit.begin();
    vector<ofVec3f *>::iterator  pit = attractorPosition.begin();
    
    for(int i = 0; i < attractorActive.size(); i++)
    {
        if(*(*bit))
        {
            ofNoFill();
            float color = 255-bgBri;
            ofSetColor(color, 25.0*(*(*lit)));
#if OF_VERSION_MINOR > 7
            ofDrawSphere(*(*pit), *(*lit));
#else 
            ofSphere(*(*pit), *(*lit));
#endif
        }
        
        ++bit;
        ++lit;
        ++pit;
    }
}

void CloudsVisualSystemCosmic::updateRadiusShader()
{
	radiFbo.begin();
    ofClear(0, 255);
    radShader.begin();
    
    radShader.setUniform1f("time", time);
    radShader.setUniform1f("size", debugGridSize);
    radiFbo.draw(0, 0);
	
    radShader.end();
    radiFbo.end();
}

void CloudsVisualSystemCosmic::applyElectroShader()
{
    accFboDst.begin();
    ofClear(0, 255);
    electroShader.begin();
    
    electroShader.setUniformTexture("posData",
                                    posFboSrc.getTextureReference(),
                                    1);
    
    electroShader.setUniformTexture("radiData",
                                    radiFbo.getTextureReference(),
                                    2);
    
    electroShader.setUniformTexture("accData",
                                    accFboSrc.getTextureReference(),
                                    3);

    electroShader.setUniform1f("resolution", debugGridSize);
    electroShader.setUniform1f("limit", electroForceLimit);
    
    accFboSrc.draw(0, 0);
    
    electroShader.end();
    accFboDst.end();
    swap(accFboDst, accFboSrc);
}

void CloudsVisualSystemCosmic::applyHomeShader()
{
    accFboDst.begin();
    ofClear(0, 255);
    homeShader.begin();
    
    homeShader.setUniformTexture("posData",
                                 posFboSrc.getTextureReference(),
                                 1);
    
    homeShader.setUniformTexture("homeData",
                                 homeFbo.getTextureReference(),
                                 2);
    
    homeShader.setUniformTexture("accData",
                                 accFboSrc.getTextureReference(),
                                 3);
    
    
    homeShader.setUniform1f("limit", homeForceLimit);
    
    accFboSrc.draw(0, 0);
    
    homeShader.end();
    accFboDst.end();
    swap(accFboDst, accFboSrc);
}

void CloudsVisualSystemCosmic::applySphereShader()
{
    vector<bool *>::iterator bit = sphereActive.begin();
    vector<float *>::iterator rit = sphereRadius.begin();
    vector<float *>::iterator lit = sphereAccLimit.begin();
    vector<ofVec3f *>::iterator  pit = spherePosition.begin();
    
    for(int i = 0; i < sphereActive.size(); i++)
    {
        if(*(*bit))
        {
            accFboDst.begin();
            ofClear(0, 255);
            sphereShader.begin();
            
            sphereShader.setUniformTexture("posData",
                                           posFboSrc.getTextureReference(),
                                           1);
            
            sphereShader.setUniformTexture("accData",
                                           accFboSrc.getTextureReference(),
                                           2);
            
            sphereShader.setUniform3f("position", (*pit)->x, (*pit)->y, (*pit)->z);
            sphereShader.setUniform1f("radius", *(*rit));
            sphereShader.setUniform1f("limit", *(*lit));
            
            accFboSrc.draw(0, 0);
            
            sphereShader.end();
            accFboDst.end();
            swap(accFboDst, accFboSrc);
        }
        
        ++bit;
        ++rit;
        ++lit;
        ++pit;
    }
}

void CloudsVisualSystemCosmic::applyAttractorShader()
{
    vector<bool *>::iterator bit = attractorActive.begin();
    vector<float *>::iterator lit = attractorAccLimit.begin();
    vector<float *>::iterator pwit = attractorPower.begin();
    vector<float *>::iterator epit = attractorExpPower.begin();
    vector<ofVec3f *>::iterator  pit = attractorPosition.begin();
    
    for(int i = 0; i < attractorActive.size(); i++)
    {
        if(*(*bit))
        {
            accFboDst.begin();
            ofClear(0, 255);
            attractorShader.begin();
            
            attractorShader.setUniformTexture("posData",
                                              posFboSrc.getTextureReference(),
                                              1);
            
            attractorShader.setUniformTexture("accData",
                                              accFboSrc.getTextureReference(),
                                              2);
            
            attractorShader.setUniform3f("position", (*pit)->x, (*pit)->y, (*pit)->z);
            attractorShader.setUniform1f("power", *(*pwit));
            attractorShader.setUniform1f("expFactor", *(*epit));
            attractorShader.setUniform1f("limit", *(*lit));
            
            accFboSrc.draw(0, 0);
            
            attractorShader.end();
            accFboDst.end();
            swap(accFboDst, accFboSrc);
        }
        
        ++bit;
        ++lit;
        ++pit;
        ++pwit;
        ++epit;
    }
}

void CloudsVisualSystemCosmic::updateAcceleration()
{
    accFboDst.begin();
    ofClear(0, 255);
    accShader.begin();
    
    accShader.setUniformTexture("accData",
                                accFboSrc.getTextureReference(),
                                1);
    
    accShader.setUniformTexture("velData",
                                velFboSrc.getTextureReference(),
                                2);
    
    accShader.setUniform1f("time", time);
    accShader.setUniform1f("timestep", timeStep);
    accShader.setUniform1f("accLimit", accLimit);
    accShader.setUniform1f("damping", damping);
    
    accFboSrc.draw(0, 0);
    
    accShader.end();
    accFboDst.end();
    swap(accFboDst, accFboSrc);
}

void CloudsVisualSystemCosmic::updateVelocity()
{
    velFboDst.begin();
    ofClear(0, 255);
    velShader.begin();
    
    velShader.setUniform1f("timestep", timeStep);
    
    velShader.setUniformTexture("velData",
                                velFboSrc.getTextureReference(),
                                1);
    
    velShader.setUniformTexture("accData",
                                accFboSrc.getTextureReference(),
                                2);
    
    velShader.setUniform1f("timestep", timeStep);
    velShader.setUniform1f("velLimit", velLimit);
    velShader.setUniform1f("damping", 1.0-damping);
    
    velFboSrc.draw(0, 0);
    
    velShader.end();
    velFboDst.end();
    swap(velFboDst, velFboSrc);
}

void CloudsVisualSystemCosmic::updatePosition()
{
    posFboDst.begin();
    ofClear(0, 255);
    posShader.begin();
    
    posShader.setUniformTexture("posData",
                                posFboSrc.getTextureReference(),
                                1);
    
    posShader.setUniformTexture("velData",
                                velFboSrc.getTextureReference(),
                                2);
    
    
    posShader.setUniform1f("size", debugGridSize);
    posShader.setUniform1f("timestep", timeStep);
    
    posFboSrc.draw(0, 0);
    
    posShader.end();
    posFboDst.end();
    swap(posFboDst, posFboSrc);
}

void CloudsVisualSystemCosmic::drawParticles()
{
	ofPushStyle();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDepthMask(false);
	ofDisableDepthTest();
//    glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
//    glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    rdrShader.begin();
    ofxColorPalette *p = colorPalettes->getPalletePointer(colorIndex);
    
    for(int i = 0; i < 4; i++)
    {
        ofFloatColor clr = p->getColor(i);        
        rdrShader.setUniform3f(("color"+ofToString(i)).c_str(), clr.r, clr.g, clr.b);
    }

    rdrShader.setUniformTexture("radiData", radiFbo.getTextureReference(), 1);
    rdrShader.setUniformTexture("posData", posFboSrc.getTextureReference(), 2);
    rdrShader.setUniform1f("radiusMultiplier", radiusMultiplier/3.0);
    rdrShader.setUniform1f("size", debugGridSize);
    rdrShader.setUniform1f("particleAlpha", particleAlpha);
    vboFloor.drawElements(GL_TRIANGLES, floorIndexSize);
    rdrShader.end();
    glDepthMask(true);
	glPopAttrib();
	ofPopStyle();
}

void CloudsVisualSystemCosmic::drawFloor()
{
	ofPushStyle();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDepthMask(false);
    ofDisableDepthTest();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
   // glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    floorShader.begin();
    floorShader.setUniformTexture("radiData", radiFbo.getTextureReference(), 1);
    floorShader.setUniformTexture("posData", posFboSrc.getTextureReference(), 2);
    floorShader.setUniform1f("size", debugGridSize);
    floorShader.setUniform1f("shadowScale", shadowScale);
    floorShader.setUniform1f("shadowOpacity", shadowOpacity);
    ofSetColor(255, 0, 0);
    vboFloor.drawElements(GL_TRIANGLES, floorIndexSize);
    floorShader.end();
	glDepthMask(true);
	glPopAttrib();
	ofPopStyle();
}

void CloudsVisualSystemCosmic::setupFboViewerGui(string name, ofFbo *fbo)
{
    ofxUISuperCanvas *fboGui = new ofxUISuperCanvas(ofToUpper(name), gui);
    fboGui->copyCanvasStyle(gui);
    fboGui->copyCanvasProperties(gui);
    fboGui->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    fboGui->setName(name+"Settings");
    fboGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    fboGui->addSpacer();
    fboGui->addBaseDraws(name, fbo);
    fboGui->autoSizeToFitWidgets();
    addGui(fboGui);
}

void CloudsVisualSystemCosmic::addSphereShaderGui(string name)
{
    int index = sphereActive.size();
    
    bool *active = new bool();      *active = false;
    float *radius = new float();    *radius = debugGridSize;
    float *limit = new float();     *limit = 1.0;
    ofVec3f *pos = new ofVec3f(0,0,0);
    
    sphereActive.push_back(active);
    sphereRadius.push_back(radius);
    sphereAccLimit.push_back(limit);
    spherePosition.push_back(pos);
    
    ofxUISuperCanvas *g = new ofxUISuperCanvas(ofToUpper(name), gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName(name+"Settings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle(name + " ACTIVE", active);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    g->addSpacer();
    
    g->addMinimalSlider(name + " POS-X", -debugGridSize*2.0, debugGridSize*2.0, &pos->x);
    g->addMinimalSlider(name + " POS-Y", -debugGridSize*2.0, debugGridSize*2.0, &pos->y);
    g->addMinimalSlider(name + " POS-Z", -debugGridSize*2.0, debugGridSize*2.0, &pos->z);
    g->addSpacer();
    
    g->addSlider(name + " RADIUS", 0, debugGridSize*2.0, radius);
    g->addSlider(name + " LIMIT", 0.0, 10.0, limit);
    
    g->autoSizeToFitWidgets();
    addGui(g);
}

void CloudsVisualSystemCosmic::addHomeShaderGui(string name)
{
    ofxUISuperCanvas *g = new ofxUISuperCanvas(ofToUpper(name), gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName(name+"Settings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle(name + " ACTIVE", &bHomingActive);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    g->addSpacer();
    
    g->addSlider(name + " LIMIT", 0.0, 10.0, &homeForceLimit);
    
    g->autoSizeToFitWidgets();
    addGui(g);
}

void CloudsVisualSystemCosmic::addElectroShaderGui(string name)
{
    ofxUISuperCanvas *g = new ofxUISuperCanvas(ofToUpper(name), gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName(name+"Settings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle(name + " ACTIVE", &bElectroActive);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    g->addSpacer();
    
    g->addSlider(name + " LIMIT", 0.0, 10.0, &electroForceLimit);
    
    g->autoSizeToFitWidgets();
    addGui(g);
}

void CloudsVisualSystemCosmic::addAttractorShaderGui(string name)
{
    int index = sphereActive.size();
    
    bool *active = new bool();          *active = false;
    float *limit = new float();         *limit = 1.0;
    float *power = new float();         *power = 1.0;
    float *expFactor = new float();     *expFactor = 1.0;
    ofVec3f *pos = new ofVec3f(0,0,0);
    
    attractorActive.push_back(active);
    attractorAccLimit.push_back(limit);
    attractorPower.push_back(power);
    attractorExpPower.push_back(expFactor);
    attractorPosition.push_back(pos);
    
    ofxUISuperCanvas *g = new ofxUISuperCanvas(ofToUpper(name), gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName(name+"Settings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle(name + " ACTIVE", active);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    g->addSpacer();
    
    g->addMinimalSlider(name + " POS-X", -debugGridSize*2.0, debugGridSize*2.0, &pos->x);
    g->addMinimalSlider(name + " POS-Y", -debugGridSize*2.0, debugGridSize*2.0, &pos->y);
    g->addMinimalSlider(name + " POS-Z", -debugGridSize*2.0, debugGridSize*2.0, &pos->z);
    g->addSpacer();
    g->addSlider(name + " POWER", -1.0, 1.0, power);
    g->addSlider(name + " EXPFACTOR", 0.0, 1.0, expFactor);
    g->addSlider(name + " LIMIT", 0.0, 10.0, limit);
    g->autoSizeToFitWidgets();
    addGui(g);
}


void CloudsVisualSystemCosmic::addGui(ofxUISuperCanvas *g)
{
    guis.push_back(g);
    guimap[g->getName()] = g;
}

void CloudsVisualSystemCosmic::clearFbo(ofFbo &fbo)
{
    fbo.begin();
    ofClear(0, 255);
    fbo.end();
}

void CloudsVisualSystemCosmic::addCurlNoiseShaderGui(string name)
{
    ofxUISuperCanvas *g = new ofxUISuperCanvas(ofToUpper(name), gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName(name+"Settings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle(name + " ACTIVE", &bNoiseActive);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    g->addSpacer();
    
    g->addSlider(name + " NOISE SCALE", 0.0, .1, &noiseScale);
    g->addSlider(name + " LIMIT", 0.0, 10.0, &noiseForceLimit);
    
    g->autoSizeToFitWidgets();
    addGui(g);
}

void CloudsVisualSystemCosmic::applyCurlNoiseShader()
{
    velFboDst.begin();
    ofClear(0, 255);
    noiseShader.begin();
    
    noiseShader.setUniformTexture("posData",
                                 posFboSrc.getTextureReference(),
                                 1);
        
    noiseShader.setUniformTexture("velData",
                                  velFboSrc.getTextureReference(),
                                  2);
    
    noiseShader.setUniform1f("noiseScale", noiseScale); 
    noiseShader.setUniform1f("time", time);
    noiseShader.setUniform1f("limit", noiseForceLimit);
    
    velFboSrc.draw(0, 0);
    
    noiseShader.end();
    velFboDst.end();
    swap(velFboDst, velFboSrc);
}