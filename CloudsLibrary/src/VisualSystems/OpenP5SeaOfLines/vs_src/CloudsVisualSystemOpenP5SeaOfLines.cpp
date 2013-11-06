//
//  CloudsVisualSystemOpenP5SeaOfLines.cpp
//

#include "CloudsVisualSystemOpenP5SeaOfLines.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("SEA OF LINES", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("SeaOfLines");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5SeaOfLines::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfGuiEvent(ofxUIEventArgs &e)
{
    
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5SeaOfLines::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5SeaOfLines::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5SeaOfLines::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5SeaOfLines::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfSetup()
{
    SOLPlayer::width = ofGetWidth();
    SOLPlayer::height = ofGetHeight();
    
    canvas.allocate(SOLPlayer::width, SOLPlayer::height);
    canvas.begin();
    {
        ofClear(0, 0);
    }
    canvas.end();
    
    // Add the players.
    float step = 20;
    for (float i = 0; i < (SOLPlayer::width / step - 1); i++) {
        for (float j = 0; j < (SOLPlayer::height / step - 1); j++) {
            if (ofRandom(3) > 1) {
                players.push_back(new SOLPlayer(i * step + step * 0.5f, j * step + step * 0.5f, step * 0.8f));
            }
        }
    }
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5SeaOfLines::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5SeaOfLines::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5SeaOfLines::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfUpdate()
{
    for (int i = 0; i < players.size(); i++) {
        players[i]->update();
    }
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfDraw()
{

}

// draw any debug stuff here
void CloudsVisualSystemOpenP5SeaOfLines::selfDrawDebug(){

}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfDrawBackground()
{
    canvas.begin();
    {
        ofSetColor(bgColor, 20);
        ofRect(0, 0, canvas.getWidth(), canvas.getHeight());
        
        ofSetColor(255, 128);
        glBegin(GL_LINES);
        for (int i = 0; i < players.size(); i++) {
            SOLPlayer * one = players[i];
            for (int j = i + 1; j < players.size(); j++) {
                SOLPlayer * two = players[j];
                float dist = ofDist(one->x, one->y, two->x, two->y);
                if (dist < ((one->size + two->size) / 2)) {
                    float ang = atan2f(one->y - two->y, one->x - two->x);
                    one->sx = cosf(ang) * one->speed;
                    one->sy = sinf(ang) * one->speed;
                }
                else if (dist < 30) {
                    glVertex2f(one->x, one->y);
                    glVertex2f(two->x, two->y);
                }
            }
        }
        glEnd();
    }
    canvas.end();
    
    canvas.draw(0, 0, ofGetWidth(), ofGetHeight());
    ofDrawBitmapString(ofToString(ofGetFrameRate(), 2) + " FPS", 10, ofGetHeight() - 20);
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5SeaOfLines::selfEnd(){

	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfExit()
{
    for (int i = 0; i < players.size(); i++) {
        delete players[i];
    }
    players.clear();
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5SeaOfLines::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5SeaOfLines::selfKeyReleased(ofKeyEventArgs & args){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfMouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemOpenP5SeaOfLines::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SeaOfLines::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfMouseReleased(ofMouseEventArgs& data)
{

}