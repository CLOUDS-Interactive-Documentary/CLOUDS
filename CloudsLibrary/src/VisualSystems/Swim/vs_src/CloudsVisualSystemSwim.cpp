//
//  CloudsVisualSystemSwim.cpp
//

#include "CloudsVisualSystemSwim.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

const float CloudsVisualSystemSwim::CAM_DAMPING = .08f;

CloudsVisualSystemSwim::CloudsVisualSystemSwim() : camSpeed(-600.f), regenerate(false), loadSeed(false), saveSeed(false)
{
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemSwim::selfSetup()
{
    snow.init(getVisualSystemDataPath());
    bubbles.init(getVisualSystemDataPath());
    creatures.init(getVisualSystemDataPath());
    
    post.init(ofGetWidth(), ofGetHeight(), true);
    //post.createPass<FxaaPass>();
    post.createPass<BloomPass>();
    
    // sound
    volumeControl.value(0);
    synth.setOutputGen(buildSynth());
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemSwim::selfBegin()
{
    // adding this here as custom gui data is loaded after setup
	generate();
    
    // sound
    ofAddListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemSwim::audioRequested);
    
    for (int i=0; i<2; i++)
    {
        if (playSample[i]) {
            soundTriggers[i].trigger();
        }
    }
}

void CloudsVisualSystemSwim::generate()
{
    snow.generate();
    creatures.generate();
    // ofxUIRadio getActive doesn't seem to work
    for (unsigned i = 0; i < seedRadio->getToggles().size(); ++i)
    {
        if (seedRadio->getToggles()[i]->getValue())
        {
            string name = seedRadio->getToggles()[i]->getName();
            if (name != "no seed data")
            {
                ostringstream oss;
                oss << getVisualSystemDataPath() << "seed/" << name;
                creatures.loadSeed(oss.str());
                break;
            }
        }
    }
}

//normal update call
void CloudsVisualSystemSwim::selfUpdate()
{
    volumeControl.value(gain);
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
    
    if (post.getWidth() != ofGetWidth() || post.getHeight() != ofGetHeight()) post.init(ofGetWidth(), ofGetHeight(), true);
    
    if (regenerate)
    {
        generate();
        regenerate = false;
    }
    
    if (saveSeed)
    {
        ofFileDialogResult result = ofSystemSaveDialog("positions.seed", "Save creature positions");
        ofLogNotice() << "Saving creature positions to " << result.getPath();
        creatures.saveSeed(result.getPath());
        saveSeed = false;
    }
    
    if (loadSeed)
    {
        ofFileDialogResult result = ofSystemLoadDialog("Load creature positions", false, getVisualSystemDataPath() + "seed");
        creatures.loadSeed(result.getPath());
        loadSeed = false;
    }
    
    // cam
    ofVec2f targetLookAngle;
    targetLookAngle.x = ofMap(GetCloudsInputY(), 0, ofGetHeight(), 10.f, -10.f, true);
    targetLookAngle.y = ofMap(GetCloudsInputX(), 0, ofGetWidth(), 20.f, -20.f, true);
    currentLookAngle.interpolate(targetLookAngle, .05);
    ofQuaternion rx, ry;
    rx.makeRotate(currentLookAngle.x, 1, 0, 0);
    ry.makeRotate(currentLookAngle.y, 0, 1, 0);
    getCameraRef().setOrientation(rx * ry);
    getCameraRef().move(0, 0, camSpeed * ofGetLastFrameTime());
    //getCameraRef().move(0, 0, ofMap(GetCloudsInputY(), 0, ofGetHeight(), -50.f, 0.f));
    //getCameraRef().setPosition(0, 0, 2000);
    
    //bubbles.update();
    creatures.update();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemSwim::selfDraw()
{
    creatures.draw(getCameraRef());
    //bubbles.draw();
    snow.draw(getCameraRef());
}

void CloudsVisualSystemSwim::selfPostDraw()
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    post.process(CloudsVisualSystem::getSharedRenderTarget(), false);
    post.getProcessedTextureReference().draw(0, ofGetHeight(), ofGetWidth(), -ofGetHeight());
    glPopAttrib();
}

//use render gui for display settings, like changing colors
void CloudsVisualSystemSwim::selfSetupRenderGui()
{    
    rdrGui->addToggle("regenerate", &regenerate);
    
    rdrGui->addMinimalSlider("camSpeed", 0.f, -1500.f, &camSpeed);
    
    rdrGui->addRangeSlider("creatureFogRange", 0.f, 10000.f, &Creature::fogStart, &Creature::fogEnd);
    
    rdrGui->addLabel("Flocking");
    rdrGui->addSpacer();
    rdrGui->addMinimalSlider("zoneRadius", 50.f, 2000.f, &creatures.zoneRadius);
    rdrGui->addMinimalSlider("alignmentLower", 0.f, 1.f, &creatures.alignmentLower);
    rdrGui->addMinimalSlider("alignmentUpper", 0.f, 1.f, &creatures.alignmentUpper);
    rdrGui->addMinimalSlider("repelStrength", 0.f, 1.f, &creatures.repelStrength);
    rdrGui->addMinimalSlider("alignStrength", 0.f, 1.f, &creatures.alignStrength);
    rdrGui->addMinimalSlider("attractStrength", 0.f, 1.f, &creatures.attractStrength);
    rdrGui->addMinimalSlider("maxDistFromCentre", 500.f, 4000.f, &creatures.maxDistFromCentre);
    
    rdrGui->addLabel("Jellies (see other menus)");
    rdrGui->addSpacer();
    rdrGui->addMinimalSlider("undulationAmt", 0.f, 2.f, &JellyCreature::undulationAmt);
    rdrGui->addRangeSlider("undulationFreqRange", 0.f, 10.f, &JellyCreature::undulationFreqMin, &JellyCreature::undulationFreqMax);
    rdrGui->addIntSlider("numJellyOne", 0, 300, &creatures.numJellyOne);
    rdrGui->addIntSlider("numJellyTwo", 0, 300, &creatures.numJellyTwo);
    
    rdrGui->addLabel("Fish One");
    rdrGui->addSpacer();
    rdrGui->addIntSlider("numGreyFish", 0, 300, &creatures.numGreyFish);
    rdrGui->addMinimalSlider("greySizeAverage", .1f, 3.f, &creatures.fishOneParams.sizeAverage);
    rdrGui->addMinimalSlider("greySizeStdDeviation", 0.f, 1.f, &creatures.fishOneParams.sizeStdDeviation);
    
    rdrGui->addLabel("Fish Two");
    rdrGui->addSpacer();
    rdrGui->addIntSlider("numYellowFish", 0, 300, &creatures.numYellowFish);
    rdrGui->addMinimalSlider("yellowSizeAverage", .1f, 3.f, &creatures.fishTwoParams.sizeAverage);
    rdrGui->addMinimalSlider("yellowSizeStdDeviation", 0.f, 1.f, &creatures.fishTwoParams.sizeStdDeviation);
    
    /*
     rdrGui->addLabel("Points");
     rdrGui->addSpacer();
     rdrGui->addIntSlider("numPointOne", 0, 1000, &creatures.numPointOne);
     rdrGui->addMinimalSlider("huePointOne", 0.f, 1.f, &creatures.huePointOne);
     rdrGui->addIntSlider("numPointTwo", 0, 1000, &creatures.numPointTwo);
     rdrGui->addMinimalSlider("huePointTwo", 0.f, 1.f, &creatures.huePointTwo);
     rdrGui->addIntSlider("numPointThree", 0, 1000, &creatures.numPointThree);
     rdrGui->addMinimalSlider("huePointThree", 0.f, 1.f, &creatures.huePointThree);
     */
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemSwim::selfSetupGui()
{
	jellyOneGui = createCustomGui("Jellyus Oneus");
    addSliders(jellyOneGui, creatures.jellyOneParams);
    
    jellyTwoGui = createCustomGui("Jellyus Twous");
    addSliders(jellyTwoGui, creatures.jellyTwoParams);
    
    snowGui = createCustomGui("Snow");
    snowGui->addRangeSlider("snowFogRange", 0.f, 10000.f, &snow.getFogStartRef(), &snow.getFogEndRef());
    snowGui->addRangeSlider("snowInnerFogRange", 0.f, 2000.f, &snow.getInnerFogStartRef(), &snow.getInnerFogEndRef());
    snowGui->addRangeSlider("snowAlphaRange", 0.f, 1.f, &snow.getAlphaMinRef(), &snow.getAlphaMaxRef());
    snowGui->addRangeSlider("snowSizeRange", 0.f, 1000.f, &snow.getSizeMinRef(), &snow.getSizeMaxRef());
    snowGui->addIntSlider("numSnowParticles", 1000, 100000, &snow.getNumParticlesRef());
    snowGui->addRangeSlider("hueRange", 0.f, 1.f, &snow.getHueMinRef(), &snow.getHueMaxRef());
    snowGui->addRangeSlider("saturationRange", 0.f, 1.f, &snow.getSaturationMinRef(), &snow.getSaturationMaxRef());
    
    seedGui = createCustomGui("Seed positions");
    ofDirectory dir;
    dir.listDir(getVisualSystemDataPath() + "seed");
    vector<string> fileNames;
    fileNames.push_back("no seed data");
    for (unsigned i = 0; i < dir.size(); ++i)
    {
        fileNames.push_back(dir.getName(i));
    }
    seedRadio = seedGui->addRadio("seed", fileNames);
    
    soundGui = createCustomGui("Sound");
    // sound
    soundGui->addToggle(soundFiles[0], &playSample[0]);
    soundGui->addToggle(soundFiles[1], &playSample[1]);
    soundGui->addSlider("Gain", 0, 1, &gain);
    
    ofAddListener(soundGui->newGUIEvent, this, &CloudsVisualSystemSwim::selfGuiEvent);
}

void CloudsVisualSystemSwim::addSliders(ofxUISuperCanvas* gui, JellyParams& params)
{
    gui->addLabel("Colour");
    //gui->addMinimalSlider("body h", 0.f, 1.f, &params.bodyHsb.x);
    gui->addRangeSlider("body h range", 0.f, 1.f, &params.bodyHMin, &params.bodyHMax);
    gui->addMinimalSlider("body s", 0.f, 1.f, &params.bodyS);
    gui->addMinimalSlider("body b", 0.f, 1.f, &params.bodyB);
    
    gui->addMinimalSlider("body alpha", 0.f, 1.f, &params.bodyAlpha);
    
    gui->addRangeSlider("tentacles h range", 0.f, 1.f, &params.tentaclesHMin, &params.tentaclesHMax);
    gui->addMinimalSlider("tentacles s", 0.f, 1.f, &params.tentaclesS);
    gui->addMinimalSlider("tentacles b", 0.f, 1.f, &params.tentaclesB);
    
    gui->addRangeSlider("pulse amt (range)", 0.f, 0.4f, &params.pulseAmtMin, &params.pulseAmtMax);
    
    gui->addLabel("Size");
    gui->addRangeSlider("width range", 10.f, 200.f, &params.widthMin, &params.widthMax);
    gui->addRangeSlider("length range", 10.f, 200.f, &params.lengthMin, &params.lengthMax);
    /*gui->addMinimalSlider("width average", 10, 200, &params.widthAverage);
    gui->addMinimalSlider("width std dev", 0, 200, &params.widthStdDeviation);
    
    gui->addMinimalSlider("length average", 10, 200, &params.lengthAverage);
    gui->addMinimalSlider("length std dev", 0, 200, &params.lengthStdDeviation);*/
    
    gui->addLabel("Shape");
    gui->addRangeSlider("spherical segment (range)", .5f * HALF_PI, PI, &params.segmentMin, &params.segmentMax);

    gui->addRangeSlider("superformula m1 (range)", 2, 20, &params.m1Min, &params.m1Max);
    gui->addRangeSlider("superformula m2 (range)", 2, 20, &params.m2Min, &params.m2Max);
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemSwim::selfPresetLoaded(string presetPath)
{
    
    //regenerate = true;
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemSwim::selfKeyPressed(ofKeyEventArgs & args)
{
    switch (args.key)
    {
        case 'R':
            regenerate = true;
            break;
            
        case 'S':
            saveSeed = true;
            break;
            
        case 'l':
            loadSeed = true;
            break;
            
        default:
            break;
    }
}

ofxUISuperCanvas* CloudsVisualSystemSwim::createCustomGui(const string& name)
{
    ofxUISuperCanvas* newGui = new ofxUISuperCanvas(name, gui);
	newGui->copyCanvasStyle(gui);
	newGui->copyCanvasProperties(gui);
	newGui->setName(name);
	newGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    newGui->addSpacer();
    
    guis.push_back(newGui);
	guimap[newGui->getName()] = newGui;
    
    return newGui;
}

void CloudsVisualSystemSwim::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
    for (int i=0; i<2; i++)
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

void CloudsVisualSystemSwim::guiRenderEvent(ofxUIEventArgs &e) {
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemSwim::selfSetupSystemGui(){
	
}

void CloudsVisualSystemSwim::guiSystemEvent(ofxUIEventArgs &e){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemSwim::selfSceneTransformation(){
	
}

// draw any debug stuff here
void CloudsVisualSystemSwim::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemSwim::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemSwim::selfEnd(){
	
    volumeControl.value(0); 
    ofRemoveListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemSwim::audioRequested);
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemSwim::selfExit(){
	
}

void CloudsVisualSystemSwim::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemSwim::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMouseReleased(ofMouseEventArgs& data){
	
}

Generator CloudsVisualSystemSwim::buildSynth()
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
    
    return (sampleGen1 * 1.0f + sampleGen2 * 1.0f) * volumeControl;
}

void CloudsVisualSystemSwim::audioRequested(ofAudioEventArgs& args)
{
    synth.fillBufferOfFloats(args.buffer, args.bufferSize, args.nChannels);
}


