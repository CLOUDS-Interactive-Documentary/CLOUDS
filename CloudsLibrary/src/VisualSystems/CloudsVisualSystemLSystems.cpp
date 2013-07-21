//
//  CloudsVisualSystemLSys.cpp
//  VSLSys
//
//  Created by Patricio Gonzalez Vivo on 5/25/13.
//
//

#include "CloudsVisualSystemLSystems.h"

string CloudsVisualSystemLSystems::getSystemName(){
	return "LSystems";
}

void CloudsVisualSystemLSystems::selfSetup(){
    angle = 5;
    axiom = "B";
    rule1 = "B=F[5+B][7-B]-F[4+B][6-B]-[3+B][5+B]-FB";
    rule2 = "F=FF";
}

void CloudsVisualSystemLSystems::selfSetupGuis(){
    
}

void CloudsVisualSystemLSystems::selfAutoMode(){
    
}

void CloudsVisualSystemLSystems::selfBegin(){
    reBuildLSys();
}

void CloudsVisualSystemLSystems::selfEnd(){
    
}

void CloudsVisualSystemLSystems::selfUpdate(){
    lsysr.update();
}

void CloudsVisualSystemLSystems::reBuildLSys(){
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
    lsysr.init();
}

void CloudsVisualSystemLSystems::selfDrawBackground()
{
    ofPushMatrix();
    ofPushStyle();
    
    ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
    ofTranslate((180.0-xRot->getPos())*10.0, (180.0-yRot->getPos())*10.0);
    ofScale(camDistance*0.1,camDistance*0.1);
    
    ofSetColor(255, originalAlpha*255);
    lsysr.originalMesh.draw();
    
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofSetColor(255, particlesAlpha*255);
    for(int i = 0; i < lsysr.activeNodes.size(); i++){
        ofPoint pos = lsysr.activeNodes[i];
        
        glPointSize(dotSize);
        glBegin(GL_POINTS);
        glVertex3f(pos.x,pos.y,pos.z);
        glEnd();
    }
    
    ofSetColor(255, traceAlpha*255);
    lsysr.growMesh.draw();
    
    ofEnableAlphaBlending();
    
    ofPopStyle();
    ofPopMatrix();
}

void CloudsVisualSystemLSystems::selfDraw(){

}

void CloudsVisualSystemLSystems::selfSetupSystemGui(){
    ofxUITextInput *uiAxiom = sysGui->addTextInput("Axiom", "B", OFX_UI_FONT_SMALL);
    uiAxiom->setAutoClear(false);
    ofxUITextInput *uiRule1 = sysGui->addTextInput("Rule1", "B=F[5+B][7-B]-F[4+B][6-B]-[3+B][5+B]-FB", OFX_UI_FONT_SMALL);
    uiRule1->setAutoClear(false);
    ofxUITextInput *uiRule2 = sysGui->addTextInput("Rule2", "F=FF", OFX_UI_FONT_SMALL);
    uiRule2->setAutoClear(false);
    sysGui->addSlider("Angle", 0, 90, &angle);
    sysGui->addSlider("Scale", 0.5, 2, &lsysScale);
    sysGui->addSlider("Depth", 1, 10, &lsysDepth);
    sysGui->addSlider("aNoise", 0.0, 2.0, &lsysr.aNoise);
    sysGui->addSlider("tNoise", 0.0, 1.0, &lsysr.tNoise);
    sysGui->addSlider("Speed", 0.0, 10, &lsysr.speed);
    sysGui->addSlider("bornTime", 0.0, 10.0, &lsysr.bornRandom);
    sysGui->addSlider("breakness", 0.0, 1.0, &lsysr.breakness);
    
    sysGui->addToggle("Grow", &lsysr.bGrow);
    sysGui->addToggle("Flow", &lsysr.bFlow);
    
    
    axiom = uiAxiom->getTextString();
    rule1 = uiRule1->getTextString();
    rule2 = uiRule2->getTextString();
    
    reBuildLSys();
}

void CloudsVisualSystemLSystems::selfSetupRenderGui(){
    rdrGui->addLabel("Show");
    rdrGui->addSlider("Oringinal_alpha", 0.0, 1.0, &originalAlpha);
    rdrGui->addSlider("Particles_alpha", 0.0, 1.0, &particlesAlpha);
    rdrGui->addSlider("Particles_size", 0.0, 10.0, &dotSize);
    rdrGui->addSlider("Trace", 0.0, 1.0, &traceAlpha);
}

void CloudsVisualSystemLSystems::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    if ( name == "Axiom"){
        ofxUITextInput *uiAxiom = (ofxUITextInput *) e.widget;
        axiom = uiAxiom->getTextString();
        reBuildLSys();
    } else if ( name == "Rule1"){
        ofxUITextInput *uiRule1 = (ofxUITextInput *) e.widget;
        rule1 = uiRule1->getTextString();
        reBuildLSys();
    } else if ( name == "Rule2"){
        ofxUITextInput *uiRule2 = (ofxUITextInput *) e.widget;
        rule2 = uiRule2->getTextString();
        reBuildLSys();
    } else if ( name == "Angle" || name == "Depth" || name == "Scale" || name == "Grow" || name == "breakness"){
        reBuildLSys();
    } 
    
    
}

void CloudsVisualSystemLSystems::selfExit()
{
    
}

void CloudsVisualSystemLSystems::selfKeyPressed(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemLSystems::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemLSystems::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemLSystems::selfMouseMoved(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemLSystems::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemLSystems::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemLSystems::selfSetupGui()
{
      
}

void CloudsVisualSystemLSystems::selfGuiEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemLSystems::selfDrawDebug()
{
    
}

void CloudsVisualSystemLSystems::selfSceneTransformation()
{
    
}

void CloudsVisualSystemLSystems::guiRenderEvent(ofxUIEventArgs &e)
{
    
}
