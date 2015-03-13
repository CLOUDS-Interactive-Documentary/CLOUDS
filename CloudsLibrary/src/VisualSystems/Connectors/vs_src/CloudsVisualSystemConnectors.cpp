//
//  CloudsVisualSystemConnectors.cpp
//

#include "CloudsVisualSystemConnectors.h"
#include "CloudsRGBDVideoPlayer.h"

//using namespace Tonic;

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemConnectors::selfSetupGui(){

	connectorGui = new ofxUISuperCanvas("CONNECTORS", gui);
	connectorGui->copyCanvasStyle(gui);
	connectorGui->copyCanvasProperties(gui);
	connectorGui->setName("Custom");
	connectorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	connectorGui->addSlider("Num Particles", 50, 64*64, &generator.numParticles);
	connectorGui->addToggle("Draw Connections", &generator.drawConnections);
	connectorGui->addSlider("Min Connection Distance", 1, 100, &generator.minDistance);
	connectorGui->addSlider("Boundary Size", 100, 1000, &generator.boundarySize);
	
	connectorGui->addSlider("Max Connections", 1, 10, &generator.maxConnections);
	
    connectorGui->addSlider("Particle speed", 0, 10, &generator.connectorsSpeed);
    
	guis.push_back(connectorGui);
	guimap[connectorGui->getName()] = connectorGui;
    

    // sound
    soundGui = new ofxUISuperCanvas("CONNECTORS Sound", gui);
	soundGui->copyCanvasStyle(gui);
	soundGui->copyCanvasProperties(gui);
	soundGui->setName("CONNECTORS Sound");
	soundGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    soundGui->addSlider("gain", 0.0, 1.0, &gain);

    soundGui->addToggle(tonicSamples[0].soundFile, &tonicSamples[0].playSample);

	guis.push_back(soundGui);
	guimap[soundGui->getName()] = soundGui;
    ofAddListener(soundGui->newGUIEvent, this, &CloudsVisualSystemConnectors::selfGuiEvent);
}

void CloudsVisualSystemConnectors::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
    
    for (int i=0; i<tonicSamples.size(); i++)
    {
        if (e.widget->getName() == tonicSamples[i].soundFile) {
            ofxUIToggle* toggle = static_cast<ofxUIToggle*>(e.widget);
            tonicSamples[i].playSample= toggle->getValue();
            if (toggle->getValue() == true) {
//                soundTriggers[i].trigger();
                tonicSamples[i].soundTrigger.trigger();
            }
        }
    }

}
void CloudsVisualSystemConnectors::selfSetDefaults(){
    primaryCursorMode = CURSOR_MODE_CAMERA;
    secondaryCursorMode = CURSOR_MODE_INACTIVE;
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemConnectors::selfSetupSystemGui(){
	
}

void CloudsVisualSystemConnectors::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemConnectors::selfSetupRenderGui(){

}

void CloudsVisualSystemConnectors::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemConnectors::selfSetup(){
    
    
    gain = 0;
    tonicSamples.push_back(TonicSample("WindChimes1.aif"));    
	generator.setup();

//	generator.setBounds(ofVec3f(-500,-500,-500), ofVec3f(500,500,500), 5);
    
    // sound
    #ifdef TONIC_SOUNDS
    synth.setOutputGen(buildSynth());
    #endif
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemConnectors::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemConnectors::selfBegin(){

    // sound
    ofAddListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemConnectors::audioRequested);
    
//    for (int i = 0; i < 1; i++){
//        if (playSample[i]) {
//            soundTriggers[i].trigger();
//        }
//    }
    
    for(int i =0; i<tonicSamples.size();i++){
        if (tonicSamples[i].playSample) {
            tonicSamples[i].soundTrigger.trigger();
        }
    }
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemConnectors::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemConnectors::selfUpdate(){
    volumeControl.value(gain);
	generator.update();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemConnectors::selfDraw(){

	ofPushStyle();
	ofSetLineWidth(1.f);
	ofDisableDepthTest();
	ofDisableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	ofNoFill();
	ofSetColor(20);
	ofBox(0,0,0, generator.boundarySize*2);

	generator.draw();
	ofPopStyle();
}

// draw any debug stuff here
void CloudsVisualSystemConnectors::selfDrawDebug(){
	generator.drawBins();
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemConnectors::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemConnectors::selfEnd(){
    volumeControl.value(0);
    ofRemoveListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemConnectors::audioRequested);    
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemConnectors::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemConnectors::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemConnectors::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemConnectors::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemConnectors::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemConnectors::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemConnectors::selfMouseReleased(ofMouseEventArgs& data){
	
}

#ifdef TONIC_SOUNDS
Tonic::Generator CloudsVisualSystemConnectors::buildSynth()
{
    string strDir = GetCloudsDataPath(true)+"sound/textures";
    ofDirectory sdir(strDir);
    Tonic::SampleTable samples[1];
    for(int i=0; i<tonicSamples.size();i++){
        string strAbsPath = ofToDataPath(strDir + "/" + tonicSamples[i].soundFile, true);
        samples[i] = Tonic::loadAudioFile(strAbsPath);
    }
    
    Tonic::Generator sampleGen1 = Tonic::BufferPlayer().setBuffer(samples[0]).loop(1).trigger(tonicSamples[0].soundTrigger);
    
    return sampleGen1 * volumeControl;
}
#endif

void CloudsVisualSystemConnectors::audioRequested(ofAudioEventArgs& args)
{
    #ifdef TONIC_SOUNDS
    synth.fillBufferOfFloats(args.buffer, args.bufferSize, args.nChannels);
    #endif
}



