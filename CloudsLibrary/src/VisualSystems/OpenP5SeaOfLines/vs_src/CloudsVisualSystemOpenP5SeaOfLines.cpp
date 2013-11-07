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
    bgAlpha = new ofx1DExtruder(0);
    bgAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(bgAlpha);
    customGui->addSlider("BG ALPHA", 0.0, 255.0, bgAlpha->getPosPtr());
    
    customGui->addSpacer();
    customGui->addRangeSlider("DISTANCES", 1.0f, 100.0f, &collideDist, &lineDist);
    customGui->addRangeSlider("TRAVEL SPEED", 1.0f, 10.0f, &minSpeed, &maxSpeed);
    customGui->addSlider("GRAVITY", -1.0f, 1.0f, &gravity);

    customGui->addSpacer();
    lineHue = new ofx1DExtruder(0);
    lineHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(lineHue);
    customGui->addSlider("LINE HUE", 0.0, 255.0, lineHue->getPosPtr());
    lineSat = new ofx1DExtruder(0);
    lineSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(lineSat);
    customGui->addSlider("LINE SAT", 0.0, 255.0, lineSat->getPosPtr());
    lineBri = new ofx1DExtruder(0);
    lineBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(lineBri);
    customGui->addSlider("LINE BRI", 0.0, 255.0, lineBri->getPosPtr());
    lineAlpha = new ofx1DExtruder(0);
    lineAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(lineAlpha);
    customGui->addSlider("LINE ALPHA", 0.0, 255.0, lineAlpha->getPosPtr());
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5SeaOfLines::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "BG ALPHA") {
        bgAlpha->setPosAndHome(bgAlpha->getPos());
	}
    
    else if (e.widget->getName() == "LINE HUE") {
        lineHue->setPosAndHome(lineHue->getPos());
	}
    else if (e.widget->getName() == "LINE SAT") {
        lineSat->setPosAndHome(lineSat->getPos());
	}
    else if (e.widget->getName() == "LINE BRI") {
        lineBri->setPosAndHome(lineBri->getPos());
	}
    else if (e.widget->getName() == "LINE ALPHA") {
        lineAlpha->setPosAndHome(lineAlpha->getPos());
    }
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
    
    canvas.allocate(ofGetWidth(), ofGetHeight());
    canvas.begin();
    {
        ofClear(0, 0);
    }
    canvas.end();
    
    // Add the players.
    float step = 20;
    for (float i = 0; i < (canvas.getWidth() / step - 1); i++) {
        for (float j = 0; j < (canvas.getHeight() / step - 1); j++) {
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
    // Update player position.
    for (int i = 0; i < players.size(); i++) {
        players[i]->x += players[i]->sx;
        players[i]->y += players[i]->sy;
        
        players[i]->sy += gravity;
        
        // Bounce off walls.
        if (players[i]->x < 0) {
            players[i]->x = 0;
            players[i]->sx *= -1;
        }
        else if (players[i]->x > canvas.getWidth()) {
            players[i]->x = canvas.getWidth();
            players[i]->sx *= -1;
        }
        if (players[i]->y < 0) {
            players[i]->y = 0;
            players[i]->sy *= -1;
        }
        else if (players[i]->y > canvas.getHeight()) {
            players[i]->y = canvas.getHeight();
            players[i]->sy *= -1;
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
    canvas.begin();
    {
        ofSetColor(bgColor, bgAlpha->getPos());
        ofRect(0, 0, canvas.getWidth(), canvas.getHeight());
        
        ofSetColor(ofColor::fromHsb(lineHue->getPos(), lineSat->getPos(), lineBri->getPos(), lineAlpha->getPos()));
        glBegin(GL_LINES);
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
                    glVertex2f(one->x, one->y);
                    glVertex2f(two->x, two->y);
                }
            }
        }
        glEnd();
    }
    canvas.end();
    
    ofSetColor(255);
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

void CloudsVisualSystemOpenP5SeaOfLines::selfMouseDragged(ofMouseEventArgs& data){

}

void CloudsVisualSystemOpenP5SeaOfLines::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SeaOfLines::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SeaOfLines::selfMouseReleased(ofMouseEventArgs& data){

}
