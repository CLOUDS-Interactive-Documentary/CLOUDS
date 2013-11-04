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
    
    customGui->addSpacer();
    textHue = new ofx1DExtruder(0);
    textHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textHue);
    customGui->addSlider("TEXT HUE", 0.0, 255.0, textHue->getPosPtr());
    textSat = new ofx1DExtruder(0);
    textSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textSat);
    customGui->addSlider("TEXT SAT", 0.0, 255.0, textSat->getPosPtr());
    textBri = new ofx1DExtruder(0);
    textBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textBri);
    customGui->addSlider("TEXT BRI", 0.0, 255.0, textBri->getPosPtr());
    textAlpha = new ofx1DExtruder(0);
    textAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textAlpha);
    customGui->addSlider("TEXT ALPHA", 0.0, 255.0, textAlpha->getPosPtr());
	
    customGui->addSpacer();
    customGui->addToggle("ALL CAPS", &TUOrbital::bAllCaps);
    customGui->addSlider("LINE LENGTH", 50, 500, &TUOrbital::lineLength);
    customGui->addSlider("FONT SIZE", 6, 128, &TUOrbital::fontSize);
    customGui->addSlider("FONT DEPTH", 0, 100, &TUOrbital::fontDepth);
    vector<string> fonts;
    fonts.push_back("HELVETICA");
    fonts.push_back("MATERIA PRO");
    fonts.push_back("MUSEO 300");
    fonts.push_back("NEW MEDIA FETT");
    ofxUIDropDownList *ddl = customGui->addDropDownList("FONT", fonts);
    //    ddl->setAutoClose(false);
    ddl->setShowCurrentSelected(true);
    //    ddl->activateToggle("DISABLE");
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5TextUniverse::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "TEXT HUE") {
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
    
    else if (e.widget->getName() == "LINE LENGTH") {
        TUOrbital::font.setLineLength(TUOrbital::lineLength);
    }
    else if (e.widget->getName() == "FONT SIZE" || e.widget->getName() == "FONT DEPTH") {
        TUOrbital::font.loadFont(TUOrbital::fontName, (int)TUOrbital::fontSize, TUOrbital::fontDepth, true);
        TUOrbital::font.setLineLength(TUOrbital::lineLength);
    }
    else if (e.widget->getName() == "HELVETICA") {
        TUOrbital::fontName = "Helvetica.ttf";
        TUOrbital::font.loadFont(TUOrbital::fontName, (int)TUOrbital::fontSize, TUOrbital::fontDepth, true);
        TUOrbital::font.setLineLength(TUOrbital::lineLength);
    }
    else if (e.widget->getName() == "MATERIA PRO") {
        TUOrbital::fontName = "MateriaPro_Light.otf";
        TUOrbital::font.loadFont(TUOrbital::fontName, (int)TUOrbital::fontSize, TUOrbital::fontDepth, true);
        TUOrbital::font.setLineLength(TUOrbital::lineLength);
    }
    else if (e.widget->getName() == "MUSEO 300") {
        TUOrbital::fontName = "Museo-300.otf";
        TUOrbital::font.loadFont(TUOrbital::fontName, (int)TUOrbital::fontSize, TUOrbital::fontDepth, true);
        TUOrbital::font.setLineLength(TUOrbital::lineLength);
    }
    else if (e.widget->getName() == "NEW MEDIA FETT") {
        TUOrbital::fontName = "GUI/NewMedia Fett.ttf";    
        TUOrbital::font.loadFont(TUOrbital::fontName, (int)TUOrbital::fontSize, TUOrbital::fontDepth, true);
        TUOrbital::font.setLineLength(TUOrbital::lineLength);
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
    currSpin = 0.0f;
    spinSpeed = 0.5f;
    
    // Load the contents of the text file.
    ofBuffer buffer = ofBufferFromFile("deconstructive.txt");
    if (buffer.size()) {
        text = new TUText(buffer.getText());
//        text->print();
    }
    
    // Build the node network.
    orbital = new TUOrbital(30, 1000);
    orbital->text = text->paragraphs[0].sentences[0].str;
    orbital->bRenderText = true;
    
    for (int i = 0; i < text->paragraphs.size(); i++) {
        if (text->paragraphs[i].sentences.size() > 1) {
            // Add a "splitter" node for the paragraph.
            orbital->children.push_back(TUOrbital(*orbital, text->paragraphs[i].str));
            orbital->children.back().bRenderText = false;
            
            for (int j = 0; j < text->paragraphs[i].sentences.size(); j++) {
                orbital->children.back().children.push_back(TUOrbital(orbital->children.back(), text->paragraphs[i].sentences[j].str));
                orbital->children.back().children.back().bRenderText = false;
                
                for (int k = 0; k < text->paragraphs[i].sentences[j].words.size(); k++) {
                    orbital->children.back().children.back().children.push_back(TUOrbital(orbital->children.back().children.back(), text->paragraphs[i].sentences[j].words[k]));
                    orbital->children.back().children.back().children.back().bRenderText = true;
                }
            }
        }
        else {
            // Skip the paragraph node.
            for (int j = 0; j < text->paragraphs[i].sentences.size(); j++) {
                orbital->children.push_back(TUOrbital(*orbital, text->paragraphs[i].sentences[j].str));
                orbital->children.back().bRenderText = false;
                
                for (int k = 0; k < text->paragraphs[i].sentences[j].words.size(); k++) {
                    orbital->children.back().children.push_back(TUOrbital(orbital->children.back(), text->paragraphs[i].sentences[j].words[k]));
                    orbital->children.back().children.back().bRenderText = true;
                }
            }
        }
    }
    
    // Load the font.
    TUOrbital::font.loadFont(TUOrbital::fontName, (int)TUOrbital::fontSize, TUOrbital::fontDepth, true);
    TUOrbital::font.setLineLength(TUOrbital::lineLength);
    
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
    // Update the extruders parameters.
    TUOrbital::textColor.setHsb(textHue->getPos(), textSat->getPos(), textBri->getPos(), textAlpha->getPos());
    TUOrbital::lineColor.setHsb(lineHue->getPos(), lineSat->getPos(), lineBri->getPos(), lineAlpha->getPos());
    TUOrbital::nodeColor.setHsb(nodeHue->getPos(), nodeSat->getPos(), nodeBri->getPos(), nodeAlpha->getPos());
    
    currSpin += spinSpeed;
    
    orbital->update(0, 0, 0, false);
}

//--------------------------------------------------------------
void CloudsVisualSystemOpenP5TextUniverse::selfDraw()
{
    ofPushStyle();
    ofPushMatrix();
    {
        ofRotate(currSpin, 0, 1, 0);
        ofSetLineWidth(TUOrbital::lineWidth);
        
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