//
//  CloudsHUDController.cpp
//  CloudsPointcloudViewer
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsHUDController.h"
#include "CloudsGlobal.h"

CloudsHUDController::CloudsHUDController(){
	hudGui = NULL;
}

void CloudsHUDController::setup(){
//	testImage.loadImage( GetCloudsDataPath() + "HUD/overlayTest.png" );
	buildLayerSets();
}

void CloudsHUDController::buildLayerSets(){
	
	//configure layers
	CloudsHUDLayerSet currentLayer;
	CloudsHUDLayer* layer;
	
	//QUESTION LAYER
	currentLayer = CLOUDS_HUD_QUESTION;
	
	layer = new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_inner.svg");
	layerSets[currentLayer].push_back( layer );
	allLayers.push_back(layer);
	layer->delayTime = .1;
	layer->duration = 1.;
	layer->startPoint = ofVec2f(0,layer->svg.getHeight());
	layer->endPoint   = ofVec2f(layer->svg.getWidth(),0);

	layer = new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_innermost.svg");
	layerSets[currentLayer].push_back( layer );
	allLayers.push_back(layer);
	layer->delayTime = .1;
	layer->duration = 1.;
	layer->startPoint = ofVec2f(layer->svg.getWidth(),0);
	layer->endPoint   = ofVec2f(0,layer->svg.getHeight());

	layer = new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_Outer.svg");	
	layerSets[currentLayer].push_back( layer );
	allLayers.push_back(layer);
	layer->duration = 1.;
	layer->startPoint = ofVec2f(layer->svg.getWidth(),0);
	layer->endPoint   = ofVec2f(0,layer->svg.getHeight());
	
//	layerSets[CLOUDS_HUD_QUESTION].push_back( new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_innermost.svg" ) );
//	layerSets[CLOUDS_HUD_QUESTION].push_back( new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_Outer.svg" ) );
	
}

void CloudsHUDController::update(){
	for(int i = 0; i < allLayers.size(); i++){
		
		allLayers[i]->update();
	}
}

void CloudsHUDController::draw(){
	
	ofPushStyle();
	ofPushMatrix();
	ofEnableAlphaBlending();
	
	drawLayer(CLOUDS_HUD_QUESTION);
	drawLayer(CLOUDS_HUD_LOWER_THIRD);
	drawLayer(CLOUDS_HUD_PROJECT_EXAMPLE);
	
//	ofScale(ofGetMouseX()*.01,
//			ofGetMouseY()*.01);
//	for(int i = 0; i < layers.size(); i++){
//		layers[i].svg.draw();
//	}
	
	ofPopMatrix();
	ofPopStyle();
}

void CloudsHUDController::drawLayer(CloudsHUDLayerSet layer){
	for(int i = 0; i < layerSets[layer].size(); i++){
		layerSets[layer][i]->draw();
	}
}

void CloudsHUDController::animateOn(CloudsHUDLayerSet layer){

	for(int i = 0; i < layerSets[layer].size(); i++){
		layerSets[layer][i]->start();
	}
}

void CloudsHUDController::animateOff(){
	
}

void CloudsHUDController::saveGuiSettings(){
	
}

void CloudsHUDController::toggleGuis(){
	
}

