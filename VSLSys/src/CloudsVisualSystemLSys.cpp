//
//  CloudsVisualSystemLSys.cpp
//  VSLSys
//
//  Created by Patricio Gonzalez Vivo on 5/25/13.
//
//

#include "CloudsVisualSystemLSys.h"

CloudsVisualSystemLSys::CloudsVisualSystemLSys(){
    
	int buttonWidth = 380;
	int buttonHeight = 30;
    gui = new ofxUICanvas(0,0,buttonWidth,ofGetHeight());    
    gui->addLabel("LSystem");
    gui->addTextInput("Axiom", "B");
    gui->addTextInput("Rule1", "B=F[5+B][7-B]-F[4+B][6-B]-[3+B][5+B]-FB");
    gui->addTextInput("Rule2", "F=FF");
    gui->addSlider("Angle", 0, 90, 5);
    gui->addSlider("Scale", 0.5, 2, 1.5);
    gui->addSlider("Depth", 1, 8, 4);
    
    gui->addLabel("Growing");
    gui->addSlider("Speed", 0.0, 10, 1.0);
    
    gui->disable();
    gui->loadSettings("GUI/guiSettings.xml");
    
    ofAddListener(gui->newGUIEvent,this,&CloudsVisualSystemLSys::guiEvent);
    
    ofxUISlider *uiAngle = (ofxUISlider *) gui->getWidget("Angle");
    angle = uiAngle->getScaledValue();
    ofxUITextInput *uiAxiom = (ofxUITextInput *) gui->getWidget("Axiom");
    axiom = uiAxiom->getTextString();
    ofxUITextInput *uiRule1 = (ofxUITextInput *) gui->getWidget("Rule1");
    rule1 = uiRule1->getTextString();
    ofxUITextInput *uiRule2 = (ofxUITextInput *) gui->getWidget("Rule2");
    rule2 = uiRule2->getTextString();
    ofxUISlider *uiScale = (ofxUISlider *) gui->getWidget("Scale");
    lsysScale = uiScale->getScaledValue();
    ofxUISlider *uiDepth = (ofxUISlider *) gui->getWidget("Depth");
    lsysDepth = uiDepth->getScaledValue();
    ofxUISlider *uiSpeed = (ofxUISlider *) gui->getWidget("Speed");
    lsysr.speed = uiSpeed->getScaledValue();

    reBuildLSys();
}

CloudsVisualSystemLSys::~CloudsVisualSystemLSys(){
    
}

string CloudsVisualSystemLSys::getSystemName(){
    return "LSystem";
}

void CloudsVisualSystemLSys::begin(){
    gui->enable();
}

void CloudsVisualSystemLSys::end(){
    gui->disable();
}

void CloudsVisualSystemLSys::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    if(name == "Angle"){
        ofxUISlider *uiAngle = (ofxUISlider *) gui->getWidget("Angle");
        angle = uiAngle->getScaledValue();
        reBuildLSys();
    } else if ( name == "Axiom"){
        ofxUITextInput *uiAxiom = (ofxUITextInput *) gui->getWidget("Axiom");
        axiom = uiAxiom->getTextString();
        reBuildLSys();
    } else if ( name == "Rule1"){
        ofxUITextInput *uiRule1 = (ofxUITextInput *) gui->getWidget("Rule1");
        rule1 = uiRule1->getTextString();
        reBuildLSys();
    } else if ( name == "Rule2"){
        ofxUITextInput *uiRule2 = (ofxUITextInput *) gui->getWidget("Rule2");
        rule2 = uiRule2->getTextString();
        reBuildLSys();
    } else if ( name == "Scale"){
        ofxUISlider *uiScale = (ofxUISlider *) gui->getWidget("Scale");
        lsysScale = uiScale->getScaledValue();
        reBuildLSys();
    } else if ( name == "Depth"){
        ofxUISlider *uiDepth = (ofxUISlider *) gui->getWidget("Depth");
        lsysDepth = uiDepth->getScaledValue();
        reBuildLSys();
    } else if ( name == "Speed"){
        ofxUISlider *uiSpeed = (ofxUISlider *) gui->getWidget("Speed");
        lsysr.speed = uiSpeed->getScaledValue();
        reBuildLSys();
    }
}

void CloudsVisualSystemLSys::reBuildLSys(){
    LSystem sys;
    sys.initialPos.set(0,0);
    
    sys.ds = lsysScale;
    
    sys.setAngle(angle);
    sys.addAxion(axiom);
    sys.addRule(rule1);
    if (rule2.size() > 0){
        sys.addRule(rule2);
    }
    
    lsysr.setup(sys, lsysDepth);
    lsysr.start();
}

void CloudsVisualSystemLSys::update(ofEventArgs & args){
    lsysr.update();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

void CloudsVisualSystemLSys::draw(ofEventArgs & args){
    ofBackground(0);
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5, ofGetHeight());
    ofScale(2,2);
    ofSetColor(255);
    lsysr.draw();
    ofPopMatrix();
}

void CloudsVisualSystemLSys::exit(ofEventArgs & args){
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
}

void CloudsVisualSystemLSys::keyPressed(ofKeyEventArgs & args){
    
}

void CloudsVisualSystemLSys::keyReleased(ofKeyEventArgs & args){
    
}

void CloudsVisualSystemLSys::mouseDragged(ofMouseEventArgs & args){
    
}

void CloudsVisualSystemLSys::mouseMoved(ofMouseEventArgs & args){
    
}

void CloudsVisualSystemLSys::mousePressed(ofMouseEventArgs & args){
    
}

void CloudsVisualSystemLSys::mouseReleased(ofMouseEventArgs & args){
    
}