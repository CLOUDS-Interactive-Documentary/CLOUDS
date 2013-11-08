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
    customGui->addToggle("TEXT CLOUD", &bTextCloudMode);
    customGui->addSlider("SPIN SPEED", 0, 5, &spinSpeed);
    customGui->addSlider("MOUSE SPEED", 0, 5, &mouseSpeed);
    customGui->addSlider("FOG DENSITY", 0.0f, 0.1f, &fogDensity);
    
    vector<string> radioBillboard;
    radioBillboard.push_back("BILLBOARD SCREEN");
    radioBillboard.push_back("BILLBOARD NODES");
    radioBillboard.push_back("BILLBOARD ORIGIN");
    customGui->addRadio("BILLBOARD", radioBillboard);
    
    customGui->addSpacer();
    customGui->addSlider("LINE WIDTH", 0.0, 10.0, &TUOrbital::lineWidth);
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
    
    customGui->addSpacer();
    customGui->addSlider("NODE SCALAR", 0.0, 20.0, &TUOrbital::nodeScalar);
    nodeHue = new ofx1DExtruder(0);
    nodeHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(nodeHue);
    customGui->addSlider("NODE HUE", 0.0, 255.0, nodeHue->getPosPtr());
    nodeSat = new ofx1DExtruder(0);
    nodeSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(nodeSat);
    customGui->addSlider("NODE SAT", 0.0, 255.0, nodeSat->getPosPtr());
    nodeBri = new ofx1DExtruder(0);
    nodeBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(nodeBri);
    customGui->addSlider("NODE BRI", 0.0, 255.0, nodeBri->getPosPtr());
    nodeAlpha = new ofx1DExtruder(0);
    nodeAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(nodeAlpha);
    customGui->addSlider("NODE ALPHA", 0.0, 255.0, nodeAlpha->getPosPtr());
    
    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5TextUniverse::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
    
    revealGui = new ofxUISuperCanvas("REVEAL", gui);
	revealGui->copyCanvasStyle(gui);
	revealGui->copyCanvasProperties(gui);
	revealGui->setName("Reveal");
	revealGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    revealGui->addSpacer();
    revealGui->addToggle("RESTART", &bRestart);
    revealGui->addRangeSlider("FADE TIME", 100, 5000, &TUOrbital::minFadeTime, &TUOrbital::maxFadeTime);
    revealGui->addRangeSlider("LINE TIME", 100, 5000, &TUOrbital::minLineTime, &TUOrbital::maxLineTime);
    
    revealGui->addSpacer();
    vector<string> modes;
    modes.push_back("INSTANT");
    modes.push_back("LEVELS");
    modes.push_back("ORDERED");
    modes.push_back("RANDOM");
    revealGui->addRadio("MODE", modes);
    
    ofAddListener(revealGui->newGUIEvent, this, &CloudsVisualSystemOpenP5TextUniverse::selfGuiEvent);
	guis.push_back(revealGui);
	guimap[customGui->getName()] = revealGui;
    
    textGui = new ofxUISuperCanvas("TEXT", gui);
	textGui->copyCanvasStyle(gui);
	textGui->copyCanvasProperties(gui);
	textGui->setName("Text");
	textGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    textGui->addSpacer();
    textHue = new ofx1DExtruder(0);
    textHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textHue);
    textGui->addSlider("TEXT HUE", 0.0, 255.0, textHue->getPosPtr());
    textSat = new ofx1DExtruder(0);
    textSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textSat);
    textGui->addSlider("TEXT SAT", 0.0, 255.0, textSat->getPosPtr());
    textBri = new ofx1DExtruder(0);
    textBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textBri);
    textGui->addSlider("TEXT BRI", 0.0, 255.0, textBri->getPosPtr());
    textAlpha = new ofx1DExtruder(0);
    textAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textAlpha);
    textGui->addSlider("TEXT ALPHA", 0.0, 255.0, textAlpha->getPosPtr());
    
    textGui->addSpacer();
    textGui->addToggle("ALL CAPS", &TUOrbital::bAllCaps);
    textGui->addSlider("LINE LENGTH", 50, 500, &TUOrbital::lineLength);
    textGui->addSlider("FONT SIZE", 6, 128, &TUOrbital::fontSize);
    textGui->addSlider("FONT DEPTH", 0, 100, &TUOrbital::fontDepth);
    
    textGui->addSpacer();
    vector<string> aligns;
    aligns.push_back("LEFT");
    aligns.push_back("CENTER");
    aligns.push_back("RIGHT");
    aligns.push_back("JUSTIFY");
    textGui->addRadio("ALIGN", aligns);
    
    textGui->addSpacer();
    vector<string> fonts;
    fonts.push_back("HELVETICA");
    fonts.push_back("MATERIA PRO");
    fonts.push_back("MUSEO 300");
    fonts.push_back("NEW MEDIA FETT");
    textGui->addRadio("FONT", fonts);
    
    textGui->addSpacer();
    vector<string> fileNames;
    for (int i = 0; i < filesDir.size(); i++) {
        fileNames.push_back(filesDir.getName(i));
    }
    ddlFiles = textGui->addDropDownList("FILES", fileNames);
    //    ddlFiles->setAutoClose(true);
    ddlFiles->setShowCurrentSelected(true);
    //    ddlFiles->activateToggle("DISABLE");
    
    ofAddListener(textGui->newGUIEvent, this, &CloudsVisualSystemOpenP5TextUniverse::selfGuiEvent);
	guis.push_back(textGui);
	guimap[customGui->getName()] = textGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "TEXT CLOUD") {
        rebuildText();
    }
    
    else if (e.widget->getName() == "BILLBOARD SCREEN") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::billboardType = 0;
        }
    }
    else if (e.widget->getName() == "BILLBOARD NODES") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::billboardType = 1;
        }
    }
    else if (e.widget->getName() == "BILLBOARD ORIGIN") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::billboardType = 2;
        }
    }
    
    else if (e.widget->getName() == "TEXT HUE") {
        textHue->setPosAndHome(textHue->getPos());
	}
    else if (e.widget->getName() == "TEXT SAT") {
        textSat->setPosAndHome(textSat->getPos());
	}
    else if (e.widget->getName() == "TEXT BRI") {
        textBri->setPosAndHome(textBri->getPos());
	}
    else if (e.widget->getName() == "TEXT ALPHA") {
        textAlpha->setPosAndHome(textAlpha->getPos());
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
    
    else if (e.widget->getName() == "NODE HUE") {
        nodeHue->setPosAndHome(nodeHue->getPos());
	}
    else if (e.widget->getName() == "NODE SAT") {
        nodeSat->setPosAndHome(nodeSat->getPos());
	}
    else if (e.widget->getName() == "NODE BRI") {
        nodeBri->setPosAndHome(nodeBri->getPos());
	}
    else if (e.widget->getName() == "NODE ALPHA") {
        nodeAlpha->setPosAndHome(nodeAlpha->getPos());
    }
    
    else if (e.widget->getName() == "INSTANT") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::revealMode = REVEAL_INSTANT;
            bRestart = true;
        }
    }
    else if (e.widget->getName() == "LEVELS") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::revealMode = REVEAL_LEVELS;
            bRestart = true;
        }
    }
    else if (e.widget->getName() == "ORDERED") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::revealMode = REVEAL_ORDERED;
            bRestart = true;
        }
    }
    else if (e.widget->getName() == "RANDOM") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::revealMode = REVEAL_RANDOM;
            bRestart = true;
        }
    }
    
    else if (e.widget->getName() == "LINE LENGTH") {
        TUOrbital::font.setLineLength(TUOrbital::lineLength);
    }
    else if (e.widget->getName() == "FONT SIZE" || e.widget->getName() == "FONT DEPTH") {
        rebuildFont();
    }
    
    else if (e.widget->getName() == "LEFT") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::textAlign = FTGL_ALIGN_LEFT;
            TUOrbital::font.setAlignment(TUOrbital::textAlign);
        }
    }
    else if (e.widget->getName() == "CENTER") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::textAlign = FTGL_ALIGN_CENTER;
            TUOrbital::font.setAlignment(TUOrbital::textAlign);
        }
    }
    else if (e.widget->getName() == "RIGHT") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::textAlign = FTGL_ALIGN_RIGHT;
            TUOrbital::font.setAlignment(TUOrbital::textAlign);
        }
    }
    else if (e.widget->getName() == "JUSTIFY") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::textAlign = FTGL_ALIGN_JUSTIFY;
            TUOrbital::font.setAlignment(TUOrbital::textAlign);
        }
    }
    
    else if (e.widget->getName() == "HELVETICA") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::fontName = getVisualSystemDataPath() + "fonts/Helvetica.ttf";
            rebuildFont();
        }
    }
    else if (e.widget->getName() == "MATERIA PRO") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::fontName = getVisualSystemDataPath() + "fonts/MateriaPro_Light.otf";
            rebuildFont();
        }
    }
    else if (e.widget->getName() == "MUSEO 300") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::fontName = getVisualSystemDataPath() + "fonts/Museo-300.otf";
            rebuildFont();
        }
    }
    else if (e.widget->getName() == "NEW MEDIA FETT") {
        if (((ofxUIToggle *)e.widget)->getValue()) {
            TUOrbital::fontName = getVisualSystemDataPath() + "fonts/NewMedia Fett.ttf";
            rebuildFont();
        }
    }
    
    else {
        // Let's look through the files dropdown for a match.
        for (int i = 0; i < ddlFiles->getToggles().size(); i++) {
            if (e.widget == ddlFiles->getToggles()[i] && ((ofxUIToggle *)e.widget)->getValue()) {
                selectedFilesIdx = i;
                rebuildText();
                break;
            }
        }
    }
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
    bRestart = true;
    
    currSpin = 0.0f;
    spinSpeed = 0.5f;
    mouseSpinX = mouseSpinY = 0.0f;
    mouseSpeed = 1.0f;
    fogDensity = 0.025f;
    bTextCloudMode = false;
    
    orbital = NULL;

    filesDir.listDir(getVisualSystemDataPath() + "textFiles");
    filesDir.sort();
    selectedFilesIdx = 0;
    rebuildText();
    
    TUOrbital::fontName = getVisualSystemDataPath() + "fonts/Helvetica.ttf";
    rebuildFont();
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
    if (bRestart) {
        orbital->restart(0);
        
        bRestart = false;
    }
    
    // Update the extruders parameters.
    TUOrbital::textColor.setHsb(textHue->getPos(), textSat->getPos(), textBri->getPos(), textAlpha->getPos());
    TUOrbital::lineColor.setHsb(lineHue->getPos(), lineSat->getPos(), lineBri->getPos(), lineAlpha->getPos());
    TUOrbital::nodeColor.setHsb(nodeHue->getPos(), nodeSat->getPos(), nodeBri->getPos(), nodeAlpha->getPos());
    
    currSpin += spinSpeed;
    
    mouseSpinX += mouseDir.x * mouseSpeed;
    mouseSpinY += mouseDir.y * mouseSpeed;
    
    orbital->update(0, 0, 0);
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::selfDraw()
{
    glPushAttrib(GL_FOG_BIT);
    
    glEnable(GL_FOG);
	glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
	glFogi(GL_FOG_MODE, GL_EXP);
    
    //	float FogCol[3]={0.8f,0.8f,0.8f}; // Define a nice light grey
    //	glFogfv(GL_FOG_COLOR, FogCol);     // Set the fog color
	glFogf(GL_FOG_DENSITY, powf(fogDensity, 2));
    GLfloat fogColor[4] = { bgColor.r / 255.,bgColor.g / 255.,bgColor.b / 255., 1.0 };
    glFogfv(GL_FOG_COLOR, fogColor);
    glEnable(GL_DEPTH_TEST);
    ofEnableAlphaBlending();

//		GLfloat fogColor[4] = {0.0, 0.0, 0.0, 1.0};
//		glFogfv (GL_FOG_COLOR, fogColor);
//		glDisable(GL_DEPTH_TEST);
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    ofPushStyle();
    ofPushMatrix();
    {
        ofRotate(currSpin, 0, 1, 0);
        ofRotateX(mouseSpinX);
        ofRotateY(mouseSpinY);
        ofSetLineWidth(TUOrbital::lineWidth);
        
        orbital->draw(getCameraRef());
    }
    ofPopMatrix();
    ofPopStyle();
    
    glPopAttrib();
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
    if (orbital != NULL) {
        delete orbital;
    }
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5TextUniverse::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5TextUniverse::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5TextUniverse::selfMouseDragged(ofMouseEventArgs& data){

}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::selfMouseMoved(ofMouseEventArgs& data)
{
    mouseDir.y = ofMap(data.x, 0, ofGetWidth(), -1, 1);
    mouseDir.x = ofMap(data.y, 0, ofGetHeight(), -1, 1);
}

void CloudsVisualSystemOpenP5TextUniverse::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5TextUniverse::selfMouseReleased(ofMouseEventArgs& data){
    
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::rebuildFont()
{
    TUOrbital::font.loadFont(TUOrbital::fontName, (int)TUOrbital::fontSize, TUOrbital::fontDepth, true);
    TUOrbital::font.setLineLength(TUOrbital::lineLength);
    TUOrbital::font.setAlignment(TUOrbital::textAlign);
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::rebuildText()
{
    // Load the contents of the text file.
    TUText * text = NULL;
    ofBuffer buffer = ofBufferFromFile(filesDir.getPath(selectedFilesIdx));
    if (buffer.size()) {
        text = new TUText(buffer.getText());
//        text->print();
    }
    else {
        ofLogError("OpenP5TextUniverse") << "Could not open text file " << filesDir.getPath(selectedFilesIdx);
        return;
    }
    
    // Build the node network.
    if (orbital != NULL) {
        delete orbital;
    }
    
    TUOrbital::numOrbitals = 0;
    orbital = new TUOrbital(30, 1000);
    
    if (bTextCloudMode) {
        // Render a single level of words.
        orbital->bRenderText = false;
        
        for (int i = 0; i < text->paragraphs.size(); i++) {
            for (int j = 0; j < text->paragraphs[i].sentences.size(); j++) {
                for (int k = 0; k < text->paragraphs[i].sentences[j].words.size(); k++) {
                    orbital->children.push_back(new TUOrbital(orbital, text->paragraphs[i].sentences[j].words[k]));
                    orbital->children.back()->bRenderText = true;
                }
            }
        }
    }
    else {
        // Render nodes for paragraphs, sentences, and words.
        orbital->text = text->paragraphs[0].sentences[0].str;
        orbital->bRenderText = true;
        
        for (int i = 0; i < text->paragraphs.size(); i++) {
            if (text->paragraphs[i].sentences.size() > 1) {
                // Add a "splitter" node for the paragraph.
                orbital->children.push_back(new TUOrbital(orbital, text->paragraphs[i].str));
                orbital->children.back()->bRenderText = false;
                
                for (int j = 0; j < text->paragraphs[i].sentences.size(); j++) {
                    if (text->paragraphs[i].sentences[j].words.size() > 1) {
                        // Add a "splitter" node for the sentence.
                        orbital->children.back()->children.push_back(new TUOrbital(orbital->children.back(), text->paragraphs[i].sentences[j].str));
                        orbital->children.back()->children.back()->bRenderText = false;
                    
                        for (int k = 0; k < text->paragraphs[i].sentences[j].words.size(); k++) {
                            orbital->children.back()->children.back()->children.push_back(new TUOrbital(orbital->children.back()->children.back(), text->paragraphs[i].sentences[j].words[k]));
                            orbital->children.back()->children.back()->children.back()->bRenderText = true;
                        }
                    }
                    else {
                        // Skip the sentence node.
                        for (int k = 0; k < text->paragraphs[i].sentences[j].words.size(); k++) {
                            orbital->children.back()->children.push_back(new TUOrbital(orbital->children.back(), text->paragraphs[i].sentences[j].words[k]));
                            orbital->children.back()->children.back()->bRenderText = true;
                        }
                    }
                }
            }
            else {
                // Skip the paragraph node.
                for (int j = 0; j < text->paragraphs[i].sentences.size(); j++) {
                    if (text->paragraphs[i].sentences[j].words.size() > 1) {
                        // Add a "splitter" node for the sentence.
                        orbital->children.push_back(new TUOrbital(orbital, text->paragraphs[i].sentences[j].str));
                        orbital->children.back()->bRenderText = false;
                        
                        for (int k = 0; k < text->paragraphs[i].sentences[j].words.size(); k++) {
                            orbital->children.back()->children.push_back(new TUOrbital(orbital->children.back(), text->paragraphs[i].sentences[j].words[k]));
                            orbital->children.back()->children.back()->bRenderText = true;
                        }
                    }
                    else {
                        // Skip the sentence node.
                        for (int k = 0; k < text->paragraphs[i].sentences[j].words.size(); k++) {
                            orbital->children.push_back(new TUOrbital(orbital, text->paragraphs[i].sentences[j].words[k]));
                            orbital->children.back()->bRenderText = true;
                        }
                    }
                }
            }
        }
    }
    
    delete text;
}
