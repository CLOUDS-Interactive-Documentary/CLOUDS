//
//  CloudsVisualSystemOpenP5TextUniverse.cpp
//

#include "CloudsVisualSystemOpenP5TextUniverse.h"


//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("ORBITAL", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Orbital");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    customGui->addSlider("SPIN SPEED", 0, 5, &spinSpeed);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5TextUniverse::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemOpenP5TextUniverse::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5TextUniverse::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5TextUniverse::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5TextUniverse::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5TextUniverse::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::selfSetup()
{
    // Set defaults.
    currSpin = 0.0f;
    spinSpeed = 0.5f;
    
    // Load the contents of the text file.
    ofBuffer buffer = ofBufferFromFile("deconstructive.txt");
    if (buffer.size()) {
        text = new TUText(buffer.getText());
        text->print();
    }
    
    // Build the node network.
    orbital = new TUOrbital(30, 1000);
    orbital->text = "If I were a writer who had that kinds of chops, I'd be wanting to do something that was deconstructive";
    
    for (int i = 0; i < text->paragraphs.size(); i++) {
        orbital->children.push_back(TUOrbital(*orbital, text->paragraphs[i].str));
        orbital->children.back().bRenderText = false;
        
        for (int j = 0; j < text->paragraphs[i].sentences.size(); j++) {
            orbital->children[i].children.push_back(TUOrbital(orbital->children[i], text->paragraphs[i].sentences[j].str));
            orbital->children[i].children.back().bRenderText = false;
            
            for (int k = 0; k < text->paragraphs[i].sentences[j].words.size(); k++) {
                orbital->children[i].children[j].children.push_back(TUOrbital(orbital->children[i].children[j], text->paragraphs[i].sentences[j].words[k]));
                orbital->children[i].children[j].children.back().bRenderText = true;
            }
        }
    }
    
    // Load the font.
    TUOrbital::font.loadFont("Museo-300.otf", 14);
    TUOrbital::font.setLineLength(150);
    
    bMouseDragged = false;
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5TextUniverse::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5TextUniverse::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5TextUniverse::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::selfUpdate()
{
    currSpin += spinSpeed;
    
    orbital->update(0, 0, 0, false);
}

void CloudsVisualSystemOpenP5TextUniverse::selfDraw()
{
    ofPushStyle();
    ofPushMatrix();
    {
        ofRotate(currSpin, 0, 1, 0);
        
        orbital->draw(getCameraRef(), bMouseDragged);
    }
    ofPopMatrix();
    ofPopStyle();
}

// draw any debug stuff here
void CloudsVisualSystemOpenP5TextUniverse::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOpenP5TextUniverse::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5TextUniverse::selfEnd(){

	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::selfExit()
{
	delete text;
    delete orbital;
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5TextUniverse::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5TextUniverse::selfKeyReleased(ofKeyEventArgs & args){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::selfMouseDragged(ofMouseEventArgs& data)
{
    bMouseDragged = true;
}

void CloudsVisualSystemOpenP5TextUniverse::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5TextUniverse::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::selfMouseReleased(ofMouseEventArgs& data)
{
    bMouseDragged = false;
}