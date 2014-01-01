//
//  CloudsVisualSystemRipples.cpp
//

#include "CloudsVisualSystemRipples.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemRipples::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("RIPPLES", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Ripples");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSpacer();
    customGui->addButton("RESTART", &bRestart);
    customGui->addSlider("DAMPING", 0, 1, &damping);
    customGui->addSlider("RADIUS", 1.0, 50.0, &radius);
    
    customGui->addSpacer();
    customGui->addToggle("DROP ON PRESS", &bDropOnPress);
    customGui->addIntSlider("RATE", 1, 30, &dropRate);
    
    customGui->addSpacer();
    customGui->addRangeSlider("DROP HUE", 0, 255, &minDropHue, &maxDropHue);
    customGui->addRangeSlider("DROP SAT", 0, 255, &minDropSat, &maxDropSat);
    customGui->addRangeSlider("DROP BRI", 0, 255, &minDropBri, &maxDropBri);
    
    customGui->addSpacer();
    tintHue = new ofx1DExtruder(0);
    tintHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(tintHue);
    customGui->addSlider("TINT HUE", 0.0, 255.0, tintHue->getPosPtr());
    tintSat = new ofx1DExtruder(0);
    tintSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(tintSat);
    customGui->addSlider("TINT SAT", 0.0, 255.0, tintSat->getPosPtr());
    tintBri = new ofx1DExtruder(0);
    tintBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(tintBri);
    customGui->addSlider("TINT BRI", 0.0, 255.0, tintBri->getPosPtr());
    tintAlpha = new ofx1DExtruder(0);
    tintAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(tintAlpha);
    customGui->addSlider("TINT ALPHA", 0.0, 255.0, tintAlpha->getPosPtr());
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemRipples::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
    
    // sound gui
	soundGui = new ofxUISuperCanvas("Ripples Sound", gui);
	soundGui->copyCanvasStyle(gui);
	soundGui->copyCanvasProperties(gui);
	soundGui->setName("Ripples Sound");
	soundGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    soundGui->addToggle("Enable sound", &bEnableSounds);
    soundGui->addSlider("Note vol", 0, 1, &volume[0]);
    soundGui->addSlider("Note noise vol", 0, 1, &volume[1]);
    soundGui->addIntSlider("Base note", 0, 11, &baseNote);
    vector<string> items;
    items.push_back("Chord Major 4");
    items.push_back("Chord Minor 4");
    items.push_back("Scale Major");
    items.push_back("Scale Blues");
    items.push_back("Scale Pentatonic");
    soundGui->addDropDownList("Scale", items);
    
	ofAddListener(soundGui->newGUIEvent, this, &CloudsVisualSystemRipples::selfGuiEvent);
	guis.push_back(soundGui);
	guimap[customGui->getName()] = soundGui;
}

void CloudsVisualSystemRipples::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "TINT HUE") {
        tintHue->setPosAndHome(tintHue->getPos());
	}
    else if (e.widget->getName() == "TINT SAT") {
        tintSat->setPosAndHome(tintSat->getPos());
	}
    else if (e.widget->getName() == "TINT BRI") {
        tintBri->setPosAndHome(tintBri->getPos());
	}
    else if (e.widget->getName() == "TINT ALPHA") {
        tintAlpha->setPosAndHome(tintAlpha->getPos());
    }
    else if (e.widget->getParent()->getName() == "Scale") {
        ofxUIToggle *toggle = (ofxUIToggle*)e.widget;
        if (toggle->getValue()) {
            setScaleByName(e.widget->getName());
        }
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemRipples::selfSetupSystemGui(){
	
}

void CloudsVisualSystemRipples::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemRipples::selfSetupRenderGui(){

}

void CloudsVisualSystemRipples::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemRipples::selfSetup()
{
    bIs2D = true;
//    bClearBackground = false;
    
    // Set defaults.
    bDropOnPress = false;
    dropRate = 1;
    damping = 0.995f;
    radius = 10.0f;
    
    minDropHue = 0;
    maxDropHue = 0;
    minDropSat = 0;
    maxDropSat = 0;
    minDropBri = 255;
    maxDropBri = 255;
    
    // Load the ripples shader.
    ripplesShader.load("", getVisualSystemDataPath() + "shaders/ripples.frag");
    
    bRestart = true;

    // sound
    bEnableSounds = true;
    noteIndex = 0;
    baseNote = 0;
    setScaleByName("Scale Pentatonic");
    mainSynth.setOutputGen(buildSynth());
}

void CloudsVisualSystemRipples::restart()
{
    float width = getCanvasWidth();
    float height = getCanvasHeight();
    
    ripplesSrcFbo.allocate(width, height, GL_RGBA32F);
    ripplesSrcFbo.begin();
    {
        ofClear(0, 0);
    }
    ripplesSrcFbo.end();
    
    ripplesDstFbo.allocate(width, height, GL_RGBA32F);
    ripplesDstFbo.begin();
    {
        ofClear(0, 0);
    }
    ripplesDstFbo.end();
    
    // Build a mesh to render a quad.
    renderMesh.clear();
    renderMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    renderMesh.addVertex(ofVec3f(0, 0));
    renderMesh.addVertex(ofVec3f(width, 0));
    renderMesh.addVertex(ofVec3f(width, height));
    renderMesh.addVertex(ofVec3f(0, height));
    renderMesh.addTexCoord(ofVec2f(0, 0));
    renderMesh.addTexCoord(ofVec2f(width, 0));
    renderMesh.addTexCoord(ofVec2f(width, height));
    renderMesh.addTexCoord(ofVec2f(0, height));
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemRipples::selfPresetLoaded(string presetPath)
{
    ofxUIDropDownList* d = (ofxUIDropDownList*)soundGui->getWidget("Scale");
	vector<int>& selected = d->getSelectedIndeces();
	if (selected.size() > 0) {
        setScaleByName(d->getToggles()[selected[0]]->getName());
	}

    bRestart = true;
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemRipples::selfBegin(){
    dontTriggerSoundCounter = 0;
    ofAddListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemRipples::audioRequested);
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemRipples::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemRipples::selfUpdate()
{    
    if (bRestart || ripplesSrcFbo.getWidth() != getCanvasWidth() || ripplesSrcFbo.getHeight() != getCanvasHeight()) {
        restart();
        bRestart = false;
    }
    
    tintColor.setHsb(tintHue->getPos(), tintSat->getPos(), tintBri->getPos(), tintAlpha->getPos());
    dropColor.setHsb(ofRandom(minDropHue, maxDropHue), ofRandom(minDropSat, maxDropSat), ofRandom(minDropBri, maxDropBri));
    
    if ((bDropOnPress && GetCloudsInputPressed()) || (!bDropOnPress && ofGetFrameNum() % dropRate == 0)) {
        ofPushStyle();
        ofPushMatrix();
        ripplesSrcFbo.begin();
        {
            ofSetColor(dropColor);
            ofNoFill();
#ifdef OCULUS_RIFT
            // I don't know why everything is flipped, but it is.
            ofCircle(getCanvasHeight()() - GetCloudsInputY(), getCanvasWidth() - GetCloudsInputX(), radius);
#else
            ofCircle(GetCloudsInputX(), GetCloudsInputY(), radius);
#endif
        }
        ripplesSrcFbo.end();
        ofPopMatrix();
        ofPopStyle();
        
        // sound
        if (dontTriggerSoundCounter == 0) {
            dontTriggerSoundCounter = 20;
            if (bEnableSounds) {
                playNote(GetCloudsInputX()/40+50);
            }
        }
    }
    

    ripplesDstFbo.begin();
    ripplesShader.begin();
    ripplesShader.setUniformTexture("backbuffer", ripplesDstFbo.getTextureReference(), 1);
    ripplesShader.setUniformTexture("tex0", ripplesSrcFbo.getTextureReference(), 2);
    ripplesShader.setUniform1f("damping", damping / 10.0f + 0.9f);  // 0.9 - 1.0 range
    {
        renderMesh.draw();
    }
    ripplesShader.end();
    ripplesDstFbo.end();

    ofPopStyle();
    
    // sound
    if (dontTriggerSoundCounter > 0)
    {
        dontTriggerSoundCounter--;
    }
    volumeControl[0].value(volume[0]);
    volumeControl[1].value(volume[1]);
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemRipples::selfDraw(){
    
}

// draw any debug stuff here
void CloudsVisualSystemRipples::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemRipples::selfDrawBackground()
{    
    ofSetColor(255, 255);
    
    ofPushStyle();
    ofSetColor(tintColor);
    ofEnableAlphaBlending();
    ripplesDstFbo.draw(0, 0);
    ofPopStyle();
    
    swap(ripplesSrcFbo, ripplesDstFbo);
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemRipples::selfEnd(){
    ofRemoveListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemRipples::audioRequested);
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemRipples::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemRipples::selfKeyPressed(int key){

}

void CloudsVisualSystemRipples::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemRipples::selfMouseDragged(int x, int y, int button){

}

void CloudsVisualSystemRipples::selfMouseMoved(int x, int y, int button){
	
}

void CloudsVisualSystemRipples::selfMousePressed(int x, int y, int button){
	
}

void CloudsVisualSystemRipples::selfMouseReleased(int x, int y, int button){
	
}

Generator CloudsVisualSystemRipples::buildSynth()
{
    for (int i=0; i<5; i++)
    {
        mixer.addInput(notes[i]);
    }
    
    return mixer >> Reverb().roomSize(100) >> BasicDelay(3, 4).feedback(0.5);
}

void CloudsVisualSystemRipples::setScaleByName(string name)
{
    scale.clear();
    
    if (name == "Chord Major 4") {
        cout<<"Scale: Selected Major 4"<<endl;
        scale.push_back(baseNote + 0);
        scale.push_back(baseNote + 4);
        scale.push_back(baseNote + 7);
        scale.push_back(baseNote + 11);
    }
    else if (name == "Chord Minor 4") {
        cout<<"Scale: Selected Minor 4"<<endl;
        scale.push_back(baseNote + 0);
        scale.push_back(baseNote + 3);
        scale.push_back(baseNote + 7);
        scale.push_back(baseNote + 10);
    }
    else if (name == "Scale Major") {
        cout<<"Scale: Selected Scale Major"<<endl;
        scale.push_back(baseNote + 0);
        scale.push_back(baseNote + 2);
        scale.push_back(baseNote + 4);
        scale.push_back(baseNote + 5);
        scale.push_back(baseNote + 7);
        scale.push_back(baseNote + 9);
        scale.push_back(baseNote + 11);
    }
    else if (name == "Scale Blues") {
        cout<<"Scale: Selected Scale Blues"<<endl;
        scale.push_back(baseNote + 0);
        scale.push_back(baseNote + 3);
        scale.push_back(baseNote + 5);
        scale.push_back(baseNote + 6);
        scale.push_back(baseNote + 7);
        scale.push_back(baseNote + 10);
    }
    else if (name == "Scale Pentatonic") {
        cout<<"Scale: Selected Scale Pentatonic"<<endl;
        scale.push_back(baseNote + 0);
        scale.push_back(baseNote + 2);
        scale.push_back(baseNote + 4);
        scale.push_back(baseNote + 7);
        scale.push_back(baseNote + 9);
    }
    
}

void CloudsVisualSystemRipples::playNote(int note)
{
    ControlSnapToScale scaleSnapper = ControlSnapToScale().setScale(scale);
    scaleSnapper.input(note);
    mutex.lock();
    
    Generator noteGen = (SineWave().freq(ControlMidiToFreq().input(scaleSnapper)) * volumeControl[0]) * ADSR(0.02, 0.7, 0.15, 0.1).trigger(1);
    Generator noiseGen = ((Noise() * volumeControl[1]) >> LPF12().cutoff(1000)) * ADSR(0.04, 0.12, 0.0, 0).trigger(1);
    notes[noteIndex++].setOutputGen(noteGen * SineWave().freq(4) + noiseGen);
    noteIndex = noteIndex%5;
    mutex.unlock();
}

void CloudsVisualSystemRipples::audioRequested(ofAudioEventArgs& args)
{
    mutex.lock();
    mainSynth.fillBufferOfFloats(args.buffer, args.bufferSize, args.nChannels);
    mutex.unlock();
}
