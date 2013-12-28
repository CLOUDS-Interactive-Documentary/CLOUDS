//
//  CloudsVisualSystemOpenP5Machine.cpp
//

#include "CloudsVisualSystemOpenP5Machine.h"


//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOpenP5Machine::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
    customGui->addWidgetDown(new ofxUILabel("BOX COLOR", OFX_UI_FONT_MEDIUM));
	customGui->addSlider("Color 1 Hue", 0.0, 1.0, &color1HSB.r);
	customGui->addSlider("Color 1 Sat", 0.0, 1.0, &color1HSB.g);
	customGui->addSlider("Color 1 Bri", 0.0, 1.0, &color1HSB.b);
    
    customGui->addWidgetDown(new ofxUILabel("BOX SCALE", OFX_UI_FONT_MEDIUM));
    customGui->addSlider("Box Scale X", 0.0, 4.0, &boxScaleX);
	customGui->addSlider("Box Scale Y", 0.0, 4.0, &boxScaleY);
	customGui->addSlider("Box Scale Z", 0.0, 4.0, &boxScaleZ);
    customGui->addWidgetDown(new ofxUILabel("Shift Position", OFX_UI_FONT_MEDIUM));
    customGui->addSlider("Shift X", 0.0, 5.0, &shiftX);
	customGui->addSlider("Shift Y", 0.0, 5.0, &shiftY);
	customGui->addSlider("Shift Z", 0.0, 5.0, &shiftZ);

	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5Machine::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemOpenP5Machine::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5Machine::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5Machine::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5Machine::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5Machine::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOpenP5Machine::selfSetup(){

    depth = 400;
    
    color1HSB.r = 200;
    color2HSB.g = 130;
    color2HSB.b = 90;

    // sound
    synth.setOutputGen(buildSynth());
    
}


// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5Machine::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5Machine::selfBegin(){
    ofAddListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemOpenP5Machine::audioRequested);	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5Machine::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP5Machine::selfUpdate(){
 
   

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP5Machine::selfDraw(){
  
    ofPushStyle();
	ofSetLineWidth(.1);
	
	glEnable(GL_DEPTH_TEST);
	
    mat->begin();
	ofColor color;
     
    framecount = (ofGetElapsedTimeMillis()/33);
    ofRotateY(framecount* (ofRadToDeg(PI/500)));
    ofTranslate(ofGetWindowWidth()/10, ofGetWindowWidth()/10, depth/2);


    for(int i=0; i<10; i++) {

        
        ofRotateX(framecount* (ofRadToDeg(PI/1000)));
        
        
        for (int y = -2; y < 2; y++) {
            for (int x = -2; x < 2; x++) {
                for (int z = -1; z < 2; z++) {
                    
                    ofPushMatrix();
                    ofTranslate(shiftX*400*x, shiftY*300*y, shiftZ*300*z);
                    color.setHsb(color1HSB.r,color1HSB.g,color1HSB.b);
                    ofSetColor(color);
                    ofScale(5*boxScaleX,5*boxScaleY,100*boxScaleZ);
                    ofBox(1, 1, 1);
                    ofPopMatrix();
       
                    ofRotateX(240);
					
                    ofPushMatrix();
                    ofTranslate(shiftX*400*x, shiftY*300*y, shiftZ*50*z);
                    ofScale(100*boxScaleX, 5*boxScaleY, 5*boxScaleZ);
                    ofBox(1, 1, 1);
                    ofPopMatrix();
                    
					ofRotateX(50);
					
                    ofPushMatrix();
                    ofTranslate(shiftX*400*x,shiftY*10*y, shiftZ*50*z);
                    ofScale(50*boxScaleX, 5*boxScaleY, 5*boxScaleZ);
                    ofBox(1, 1, 1);
                    ofPopMatrix();
                    
                    ofRotateX(160);
					
                    ofPushMatrix();
                    ofRotateY(framecount* (ofRadToDeg(PI/400)));
                    ofTranslate(shiftX*100*x, shiftY*300*y, shiftZ*300*z);
                    ofScale(60*boxScaleX, 40*boxScaleY, 20*boxScaleZ);
                    ofBox(1, 1, 1);
                    ofPopMatrix();
        
                    
                }
            }
        }
    }
    
    
	mat->end();
    ofPopStyle();
	
}


// draw any debug stuff here
void CloudsVisualSystemOpenP5Machine::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOpenP5Machine::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5Machine::selfEnd(){
	ofRemoveListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemOpenP5Machine::audioRequested);
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOpenP5Machine::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5Machine::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5Machine::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5Machine::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Machine::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5Machine::selfMousePressed(int x, int y, int button){
//	if (gui->isHit(x, y)) {
//        cam.disableMouseInput();
//    }
}

void CloudsVisualSystemOpenP5Machine::selfMouseReleased(int x, int y, int button){
//     cam.enableMouseInput();
	
}

Generator CloudsVisualSystemOpenP5Machine::buildSynth()
{
    string strDir = GetCloudsDataPath()+"sound/textures/";
    ofDirectory sdir(strDir);
    string strAbsPath = sdir.getAbsolutePath() + "/Machine.aif";
    
    SampleTable sample = loadAudioFile(strAbsPath);
    
    Generator sampleGen = BufferPlayer().setBuffer(sample).trigger(1).loop(1);
    
    return sampleGen * .07;
}

void CloudsVisualSystemOpenP5Machine::audioRequested(ofAudioEventArgs& args)
{
    synth.fillBufferOfFloats(args.buffer, args.bufferSize, args.nChannels);
}


