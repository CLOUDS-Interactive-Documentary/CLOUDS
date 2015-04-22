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
#include "CloudsLocalization.h"


CloudsHUDController::CloudsHUDController(){
	hudGui = NULL;

    bSkipAVideoFrame = false;
    bDrawHud = true;
//    bDrawHome = true;


    bActJustStarted = false;
	cuedClipEndTime = 0;
    bVisualSystemDisplayed = false;
    bLowerThirdCued = false;

	resetHoverChangedTime = 0;
	bResetIsHovered = false;
	bResetIsPressed = false;
	bResetIsClicked = false;

	isPlaying = false;

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

//	hudLabelMap["ResetButtonTextBox"]->setText(GetTranslationForString("RESET"));
//    hudLabelMap["ResetButtonTextBox"]->clearTextOnAnimateOut = false;

	home.setup();
    
	//manually load reset triangle
	resetTriangle.addVertex(ofVec3f(1366.857,839.217));
	resetTriangle.addVertex(ofVec3f(1366.857,851.783));
	resetTriangle.addVertex(ofVec3f(1373.139,845.5));

	resetTriangle.setMode(OF_PRIMITIVE_TRIANGLES);
	 
}

void CloudsHUDController::actBegan(CloudsActEventArgs& args){
	bDrawHud = true;
	bActJustStarted = true;
	animateOn( CLOUDS_HUD_QUESTION );
    animateOn( CLOUDS_HUD_HOME );
 
}

void CloudsHUDController::actEnded(CloudsActEventArgs& args){
    //Keep the home button on
	animateOff( CLOUDS_HUD_LOWER_THIRD );
 	animateOff( CLOUDS_HUD_QUESTION );
	animateOff( CLOUDS_HUD_PROJECT_EXAMPLE );
	animateOff( CLOUDS_HUD_PAUSE );
}

void CloudsHUDController::clearQuestion(){
	hudLabelMap["QuestionTextBox"]->setText("", false);
}

void CloudsHUDController::clipBegan(CloudsClipEventArgs& args){
	respondToClip(args.chosenClip);
}

void CloudsHUDController::clipEnded(){
    //TODO mark this speaker gone.
}

void CloudsHUDController::visualSystemBegan(CloudsVisualSystemEventArgs& args){
//	bDrawHud = false;
    respondToSystem(args.preset);

    bVisualSystemDisplayed = true;
}

void CloudsHUDController::visualSystemEnded(CloudsVisualSystemEventArgs& args){
//	bDrawHud = true;
    bVisualSystemDisplayed = false;
}

void CloudsHUDController::questionProposed(CloudsQuestionEventArgs& args){

}

void CloudsHUDController::questionSelected(CloudsQuestionEventArgs& args){
    populateQuestion( args.question, true);
}

void CloudsHUDController::topicChanged(CloudsTopicEventArgs& args){
//	if(!bActJustStarted){
//		animateOff( CLOUDS_HUD_QUESTION );
//	}
//	bActJustStarted = false;
}

void CloudsHUDController::preRollRequested(CloudsPreRollEventArgs& args){
	
}

void CloudsHUDController::respondToClip(CloudsClip* clip){
	
//	cout << "ID's on clip " << clip.name << " and fcp id? " << clip.fcpFileId << endl;
//	cout << "Clip is " <<  clip.getLinkName() << endl;
//	cout << "speaker: " << speaker.firstName << " " << speaker.lastName << endl;
	    
	//LOWER THIRD
    //update lower third, but only if the speaker has changed
#ifndef OCULUS_RIFT
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

void CloudsHUDController::respondToSystem(const CloudsVisualSystemPreset& preset){
    
    populateVisualSystem(preset.credits.creator, preset.credits.name, true );

    animateOff(CLOUDS_HUD_PROJECT_EXAMPLE);

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

//void CloudsHUDController::populateMap(const string& leftBox, const string& rightBox, bool forceOn){
//    hudLabelMap["TopicTextBoxLeft"]->setText( leftBox, forceOn );
//    hudLabelMap["TopicTextBoxRight"]->setText( rightBox, forceOn );
//    
//    if( forceOn ){
//        animateOn( CLOUDS_HUD_MAP );
//    }
//}

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
void CloudsHUDController::populateLowerThird(const string& firstName,
                                             const string& lastName,
                                             const string& title,
                                             const string& location,
                                             const string& textbox,
                                             bool forceOn)
{
    
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

void CloudsHUDController::populateVisualSystem(const string& creatorsName,
                                               const string& systemName,
                                               bool forceOn )
{
    
    CloudsHUDLabel* creatorsNameBox  = hudLabelMap["BylineFirstNameTextBox_1_"];
    CloudsHUDLabel* systemNameBox  = hudLabelMap["BylineLastNameTextBox"];
    
    creatorsNameBox->setText( creatorsName, forceOn );
    systemNameBox->setText( systemName, forceOn );
    
    int firstNameRight = creatorsNameBox->getRightEdge();
    int lastNameRight = systemNameBox->getRightEdge();
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
    
    //TODO: do we want to add things into this local
    locationLabel->setText( "", forceOn );
    titleLabel->setText( "", forceOn );
    
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
    
    //TODO: do we want bio text
    descLabel->setText( "", forceOn );
    
    if( forceOn ){
        animateOn( CLOUDS_HUD_LOWER_THIRD );
    }
    
}

void CloudsHUDController::populateProjectExample(const string& videoPath, const string& textLeft, const string& textRight, const string& textTop, bool forceOn) {
	if( isPlaying){
        videoPlayer.stop();
    }
    
    if( ofFile(videoPath).exists() ){
       isPlaying =  videoPlayer.loadMovie(videoPath);
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

    CloudsHUDLayer* homeLayer = new CloudsHUDLayer();
    homeLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_HOME");
    layerSets[CLOUDS_HUD_HOME].push_back( homeLayer );
    allLayers.push_back( homeLayer );

    CloudsHUDLayer* lowerThirdLayer = new CloudsHUDLayer();
    lowerThirdLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_LOWER_THIRD");
    layerSets[CLOUDS_HUD_LOWER_THIRD].push_back( lowerThirdLayer );
    allLayers.push_back( lowerThirdLayer );
    
    CloudsHUDLayer* questionLayer = new CloudsHUDLayer();
    questionLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_QUESTION");
    layerSets[CLOUDS_HUD_QUESTION].push_back( questionLayer );
    allLayers.push_back( questionLayer );
    
    CloudsHUDLayer* pauseLayer = new CloudsHUDLayer();
    pauseLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_PAUSE");
    layerSets[CLOUDS_HUD_PAUSE].push_back( pauseLayer );
    allLayers.push_back( pauseLayer );
    
    CloudsHUDLayer* projectExampleLayer = new CloudsHUDLayer();
    projectExampleLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_PROJECT_EXAMPLE");
    layerSets[CLOUDS_HUD_PROJECT_EXAMPLE].push_back( projectExampleLayer );
    allLayers.push_back( projectExampleLayer );
    
    for( int i=0; i<allLayers.size(); i++ ){

		for( int s = 0; s < allLayers[i]->svg.getMeshes().size(); s++){
			ofVboMesh& m = allLayers[i]->svg.getMeshes()[s].mesh;
			for(int v = 0; v < m.getNumVertices(); v++){
				m.addNormal(ofVec3f(ofRandomuf(),0,0));
			}
		}

        allLayers[i]->duration = 1.5;
        allLayers[i]->delayTime = 0;
        allLayers[i]->startPoint = ofVec2f(allLayers[i]->svg.getWidth(),0);
        allLayers[i]->endPoint   = ofVec2f(0,allLayers[i]->svg.getHeight());
    }
    
    home.bounds = homeLayer->svg.getMeshByID("HomeButtonFrame")->bounds;
    home.bounds.scaleFromCenter(1.5);
    
    svgVideoBounds = projectExampleLayer->svg.getMeshByID("ProjectExampleFrame")->bounds;
	videoBounds = svgVideoBounds;
    
    hudBounds.set( 0, 0, allLayers[0]->svg.getWidth(), allLayers[0]->svg.getHeight() );
    
	
//	cout << "HUD BOUNDS " << hudBounds.width << " / " << hudBounds.height << endl;
//  cout << "SCREEN " << ofGetScreenWidth() << " / " << ofGetScreenHeight() << endl;
}

void CloudsHUDController::calculateFontSizes(){
    // temporary allocate
    int minFontSize = 1;
    int maxFontSize = 70;
    #ifdef OCULUS_RIFT
	//string fontPath = GetCloudsDataPath() + "font/Blender-MEDIUM.ttf";
	string fontPath = GetMediumFontPath();
	#else
	//string fontPath = GetCloudsDataPath() + "font/Blender-THIN.ttf";
	string fontPath = GetThinFontPath();
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

//  were used for map
//    TopicTextBoxLeft            = getLayoutForLayer("TopicTextBoxLeft", fontPath);
//    TopicTextBoxRight           = getLayoutForLayer("TopicTextBoxRight", fontPath);
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
    
    ofLogError("CloudsHUDController::getLayoutForLayer") << "Mesh not found " << layerName;
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

    ofLogError("CloudsHUDController::getLayoutForLayer") << "Mesh not found " << layerName;

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
			newFont->setLetterSpacing(kerning * .08);
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

    //cout << "IS RESET VISIBLE "    << (hudLabelMap["ResetButtonTextBox"]->isVisible() ? "YES" : "NO") << endl;
    //cout << "IS RESET LAYOUT " << (hudLabelMap["ResetButtonTextBox"]->layout == NULL ? "NULL" : "SET") << endl;
    //cout << "IS RESET TEXT  "     << hudLabelMap["ResetButtonTextBox"]->getText() << endl;
    
//	cout << "CURRENT QUESTION " << hudLabelMap["QuestionTextBox"]->getText() << " VISIBLE? " << (hudLabelMap["QuestionTextBox"]->isVisible() ? "YES" : "NO") << endl;

	for(int i = 0; i < allLayers.size(); i++){
		allLayers[i]->update();
	}
	float scaleToWidth  = CloudsVisualSystem::getStaticRenderTarget().getWidth()  - 20; //20 for hardcoded bleed
	float scaleToHeight = CloudsVisualSystem::getStaticRenderTarget().getHeight() - 20;
	float xScale = scaleToWidth/hudBounds.width;
	float yScale = scaleToHeight/hudBounds.height;
    
	bool xDominantScale = xScale < yScale;
    scaleAmt	= xDominantScale ? xScale : yScale;
	scaleOffset = xDominantScale ? 
		ofVec2f(0, scaleToHeight- hudBounds.height*scaleAmt)*.5 :
		ofVec2f(scaleToWidth - hudBounds.width*scaleAmt, 0)*.5;

   if( isPlaying){
	   	if(! videoPlayer.isPlaying()){
			videoPlayer.play();
		}
        if( videoPlayer.isFrameNew() ){
            bSkipAVideoFrame = false;
            
            videoBounds.set(0, 0, videoPlayer.getWidth(), videoPlayer.getHeight() );
            videoBounds.scaleTo( svgVideoBounds );
        }

        videoPlayer.update();
    }
	
    
    home.interactiveBounds.x = home.bounds.x * scaleAmt + scaleOffset.x;
	home.interactiveBounds.y = home.bounds.y * scaleAmt + scaleOffset.y;
	home.interactiveBounds.width  = home.bounds.width  * scaleAmt;
	home.interactiveBounds.height = home.bounds.height * scaleAmt;

    home.update();
    if( home.wasActivated() ){
        if( !hudOpenMap[CLOUDS_HUD_PAUSE] ){
            animateOn( CLOUDS_HUD_PAUSE );
        }
        else{
            //TODO: save the current state before this pause
            animateOff( CLOUDS_HUD_PAUSE );
            animateOff( CLOUDS_HUD_QUESTION );
            animateOff( CLOUDS_HUD_LOWER_THIRD );
        }
    }
    ///////////////////////////////

	updateReset();
}

void CloudsHUDController::updateReset(){
	ofRectangle resetRect = layerSets[CLOUDS_HUD_PAUSE][0]->svg.getMeshByID("ResetButtonBacking")->bounds;
    
    
	scaledResetRect.x = resetRect.x * scaleAmt + scaleOffset.x;
	scaledResetRect.y = resetRect.y * scaleAmt + scaleOffset.y;
	scaledResetRect.width = resetRect.width * scaleAmt;
	scaledResetRect.height = resetRect.height * scaleAmt;
	//cout << "Reset Rect is " << tempScaledResetRect.x << " " << tempScaledResetRect.y << endl; 
}

void CloudsHUDController::mouseMoved(ofMouseEventArgs& args){

	bool orig = bResetIsHovered;
	bResetIsHovered = hudOpenMap[CLOUDS_HUD_PAUSE] && scaledResetRect.inside(args.x,args.y);
    
    //cout << "RESET HOVERED? " << (bResetIsHovered ? "YES" : "NO") << endl;
    
	if(orig != bResetIsHovered){
		resetHoverChangedTime = ofGetElapsedTimef();
	}
}

void CloudsHUDController::mousePressed(ofMouseEventArgs& args){
	
	bResetIsPressed = false;
    
	if(scaledResetRect.inside(args.x,args.y) && hudOpenMap[CLOUDS_HUD_PAUSE]){
		bResetIsPressed  = true;
    }
    
    if( hudOpenMap[CLOUDS_HUD_HOME] && home.hitTest(args.x, args.y) ){
        home.activate();
    }
}

void CloudsHUDController::mouseReleased(ofMouseEventArgs& args){

	if(hudOpenMap[CLOUDS_HUD_PAUSE]){
		bResetIsClicked = bResetIsPressed &&  scaledResetRect.inside(args.x,args.y);
	}
	bResetIsPressed = false;
}

bool CloudsHUDController::isResetHit(){

	if(hudOpenMap[CLOUDS_HUD_PAUSE]) return false;

	bool b = bResetIsClicked && hudLabelMap["ResetButtonTextBox"]->isVisible();
	bResetIsClicked = false;
	return b;
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
    
    if( videoPlayer.isPlaying() ){
        ofSetColor(255, 255, 255, 255*0.7);
        if( !bSkipAVideoFrame ){
			videoPlayer.draw( videoBounds.x, videoBounds.y, videoBounds.width, videoBounds.height );
        }
        ofSetColor(255, 255, 255, 255);
    }

	drawLayer(CLOUDS_HUD_HOME);
	drawLayer(CLOUDS_HUD_QUESTION);
	drawLayer(CLOUDS_HUD_LOWER_THIRD);
	drawLayer(CLOUDS_HUD_PROJECT_EXAMPLE);
	drawLayer(CLOUDS_HUD_PAUSE);
	

    for (map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        (it->second)->draw();
    }
    
	if (hudOpenMap[CLOUDS_HUD_HOME]){
		home.draw();
    }

	if(bResetIsPressed){
		ofSetColor(200,30,0,200);
		resetTriangle.draw();
	}

	ofPopMatrix();
	ofPopStyle();

	ofPushStyle();
	ofEnableAlphaBlending();
    
    
	float resetHoverAlpha = ofMap(ofGetElapsedTimef() - resetHoverChangedTime, 0, .5, 0.0, 1.0, true);
	if(!bResetIsHovered) resetHoverAlpha = 1.0 - resetHoverAlpha;
    
	ofFill();
	ofSetColor(200,30,0, 255*resetHoverAlpha*.3);
	ofRect(scaledResetRect);
	ofNoFill();
	ofSetColor(200,30,0, 255*resetHoverAlpha*.7);
	ofRect(scaledResetRect);
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

	//JG Yebizo Festival -- Commenting out other billboard modes for now
    //if (layerBillboard[layer] == CLOUDS_HUD_BILLBOARD_OCULUS) {
    //    // Billboard rotation using the Oculus orientation.
    //    float angle;
    //    ofVec3f axis;
    //    CloudsVisualSystem::getOculusRift().getOrientationQuat().getRotate(angle, axis);
    //    ofRotate(angle, axis.x, axis.y, axis.z);
    //    ofScale(-1, 1, 1);
    //}
//    else if (layerBillboard[layer] == CLOUDS_HUD_BILLBOARD_CAMERA) {
        // Billboard rotation using the camera.
        ofNode node;
        node.setPosition(layerPos);
        node.lookAt(camPos);
        ofVec3f axis;
        float angle;
        node.getOrientationQuat().getRotate(angle, axis);
        ofRotate(angle, axis.x, axis.y, axis.z);
//    }
//    else {
////        ofRotateY(layerRotationH[layer]);
////        ofRotateX(layerRotationV[layer]);
//        ofScale(-1, 1, 1);
//    }
    
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
    if (layer == CLOUDS_HUD_LOWER_THIRD && hudOpenMap[CLOUDS_HUD_LOWER_THIRD]) {
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
        if ((layer & it->first) != 0 /*&& it->first != CLOUDS_HUD_QUESTION*/) {
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
        hudLabelMap["BylineBodyCopyTextBox"]->animateIn( true );
		hudLabelMap["ResetButtonTextBox"]->animateIn( true );
    }
    else if( (layer & CLOUDS_HUD_PROJECT_EXAMPLE) != 0 ){
        hudLabelMap["ProjectExampleTextboxLeft"]->animateIn( true );
        hudLabelMap["ProjectExampleTextboxRight"]->animateIn( true );
        hudLabelMap["ProjectExampleTextBoxTop"]->animateIn( true );
    }
    else if( (layer & CLOUDS_HUD_PAUSE) != 0 ){
		hudLabelMap["ResetButtonTextBox"]->animateIn( true );
    }
    else if( (layer & CLOUDS_HUD_QUESTION) != 0 ){
        hudLabelMap["QuestionTextBox"]->animateIn( true );
    }
}

void CloudsHUDController::animateOff(CloudsHUDLayerSet layer){
	//bIsHudOpen = false;
    
    if (isPlaying) {
		isPlaying = false;
        videoPlayer.stop();
        videoPlayer.close();
    }

	 
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
//#ifndef OCULUS_RIFT
        hudLabelMap["ResetButtonTextBox"]->animateOut();
//#endif
    }
    else if( (layer & CLOUDS_HUD_PROJECT_EXAMPLE) != 0 ){
        hudLabelMap["ProjectExampleTextboxLeft"]->animateOut();
        hudLabelMap["ProjectExampleTextboxRight"]->animateOut();
        hudLabelMap["ProjectExampleTextBoxTop"]->animateOut();
    }
    else if( (layer & CLOUDS_HUD_QUESTION) != 0 ){
        hudLabelMap["QuestionTextBox"]->animateOut();
    }
    else if( (layer & CLOUDS_HUD_PAUSE) != 0 ){
        
    }
}

void CloudsHUDController::saveGuiSettings(){
	
}

void CloudsHUDController::toggleGuis(){
	
}

