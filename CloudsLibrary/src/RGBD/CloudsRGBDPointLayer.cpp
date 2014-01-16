//
//  CloudsRGBDPointLayer.cpp
//  CloudsPointcloudViewer
//
//  Created by Tekrental on 1/15/14.
//
//

#include "CloudsRGBDPointLayer.h"
#include "CloudsVisualSystem.h"
#include "CloudsRGBDVideoPlayer.h"

CloudsRGBDPointLayer::CloudsRGBDPointLayer(){
    setDefaults();
}

CloudsRGBDPointLayer::~CloudsRGBDPointLayer(){
    if(eventsRegistered){
//        ofAddListener(pointsGui->newGUIEvent, this, &CloudsRGBDPointLayer::selfGuiEvent);
    }
}

void CloudsRGBDPointLayer::setDefaults(){
    
	drawPoints = true;
    pointColorBoost = .0;
    pointSkinBoost = .0;
	numRandomPoints = 20000;
	pointSize.min = 1.0;
	pointSize.max = 3.0;
	pointAlpha = 1.0;
	pointFlowPosition = 0.0;
	pointFlowSpeed = 0.0;
	pointsFlowUp = false;
	actuatorPosition = 0;
    
    pointXSimplify = 2.0;
    pointYSimplify = 2.0;
    
    eventsRegistered = false;
    refreshPointcloud = true;
    
    normalMix = 0.0;
    normalExtend = 0.0;
    
}

ofxUISuperCanvas* CloudsRGBDPointLayer::createGui(ofxUISuperCanvas* gui, string name){
    
   	pointsGui = new ofxUISuperCanvas( ofToUpper(name), gui);
	pointsGui->copyCanvasStyle(gui);
    pointsGui->copyCanvasProperties(gui);
    pointsGui->setName(name);
    pointsGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

	ofxUIToggle* toggle;
	toggle = pointsGui->addToggle("ENABLE", &drawPoints);
	toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
	pointsGui->resetPlacer();
	pointsGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
	pointsGui->addWidgetToHeader(toggle);
	
	pointsGui->addSlider("Face Min Radius", 0, 600., &pointFaceMinRadius);
	pointsGui->addSlider("Face Falloff", 0, 600., &pointFaceFalloff);
	pointsGui->addSlider("Point Alpha", 0, 1.0, &pointAlpha);
	pointsGui->addSlider("Point Color Boost", 0, 1.0, &pointColorBoost);
	pointsGui->addSlider("Point Skin Boost", 0, 1.0, &pointSkinBoost);
    pointsGui->addSpacer();
    pointsGui->addSlider("Point X Simplify", 1.0, 8, &pointXSimplify);
    pointsGui->addSlider("Point Y Simplify", 1.0, 8, &pointYSimplify);
	pointsGui->addRangeSlider("Point Size", 0.0, 3.0, &pointSize.min, &pointSize.max);
	pointsGui->addSlider("Point Face Overlap",0., 1.0, &pointHeadOverlap);
	pointsGui->addSlider("Point Flow", 0, 1.0, &pointFlowSpeed);
	pointsGui->addToggle("Points Flow Up", &pointsFlowUp);
    pointsGui->addSpacer();
	pointsGui->addSlider("Actuator Speed", 0., 20.0, &actuatorSpeed);
	pointsGui->addSlider("Actuator Phase", 0., 360., &actuatorPhase);
	pointsGui->addSlider("Actuator Angle", 0., 180, &actuatorAngle);
	pointsGui->addSlider("Actuator Max Affect", 0., 1.0, &actuatorMaxAffect);
    pointsGui->addSpacer();
	pointsGui->addSlider("Normal Mix", 0., 1.0, &normalMix);
	pointsGui->addSlider("Normal Extend", 0., 200, &normalExtend);
    
    ofAddListener(pointsGui->newGUIEvent, this, &CloudsRGBDPointLayer::selfGuiEvent);
    
    eventsRegistered = true;
    
    return pointsGui;
}

void CloudsRGBDPointLayer::generatePoints(){
    
    
    points.setUsage( GL_STATIC_DRAW );
    
    if(pointXSimplify <= 0.0) pointXSimplify = 1.0;
    if(pointYSimplify <= 0.0) pointYSimplify = 1.0;
    
    points.clear();
    for (float y = 0; y < 480; y += pointYSimplify){
        for (float x = 0; x < 640; x += pointXSimplify){
            points.addVertex( ofVec3f(x,y,0) );
        }
    }
    
    points.setMode(OF_PRIMITIVE_POINTS);
    refreshPointcloud = false;
}

void CloudsRGBDPointLayer::update(){
    if(actuatorSpeed == 0){
        actuatorPosition = 0;
    }
    else{
        actuatorPosition += actuatorSpeed;
    }
    
    ofVec3f vx = ofVec3f(1,0,0);
    ofVec3f vy = ofVec3f(0,1,0);
    ofVec3f vz = ofVec3f(0,0,1);

    //rotate 3 vecs down to shine angle then spready them out apart evenly, modulated by the spin position
    actuator = vx.getRotated(actuatorAngle, vz).getRotated(actuatorPhase + actuatorPosition, vy);

    if(refreshPointcloud){
        generatePoints();
    }
    
	pointFlowPosition += powf(pointFlowSpeed,2.0);
}

void CloudsRGBDPointLayer::draw(){
    
    if(!drawPoints){
        return;
    }
    
    if(pointShader == NULL){
        ofLogError("CloudsRGBDPointLayer::draw") << "Point shader not set";
        return;
    }
    
    pointShader->begin();
    
    CloudsVisualSystem::getRGBDVideoPlayer().flowPosition = pointFlowPosition * (pointsFlowUp?-1:1);
    
    CloudsVisualSystem::getRGBDVideoPlayer().setupProjectionUniforms(*pointShader);

    pointShader->setUniform1f("headMinRadius", pointFaceMinRadius);
    pointShader->setUniform1f("headFalloff", pointFaceFalloff);
    pointShader->setUniform1f("edgeAttenuateBase",powf(edgeAttenuate,2.0));

    pointShader->setUniform1f("headOverlap", pointHeadOverlap);
    pointShader->setUniform1f("pointSizeMin", pointSize.min);
    pointShader->setUniform1f("pointSizeMax", pointSize.max);
    
    pointShader->setUniform1f("alpha",
                             pointAlpha *
                             CloudsVisualSystem::getRGBDVideoPlayer().getFadeIn() *
                             CloudsVisualSystem::getRGBDVideoPlayer().getFadeOut() *
                             *visualSystemFadeValue);

    pointShader->setUniform3f("actuatorDirection",
                             actuator.x,
                             actuator.y,
                             actuator.z);
    
    pointShader->setUniform1f("actuatorMaxAffect", actuatorMaxAffect);
    pointShader->setUniform1f("colorBoost", pointColorBoost);
    pointShader->setUniform1f("skinBoost", pointSkinBoost);

    pointShader->setUniform1f("normalMix", normalMix);
	pointShader->setUniform1f("normalExtend", normalExtend);

    points.draw();

    pointShader->end();
}

void CloudsRGBDPointLayer::selfGuiEvent(ofxUIEventArgs &e){
    if(e.widget->getName() == "Point X Simplify" ||
       e.widget->getName() == "Point Y Simplify")
    {
        refreshPointcloud = true;
    }
}

