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
    
    customGui->addSpacer();
    customGui->addSlider("BG ALPHA", 0.0, 1.0, &bgAlpha);
    
    customGui->addSpacer();
    customGui->addRangeSlider("DISTANCES", 1.0f, 100.0f, &collideDist, &lineDist);
    customGui->addRangeSlider("TRAVEL SPEED", 1.0f, 10.0f, &minSpeed, &maxSpeed);
    customGui->addSlider("GRAVITY", -1.0f, 1.0f, &gravity);

    customGui->addSpacer();
    customGui->addSlider("LINE HUE 1", 0.0, 0.99999, &lineParams1[0]);
    customGui->addSlider("LINE SAT 1", 0.0, 1.0, &lineParams1[1]);
    customGui->addSlider("LINE BRI 1", 0.0, 1.0, &lineParams1[2]);
    customGui->addSlider("LINE ALPHA 1", 0.0, 1.0, &lineParams1[3]);
    
    customGui->addSpacer();
    customGui->addSlider("LINE HUE 2", 0.0, 0.99999, &lineParams2[0]);
    customGui->addSlider("LINE SAT 2", 0.0, 1.0, &lineParams2[1]);
    customGui->addSlider("LINE BRI 2", 0.0, 1.0, &lineParams2[2]);
    customGui->addSlider("LINE ALPHA 2", 0.0, 1.0, &lineParams2[3]);
	
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
    // Set defaults.
    collideDist = 1.0f;
    lineDist = 30.0f;
    minSpeed = 4.0f;
    maxSpeed = 6.0f;
    gravity = 0.1f;
	
    bIs2D = true;
    bClearBackground = false;
    
    mesh.setMode(OF_PRIMITIVE_LINES);
    mesh.setUsage(GL_STREAM_DRAW);
    
    // Add the players.
    float step = 20;
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    for (float i = 0; i < (getCanvasWidth() / step - 1); i++) {
        for (float j = 0; j < (getCanvasHeight() / step - 1); j++) {
            if (ofRandom(3) > 1) {
                SOLPlayer * player = new SOLPlayer();
                
                player->x = i * step + step * 0.5f;
                player->y = j * step + step * 0.5f;
                
                player->speed = ofRandom(minSpeed, maxSpeed);
                
                float angle = ofRandom(TWO_PI);
                player->sx = cosf(angle) * player->speed;
                player->sy = sinf(angle) * player->speed;
                
                players.push_back(player);
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
    mesh.clear();
    lineColor1.setHsb(lineParams1[0], lineParams1[1], lineParams1[2], lineParams1[3]);
    lineColor2.setHsb(lineParams2[0], lineParams2[1], lineParams2[2], lineParams2[3]);

    // First pass: Update player position.
    for (int i = 0; i < players.size(); i++) {
        players[i]->x += players[i]->sx;
        players[i]->y += players[i]->sy;
        
        players[i]->sy += gravity;
        
        // Bounce off walls.
        if (players[i]->x < 0) {
            players[i]->x = 0;
            players[i]->sx *= -1;
        }
        //MA: changed ofGetWidth() to getCanvasWidth()
        else if (players[i]->x > getCanvasWidth()) {
            players[i]->x = getCanvasWidth();
            players[i]->sx *= -1;
        }
        if (players[i]->y < 0) {
            players[i]->y = 0;
            players[i]->sy *= -1;
        }
        //MA: changed ofGetHeight() to getCanvasHeight()
        else if (players[i]->y > getCanvasHeight()) {
            players[i]->y = getCanvasHeight();
            players[i]->sy *= -1;
        }
        
        mesh.addVertex(ofVec3f(players[i]->x, players[i]->y));
        mesh.addColor((i%2 == 0)? lineColor1:lineColor2);
    }
    
    // Second pass: Handle collisions and proximity.
    for (int i = 0; i < players.size(); i++) {
        SOLPlayer * one = players[i];
        for (int j = i + 1; j < players.size(); j++) {
            SOLPlayer * two = players[j];
            float dist = ofDist(one->x, one->y, two->x, two->y);
            if (dist < collideDist) {
                one->speed = ofRandom(minSpeed, maxSpeed);
                
                float ang = atan2f(one->y - two->y, one->x - two->x);
                one->sx = cosf(ang) * one->speed;
                one->sy = sinf(ang) * one->speed;
            }
            else if (dist < lineDist) {
                mesh.addIndex(i);
                mesh.addIndex(j);
            }
        }
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
    ofSetColor(bgColor, bgAlpha * 255);
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    ofRect(0, 0, getCanvasWidth(), getCanvasHeight());
    glDisable(GL_LINE_SMOOTH);
    ofSetColor(255);
    
    mesh.draw();
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

void CloudsVisualSystemOpenP5SeaOfLines::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemOpenP5SeaOfLines::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SeaOfLines::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SeaOfLines::selfMouseReleased(ofMouseEventArgs& data){

}
