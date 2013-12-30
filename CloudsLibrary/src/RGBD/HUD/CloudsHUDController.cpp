//
//  CloudsHUDController.cpp
//  CloudsPointcloudViewer
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsHUDController.h"
#include "CloudsGlobal.h"
#include "CloudsInput.h"
#include "CloudsClip.h"
#include "CloudsSpeaker.h"

CloudsHUDController::CloudsHUDController(){
	hudGui = NULL;
}

void CloudsHUDController::setup(){
//	testImage.loadImage( GetCloudsDataPath() + "HUD/overlayTest.png" );
	buildLayerSets();
    calculateFontSizes();
	
	home.setup();
}

//Our textboxes are:
//BylineBodyCopyTextBox.svg has text box: BylineBodyCopyTextBox with bounds 741.999 725.021 583.814 63.34
//BylineFirstNameTextBox.svg has text box: BylineFirstNameTextBox_1_ with bounds 200.813 719.021 80.314 26.147
//BylineLastNameTextBox.svg has text box: BylineLastNameTextBox with bounds 200.813 754.021 278.314 37.147
//SVG/BylineTopicTextBoxBottom.svg has text box: BylineTopicTextBoxBottom with bounds 498.656 770.521 146.314 13.147
//BylineTopicTextBoxTop.svg has text box: BylineTopicTextBoxTop with bounds 498.656 728.676 182.314 13.147
//ProjectExampleTextboxLeft.svg has text box: ProjectExampleTextboxLeft with bounds 875.406 388.5 318.814 15.34
//ProjectExampleTextboxRight.svg has text box: ProjectExampleTextboxRight with bounds 1305.19 388.5 40.127 15.34
//ProjectExampleTextBoxTop.svg has text box: ProjectExampleTextBoxTop with bounds 875.406 63.5 158.5 15.34
//QuestionTextBox.svg has text box: QuestionTextBox with bounds 114.656 68.926 468.314 20.147
//ResetButtonTextBox.svg has text box: ResetButtonTextBox with bounds 1269.64 837.294 67.314 16.147
//TopicTextBoxLeft.svg has text box: TopicTextBoxLeft with bounds 114.656 114.926 71.314 13.147
//TopicTextBoxRight.svg has text box: TopicTextBoxRight with bounds 247.999 114.926 122.314 13.147

void CloudsHUDController::clipBegan(CloudsClip& clip){
	
	cout << "ID's on clip " << clip.name << " and fcp id? " << clip.fcpFileId << endl;
	CloudsSpeaker speaker = CloudsSpeaker::speakers[ clip.person ];
	cout << "Clip is " <<  clip.getLinkName() << endl;
	cout << "speaker: " << speaker.firstName << " " << speaker.lastName << endl;
	
	if(clip.hasProjectExample){
		CloudsProjectExample example = clip.projectExample;
		cout << "project " << example.title << " with videos? " << example.exampleVideos.size() << endl;
	}
}

void CloudsHUDController::buildLayerSets(){
	
	//configure layers
	CloudsHUDLayerSet currentLayer;
//	CloudsHUDLayer* layer;
	
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

    
    CloudsHUDLayer* lowerThirdLayer = new CloudsHUDLayer();
    lowerThirdLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_LOWER_THIRD");
    layerSets[CLOUDS_HUD_LOWER_THIRD].push_back( lowerThirdLayer );
    allLayers.push_back( lowerThirdLayer );
    
    CloudsHUDLayer* questionLayer = new CloudsHUDLayer();
    questionLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_QUESTION");
    layerSets[CLOUDS_HUD_QUESTION].push_back( questionLayer );
    allLayers.push_back( questionLayer );
    
    CloudsHUDLayer* mapLayer = new CloudsHUDLayer();
    mapLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_MAP");
    layerSets[CLOUDS_HUD_MAP].push_back( mapLayer );
    allLayers.push_back( mapLayer );
    
    CloudsHUDLayer* projectExampleLayer = new CloudsHUDLayer();
    projectExampleLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_PROJECT_EXAMPLE");
    layerSets[CLOUDS_HUD_PROJECT_EXAMPLE].push_back( projectExampleLayer );
    allLayers.push_back( projectExampleLayer );

    
    for( int i=0; i<allLayers.size(); i++ ){
        allLayers[i]->duration = 1.5;
        allLayers[i]->delayTime = ofRandomuf();
        
        allLayers[i]->startPoint = ofVec2f(allLayers[i]->svg.getWidth(),0);
        allLayers[i]->endPoint   = ofVec2f(0,allLayers[i]->svg.getHeight());
    }
	
//	layerSets[CLOUDS_HUD_QUESTION].push_back( new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_innermost.svg" ) );
//	layerSets[CLOUDS_HUD_QUESTION].push_back( new CloudsHUDLayer(GetCloudsDataPath() + "HUD/01_MAIN_Outer.svg" ) );
	
}

void CloudsHUDController::calculateFontSizes(){
    // temporary allocate
    int minFontSize = 1;
    int maxFontSize = 40;
    
    for( int i=minFontSize; i<maxFontSize; i++){
        ofxFTGLFont *tmp = new ofxFTGLFont();
        tmp->loadFont( GetCloudsDataPath() + "font/Blender-THIN.ttf", i );
        tempFontList.push_back( tmp );
    }

    
    BylineBodyCopyTextBox       = getLayoutForLayer("BylineBodyCopyTextBox");
    BylineFirstNameTextBox      = getLayoutForLayer("BylineFirstNameTextBox_1_");
    BylineLastNameTextBox       = getLayoutForLayer("BylineLastNameTextBox");
    BylineTopicTextBoxBottom    = getLayoutForLayer("BylineTopicTextBoxBottom");
    BylineTopicTextBoxTop       = getLayoutForLayer("BylineTopicTextBoxTop");
    ResetButtonTextBox          = getLayoutForLayer("ResetButtonTextBox");
    QuestionTextBox             = getLayoutForLayer("QuestionTextBox");
    TopicTextBoxLeft            = getLayoutForLayer("TopicTextBoxLeft");
    TopicTextBoxRight           = getLayoutForLayer("TopicTextBoxRight");
    ProjectExampleTextboxLeft   = getLayoutForLayer("ProjectExampleTextboxLeft");
    ProjectExampleTextboxRight  = getLayoutForLayer("ProjectExampleTextboxRight");
    ProjectExampleTextBoxTop    = getLayoutForLayer("ProjectExampleTextBoxTop");
    
    
    // cleanup!
    for( int i=0; i<tempFontList.size(); i++ ){
        delete tempFontList[i];
    }
    tempFontList.clear();
}

ofxFTGLSimpleLayout* CloudsHUDController::getLayoutForLayer( string layerName ) {
    for( int i=0; i<allLayers.size(); i++ ){
        SVGMesh* textMesh = allLayers[i]->svg.getMeshByID( layerName );
        
        if( textMesh != NULL ){
            float maxHeight = textMesh->bounds.height;
            int fontSize = getFontSizeForMesh( textMesh );
            
//            cout << "The correct font size is " << fontSize << ". Width is " << textMesh->bounds.width << endl;
            
            // make a layout
            ofxFTGLSimpleLayout *newLayout = new ofxFTGLSimpleLayout();
            newLayout->loadFont( GetCloudsDataPath() + "font/Blender-THIN.ttf", fontSize );
            newLayout->setLineLength( textMesh->bounds.width );
            
            // make a label
            CloudsHUDLabel newLabel;
            newLabel.layout = newLayout;
            newLabel.bounds = textMesh->bounds;
            
            hudLabelList.push_back( newLabel );
            
            return newLayout;
        }
    }
    
    return NULL;
}

int CloudsHUDController::getFontSizeForMesh( SVGMesh* textMesh ){
    if( !textMesh ){
        ofLogError("CloudsHUDController :: Text box not found");
        return 0;
    }
    
    int fontSize = 0;
    float textBoxHeight = textMesh->bounds.height;
    
    for( int k=0; k<tempFontList.size()-1; k++){
        if( tempFontList[k]->getLineHeight() <= textBoxHeight && tempFontList[k+1]->getLineHeight() > textBoxHeight ){
            fontSize = 1 + k;
            break;
        }
    }
    
    return fontSize;
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
	drawLayer(CLOUDS_HUD_MAP);
    
    for( int i=0; i<hudLabelList.size(); i++ ){
        hudLabelList[i].draw();
    }
    
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
    if( layer == CLOUDS_HUD_FULL ){
        for( int i=0; i<layerSets.size(); i++ ){
            for(int k = 0; k < layerSets[(CloudsHUDLayerSet)i].size(); i++){
                layerSets[(CloudsHUDLayerSet)i][k]->start();
            }
        }
    }
    else{
        for(int i = 0; i < layerSets[layer].size(); i++){
            layerSets[layer][i]->start();
        }
    }
}

void CloudsHUDController::animateOff(){
	
}

void CloudsHUDController::saveGuiSettings(){
	
}

void CloudsHUDController::toggleGuis(){
	
}

