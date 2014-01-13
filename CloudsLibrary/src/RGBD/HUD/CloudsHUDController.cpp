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
#include "CloudsVisualSystem.h"

CloudsHUDController::CloudsHUDController(){
	hudGui = NULL;
    bIsHudOpen = false;
    bSkipAVideoFrame = false;
    bDrawHud = true;
    bDrawHome = true;
	
    scaleAmt = 1.0;
    margin = 40;
    
#ifdef OCULUS_RIFT
    for (int i = 0; i < CLOUDS_HUD_LAYER_COUNT; i++) {
        layerDistance[i] = 300;
        layerRotation[i] = 0;
        layerBillboard[i] = CLOUDS_HUD_BILLBOARD_CAMERA;
    }
#endif
}

void CloudsHUDController::setup(){
	buildLayerSets();
    calculateFontSizes();
	
	home.setup();
    
    cout << "canvas width: " << ofGetWidth() << endl;
    cout << "cancas height: " << ofGetHeight() << endl;
}

void CloudsHUDController::actBegan(CloudsActEventArgs& args){
//	bDrawHud = true;
}

void CloudsHUDController::actEnded(CloudsActEventArgs& args){
	animateOff( CLOUDS_HUD_FULL );
}

void CloudsHUDController::clipBegan(CloudsClipEventArgs& args){
	respondToClip(args.chosenClip);
}

void CloudsHUDController::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	bDrawHud = false;
//    animateOff();
}

void CloudsHUDController::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	bDrawHud = true;
}

void CloudsHUDController::questionProposed(CloudsQuestionEventArgs& args){
//    populateQuestion( args.question, true);
}
void CloudsHUDController::questionSelected(CloudsQuestionEventArgs& args){
    populateQuestion( args.question, true);
}

void CloudsHUDController::topicChanged(CloudsTopicEventArgs& args){
    animateOff( CLOUDS_HUD_QUESTION );
}

void CloudsHUDController::preRollRequested(CloudsPreRollEventArgs& args){
	
}

void CloudsHUDController::respondToClip(CloudsClip& clip){
	
//	cout << "ID's on clip " << clip.name << " and fcp id? " << clip.fcpFileId << endl;
//	cout << "Clip is " <<  clip.getLinkName() << endl;
//	cout << "speaker: " << speaker.firstName << " " << speaker.lastName << endl;
	
// LOWER THIRD
	
    
    //update lower third, but only if the speaker has changed
    if(speaker.fcpID != CloudsSpeaker::speakers[ clip.person ].fcpID){
        speaker = CloudsSpeaker::speakers[ clip.person ];
        populateLowerThird(speaker.firstName, speaker.lastName, speaker.location2, speaker.title, speaker.byline1, true );
    }
    
// PROJECT EXAMPLE
	if(clip.hasProjectExample && clip.projectExample.exampleVideos.size() ){
		CloudsProjectExample example = clip.projectExample;
        string videoPath = example.exampleVideos[ (int)ofRandom(0, example.exampleVideos.size()) ];
        populateProjectExample( videoPath, example.creatorName, "", example.title, true );
	}
	else{
        animateOff(CLOUDS_HUD_PROJECT_EXAMPLE);
    }
}

void CloudsHUDController::questionHoverOn(string question){
	populateQuestion(question,true);
}

void CloudsHUDController::questionHoverOff(){
	animateOff( CLOUDS_HUD_QUESTION );
}

void CloudsHUDController::populateMap( string leftBox, string rightBox, bool forceOn){
    hudLabelMap["TopicTextBoxLeft"]->setText( leftBox );
    hudLabelMap["TopicTextBoxRight"]->setText( rightBox );
    
    if( forceOn ){
        animateOn( CLOUDS_HUD_MAP );
    }
}

void CloudsHUDController::populateQuestion( string question, bool forceOn ){
	if(hudLabelMap["QuestionTextBox"]->getText() == question){
		return;
	}
	else if(question == ""){
		animateOff( CLOUDS_HUD_QUESTION );
	}
	else{
		hudLabelMap["QuestionTextBox"]->setText( question );
		
		if( forceOn ){
			animateOn( CLOUDS_HUD_QUESTION );
		}
	}
}

//BIO
void CloudsHUDController::populateLowerThird( string firstName, string lastName, string title, string location, string textbox, bool forceOn ) {
    
    CloudsHUDLabel* firstNameLabel  = hudLabelMap["BylineFirstNameTextBox_1_"];
    CloudsHUDLabel* lastNameLabel  = hudLabelMap["BylineLastNameTextBox"];
    
    firstNameLabel->setText( firstName );
    lastNameLabel->setText( lastName );
    
    int firstNameRight = firstNameLabel->getRightEdge();
    int lastNameRight = lastNameLabel->getRightEdge();
    int rightEdge = 0;
    
    if(firstNameRight > lastNameRight)
        rightEdge = firstNameRight;
    else
        rightEdge = lastNameRight;

    cout<< "right edge: " << rightEdge << endl;
    
    //move these over to float left of name
    CloudsHUDLabel* locationLabel = hudLabelMap["BylineTopicTextBoxTop"];
    CloudsHUDLabel* titleLabel = hudLabelMap["BylineTopicTextBoxBottom"];
    locationLabel->bounds.x = rightEdge + margin;
    titleLabel->bounds.x = rightEdge + margin;
    
    locationLabel->setText( location );
    titleLabel->setText( title );
    
    //description
    ////reset to default
    CloudsHUDLabel* descLabel = hudLabelMap["BylineBodyCopyTextBox"];
    descLabel->bounds = defaultBioBounds;
    descLabel->layout->setLineLength(defaultBioBounds.width);
    int descLeftEdge = descLabel->bounds.getLeft();
    
    if(locationLabel->getRightEdge() > titleLabel->getRightEdge())
        rightEdge = locationLabel->getRightEdge();
    else
        rightEdge = titleLabel->getRightEdge();
    
    if(rightEdge + margin >= descLeftEdge){
        descLabel->bounds.x = rightEdge+margin;
        descLabel->layout->setLineLength(defaultBioBounds.width - (descLabel->bounds.x - defaultBioBounds.x));
    }
    
    descLabel->setText( textbox );
    
    if( forceOn ){
        animateOn( CLOUDS_HUD_LOWER_THIRD );
    }
}

void CloudsHUDController::populateProjectExample(string videoPath, string textLeft, string textRight, string textTop, bool forceOn) {
    if( videoPlayer.isPlaying() ){
        videoPlayer.stop();
    }
    
    if( ofFile(videoPath).exists() ){
        videoPlayer.loadMovie(videoPath);
        videoPlayer.play();
        
        bSkipAVideoFrame = true;
        
        hudLabelMap["ProjectExampleTextboxLeft"]->setText( textLeft );
        hudLabelMap["ProjectExampleTextboxRight"]->setText( textRight );
        hudLabelMap["ProjectExampleTextBoxTop"]->setText( textTop );
        
        if( forceOn ){
            animateOn( CLOUDS_HUD_PROJECT_EXAMPLE );
        }
    }else{
        cout << "CloudsHUDController :: Project example video does not exist: " << videoPath << endl;
    }
}

void CloudsHUDController::buildLayerSets(){
	
	//configure layers
	CloudsHUDLayerSet currentLayer;

	currentLayer = CLOUDS_HUD_QUESTION;
    
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
    
	//////////
	//TODO REPLACE WITH LINE WIPE SHADERS
    for( int i=0; i<allLayers.size(); i++ ){
        allLayers[i]->duration = 1.5;
        allLayers[i]->delayTime = 0;
        
        allLayers[i]->startPoint = ofVec2f(allLayers[i]->svg.getWidth(),0);
        allLayers[i]->endPoint   = ofVec2f(0,allLayers[i]->svg.getHeight());
    }
    ///////////
	
    home.bounds = lowerThirdLayer->svg.getMeshByID("HomeButtonFrame")->bounds;
    home.bounds.scaleFromCenter(1.5);
    
    svgVideoBounds = projectExampleLayer->svg.getMeshByID("ProjectExampleFrame")->bounds;
	videoBounds = svgVideoBounds;
    
    hudBounds.set( 0, 0, allLayers[0]->svg.getWidth(), allLayers[0]->svg.getHeight() );
    
//	cout << "HUD BOUNDS " << hudBounds.width << " / " << hudBounds.height << endl;
//    cout << "SCREEN " << ofGetScreenWidth() << " / " << ofGetScreenHeight() << endl;
}

void CloudsHUDController::calculateFontSizes(){
    // temporary allocate
    int minFontSize = 1;
    int maxFontSize = 70;
    
    for(int i = minFontSize; i < maxFontSize; i++){
        ofxFTGLFont *tmp = new ofxFTGLFont();
        tmp->loadFont( GetCloudsDataPath() + "font/Blender-THIN.ttf", i );
        tempFontList.push_back( tmp );
    }

    //BIO
    ////first name
    
    BylineFirstNameTextBox      = getFontForLayer("BylineFirstNameTextBox_1_", GetCloudsDataPath() + "font/Blender-THIN.ttf", 50);
    ////last name
    BylineLastNameTextBox       = getFontForLayer("BylineLastNameTextBox", GetCloudsDataPath() + "font/Blender-THIN.ttf", 50);
    ////title
    BylineTopicTextBoxBottom    = getFontForLayer("BylineTopicTextBoxBottom", GetCloudsDataPath() + "font/Blender-THIN.ttf", 35);
    ////location
    BylineTopicTextBoxTop       = getFontForLayer("BylineTopicTextBoxTop", GetCloudsDataPath() + "font/Blender-THIN.ttf", 35);
    ////description
    BylineBodyCopyTextBox       = getLayoutForLayer("BylineBodyCopyTextBox", GetCloudsDataPath() + "font/Blender-THIN.ttf", false);
    defaultBioBounds            = hudLabelMap["BylineBodyCopyTextBox"]->bounds;
    
    ResetButtonTextBox          = getLayoutForLayer("ResetButtonTextBox", GetCloudsDataPath() + "font/Blender-THIN.ttf");
    QuestionTextBox             = getLayoutForLayer("QuestionTextBox", GetCloudsDataPath() + "font/Blender-THIN.ttf");
    TopicTextBoxLeft            = getLayoutForLayer("TopicTextBoxLeft", GetCloudsDataPath() + "font/Blender-THIN.ttf");
    TopicTextBoxRight           = getLayoutForLayer("TopicTextBoxRight", GetCloudsDataPath() + "font/Blender-THIN.ttf");
    ProjectExampleTextboxLeft   = getLayoutForLayer("ProjectExampleTextboxLeft", GetCloudsDataPath() + "font/Blender-THIN.ttf");
    ProjectExampleTextboxRight  = getLayoutForLayer("ProjectExampleTextboxRight", GetCloudsDataPath() + "font/Blender-THIN.ttf");
    ProjectExampleTextBoxTop    = getLayoutForLayer("ProjectExampleTextBoxTop", GetCloudsDataPath() + "font/Blender-THIN.ttf");
    
    // cleanup!
    for( int i=0; i<tempFontList.size(); i++ ){
        delete tempFontList[i];
    }
    tempFontList.clear();
}

ofxFTGLSimpleLayout* CloudsHUDController::getLayoutForLayer( string layerName, string fontPath ) {
    for( int i=0; i<allLayers.size(); i++ ){
        SVGMesh* textMesh = allLayers[i]->svg.getMeshByID( layerName );
        
        if( textMesh != NULL ){
            textMesh->visible = false;
            
            float maxHeight = textMesh->bounds.height;
            int fontSize = getFontSizeForMesh( textMesh );
            
//            cout << "The correct font size is " << fontSize << " << endl;
            
            // make a layout
            ofxFTGLSimpleLayout *newLayout = new ofxFTGLSimpleLayout();
            newLayout->loadFont( fontPath, fontSize );
            newLayout->setLineLength( 999 );
            
            if( layerName == "BylineBodyCopyTextBox" ){         // this is the main body copy in the lower thirds
                newLayout->loadFont( fontPath, floor(fontSize/4.5) );
                newLayout->setLineLength( textMesh->bounds.width );
            }
            
            // make a label
            CloudsHUDLabel *newLabel = new CloudsHUDLabel();
            newLabel->setup( newLayout, textMesh->bounds );
            hudLabelMap[layerName] = newLabel;
            
            return newLayout;
        }
    }
    
    return NULL;
}

ofxFTGLSimpleLayout* CloudsHUDController::getLayoutForLayer( string layerName, string fontPath, bool caps ) {
    for( int i=0; i<allLayers.size(); i++ ){
        SVGMesh* textMesh = allLayers[i]->svg.getMeshByID( layerName );
        
        if( textMesh != NULL ){
            textMesh->visible = false;
            
            float maxHeight = textMesh->bounds.height;
            int fontSize = getFontSizeForMesh( textMesh );
            
            //            cout << "The correct font size is " << fontSize << " << endl;
            
            // make a layout
            ofxFTGLSimpleLayout *newLayout = new ofxFTGLSimpleLayout();
            newLayout->loadFont( fontPath, fontSize );
            newLayout->setLineLength( 999 );
            
            if( layerName == "BylineBodyCopyTextBox" ){         // this is the main body copy in the lower thirds
                newLayout->loadFont( fontPath, floor(fontSize/5) );
                newLayout->setLineLength( textMesh->bounds.width );
            }
            
            // make a label
            CloudsHUDLabel *newLabel = new CloudsHUDLabel();
            newLabel->setup( newLayout, textMesh->bounds );
            newLabel->caps = caps;
            hudLabelMap[layerName] = newLabel;
            
            return newLayout;
        }
    }
    
    return NULL;
    
}

ofxFTGLFont* CloudsHUDController::getFontForLayer( string layerName, string fontPath, int kerning ) {
    for( int i=0; i<allLayers.size(); i++ ){
        SVGMesh* textMesh = allLayers[i]->svg.getMeshByID( layerName );
        
        
        if( textMesh != NULL ){
            textMesh->visible = false;
            
            float maxHeight = textMesh->bounds.height;
            int fontSize = getFontSizeForMesh( textMesh );
            
            //            cout << "The correct font size is " << fontSize << " << endl;
            
            // make a layout
            ofxFTGLFont *newFont = new ofxFTGLFont();
            newFont->setTracking(kerning * .08);
            newFont->loadFont( fontPath, fontSize );
           // newLayout->setLineLength( 999 );
            
            if( layerName == "BylineBodyCopyTextBox" ){         // this is the main body copy in the lower thirds
                newFont->loadFont( fontPath, floor(fontSize/4.5) );
              //  newFont->setLineLength( textMesh->bounds.width );
            }
            
            // make a label
            CloudsHUDLabel *newLabel = new CloudsHUDLabel();
            newLabel->setup( newFont, textMesh->bounds );
            //newLabel->caps = caps;
            hudLabelMap[layerName] = newLabel;
            
            return newFont;
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
        if( tempFontList[k]->getStringBoundingBox("W",0,0).height <= textBoxHeight && tempFontList[k+1]->getStringBoundingBox("W",0,0).height > textBoxHeight ){
            fontSize = 1 + k;
            break;
        }
    }
    
    return fontSize;
}

ofVec2f CloudsHUDController::getSize(bool bScaled){
    return ofVec2f(hudBounds.width, hudBounds.height) * (bScaled? scaleAmt : 1.0);
}

ofVec2f CloudsHUDController::getCenter(bool bScaled){
    return ofVec2f(hudBounds.width * 0.5, hudBounds.height * 0.5) * (bScaled? scaleAmt : 1.0);
}

void CloudsHUDController::update(){
	for(int i = 0; i < allLayers.size(); i++){
		allLayers[i]->update();
	}
    
    float xScale = ofGetWindowWidth()/hudBounds.width;
    float yScale = ofGetWindowHeight()/hudBounds.height;
    
    scaleAmt = (xScale < yScale) ? xScale : yScale;

    home.hudScale = scaleAmt;
	home.update();
    
    if( videoPlayer.isPlaying() ){
        if( videoPlayer.isFrameNew() ){
            bSkipAVideoFrame = false;
            
            videoBounds.set(0, 0, videoPlayer.getWidth(), videoPlayer.getHeight() );
            videoBounds.scaleTo( svgVideoBounds );
        }
        videoPlayer.update();
    }
    
    if( home.wasHomeOpened() ){
        if( !bIsHudOpen ){
            animateOn( CLOUDS_HUD_FULL );
        }else{
            animateOff();
        }
    }
}

void CloudsHUDController::setHomeEnabled(bool enable){
	bDrawHome = enable;
}

void CloudsHUDController::draw(){
    
    if( !bDrawHud )
        return;
    
	ofPushStyle();
	ofPushMatrix();
	ofEnableAlphaBlending();
	
    ofTranslate( (ofGetWindowSize() - getSize() ) * 0.5 );
    ofScale( scaleAmt, scaleAmt );
    
    // EZ: Debug overlay rect dimensions
//    ofSetColor(255, 0, 0, 127);
//    ofRect(0, 0, hudBounds.width, hudBounds.height);
//    ofSetColor(255);
    
    if( videoPlayer.isPlaying() ){
        ofSetColor(255, 255, 255, 255*0.7);
        if( !bSkipAVideoFrame ){
            videoPlayer.draw( videoBounds.x, videoBounds.y, videoBounds.width, videoBounds.height );
        }
        ofSetColor(255, 255, 255, 255);
    }
    
	drawLayer(CLOUDS_HUD_QUESTION);
	
//	cout << "drawing question: " << hudLabelMap["QuestionTextBox"]->getText() << endl;
	
	drawLayer(CLOUDS_HUD_LOWER_THIRD);
	drawLayer(CLOUDS_HUD_PROJECT_EXAMPLE);
	drawLayer(CLOUDS_HUD_MAP);
    
    for( map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it ){
        (it->second)->draw();
    }
    
	if(bDrawHome)
		home.draw();
	
	
	ofPopMatrix();
	ofPopStyle();
}

void CloudsHUDController::drawLayer(CloudsHUDLayerSet layer){
	for(int i = 0; i < layerSets[layer].size(); i++){
		layerSets[layer][i]->draw();
	}
}

#ifdef OCULUS_RIFT
void CloudsHUDController::draw3D(ofCamera* cam){
    
    if( !bDrawHud )
        return;
    
	ofPushStyle();
	ofPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    ofEnableAlphaBlending();
    
    drawLayer3D(CLOUDS_HUD_QUESTION, cam);
    drawLayer3D(CLOUDS_HUD_LOWER_THIRD, cam);
	drawLayer3D(CLOUDS_HUD_PROJECT_EXAMPLE, cam);
	drawLayer3D(CLOUDS_HUD_MAP, cam);
	
    glPopAttrib();
	ofPopMatrix();
	ofPopStyle();
}

void CloudsHUDController::drawLayer3D(CloudsHUDLayerSet layer, ofCamera* cam){
    ofPushMatrix();
    
    ofVec3f camPos = cam->getGlobalPosition();
    
    // Calculate the base position.
    static ofVec3f upAxis = ofVec3f(0.0, 1.0, 0.0);
    ofVec3f basePos = camPos + (cam->getLookAtDir().getScaled(layerDistance[layer]));
    basePos.rotate(layerRotation[layer], camPos, upAxis);
    
    // Get the total layer bounds.
    ofRectangle layerBounds;
	for(int i = 0; i < layerSets[layer].size(); i++){
        if (i == 0) layerBounds = layerSets[layer][i]->svg.getBounds();
        else layerBounds.growToInclude(layerSets[layer][i]->svg.getBounds());
	}
    
    // Translate to the layer center pos.
    ofVec3f layerPos = basePos + (getCenter(false) - layerBounds.getCenter());
    ofTranslate(layerPos);

    if (layerBillboard[layer] == CLOUDS_HUD_BILLBOARD_OCULUS) {
        // Billboard rotation using the Oculus orientation.
//        ofVec3f eulerAngles = (CloudsVisualSystem::getOculusRift().getOrientationQuat() * cam.getOrientationQuat()).getEuler();
//        ofRotateY(layerRotation[layer]);
//        ofRotate(-eulerAngles.z, 1, 0, 0);
        float angle;
        ofVec3f axis;
        (CloudsVisualSystem::getOculusRift().getOrientationQuat() * cam->getOrientationQuat()).getRotate(angle, axis);
        ofRotate(angle, axis.x, axis.y, axis.z);
        ofScale(-1, 1, 1);
    }
    else if (layerBillboard[layer] == CLOUDS_HUD_BILLBOARD_CAMERA) {
        // Billboard rotation using the camera.
        ofNode node;
        node.setPosition(layerPos);
        node.lookAt(camPos);
        ofVec3f axis;
        float angle;
        node.getOrientationQuat().getRotate(angle, axis);
        ofRotate(angle, axis.x, axis.y, axis.z);
    }
    else {
        ofRotateY(layerRotation[layer]);
    }
    
    // Debug circle.
//    ofSetColor(255);
//    ofCircle(0, 0, 25);
    
    // Draw the video player if we're on the right layer.
    if (layer == CLOUDS_HUD_PROJECT_EXAMPLE && videoPlayer.isPlaying()) {
        ofSetColor(255, 255, 255, 255*0.7);
        if( !bSkipAVideoFrame ){
            videoPlayer.draw( videoBounds.x, videoBounds.y, videoBounds.width, videoBounds.height );
        }
    }
    
    // Draw the layer.
    ofScale(-scaleAmt, -scaleAmt, 1);
    ofTranslate(-layerBounds.getCenter());
    ofSetColor(255);
    drawLayer(layer);
    
    // Draw the home button if we're on the right layer.
    if (layer == CLOUDS_HUD_LOWER_THIRD && bDrawHome) {
        home.draw();
    }
    
    // Draw the associated text labels.
    for( map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it ){
        bool bFound = false;
        for(int i = 0; i < layerSets[layer].size(); i++){
            if (layerSets[layer][i]->svg.getMeshByID(it->first) != NULL) {
                bFound = true;
                break;
            }
        }
        if (bFound) {
            (it->second)->draw();
        }
    }
    
    ofPopMatrix();
}
#endif

void CloudsHUDController::animateOn(CloudsHUDLayerSet layer){
    //bIsHudOpen = true;
	
    if(hudOpenMap[layer]){
		return;
	}
	
    if( layer == CLOUDS_HUD_FULL ){
        for( int i=0; i<layerSets.size(); i++ ){
            for(int k = 0; k < layerSets[(CloudsHUDLayerSet)i].size(); i++){
                layerSets[(CloudsHUDLayerSet)i][k]->start();
				hudOpenMap[(CloudsHUDLayerSet)i] = true;
            }
        }
    }
    else {
        for(int i = 0; i < layerSets[layer].size(); i++){
            layerSets[layer][i]->start();
			hudOpenMap[layer] = true;
        }
		hudOpenMap[layer] = true;	
    }
	
	
}

void CloudsHUDController::animateOff(CloudsHUDLayerSet layer){
	//bIsHudOpen = false;
	if(!hudOpenMap[layer]){
		return;
	}

    if( videoPlayer.isPlaying() ){
        videoPlayer.stop();
        videoPlayer.close();
    }
    
    if( layer == CLOUDS_HUD_FULL ){
        for( int i=0; i<layerSets.size(); i++ ){
            for(int k = 0; k < layerSets[(CloudsHUDLayerSet)i].size(); i++){
                layerSets[(CloudsHUDLayerSet)i][k]->close();
				hudOpenMap[(CloudsHUDLayerSet)i] = false;
            }
        }
    }
    else{
        for(int i = 0; i < layerSets[layer].size(); i++){
            layerSets[layer][i]->close();
			hudOpenMap[layer] = false;
        }
    }
    
    // animate out text, this is sub-optimal
    if( layer == CLOUDS_HUD_FULL ){
        for( map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it ){
            (it->second)->animateOut();
        }
    }else if( layer == CLOUDS_HUD_LOWER_THIRD ){
        hudLabelMap["BylineFirstNameTextBox_1_"]->animateOut();
        hudLabelMap["BylineLastNameTextBox"]->animateOut();
        hudLabelMap["BylineTopicTextBoxTop"]->animateOut();
        hudLabelMap["BylineTopicTextBoxBottom"]->animateOut();
        hudLabelMap["BylineBodyCopyTextBox"]->animateOut();
    }else if( layer == CLOUDS_HUD_PROJECT_EXAMPLE ){
        hudLabelMap["ProjectExampleTextboxLeft"]->animateOut();
        hudLabelMap["ProjectExampleTextboxRight"]->animateOut();
        hudLabelMap["ProjectExampleTextBoxTop"]->animateOut();
    }else if( layer == CLOUDS_HUD_MAP ){
        
    }else if( layer == CLOUDS_HUD_QUESTION ){
        hudLabelMap["QuestionTextBox"]->animateOut();
    }
}

void CloudsHUDController::saveGuiSettings(){
	
}

void CloudsHUDController::toggleGuis(){
	
}

