//
//  CloudsVisualSystemLSys.cpp
//  VSLSys
//
//  Created by Patricio Gonzalez Vivo on 5/25/13.
//
//

#include "CloudsVisualSystemLSystems.h"

string CloudsVisualSystemLSystems::getSystemName()
{
	return "LSystems";
}

void CloudsVisualSystemLSystems::selfSetup()
{
//    ofDisableArbTex();
    shader.load("shader");
    ofLoadImage(dot, "dot.png");
    
    angle = 5;
    axiom = "B";
    rule1 = "B=F[5+B][7-B]-F[4+B][6-B]-[3+B][5+B]-FB";
    rule2 = "F=FF";
}

void CloudsVisualSystemLSystems::selfSetupGuis()
{
    
}

void CloudsVisualSystemLSystems::selfAutoMode()
{
    
}

void CloudsVisualSystemLSystems::selfUpdate()
{
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
    lsysr.start();
}

void CloudsVisualSystemLSystems::selfDrawBackground()
{
    
}

void CloudsVisualSystemLSystems::selfDrawDebug()
{
    
}

void CloudsVisualSystemLSystems::selfSceneTransformation()
{
    
}

void CloudsVisualSystemLSystems::selfDraw()
{
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    mat->begin();
    ofPushMatrix();
//    ofRotate(90, 1.0, 0.0, 0.0);

//    ofMesh nodes;
//    nodes.setMode(OF_PRIMITIVE_POINTS);
    for(int i = 0; i < lsysr.activeNodes.size(); i++){
        ofPoint pos = lsysr.activeNodes.getVertices()[i];
        float intensity = ofNoise(pos.x*0.01, pos.y*0.05, ofGetElapsedTimef()*0.01,(float)(i)*0.01)*0.7;
        
        ofPushMatrix();
        ofSetRectMode(OF_RECTMODE_CENTER);
        
        billBoard(cam.getGlobalPosition(), ofVec3f(0,0,0));
        ofSetColor(255, 0, 0);
        dot.draw(0, 0, 100, 100);
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofPopMatrix();
        
//        nodes.addNormal(ofPoint(intensity,intensity,intensity));
//        nodes.addColor(ofFloatColor(1.0,intensity));
//        nodes.addVertex(pos);
    }
    
//    glDepthMask(GL_FALSE);
//    ofEnableBlendMode(OF_BLENDMODE_ADD);
//	ofEnablePointSprites();
//    shader.begin();
//    shader.setUniform1f("dotSize",dotSize);
//    shader.setUniformTexture("tex", dot,0);
//    nodes.draw();
//    shader.end();
//    ofDisablePointSprites();
//	ofDisableBlendMode();
//    glDepthMask(GL_TRUE);
    
    ofSetColor(255);
    lsysr.draw();
    
    ofPopMatrix();
    mat->end();
    
    
}

void CloudsVisualSystemLSystems::selfExit()
{
    
}

void CloudsVisualSystemLSystems::selfBegin()
{
    
}

void CloudsVisualSystemLSystems::selfEnd()
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


void CloudsVisualSystemLSystems::selfSetupSystemGui()
{
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
    sysGui->addSlider("bornTime", 0.0, 2, &lsysr.bornRandom);
    sysGui->addSlider("dotSize", 0.0, 100.0, &dotSize);
    sysGui->addButton("REGENERATE", true);
    
    axiom = uiAxiom->getTextString();
    rule1 = uiRule1->getTextString();
    rule2 = uiRule2->getTextString();
    
    reBuildLSys();
}

void CloudsVisualSystemLSystems::guiSystemEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    if ( name == "Axiom"){
        ofxUITextInput *uiAxiom = (ofxUITextInput *) e.widget;
        axiom = uiAxiom->getTextString();
    } else if ( name == "Rule1"){
        ofxUITextInput *uiRule1 = (ofxUITextInput *) e.widget;
        rule1 = uiRule1->getTextString();
    } else if ( name == "Rule2"){
        ofxUITextInput *uiRule2 = (ofxUITextInput *) e.widget;
        rule2 = uiRule2->getTextString();
    } else if ( name == "REGENERATE"){
        lsysr.stop();
    }
    
    reBuildLSys();
}

void CloudsVisualSystemLSystems::selfSetupRenderGui()
{
    
}

void CloudsVisualSystemLSystems::guiRenderEvent(ofxUIEventArgs &e)
{
    
}
