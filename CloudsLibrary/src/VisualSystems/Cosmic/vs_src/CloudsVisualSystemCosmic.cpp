#include "CloudsVisualSystemCosmic.h"
//#include "CloudsRGBDVideoPlayer.h"

void CloudsVisualSystemCosmic::setupFloorVbo()
{
    int floorSize = debugGridSize*debugGridSize*4;
    floorIndexSize = debugGridSize*debugGridSize*6;     
    
    ofFloatColor *floorColors = new ofFloatColor[floorSize];
    ofVec2f *floorTexCoords = new ofVec2f[floorSize];
    ofVec3f *floor = new ofVec3f[floorSize];
    ofIndexType* floorindex = new ofIndexType[floorIndexSize];
    int index = 0;
    int x = 0;
    int y = 0;
    
    for(int i = 0; i < floorSize; i+=4)
    {
        floorColors[i].set(1.0, 1.0, 1.0, 1.0);
        floorColors[i+1].set(1.0, 1.0, 1.0, 1.0);
        floorColors[i+2].set(1.0, 1.0, 1.0, 1.0);
        floorColors[i+3].set(1.0, 1.0, 1.0, 1.0);
        
        
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
    delete[] floorColors;
    delete[] floorindex;
    delete[] floor;
    shadowScale = 4.0;
    shadowOpacity = 1.0;
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
    
    cout << "Rows: " << rows << endl;
    cout << "Cols: " << cols << endl;
    cout << "Verts: " << size << endl;
    cout << "Tris: " << numTris << endl;
    cout << "Indi: " << numIndi << endl;
    
    verts = new ofVec3f[size];
    colors = new ofFloatColor[size];
    normals = new ofVec3f[size];
    texCoords = new ofVec2f[size];
    indices = new ofIndexType[numIndi];
    
    float offSetX = (debugGridSize-interval)*-.5;
    float offSetY = (debugGridSize-interval)*-.5;
    
    colorIndex = 0;
    ofxColorPalette *p = colorPalettes->getPalletePointer(colorIndex);
    
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
    
    homeFbo.allocate(cols, rows, GL_RGB32F);
    homeFbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    
    float *rad = new float[numfloat];
    for(int i = 0; i < numfloat-3; i++)
    {
        rad[i + 0] = ofRandom(0.5, 1.0);
        rad[i + 1] = ofRandom(0.5, 1.0);
        rad[i + 2] = ofRandom(0.5, 1.0);
    }
    
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

    for(float y = 0; y < rows; y++)
    {
        for(float x = 0; x < cols; x++)
        {
            verts[ID(x,y)].set(offSetX+x*interval, offSetY+y*interval, 0);
            colors[ID(x,y)] = p->getColor(x+y);                                   //get color palettes
            normals[ID(x,y)].set(0, 1, 0);
            texCoords[ID(x,y)].set(x, y);
        }
    }
    
    int vIndex = 0;
    for(int i = 0; i < numIndi; i+=6)
    {
        if((vIndex+1) % cols == 0)
        {
            i-=6;
            vIndex++;
            continue;
        }
        
        int i0 = vIndex;            int i1 = vIndex+1;
        int i3 = vIndex+(cols);     int i4 = i1+(cols);
        
        indices[i] = i0;
        indices[i+1] = i3;
        indices[i+2] = i1;
        
        indices[i+3] = i1;
        indices[i+4] = i3;
        indices[i+5] = i4;
        
        vIndex++;
    }
    
    vbo.setVertexData(verts, size, GL_STATIC_DRAW);
    vbo.setNormalData(normals, size, GL_STATIC_DRAW);
    vbo.setColorData(colors, size, GL_STATIC_DRAW);
    vbo.setTexCoordData(texCoords, size, GL_STATIC_DRAW);
    vbo.setIndexData(indices, numIndi, GL_STATIC_DRAW);
    accShader.load(getVisualSystemDataPath()+"shaders/accShader");
    velShader.load(getVisualSystemDataPath()+"shaders/velShader");
    posShader.load(getVisualSystemDataPath()+"shaders/posShader");
    rdrShader.load(getVisualSystemDataPath()+"shaders/renderShader");
    sphereShader.load(getVisualSystemDataPath()+"shaders/sphereShader");
    
    ofDisableArbTex();
    ofLoadImage(glow, getVisualSystemDataPath()+"images/glow.png");
    spriteSize = glow.getWidth();
    cout << spriteSize << endl;
    ofEnableArbTex();
    
    timeStep = 0.001;
    radiusMultiplier = 1.0;
    damping = 0.0;
    

    homeShader.load(getVisualSystemDataPath()+"shaders/homeShader");
    bHomingActive = true;
    homeForceLimit = 1.0;
    
    electroShader.load(getVisualSystemDataPath()+"shaders/electroShader");
    bElectroActive = true;
    electroForceLimit = 1.0;
    
    attractorShader.load(getVisualSystemDataPath()+"shaders/attractorShader");
    
    radShader.load(getVisualSystemDataPath()+"shaders/radShader");

    floorShader.load(getVisualSystemDataPath()+"shaders/floorShader");
    
    
    noiseShader.load(getVisualSystemDataPath()+"shaders/noiseShader");
    delete[] rad;
    
    setupFloorVbo();
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
    rdrGui->addSlider("SHADOW SCALE", 0.0, 32.0, &shadowScale);
    rdrGui->addSlider("SHADOW OPACITY", 0.0, 1.0, &shadowOpacity);
}

void CloudsVisualSystemCosmic::guiRenderEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
    if(name == "COLOR PALETTE")
    {
        ofxColorPalette *p = colorPalettes->getPalletePointer(colorIndex);
        for(float x = 0; x < cols; x++)
        {
            for(float y = 0; y < rows; y++)
            {
                colors[ID(x,y)] = p->getColor(x+y);
            }
        }
        vbo.setColorData(colors, size, GL_STATIC_DRAW);
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
	
    time = ofGetElapsedTimef();
    
    if(bUpdateRadius)
    {
        updateRadiusShader();
    }
	
    if(bUpdateAcceleration)
    {
        clearFbo(accFboSrc);
		if(bHomingActive) applyHomeShader();
        if(bElectroActive) applyElectroShader();
        applyAttractorShader();
        applySphereShader();
        updateAcceleration();
    }
    if(bUpdateVelocity)
    {
        updateVelocity();
        if(bNoiseActive) applyCurlNoiseShader();
    }
    if(bUpdatePosition)
    {
        updatePosition();
    }
}

void CloudsVisualSystemCosmic::selfDraw()
{
	
//	return;
	
    drawFloor();
    drawParticles();
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
	clear();
}

void CloudsVisualSystemCosmic::selfExit()
{

}

void CloudsVisualSystemCosmic::clear(){
	if(!vbosAllocated){
		return;
	}
	
    vboFloor.clear();
	vbo.clear();
    
    delete[] pos;
    delete[] vel;
    
    delete[] verts;
    delete[] colors;
    delete[] normals;
    delete[] texCoords;
    delete[] indices;
    
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
    
    delete colorPalettes;
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
	ofMesh m;
	m.addVertex(ofVec3f(0,0,0));
	m.addVertex(ofVec3f(radiFbo.getWidth(),0,0));
	m.addVertex(ofVec3f(0,radiFbo.getHeight(),0));
	m.addVertex(ofVec3f(radiFbo.getWidth(),radiFbo.getHeight(),0));
	
	m.addTexCoord(ofVec2f(0,0));
	m.addTexCoord(ofVec2f(radiFbo.getWidth(),0));
	m.addTexCoord(ofVec2f(0,radiFbo.getHeight()));
	m.addTexCoord(ofVec2f(radiFbo.getWidth(),radiFbo.getHeight()));
	
	m.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	
    radiFbo.begin();
    ofClear(0, 255);
    radShader.begin();
    
    radShader.setUniform1f("time", time);
    radShader.setUniform1f("size", debugGridSize);
//    radiFbo.draw(0, 0);
    m.draw();
	
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
                                    posFboSrc.getTextureReference().getTextureData().textureID);
    
    electroShader.setUniformTexture("radiData",
                                    radiFbo.getTextureReference(),
                                    radiFbo.getTextureReference().getTextureData().textureID);
    
    electroShader.setUniformTexture("accData",
                                    accFboSrc.getTextureReference(),
                                    accFboSrc.getTextureReference().getTextureData().textureID);
    
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
                                 posFboSrc.getTextureReference().getTextureData().textureID);
    
    homeShader.setUniformTexture("homeData",
                                 homeFbo.getTextureReference(),
                                 homeFbo.getTextureReference().getTextureData().textureID);
    
    homeShader.setUniformTexture("accData",
                                 accFboSrc.getTextureReference(),
                                 accFboSrc.getTextureReference().getTextureData().textureID);
    
    
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
                                           posFboSrc.getTextureReference().getTextureData().textureID);
            
            sphereShader.setUniformTexture("accData",
                                           accFboSrc.getTextureReference(),
                                           accFboSrc.getTextureReference().getTextureData().textureID);
            
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
                                              posFboSrc.getTextureReference().getTextureData().textureID);
            
            attractorShader.setUniformTexture("accData",
                                              accFboSrc.getTextureReference(),
                                              accFboSrc.getTextureReference().getTextureData().textureID);
            
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
                                accFboSrc.getTextureReference().getTextureData().textureID);
    
    accShader.setUniformTexture("velData",
                                velFboSrc.getTextureReference(),
                                velFboSrc.getTextureReference().getTextureData().textureID);
    
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
                                velFboSrc.getTextureReference().getTextureData().textureID);
    
    velShader.setUniformTexture("accData",
                                accFboSrc.getTextureReference(),
                                accFboSrc.getTextureReference().getTextureData().textureID);
    
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
                                posFboSrc.getTextureReference().getTextureData().textureID);
    
    posShader.setUniformTexture("velData",
                                velFboSrc.getTextureReference(),
                                velFboSrc.getTextureReference().getTextureData().textureID);
    

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
	
    glDepthMask(false);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ofEnablePointSprites();
    
    rdrShader.begin();
    rdrShader.setUniformTexture("glow", glow, glow.getTextureData().textureID);
    rdrShader.setUniformTexture("radiData", radiFbo.getTextureReference(), radiFbo.getTextureReference().getTextureData().textureID);
    rdrShader.setUniformTexture("posData", posFboSrc.getTextureReference(), posFboSrc.getTextureReference().getTextureData().textureID);
    rdrShader.setUniform1f("radiusMultiplier", radiusMultiplier);
    rdrShader.setUniform1f("spriteSize", spriteSize);
    
    glow.bind();
    vbo.draw(GL_POINTS, 0, size);
    glow.unbind();
    
    rdrShader.end();
    ofDisablePointSprites();
    glDepthMask(true);
	
	ofPopStyle();
}

void CloudsVisualSystemCosmic::drawFloor()
{
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_DEPTH_TEST);
    glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    
    floorShader.begin();
    
    floorShader.setUniformTexture("radiData", radiFbo.getTextureReference(), radiFbo.getTextureReference().getTextureData().textureID);
    floorShader.setUniformTexture("posData", posFboSrc.getTextureReference(), posFboSrc.getTextureReference().getTextureData().textureID);
    floorShader.setUniform1f("radiusMultiplier", radiusMultiplier);
    floorShader.setUniform1f("size", debugGridSize);
    floorShader.setUniform1f("shadowScale", shadowScale);
    floorShader.setUniform1f("shadowOpacity", shadowOpacity);
    
    vboFloor.drawElements(GL_TRIANGLES, floorIndexSize);

    floorShader.end();
	glPopAttrib();
}

//void CloudsVisualSystemCosmic::drawTexturedQuad(float x, float y, float w, float h, float texWidth, float texHeight)
//{
//    glBegin (GL_QUADS);
//    
//    glTexCoord2f (0.0, 0.0);
//    glVertex3f (x, y, 0.0);
//    
//    glTexCoord2f (texWidth, 0.0);
//    glVertex3f (x+w, y, 0.0);
//    
//    
//    glTexCoord2f (texWidth, texHeight);
//    glVertex3f (x+w, y+h, 0.0);
//    
//    glTexCoord2f (0.0, texHeight);
//    glVertex3f (x, y+h, 0.0);
//    
//    glEnd ();
//}

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
                                 posFboSrc.getTextureReference().getTextureData().textureID);
        
    noiseShader.setUniformTexture("velData",
                                  velFboSrc.getTextureReference(),
                                  velFboSrc.getTextureReference().getTextureData().textureID);
    
    noiseShader.setUniform1f("noiseScale", noiseScale); 
    noiseShader.setUniform1f("time", time);
    noiseShader.setUniform1f("limit", noiseForceLimit);
    
    velFboSrc.draw(0, 0);
    
    noiseShader.end();
    velFboDst.end();
    swap(velFboDst, velFboSrc);
}