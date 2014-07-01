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
#ifdef OCULUS_RIFT
#include "CloudsVisualSystem.h"
#endif

CloudsHUDController::CloudsHUDController(){
	hudGui = NULL;
    bIsHudOpen = false;
    bSkipAVideoFrame = false;
    bDrawHud = true;
    bDrawHome = true;


    bActJustStarted = false;
	cuedClipEndTime = 0;
    bVisualSystemDisplayed = false;
    bLowerThirdCued = false;
	
	bResetIsHovered = false;
	bResetIsPressed = false;
	bResetIsClicked = false;

    scaleAmt = 1.0;
    margin = 40;
    
#ifdef OCULUS_RIFT
    // set defaults
    // there might be a better way of doing this...
    layerDistance[CLOUDS_HUD_QUESTION]         = 300;
    layerDistance[CLOUDS_HUD_LOWER_THIRD]      = 300;
    layerDistance[CLOUDS_HUD_PROJECT_EXAMPLE]  = 300;
    layerDistance[CLOUDS_HUD_MAP]              = 300;
    
    layerRotationH[CLOUDS_HUD_QUESTION]        = 0;
    layerRotationH[CLOUDS_HUD_LOWER_THIRD]     = 0;
    layerRotationH[CLOUDS_HUD_PROJECT_EXAMPLE] = 0;
    layerRotationH[CLOUDS_HUD_MAP]             = 0;
    
    layerRotationV[CLOUDS_HUD_QUESTION]        = 0;
    layerRotationV[CLOUDS_HUD_LOWER_THIRD]     = 0;
    layerRotationV[CLOUDS_HUD_PROJECT_EXAMPLE] = 0;
    layerRotationV[CLOUDS_HUD_MAP]             = 0;
    
    layerBillboard[CLOUDS_HUD_QUESTION]        = CLOUDS_HUD_BILLBOARD_CAMERA;
    layerBillboard[CLOUDS_HUD_LOWER_THIRD]     = CLOUDS_HUD_BILLBOARD_CAMERA;
    layerBillboard[CLOUDS_HUD_PROJECT_EXAMPLE] = CLOUDS_HUD_BILLBOARD_CAMERA;
    layerBillboard[CLOUDS_HUD_MAP]             = CLOUDS_HUD_BILLBOARD_CAMERA;
#endif
}

void CloudsHUDController::setup(){
	
	buildLayerSets();
    calculateFontSizes();

#ifdef MOUSE_INPUT
	ofAddListener(ofEvents().mouseMoved,this, &CloudsHUDController::mouseMoved);
	ofAddListener(ofEvents().mousePressed,this, &CloudsHUDController::mousePressed);
	ofAddListener(ofEvents().mouseReleased,this, &CloudsHUDController::mouseReleased);
#endif

	hudLabelMap["ResetButtonTextBox"]->setText("RESET");

	home.setup();
    
    cout << "canvas width: " << ofGetWidth() << endl;
    cout << "cancas height: " << ofGetHeight() << endl;
}

void CloudsHUDController::actBegan(CloudsActEventArgs& args){
	bDrawHud = true;
	bActJustStarted = true;
	animateOn( CLOUDS_HUD_QUESTION );
}

void CloudsHUDController::actEnded(CloudsActEventArgs& args){
	animateOff( CLOUDS_HUD_FULL );
}

void CloudsHUDController::clearQuestion(){
	hudLabelMap["QuestionTextBox"]->setText("", false);
}

void CloudsHUDController::clipBegan(CloudsClipEventArgs& args){
	respondToClip(args.chosenClip);
}

void CloudsHUDController::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	bDrawHud = false;
//    animateOff();
    bVisualSystemDisplayed = true;
}

void CloudsHUDController::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	bDrawHud = true;
    bVisualSystemDisplayed = false;
}

void CloudsHUDController::questionProposed(CloudsQuestionEventArgs& args){
//    populateQuestion( args.question, true);
}

void CloudsHUDController::questionSelected(CloudsQuestionEventArgs& args){
    populateQuestion( args.question, true);
}

void CloudsHUDController::topicChanged(CloudsTopicEventArgs& args){
	if(!bActJustStarted){
		animateOff( CLOUDS_HUD_QUESTION );
	}
	bActJustStarted = false;
}

void CloudsHUDController::preRollRequested(CloudsPreRollEventArgs& args){
	
}

void CloudsHUDController::respondToClip(CloudsClip* clip){
	
//	cout << "ID's on clip " << clip.name << " and fcp id? " << clip.fcpFileId << endl;
//	cout << "Clip is " <<  clip.getLinkName() << endl;
//	cout << "speaker: " << speaker.firstName << " " << speaker.lastName << endl;
	    
	//LOWER THIRD
    //update lower third, but only if the speaker has changed
    if(speaker.fcpID != CloudsSpeaker::speakers[ clip->person ].fcpID){
        speaker = CloudsSpeaker::speakers[ clip->person ];
        populateLowerThird(speaker.firstName, speaker.lastName, speaker.location2, speaker.title, speaker.byline1, false );
        
        if (bVisualSystemDisplayed) {
            // cue up the lower third until the visual system is done
            bLowerThirdCued = true;
            cuedClipEndTime = ofGetElapsedTimef() + clip->getDuration();
        }
        else {
            // display the lower third right away
            animateOn(CLOUDS_HUD_LOWER_THIRD);
        }
    }
    
// PROJECT EXAMPLE
#ifndef OCULUS_RIFT
	if(clip->hasProjectExample && clip->projectExample.exampleVideos.size() ){
		CloudsProjectExample example = clip->projectExample;
        string videoPath = example.exampleVideos[ (int)ofRandom(0, example.exampleVideos.size()) ];
        populateProjectExample( videoPath, example.creatorName, "", example.title, true );
	}
	else{
        animateOff(CLOUDS_HUD_PROJECT_EXAMPLE);
    }
#endif
}

void CloudsHUDController::questionHoverOn(const string& question,bool animate){
	populateQuestion(question, true, animate);
}

void CloudsHUDController::questionHoverOff(){
	animateOff( CLOUDS_HUD_QUESTION );
}

void CloudsHUDController::playCued(){
    if (bLowerThirdCued) {
        // display the lower third if the cued clip is still running
        if (ofGetElapsedTimef() < cuedClipEndTime) {
            animateOn(CLOUDS_HUD_LOWER_THIRD);
        }
        bLowerThirdCued = false;
    }
}

void CloudsHUDController::populateMap(const string& leftBox, const string& rightBox, bool forceOn){
    hudLabelMap["TopicTextBoxLeft"]->setText( leftBox, forceOn );
    hudLabelMap["TopicTextBoxRight"]->setText( rightBox, forceOn );
    
    if( forceOn ){
        animateOn( CLOUDS_HUD_MAP );
    }
}

void CloudsHUDController::populateQuestion(const string& question, bool forceOn, bool animate){
    if(question == ""){
		animateOff( CLOUDS_HUD_QUESTION );
	}
	else{
		hudLabelMap["QuestionTextBox"]->setText( question, forceOn );
		if( forceOn ){
			if(animate){
				animateOn( CLOUDS_HUD_QUESTION );
			}
			else{
				hudLabelMap["QuestionTextBox"]->instantIn();
			}
		}
	}
}

//BIO
void CloudsHUDController::populateLowerThird(const string& firstName, const string& lastName, const string& title, const string& location, const string& textbox, bool forceOn) {
    
    CloudsHUDLabel* firstNameLabel  = hudLabelMap["BylineFirstNameTextBox_1_"];
    CloudsHUDLabel* lastNameLabel  = hudLabelMap["BylineLastNameTextBox"];
    
    firstNameLabel->setText( firstName, forceOn );
    lastNameLabel->setText( lastName, forceOn );
    
    int firstNameRight = firstNameLabel->getRightEdge();
    int lastNameRight = lastNameLabel->getRightEdge();
    int rightEdge = 0;
    
    if(firstNameRight > lastNameRight){
        rightEdge = firstNameRight;
    }
	else{
        rightEdge = lastNameRight;
	}

//    cout<< "right edge: " << rightEdge << endl;
    
    //move these over to float left of name
    CloudsHUDLabel* locationLabel = hudLabelMap["BylineTopicTextBoxTop"];
    CloudsHUDLabel* titleLabel = hudLabelMap["BylineTopicTextBoxBottom"];
    locationLabel->bounds.x = rightEdge + margin;
    titleLabel->bounds.x = rightEdge + margin;
    
    locationLabel->setText( location, forceOn );
    titleLabel->setText( title, forceOn );
    
    //description
    ////reset to default
    CloudsHUDLabel* descLabel = hudLabelMap["BylineBodyCopyTextBox"];
    descLabel->bounds = defaultBioBounds;
    descLabel->layout->setLineLength(defaultBioBounds.width);
    int descLeftEdge = descLabel->bounds.getLeft();
    
    if(locationLabel->getRightEdge() > titleLabel->getRightEdge()){
        rightEdge = locationLabel->getRightEdge();
	}
	else{
        rightEdge = titleLabel->getRightEdge();
	}

    if(rightEdge + margin >= descLeftEdge){
        descLabel->bounds.x = rightEdge+margin;
        descLabel->layout->setLineLength(defaultBioBounds.width - (descLabel->bounds.x - defaultBioBounds.x));
    }
    
    descLabel->setText( textbox, forceOn );
    
    if( forceOn ){
        animateOn( CLOUDS_HUD_LOWER_THIRD );
    }
}

void CloudsHUDController::populateProjectExample(const string& videoPath, const string& textLeft, const string& textRight, const string& textTop, bool forceOn) {
    if( videoPlayer.isPlaying() ){
        videoPlayer.stop();
    }
    
    if( ofFile(videoPath).exists() ){
        videoPlayer.loadMovie(videoPath);
        videoPlayer.play();
        
        bSkipAVideoFrame = true;
        
        hudLabelMap["ProjectExampleTextboxLeft"]->setText( textLeft, forceOn );
        hudLabelMap["ProjectExampleTextboxRight"]->setText( textRight, forceOn );
        hudLabelMap["ProjectExampleTextBoxTop"]->setText( textTop, forceOn );
        
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
    #ifdef OCULUS_RIFT
	string fontPath = GetCloudsDataPath() + "font/Blender-MEDIUM.ttf";
	#else
	string fontPath = GetCloudsDataPath() + "font/Blender-THIN.ttf";
	#endif

    for(int i = minFontSize; i < maxFontSize; i++){
        ofxFTGLFont *tmp = new ofxFTGLFont();
        tmp->loadFont(fontPath , i );
        tempFontList.push_back( tmp );
    }

    //BIO
    ////first name
    BylineFirstNameTextBox      = getFontForLayer("BylineFirstNameTextBox_1_", fontPath, 50);
    ////last name
    BylineLastNameTextBox       = getFontForLayer("BylineLastNameTextBox", fontPath, 50);
    ////title
    BylineTopicTextBoxBottom    = getFontForLayer("BylineTopicTextBoxBottom", fontPath, 35);
    ////location
    BylineTopicTextBoxTop       = getFontForLayer("BylineTopicTextBoxTop", fontPath, 35);
    ////description
    BylineBodyCopyTextBox       = getLayoutForLayer("BylineBodyCopyTextBox", fontPath, false);
    defaultBioBounds            = hudLabelMap["BylineBodyCopyTextBox"]->bounds;
    
    ResetButtonTextBox          = getLayoutForLayer("ResetButtonTextBox", fontPath);
    QuestionTextBox             = getLayoutForLayer("QuestionTextBox", fontPath);
    TopicTextBoxLeft            = getLayoutForLayer("TopicTextBoxLeft", fontPath);
    TopicTextBoxRight           = getLayoutForLayer("TopicTextBoxRight", fontPath);
    ProjectExampleTextboxLeft   = getLayoutForLayer("ProjectExampleTextboxLeft", fontPath);
    ProjectExampleTextboxRight  = getLayoutForLayer("ProjectExampleTextboxRight", fontPath);
    ProjectExampleTextBoxTop    = getLayoutForLayer("ProjectExampleTextBoxTop", fontPath);
    
    // cleanup!
    for( int i=0; i<tempFontList.size(); i++ ){
        delete tempFontList[i];
    }
    tempFontList.clear();
}

ofxFTGLSimpleLayout* CloudsHUDController::getLayoutForLayer(const string& layerName, const string& fontPath) {
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

ofxFTGLSimpleLayout* CloudsHUDController::getLayoutForLayer(const string& layerName, const string& fontPath, bool caps) {
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

ofxFTGLFont* CloudsHUDController::getFontForLayer(const string& layerName, const string& fontPath, int kerning) {
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
	//HACK TO KEEP QUESTION ON after asked
	if(hudLabelMap["QuestionTextBox"]->getText() != "" && !hudLabelMap["QuestionTextBox"]->isVisible()){
		hudLabelMap["QuestionTextBox"]->animateIn();
		
	}
//	cout << "CURRENT QUESTION " << hudLabelMap["QuestionTextBox"]->getText() << " VISIBLE? " << (hudLabelMap["QuestionTextBox"]->isVisible() ? "YES" : "NO") << endl;

	for(int i = 0; i < allLayers.size(); i++){
		allLayers[i]->update();
	}
    
    float xScale = ofGetWindowWidth()/hudBounds.width;
    float yScale = ofGetWindowHeight()/hudBounds.height;
    
	bool xDominantScale = xScale < yScale;
    scaleAmt	= xDominantScale ? xScale : yScale;
	scaleOffset = xDominantScale ? 
		ofVec2f(0, ofGetWindowHeight()- hudBounds.height*scaleAmt)*.5 :
		ofVec2f(ofGetWindowWidth() - hudBounds.width*scaleAmt, 0)*.5;


    if( videoPlayer.isPlaying() ){
        if( videoPlayer.isFrameNew() ){
            bSkipAVideoFrame = false;
            
            videoBounds.set(0, 0, videoPlayer.getWidth(), videoPlayer.getHeight() );
            videoBounds.scaleTo( svgVideoBounds );
        }
        videoPlayer.update();
    }
	
	/////////////////JG Barbican Disable HOME
    //home.hudScale = scaleAmt;
	//home.update();    
    //if( home.wasHomeOpened() ){
    //    if( !bIsHudOpen ){
    //       animateOn( CLOUDS_HUD_FULL );
    //    }else{
    //        animateOff();
    //    }
    //}
	/////////////////////////////////

	updateReset();
}

void CloudsHUDController::updateReset(){
	ofRectangle resetRect = layerSets[CLOUDS_HUD_LOWER_THIRD][0]->svg.getMeshByID("ResetButtonBacking")->bounds;
	scaledResetRect.x = resetRect.x * scaleAmt + scaleOffset.x;
	scaledResetRect.y = resetRect.y * scaleAmt + scaleOffset.y;
	scaledResetRect.width = resetRect.width * scaleAmt;
	scaledResetRect.height = resetRect.height * scaleAmt;
	//cout << "Reset Rect is " << tempScaledResetRect.x << " " << tempScaledResetRect.y << endl; 
}

void CloudsHUDController::mouseMoved(ofMouseEventArgs& args){
	bResetIsHovered = scaledResetRect.inside(args.x,args.y);
}

void CloudsHUDController::mousePressed(ofMouseEventArgs& args){
	bResetIsPressed = scaledResetRect.inside(args.x,args.y);
}

void CloudsHUDController::mouseReleased(ofMouseEventArgs& args){
	bResetIsClicked = bResetIsPressed &&  scaledResetRect.inside(args.x,args.y);
	bResetIsPressed = false;
}

bool CloudsHUDController::isResetHit(){
	return bResetIsClicked;
}

void CloudsHUDController::setHomeEnabled(bool enable){
	bDrawHome = enable;
}

bool CloudsHUDController::isHomeEnabled(){
    return bDrawHome;
}

void CloudsHUDController::setHudEnabled(bool enable){
	bDrawHud = enable;
}

bool CloudsHUDController::isHudEnabled(){
    return bDrawHud;
}

void CloudsHUDController::draw(){
    
    if( !bDrawHud ){
        return;
	}
	
	ofPushStyle();
	ofPushMatrix();
	ofEnableAlphaBlending();
	ofSetLineWidth(1);
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

	////JG TEMP COMMENT OUT
	drawLayer(CLOUDS_HUD_QUESTION);
	drawLayer(CLOUDS_HUD_LOWER_THIRD);
	drawLayer(CLOUDS_HUD_PROJECT_EXAMPLE);
//	drawLayer(CLOUDS_HUD_MAP);
	////JG TEMP COMMENT OUT
	
    for (map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        (it->second)->draw();
    }
    
	if (bDrawHome && hudOpenMap[CLOUDS_HUD_LOWER_THIRD]){
		////JG TEMP COMMENT OUT
		home.draw();
    }
	
	ofPopMatrix();
	ofPopStyle();
}

void CloudsHUDController::drawLayer(CloudsHUDLayerSet layer){
	for(int i = 0; i < layerSets[layer].size(); i++){
		layerSets[layer][i]->draw();
	}
}

#ifdef OCULUS_RIFT
void CloudsHUDController::draw3D(ofCamera* cam, ofVec2f offset){
    
    if( !bDrawHud ){
        return;
	}
    
	ofPushStyle();
	ofPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
 
	glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    ofEnableAlphaBlending();
	ofSetLineWidth(1);

    drawLayer3D(CLOUDS_HUD_QUESTION, cam, offset);
    drawLayer3D(CLOUDS_HUD_LOWER_THIRD, cam, offset);
	drawLayer3D(CLOUDS_HUD_PROJECT_EXAMPLE, cam, offset);
	drawLayer3D(CLOUDS_HUD_MAP, cam, offset);
	
    glPopAttrib();
    
	ofPopMatrix();
	ofPopStyle();
}

void CloudsHUDController::drawLayer3D(CloudsHUDLayerSet layer, ofCamera* cam, ofVec2f& offset){
    ofPushMatrix();
    
    // Hook up to the camera to keep the layer steady.
    ofMatrix4x4 baseRotation;
    ofTranslate(cam->getPosition());
    baseRotation.makeRotationMatrix(cam->getOrientationQuat());
    ofMultMatrix(baseRotation);
    
    ofVec3f camPos = ofVec3f();  //cam->getPosition();
    
    // Calculate the base position.
    static ofVec3f yAxis = ofVec3f(0.0, 1.0, 0.0);
    static ofVec3f xAxis = ofVec3f(1.0, 0.0, 0.0);
//    ofVec3f basePos = camPos + (cam->getLookAtDir().getScaled(layerDistance[layer]));
//    ofVec3f basePos(0, 0, -layerDistance[layer]);
    ofVec3f basePos(offset.x, offset.y, -layerDistance[layer]);
    basePos.rotate(layerRotationH[layer], camPos, yAxis);
    basePos.rotate(layerRotationV[layer], camPos, xAxis);
    
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
        float angle;
        ofVec3f axis;
        CloudsVisualSystem::getOculusRift().getOrientationQuat().getRotate(angle, axis);
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
//        ofRotateY(layerRotationH[layer]);
//        ofRotateX(layerRotationV[layer]);
        ofScale(-1, 1, 1);
    }
    
    // Debug circle.
//    ofSetColor(255);
//    ofCircle(0, 0, 25);
    
    // Transform for rendering the layer.
    ofScale(-scaleAmt, -scaleAmt, 1);
    ofTranslate(-layerBounds.getCenter());

    // Draw the video player if we're on the right layer.
    if (layer == CLOUDS_HUD_PROJECT_EXAMPLE && videoPlayer.isPlaying()) {
        ofSetColor(255, 255, 255, 255*0.7);
        if( !bSkipAVideoFrame ){
//            videoPlayer.draw( videoBounds.x, videoBounds.y, videoBounds.width, videoBounds.height );
        }
    }
    
    // Draw the layer.
    ofSetColor(255);
    drawLayer(layer);
    
    // Draw the home button if we're on the right layer.
    if (layer == CLOUDS_HUD_LOWER_THIRD && bDrawHome && hudOpenMap[CLOUDS_HUD_LOWER_THIRD]) {
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
	
    for (map<CloudsHUDLayerSet, vector<CloudsHUDLayer*> >::iterator it = layerSets.begin(); it != layerSets.end(); ++it) {
        if ((layer & it->first) != 0 && it->first != CLOUDS_HUD_QUESTION) {
            hudOpenMap[it->first] = true;
            for (int i = 0; i < it->second.size(); i++) {
                it->second[i]->start();
            }
        }
    }
    
    // animate in text, this is sub-optimal
    if( layer == CLOUDS_HUD_FULL ){
        for( map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it ){
            (it->second)->animateIn( true );
        }
    }
    else if( (layer & CLOUDS_HUD_LOWER_THIRD) != 0 ){
        hudLabelMap["BylineFirstNameTextBox_1_"]->animateIn( true );
        hudLabelMap["BylineLastNameTextBox"]->animateIn( true );
        hudLabelMap["BylineTopicTextBoxTop"]->animateIn( true );
        hudLabelMap["BylineTopicTextBoxBottom"]->animateIn( true );
		//JG TEMP
        hudLabelMap["BylineBodyCopyTextBox"]->animateIn( true );
//JG TEMP
    }
    else if( (layer & CLOUDS_HUD_PROJECT_EXAMPLE) != 0 ){
//JG TEMP
        hudLabelMap["ProjectExampleTextboxLeft"]->animateIn( true );
        hudLabelMap["ProjectExampleTextboxRight"]->animateIn( true );
        hudLabelMap["ProjectExampleTextBoxTop"]->animateIn( true );
//JG TEMP
    }
    else if( (layer & CLOUDS_HUD_MAP) != 0 ){
        
    }
    else if( (layer & CLOUDS_HUD_QUESTION) != 0 ){
        hudLabelMap["QuestionTextBox"]->animateIn( true );
    }
}

void CloudsHUDController::animateOff(CloudsHUDLayerSet layer){
	//bIsHudOpen = false;
    
    if (videoPlayer.isPlaying()) {
        videoPlayer.stop();
        videoPlayer.close();
    }
    
    // EZ: CODE BELOW IS FOR INSTANT OUT (TEMP!!!)
	// JG: BARBICAN PUTTIN BACK IN
	/*
    for (map<CloudsHUDLayerSet, vector<CloudsHUDLayer*> >::iterator it = layerSets.begin(); it != layerSets.end(); ++it) {
        if ((layer & it->first) != 0) {
            hudOpenMap[it->first] = false;
            for (int i = 0; i < it->second.size(); i++) {
                it->second[i]->close(false);
            }
        }
    }
    
    // instant out text, this is sub-optimal
    if( layer == CLOUDS_HUD_FULL ){
        for( map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it ){
            (it->second)->instantOut();
        }
    }
    else if( (layer & CLOUDS_HUD_LOWER_THIRD) != 0 ){
        hudLabelMap["BylineFirstNameTextBox_1_"]->instantOut();
        hudLabelMap["BylineLastNameTextBox"]->instantOut();
        hudLabelMap["BylineTopicTextBoxTop"]->instantOut();
        hudLabelMap["BylineTopicTextBoxBottom"]->instantOut();
        hudLabelMap["BylineBodyCopyTextBox"]->instantOut();
    }
    else if( (layer & CLOUDS_HUD_PROJECT_EXAMPLE) != 0 ){
        hudLabelMap["ProjectExampleTextboxLeft"]->instantOut();
        hudLabelMap["ProjectExampleTextboxRight"]->instantOut();
        hudLabelMap["ProjectExampleTextBoxTop"]->instantOut();
    }
    else if( (layer & CLOUDS_HUD_MAP) != 0 ){
    
    }
    else if( (layer & CLOUDS_HUD_QUESTION) != 0 ){
        hudLabelMap["QuestionTextBox"]->instantOut();
    }
*/    
    // EZ: CODE BELOW IS FOR ANIMATING, LET'S JUST INSTANT OUT FOR NOW
    for (map<CloudsHUDLayerSet, vector<CloudsHUDLayer*> >::iterator it = layerSets.begin(); it != layerSets.end(); ++it) {
        if ((layer & it->first) != 0) {
            hudOpenMap[it->first] = false;
            for (int i = 0; i < it->second.size(); i++) {
                it->second[i]->close();
            }
        }
    }
    
    // animate out text, this is sub-optimal
    if( layer == CLOUDS_HUD_FULL ){
        for( map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it ){
            (it->second)->animateOut();
        }
    }
    else if( (layer & CLOUDS_HUD_LOWER_THIRD) != 0 ){
        hudLabelMap["BylineFirstNameTextBox_1_"]->animateOut();
        hudLabelMap["BylineLastNameTextBox"]->animateOut();
        hudLabelMap["BylineTopicTextBoxTop"]->animateOut();
        hudLabelMap["BylineTopicTextBoxBottom"]->animateOut();
        hudLabelMap["BylineBodyCopyTextBox"]->animateOut();
    }
    else if( (layer & CLOUDS_HUD_PROJECT_EXAMPLE) != 0 ){
        hudLabelMap["ProjectExampleTextboxLeft"]->animateOut();
        hudLabelMap["ProjectExampleTextboxRight"]->animateOut();
        hudLabelMap["ProjectExampleTextBoxTop"]->animateOut();
    }
    else if( (layer & CLOUDS_HUD_MAP) != 0 ){
        
    }
    else if( (layer & CLOUDS_HUD_QUESTION) != 0 ){
        hudLabelMap["QuestionTextBox"]->animateOut();
    }
}

void CloudsHUDController::saveGuiSettings(){
	
}

void CloudsHUDController::toggleGuis(){
	
}

