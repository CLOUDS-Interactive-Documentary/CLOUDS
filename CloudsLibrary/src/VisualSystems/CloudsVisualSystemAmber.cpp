//
//  CloudsVisualSystemAmber.cpp
//  VSCloudsRezanator
//
//  Created by Syed Reza Ali on 5/25/13.
//
//

#include "CloudsVisualSystemAmber.h"
#include "ofxGenerative.h"

string CloudsVisualSystemAmber::getSystemName()
{
	return "Amber";
}

void CloudsVisualSystemAmber::selfSetup()
{
    noiseScaleX = 1.0;
    noiseScaleY = 1.0;
    bufferSize = 256;
    bufferData = new float[bufferSize];
    
    for(int i = 0; i < bufferSize; i++)
    {
        bufferData[i] = 0.0;
    }
    
    glow = new ofImage();
    glow->loadImage("images/glow.png");
    particleAlpha = 200;
    
    electro = new ofxElectroStaticBehavior();
    sphere = new ofxSphericalAttractionBehavior();
    sphere1 = new ofxSphericalAttractionBehavior();
    homing = new ofxHomingBehavior();
    perlin = new ofxPerlinBehavior();
    damper = new ofxDamperBehavior();
    distort = new ofxDistorterBehavior();
    buffer = new ofxBufferEffectorBehavior();
    //        buffer->setBuffer(audioInMix, bufferSize);
    
    buffer->setBuffer(bufferData, bufferSize);
    
    glowRenderer = new ofxRParticleGlowieRenderer();
    glowRenderer->setCamPtr(&cam);
    glowRenderer->setAxisAngleOffsets(xRot, yRot, zRot);
    glowRenderer->setGlowImage(glow);
    
    ps = new ofxRParticleSystem();
    renderer = ps->getRenderer();
    ps->setSolver(new ofxVerletSolver());
    ps->setRenderer(glowRenderer);
    renderer = glowRenderer;
    
    ps->addBehavior(sphere);
    ps->addBehavior(sphere1);
    ps->addBehavior(homing);
    ps->addBehavior(perlin);
    ps->addBehavior(damper);
    ps->addBehavior(distort);
    ps->addBehavior(electro);
    ps->addBehavior(buffer);
    
    electro->setParticlesPtr(ps->getParticlesPtr());
    
    debugGridSize = 8;
    float width = debugGridSize;
    float height = width;
    float depth = height;
    
    int i = 0;
    for (int z = 0; z <= depth; z++)
    {
        for (int y = 0; y <= height; y++)
        {
            for (int x = 0; x <= width; x++)
            {
                i++;
                float px = -width/2.0+(float)x;
                float py = -height/2.0+(float)y;
                float pz = -depth/2.0+(float)z;
                ofxRParticle p = ofxRParticle(ofVec3f(px, py, pz));
                //                    p.setColor(colorPalletes->getPallete(i).getColor(i));
                p.setColor(ofColor(0,particleAlpha));
                p.setVel(ofVec3f(ofRandom(-10,10), ofRandom(-10,10), ofRandom(-10,10)));
                ps->addParticle(p);
            }
        }
    }
}

void CloudsVisualSystemAmber::selfSetupGuis()
{
    setupSphereGui(sphereGui, sphere, "SPHERE", "SphereSettings");
    setupSphereGui(sphereGui1, sphere1, "SPHERE1", "Sphere1Settings");
    setupHomingGui(homingGui);
    setupPerlinGui(perlinGui);
    setupDamperGui(damperGui);
    setupDistortGui(distortGui);
    setupElectroGui(electroGui);
    setupBufferGui(bufferGui);   
}

void CloudsVisualSystemAmber::selfUpdate()
{
    float time = ofGetElapsedTimef();
    for(int i = 0; i < bufferSize; i++)
    {
        bufferData[i] = ofSignedNoise(time*noiseScaleX, i*noiseScaleY);
    }
    
    ps->update();
}

void CloudsVisualSystemAmber::selfDrawBackground()
{
    
}

void CloudsVisualSystemAmber::selfDrawDebug()
{
    if(sphere->isEnabled())
    {
        ofNoFill();
        float color = 255-bgBri->getPos();
        ofSetColor(color, 100*sphere->getMagnitude());
        ofDrawSphere(sphere->getPosition(), sphere->getRadius());
    }
    
    if(sphere1->isEnabled())
    {
        ofNoFill();
        float color = 255-bgBri->getPos();
        ofSetColor(color, 100*sphere1->getMagnitude());
        ofDrawSphere(sphere1->getPosition(), sphere1->getRadius());
    }
    
    if(distort->isEnabled())
    {
        ofNoFill();
        float color = 255-bgBri->getPos();
        ofSetColor(color, 100*distort->getMagnitude());
        ofDrawSphere(distort->getPosition(), distort->getRadius());
    }
}

void CloudsVisualSystemAmber::selfSceneTransformation()
{
    
}

void CloudsVisualSystemAmber::selfDraw()
{
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    ps->draw();
}

void CloudsVisualSystemAmber::selfExit()
{
    delete glow;
    delete ps;    
}

void CloudsVisualSystemAmber::selfBegin()
{
    
}

void CloudsVisualSystemAmber::selfEnd()
{
    
}

void CloudsVisualSystemAmber::selfKeyPressed(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemAmber::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemAmber::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemAmber::selfMouseMoved(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemAmber::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemAmber::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemAmber::selfSetupGui()
{
    
}

void CloudsVisualSystemAmber::selfGuiEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemAmber::selfSetupSystemGui()
{
    sysGui->addButton("RANDOMIZE", false);
    sysGui->addSlider("DT", 0.0, 1.0, ps->getDt());
    sysGui->addSlider("ACCLIMIT", 0.0, 10.0, ps->getAccelerationLimit());
    sysGui->addSlider("VELLIMIT", 0.0, 10.0, ps->getVelocityLimit());    
}

void CloudsVisualSystemAmber::guiSystemEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    
    if(name == "RANDOMIZE")
    {
        ps->randomize(10.0);
    }    
}

void CloudsVisualSystemAmber::selfSetupRenderGui()
{
    rdrGui->addToggle("ADDITIVE BLENDING", renderer->getAdditiveBlendingPtr());
    rdrGui->addSlider("POINTSIZE", 0.0, 10.0, renderer->getPointSizePtr());
//    rdrGui->addSlider("COLOR PALETTE", 0.0, colorPalletes->getCount(), 0.0)->setIncrement(1.0);
    rdrGui->addSlider("ALPHA", 0.0, 255.0, &particleAlpha);
}

void CloudsVisualSystemAmber::guiRenderEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    if(name == "ALPHA")
    {
        vector<ofxRParticle> *particles = ps->getParticlesPtr();
        for(vector<ofxRParticle>::iterator it = (*particles).begin(); it != (*particles).end(); it++)
        {
            it->setColorAlpha(particleAlpha);
        }
    }
//    else if(name == "COLOR PALETTE")
//    {
//        ofxUISlider *s = (ofxUISlider *) e.widget;
//        ofxColorPalette *colors = colorPalletes->getPalletePointer(s->getScaledValue());
//        vector<ofxRParticle> *particles = ps->getParticlesPtr();
//        int index = 0;
//        for(vector<ofxRParticle>::iterator it = (*particles).begin(); it != (*particles).end(); it++)
//        {
//            it->setColor(colors->getColor(index));
//            it->setColorAlpha(particleAlpha);
//            index++;
//        }
//    }
}

void CloudsVisualSystemAmber::setupSphereGui(ofxUISuperCanvas *g, ofxSphericalAttractionBehavior *s, string _name, string _settings)
{
    g = new ofxUISuperCanvas(_name, gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName(_settings);
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ACTIVE", s->getEnabledPtr());
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    
    g->addSpacer();
    g->addSlider("SPHERE POWER", 0.0, 1.0, s->getMagnitudePtr());
    g->addSlider("SPHERE RADIUS", 0.1, debugGridSize*4.0, s->getRadiusPtr());
    g->add2DPad("S POSITION", ofVec3f(-debugGridSize, debugGridSize), ofVec3f(-debugGridSize, debugGridSize), s->getPositionPtr());
    
    g->autoSizeToFitWidgets();
    guis.push_back(g);
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystemAmber::guiSphereEvent);
    guis.push_back(g);
}


void CloudsVisualSystemAmber::guiSphereEvent(ofxUIEventArgs &e)
{
    //        string name = e.widget->getName();
    //        if(name == "ACTIVE")
    //        {
    //            ofxUIToggle *t = (ofxUIToggle *) e.widget;
    //            t->setLabelVisible(t->getValue());
    //        }
}

void CloudsVisualSystemAmber::setupHomingGui(ofxUISuperCanvas *g)
{
    g = new ofxUISuperCanvas("HOMING", gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName("HomingSettings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ACTIVE", homing->getEnabledPtr());
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    
    g->addSpacer();
    g->addSlider("HOMING POWER", 0.0, 1.0, homing->getMagnitudePtr());
    
    g->autoSizeToFitWidgets();
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystemAmber::guiHomingEvent);
    guis.push_back(g);
}

void CloudsVisualSystemAmber::guiHomingEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
}

void CloudsVisualSystemAmber::setupPerlinGui(ofxUISuperCanvas *g)
{
    g = new ofxUISuperCanvas("PERLIN", gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName("PerlinSettings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ACTIVE", perlin->getEnabledPtr());
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    
    g->addSpacer();
    g->addSlider("PERLIN POWER", 0.0, 1.0, perlin->getMagnitudePtr());
    
    g->autoSizeToFitWidgets();
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystemAmber::guiPerlinEvent);
    guis.push_back(g);
}

void CloudsVisualSystemAmber::guiPerlinEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
}

void CloudsVisualSystemAmber::setupDamperGui(ofxUISuperCanvas *g)
{
    g = new ofxUISuperCanvas("DAMPER", gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName("DamperSettings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ACTIVE", damper->getEnabledPtr());
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    
    g->addSpacer();
    g->addSlider("DAMPER POWER", 0.0, 1.0, damper->getMagnitudePtr());
    
    
    g->autoSizeToFitWidgets();
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystemAmber::guiDamperEvent);
    guis.push_back(g);
}

void CloudsVisualSystemAmber::guiDamperEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
}

void CloudsVisualSystemAmber::setupDistortGui(ofxUISuperCanvas *g)
{
    g = new ofxUISuperCanvas("DISTORT", gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName("DistortSettings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ACTIVE", distort->getEnabledPtr());
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    
    g->addSpacer();
    g->addSlider("DISTORT POWER", -1.0, 1.0, distort->getMagnitudePtr());
    g->addSlider("DISTORT RADIUS", 0.1, debugGridSize*4.0, distort->getRadiusPtr());
    g->addSlider("DISTORT EXP", 1.0, 4.0, distort->getExpFactorPtr());
    g->add2DPad("D POSITION", ofVec3f(-debugGridSize, debugGridSize), ofVec3f(-debugGridSize, debugGridSize), distort->getPositionPtr());
    
    g->autoSizeToFitWidgets();
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystemAmber::guiDistortEvent);
    guis.push_back(g);
}

void CloudsVisualSystemAmber::guiDistortEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
}

void CloudsVisualSystemAmber::setupElectroGui(ofxUISuperCanvas *g)
{
    g = new ofxUISuperCanvas("ELECTRO", gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName("ElectroSettings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ACTIVE", electro->getEnabledPtr());
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    
    g->addSpacer();
    g->addSlider("ELECTRO POWER", 0.0, 10.0, electro->getMagnitudePtr());
    
    g->autoSizeToFitWidgets();
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystemAmber::guiElectroEvent);
    guis.push_back(g);
}

void CloudsVisualSystemAmber::guiElectroEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
}

void CloudsVisualSystemAmber::setupBufferGui(ofxUISuperCanvas *g)
{
    g = new ofxUISuperCanvas("BUFFER", gui);
    g->copyCanvasStyle(gui);
    g->copyCanvasProperties(gui);
    g->setPosition(guis[guis.size()-1]->getRect()->x+guis[guis.size()-1]->getRect()->getWidth()+1, 0);
    g->setName("BufferSettings");
    g->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    ofxUIToggle *toggle = g->addToggle("ACTIVE", buffer->getEnabledPtr());
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    g->resetPlacer();
    g->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    g->addWidgetToHeader(toggle);
    
    g->addSpacer();
    g->addSlider("BUFFER POWER", 0.0, 10.0, buffer->getMagnitudePtr());
    g->addSlider("NOISE SCALE X", 0.0, 1.0, &noiseScaleX);
    g->addSlider("NOISE SCALE Y", 0.0, 1.0, &noiseScaleY);
    g->addWaveform("BUFFER GRAPH", buffer->getBuffer(), buffer->getBufferSize());
    
    g->autoSizeToFitWidgets();
    ofAddListener(g->newGUIEvent,this,&CloudsVisualSystemAmber::guiBufferEvent);
    guis.push_back(g);
}

void CloudsVisualSystemAmber::guiBufferEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
}