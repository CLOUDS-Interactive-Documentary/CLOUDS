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
	
	home.setup();
}

void CloudsHUDController::buildLayerSets(){
	
	//configure layers
	CloudsHUDLayerSet currentLayer;
	CloudsHUDLayer* layer;
	
	//QUESTION LAYER
	currentLayer = CLOUDS_HUD_QUESTION;
	
//	layer = new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_inner.svg");
//	layerSets[currentLayer].push_back( layer );
//	allLayers.push_back(layer);
//	layer->delayTime = .1;
//	layer->duration = 1.;
//	layer->startPoint = ofVec2f(0,layer->svg.getHeight());
//	layer->endPoint   = ofVec2f(layer->svg.getWidth(),0);
//
//	layer = new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_innermost.svg");
//	layerSets[currentLayer].push_back( layer );
//	allLayers.push_back(layer);
//	layer->delayTime = .1;
//	layer->duration = 1.;
//	layer->startPoint = ofVec2f(layer->svg.getWidth(),0);
//	layer->endPoint   = ofVec2f(0,layer->svg.getHeight());

//	layer = new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_Outer.svg");	
//	layerSets[currentLayer].push_back( layer );
//	allLayers.push_back(layer);
//	layer->duration = 1.;
//	layer->startPoint = ofVec2f(layer->svg.getWidth(),0);
//	layer->endPoint   = ofVec2f(0,layer->svg.getHeight());

//	layer = new CloudsHUDLayer();
//	layer->parse(GetCloudsDataPath() + "HUD/CLOUDS_UI_v59-01.svg");
//	layerSets[currentLayer].push_back( layer );
//	allLayers.push_back(layer);
//	layer->duration = 1.;
//	layer->startPoint = ofVec2f(layer->svg.getWidth(),0);
//	layer->endPoint   = ofVec2f(0,layer->svg.getHeight());

	ofDirectory testSVGDir(GetCloudsDataPath() + "HUD/SVG");
	testSVGDir.allowExt("svg");
	testSVGDir.listDir();
	for(int i = 0; i < testSVGDir.numFiles(); i++){
		cout << "Loading " << testSVGDir.getName(i) << endl;
		layer = new CloudsHUDLayer();
		layer->parse(testSVGDir.getPath(i));
		layerSets[currentLayer].push_back( layer );
		allLayers.push_back(layer);
		
		layer->duration = 1.5;
		layer->delayTime = ofRandomuf();
		
		layer->startPoint = ofVec2f(layer->svg.getWidth(),0);
		layer->endPoint   = ofVec2f(0,layer->svg.getHeight());
		
	}
	
//	layerSets[CLOUDS_HUD_QUESTION].push_back( new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_innermost.svg" ) );
//	layerSets[CLOUDS_HUD_QUESTION].push_back( new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_Outer.svg" ) );
	
}

void CloudsHUDController::update(){
	for(int i = 0; i < allLayers.size(); i++){
		
		allLayers[i]->update();
	}
	
	home.update();
}

void CloudsHUDController::draw(){
	
	ofPushStyle();
	ofPushMatrix();
	ofEnableAlphaBlending();
	
//	ofSetColor(255,255,255,ofGetMouseX());
	drawLayer(CLOUDS_HUD_QUESTION);
	drawLayer(CLOUDS_HUD_LOWER_THIRD);
	drawLayer(CLOUDS_HUD_PROJECT_EXAMPLE);
	
	home.draw();
	
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

