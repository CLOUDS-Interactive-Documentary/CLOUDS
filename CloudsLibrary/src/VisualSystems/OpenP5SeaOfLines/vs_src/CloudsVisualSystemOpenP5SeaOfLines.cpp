//
//  CloudsVisualSystemOpenP5SeaOfLines.cpp
//

#include "CloudsVisualSystemOpenP5SeaOfLines.h"

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("SEA OF LINES", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("SeaOfLines");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

    customGui->addIntSlider("NUM PARTICLES", 1, 2000, &numParticles);

    customGui->addSpacer();
    customGui->addSlider("BG ALPHA", 0.0, 1.0, &bgAlpha);
    
    customGui->addSpacer();
    customGui->addRangeSlider("DISTANCES", 1.0f, 100.0f, &collideDist, &lineDist);
    customGui->addSlider("BASE DISTANCE", 1.0f, 100.0f, &baseDist);
    customGui->addRangeSlider("TRAVEL SPEED", 1.0f, 10.0f, &minSpeed, &maxSpeed);
    customGui->addSlider("CURSOR RANGE", 0.0f, 1.0f, &cursorRange);
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
void CloudsVisualSystemOpenP5SeaOfLines::selfSetDefaults()
{
    collideDist = 1.0f;
    lineDist = 30.0f;
    baseDist = 2.0f;
    minSpeed = 4.0f;
    maxSpeed = 6.0f;
    cursorRange = 1.0f;
    gravity = 0.1f;
    
    numParticles = 500;

    primaryCursorMode   = CURSOR_MODE_DRAW;
    secondaryCursorMode = CURSOR_MODE_INACTIVE;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.getName() == "DISTANCES") {
        baseDist = MAX(baseDist, collideDist + 1.0f);
    }
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::generateParticles(){
    
    players.clear();
    
    // Add the players.
    float step = 20;
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    
    //jg control player count by looping over this until it's done
    while(true){
        for (float i = 0; i < (getCanvasWidth() / step - 1); i++) {
            for (float j = 0; j < (getCanvasHeight() / step - 1); j++) {
                if (ofRandom(3) > 1) {
                    
                    players.push_back(SOLPlayer());
                    
                    SOLPlayer& player = players.back();
                    
                    player.x = i * step + step * 0.5f;
                    player.y = j * step + step * 0.5f;
                    
                    player.speed = ofRandom(minSpeed, maxSpeed);
                    
                    float angle = ofRandom(TWO_PI);
                    player.sx = cosf(angle) * player.speed;
                    player.sy = sinf(angle) * player.speed;
                    
                    if(players.size() == numParticles){
                        return;
                    }
                }
            }
        }
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5SeaOfLines::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5SeaOfLines::guiSystemEvent(ofxUIEventArgs &e){
	
}

//use render gui for display settings, like changing colors
//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfSetupRenderGui(){

}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfSetup()
{
    bIs2D = true;
    bClearBackground = false;
    
    mesh.setMode(OF_PRIMITIVE_LINES);
    mesh.setUsage(GL_STREAM_DRAW);
    
    

}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5SeaOfLines::selfPresetLoaded(string presetPath){


    generateParticles();

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
    
    if(players.size() != numParticles){
        generateParticles();
    }
    
    mesh.clear();
    lineColor1.setHsb(lineParams1[0], lineParams1[1], lineParams1[2], lineParams1[3]);
    lineColor2.setHsb(lineParams2[0], lineParams2[1], lineParams2[2], lineParams2[3]);

    // First pass: Update player position and bounce off walls.
    for (int i = 0; i < players.size(); i++) {
        players[i].x += players[i].sx;
        players[i].y += players[i].sy;
        
        players[i].sy += gravity;
        
        // Bounce off walls.
        if (players[i].x < 0) {
            players[i].x = 0;
            players[i].sx *= -1;
//            players[i].sx = ABS(players[i].sx);
//            players[i].x += getCanvasWidth();
        }
        else if (players[i].x > getCanvasWidth()) {
            players[i].x = getCanvasWidth();
            players[i].sx *= -1;
//            players[i].sx = ABS(players[i].sx) * -1;
//            players[i].x -= getCanvasWidth();
        }
        if (players[i].y < 0) {
            players[i].y = 0;
            players[i].sy *= -1;
        }
        else if (players[i].y > getCanvasHeight()) {
            players[i].y = getCanvasHeight();
            players[i].sy *= -1;
        }
        
        mesh.addVertex(ofVec3f(players[i].x, players[i].y));
        mesh.addColor((i%2 == 0)? lineColor1:lineColor2);
    }

    // Second pass: Handle collisions and proximity.
    float cursorMaxRadius = getCanvasWidth() * cursorRange;
    float cursorDist, mappedLineDist;
    for (int i = 0; i < players.size(); i++) {
        SOLPlayer& one = players[i];
        cursorDist = ofDist(one.x, one.y, cursor.x, cursor.y);
        if (cursorMaxRadius == 0) {
            mappedLineDist = lineDist;
        }
        else {
            mappedLineDist = ofMap(cursorDist, 0, cursorMaxRadius, lineDist, collideDist);
        }

        for (int j = i + 1; j < players.size(); j++) {
            SOLPlayer& two = players[j];
            float dist = ofDist(one.x, one.y, two.x, two.y);
            if (dist < collideDist) {
                one.speed = ofRandom(minSpeed, maxSpeed);
                
                float ang = atan2f(one.y - two.y, one.x - two.x);
                one.sx = cosf(ang) * one.speed;
                one.sy = sinf(ang) * one.speed;
            }
            else if (dist < mappedLineDist || dist < baseDist) {
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
//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfDrawDebug(){

}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfDrawBackground()
{
    ofSetColor(bgColor, bgAlpha * 255);

    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    ofRect(0, 0, getCanvasWidth(), getCanvasHeight());
    glDisable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);

    ofSetColor(255);
    
    mesh.draw();
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5SeaOfLines::selfEnd(){
    players.clear();
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfExit()
{
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
    cursor.set(data.x, data.y);
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5SeaOfLines::selfMouseMoved(ofMouseEventArgs& data)
{
    cursor.set(data.x, data.y);
}

void CloudsVisualSystemOpenP5SeaOfLines::selfMousePressed(ofMouseEventArgs& data){
	
}
void CloudsVisualSystemOpenP5SeaOfLines::selfMouseReleased(ofMouseEventArgs& data){

}
