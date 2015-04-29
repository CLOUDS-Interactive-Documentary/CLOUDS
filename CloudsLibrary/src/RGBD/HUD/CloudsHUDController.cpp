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
    
    
    bJustPaused = false;
    bJustUnpaused = false;
    bActJustStarted = false;
	cuedClipEndTime = 0;
    bVisualSystemDisplayed = false;
    bLowerThirdCued = false;

    currentTab = CLOUDS_HUD_RESEARCH_TAB_TOPICS;
    bIsScrollUpHover = false;
    bIsScrollDownHover = false;
    bIsScrollUpPressed = false;
    bIsScrollDownPressed = false;
    bIsHoldScrolling = false;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS].scrollPosition = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS].totalScrollHeight = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE].scrollPosition = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE].totalScrollHeight = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_VISUALS].scrollPosition = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_VISUALS].totalScrollHeight = 0;
    
    currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS];
 
    scrollPressedTime = 0;
    
    fakeConfirmHovered = false;
    fakeConfirmPressed = false;
    fakeConfirmClicked = false;

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
	hudLabelMap["QuestionTextBox_1_"]->setText("", false);
}

void CloudsHUDController::clipBegan(CloudsClipEventArgs& args){
	respondToClip(args.chosenClip);
    
}

void CloudsHUDController::clipEnded(){
    //TODO mark this speaker gone.
    currentClip = NULL;
}

void CloudsHUDController::visualSystemBegan(CloudsVisualSystemEventArgs& args){
//	bDrawHud = false;
    respondToSystem(args.preset);

    bVisualSystemDisplayed = true;
}

void CloudsHUDController::visualSystemEnded(CloudsVisualSystemEventArgs& args){
//	bDrawHud = true;
    bVisualSystemDisplayed = false;
    if(currentClip != NULL){
        respondToClip(currentClip);
    }
    else{
        animateOff(CLOUDS_HUD_LOWER_THIRD);
    }
}

void CloudsHUDController::questionProposed(CloudsQuestionEventArgs& args){

}

void CloudsHUDController::questionSelected(CloudsQuestionEventArgs& args){
    populateQuestion( args.question, true);
}

void CloudsHUDController::topicChanged(CloudsTopicEventArgs& args){

}

void CloudsHUDController::preRollRequested(CloudsPreRollEventArgs& args){
	
}

void CloudsHUDController::respondToClip(CloudsClip* clip){
	
//	cout << "ID's on clip " << clip.name << " and fcp id? " << clip.fcpFileId << endl;
//	cout << "Clip is " <<  clip.getLinkName() << endl;
//	cout << "speaker: " << speaker.firstName << " " << speaker.lastName << endl;
    
	//LOWER THIRD
    //update lower third, but only if the speaker has changed

//#ifndef OCULUS_RIFT
//    if(currentSpeaker.fcpID != CloudsSpeaker::speakers[ clip->person ].fcpID){
        currentSpeaker = CloudsSpeaker::speakers[ clip->person ];
        
        if (bVisualSystemDisplayed) {
            // cue up the lower third until the visual system is done
            bLowerThirdCued = true;
            cuedClipEndTime = ofGetElapsedTimef() + clip->getDuration();
        }
        else {
            populateLowerThird(currentSpeaker.firstName, currentSpeaker.lastName, currentSpeaker.location2, currentSpeaker.title, currentSpeaker.byline1, false );
            // display the lower third right away
            animateOn(CLOUDS_HUD_LOWER_THIRD);
        }
//    }
    
    //PROJECT EXAMPLE
	if(clip->hasProjectExample && clip->projectExample.exampleVideos.size() ){
		CloudsProjectExample example = clip->projectExample;
        string videoPath = example.exampleVideos[ (int)ofRandom(0, example.exampleVideos.size()) ];
        populateProjectExample( videoPath, example.creatorName, "", example.title, true );
	}
	else{
        animateOff(CLOUDS_HUD_PROJECT_EXAMPLE);
    }
//#endif
    currentClip = clip;
 
}

void CloudsHUDController::respondToSystem(const CloudsVisualSystemPreset& preset){
    
    populateVisualSystem(preset.credits.line1, preset.credits.line2, true );

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

void CloudsHUDController::populateQuestion(const string& question, bool forceOn, bool animate){
    if(question == ""){
		animateOff( CLOUDS_HUD_QUESTION );
	}
	else{
		hudLabelMap["QuestionTextBox_1_"]->setText( question, forceOn );
		if( forceOn ){
			if(animate){
				animateOn( CLOUDS_HUD_QUESTION );
			}
			else{
				hudLabelMap["QuestionTextBox_1_"]->instantIn();
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
                                             bool  forceOn)
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
    
    hudLabelMap["BioTextBox"]->setText(textbox);

    
    //description
    ////reset to default
    //get rid of this for now
//    CloudsHUDLabel* descLabel = hudLabelMap["BylineBodyCopyTextBox"];
//    descLabel->bounds = defaultBioBounds;
//    descLabel->layout->setLineLength(defaultBioBounds.width);
//    int descLeftEdge = descLabel->bounds.getLeft();
    
//    if(locationLabel->getRightEdge() > titleLabel->getRightEdge()){
//        rightEdge = locationLabel->getRightEdge();
//	}
//	else{
//        rightEdge = titleLabel->getRightEdge();
//	}
//
//    if(rightEdge + margin >= descLeftEdge){
//        descLabel->bounds.x = rightEdge+margin;
//        descLabel->layout->setLineLength(defaultBioBounds.width - (descLabel->bounds.x - defaultBioBounds.x));
//    }
//    
//    descLabel->setText( textbox, forceOn );
    
    if( forceOn ){
        animateOn( CLOUDS_HUD_LOWER_THIRD );
    }
}

void CloudsHUDController::populateVisualSystem(const string& creditLine1,
                                               const string& creditLine2,
                                               bool forceOn )
{
    
    CloudsHUDLabel* topBox  = hudLabelMap["VSCreditsTextBoxTop"];
    CloudsHUDLabel* bottomBox  = hudLabelMap["VSCreditsTextBoxBottom"];
    
    topBox->setText( creditLine1, forceOn );
    bottomBox->setText( creditLine2, forceOn );
    
//    int topLineRight = topBox->getRightEdge();
//    int bottomLineRight  = bottomBox->getRightEdge();
//    int rightEdge = 0;
//    
//    if(firstNameRight > lastNameRight){
//        rightEdge = firstNameRight;
//    }
//	else{
//        rightEdge = lastNameRight;
//	}
//    
    //    cout<< "right edge: " << rightEdge << endl;
    
    //move these over to float left of name
//    CloudsHUDLabel* locationLabel = hudLabelMap["VSCreditsTextBoxTop"];
//    CloudsHUDLabel* titleLabel = hudLabelMap["VSCreditsTextBoxBottom"];
//    locationLabel->bounds.x = rightEdge + margin;
//    titleLabel->bounds.x = rightEdge + margin;
    
//    //TODO: do we want to add things into this local
//    locationLabel->setText( "", forceOn );
//    titleLabel->setText( "", forceOn );
    
//    //description
//    ////reset to default
//    CloudsHUDLabel* descLabel = hudLabelMap["BylineBodyCopyTextBox"];
//    descLabel->bounds = defaultBioBounds;
//    descLabel->layout->setLineLength(defaultBioBounds.width);
//    int descLeftEdge = descLabel->bounds.getLeft();
//    
//    if(locationLabel->getRightEdge() > titleLabel->getRightEdge()){
//        rightEdge = locationLabel->getRightEdge();
//	}
//	else{
//        rightEdge = titleLabel->getRightEdge();
//	}
//    
//    if(rightEdge + margin >= descLeftEdge){
//        descLabel->bounds.x = rightEdge+margin;
//        descLabel->layout->setLineLength(defaultBioBounds.width - (descLabel->bounds.x - defaultBioBounds.x));
//    }
//    
//    //TODO: do we want bio text
//    descLabel->setText( "", forceOn );
    
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

    CloudsHUDLayer* homeLayer = new CloudsHUDLayer();
    homeLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_HOME");
    layers[CLOUDS_HUD_HOME] = homeLayer;
    allLayers.push_back( homeLayer );

    CloudsHUDLayer* lowerThirdLayer = new CloudsHUDLayer();
    lowerThirdLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_LOWER_THIRD");
    layers[CLOUDS_HUD_LOWER_THIRD] = lowerThirdLayer ;
    allLayers.push_back( lowerThirdLayer );
    
    CloudsHUDLayer* questionLayer = new CloudsHUDLayer();
    questionLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_QUESTION");
    layers[CLOUDS_HUD_QUESTION] = questionLayer;
    allLayers.push_back( questionLayer );
    
    CloudsHUDLayer* pauseLayer = new CloudsHUDLayer();
    pauseLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_PAUSE");
    layers[CLOUDS_HUD_PAUSE] = pauseLayer;
    allLayers.push_back( pauseLayer );
    
    CloudsHUDLayer* projectExampleLayer = new CloudsHUDLayer();
    projectExampleLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_PROJECT_EXAMPLE");
    layers[CLOUDS_HUD_PROJECT_EXAMPLE] = projectExampleLayer;
    allLayers.push_back( projectExampleLayer );

    CloudsHUDLayer* nextLayer = new CloudsHUDLayer();
    nextLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_HUD_NEXT");
    layers[CLOUDS_HUD_NEXT] = nextLayer;
    allLayers.push_back( nextLayer );

    CloudsHUDLayer* researchListLayer = new CloudsHUDLayer();
    researchListLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_RESEARCH_LIST");
    layers[CLOUDS_HUD_RESEARCH_LIST] = researchListLayer;
    allLayers.push_back( researchListLayer );
    
    CloudsHUDLayer* researchNavLayer = new CloudsHUDLayer();
    researchNavLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_RESEARCH_NAV");
    layers[CLOUDS_HUD_RESEARCH_NAV] = researchNavLayer;
    allLayers.push_back( researchNavLayer );
    
    CloudsHUDLayer* researchShuffleLayer = new CloudsHUDLayer();
    researchShuffleLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_RESEARCH_SHUFFLE");
    layers[CLOUDS_HUD_RESEARCH_SHUFFLE] = researchShuffleLayer;
    allLayers.push_back( researchShuffleLayer );
    
    for( int i = 0; i < allLayers.size(); i++ ){

        //TODO: filled meshes shouldn't be done this way
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
    
    bioBounds = pauseLayer->svg.getMeshByID("BioFrame")->bounds;
    
    svgVideoBounds = projectExampleLayer->svg.getMeshByID("ProjectExampleFrame")->bounds;
	videoBounds = svgVideoBounds;
    researchScrollBounds = layers[CLOUDS_HUD_RESEARCH_LIST]->svg.getMeshByID("ListBacking")->bounds;

    scrollUpBounds = ofRectangle(researchScrollBounds.x,researchScrollBounds.y,researchScrollBounds.width,20);
    scrollDownBounds = ofRectangle(researchScrollBounds.x,researchScrollBounds.getMaxY()-20,researchScrollBounds.width,20);

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
    getLabelForLayer("BylineFirstNameTextBox_1_", fontPath, 50);
    ////last name
    getLabelForLayer("BylineLastNameTextBox", fontPath, 50);
    ////title
    getLabelForLayer("BylineTopicTextBoxBottom", fontPath);
    ////location
    getLabelForLayer("BylineTopicTextBoxTop", fontPath);
    getLabelForLayer("VSCreditsTextBoxTop", fontPath);
    getLabelForLayer("VSCreditsTextBoxBottom", fontPath);
    getLabelForLayer("QuestionTextBox_1_", fontPath);

    getLabelForLayer("ProjectExampleTextboxLeft", fontPath);
    getLabelForLayer("ProjectExampleTextboxRight", fontPath);
    getLabelForLayer("ProjectExampleTextBoxTop", fontPath);
    
    
    //pause
    getLabelForLayer("ExploreTextBox", fontPath);
    getLabelForLayer("SeeMoreTextBox", fontPath);
    getLabelForLayer("NextButtonTextBox", fontPath);
    getLabelForLayer("BioTitleTextBox", fontPath);
    getLabelForLayer("BioTextBox", fontPath); //use layout

    
    getLabelForLayer("ResetButtonTextBox", fontPath);
    //research stuff
    ResearchTopicListLabel = getLabelForLayer("ListTextBoxes", fontPath);
    
    //research navigation
    getLabelForLayer("MapTextBox", fontPath);
    getLabelForLayer("PeopleTextBox", fontPath);
    getLabelForLayer("VisualsTextBox", fontPath);
    getLabelForLayer("RSResetButtonTextBox", fontPath);
    getLabelForLayer("ShuffleButtonTextBox", fontPath);
    
    // cleanup!
    for( int i=0; i<tempFontList.size(); i++ ){
        delete tempFontList[i];
    }
    tempFontList.clear();
    
 
    scrollIncrement = hudLabelMap["ListTextBoxes"]->bounds.height * 1.5;
 
    hudLabelMap["MapTextBox"]->setText(GetTranslationForString("MAP"), false);
    hudLabelMap["MapTextBox"]->clearTextOnAnimateOut = false;
    hudLabelMap["PeopleTextBox"]->setText(GetTranslationForString("PEOPLE"), false);
    hudLabelMap["PeopleTextBox"]->clearTextOnAnimateOut = false;
    hudLabelMap["VisualsTextBox"]->setText(GetTranslationForString("VISUALS"), false);
    hudLabelMap["VisualsTextBox"]->clearTextOnAnimateOut = false;
    hudLabelMap["RSResetButtonTextBox"]->setText(GetTranslationForString("EXIT"), false); //this one may change...
    hudLabelMap["RSResetButtonTextBox"]->clearTextOnAnimateOut = false;
 
    hudLabelMap["ResetButtonTextBox"]->setText(GetTranslationForString("EXIT"), false);
    hudLabelMap["ResetButtonTextBox"]->clearTextOnAnimateOut = false;
    hudLabelMap["NextButtonTextBox"]->setText(GetTranslationForString("NEXT"), false);
    hudLabelMap["NextButtonTextBox"]->clearTextOnAnimateOut = false;
    hudLabelMap["ExploreTextBox"]->setText(GetTranslationForString("EXPLORE THE MAP"), false);
    hudLabelMap["ExploreTextBox"]->clearTextOnAnimateOut = false;
    hudLabelMap["SeeMoreTextBox"]->setText(GetTranslationForString("SEE MORE OF THIS PERSON"), false); //todo dynmic name
    hudLabelMap["SeeMoreTextBox"]->clearTextOnAnimateOut = false;

    hudLabelMap["ShuffleButtonTextBox"]->setText(GetTranslationForString("SHUFFLE ALL"), false); //todo dynmic name
    hudLabelMap["ShuffleButtonTextBox"]->clearTextOnAnimateOut = false;
    
    hudLabelMap["BioTitleTextBox"]->setText(GetTranslationForString("BIO"), false);
    hudLabelMap["BioTitleTextBox"]->clearTextOnAnimateOut = false;
    
    hudLabelMap["BioTextBox"]->clearTextOnAnimateOut = false;
    hudLabelMap["BioTextBox"]->layout->setLineLength(hudLabelMap["BioTextBox"]->bounds.width);

    
    
}

//ofxFTGLSimpleLayout* CloudsHUDController::getLayoutForLayer(const string& layerName, const string& fontPath) {
//    
//    for( int i=0; i<allLayers.size(); i++ ){
//        SVGMesh* textMesh = allLayers[i]->svg.getMeshByID( layerName );
//        
//        if( textMesh != NULL ){
//            textMesh->visible = false;
//            
//            float maxHeight = textMesh->bounds.height;
//            int fontSize = getFontSizeForMesh( textMesh );
//            
////            cout << "The correct font size is " << fontSize << " << endl;
//            
//            // make a layout
//            ofxFTGLSimpleLayout *newLayout = new ofxFTGLSimpleLayout();
//            newLayout->loadFont( fontPath, fontSize );
//            newLayout->setLineLength( 999 );
//            
////            if( layerName == "BylineBodyCopyTextBox" ){         // this is the main body copy in the lower thirds
////                newLayout->loadFont( fontPath, floor(fontSize/4.5) );
////                newLayout->setLineLength( textMesh->bounds.width );
////            }
//            
//            // make a label
//            CloudsHUDLabel *newLabel = new CloudsHUDLabel();
//            newLabel->setup( newLayout, textMesh->bounds );
//            hudLabelMap[layerName] = newLabel;
//            
//            return newLayout;
//        }
//    }
//    
//    ofLogError("CloudsHUDController::getLayoutForLayer") << "Mesh not found " << layerName;
//    
//    return NULL;
//}

CloudsHUDLabel* CloudsHUDController::getLabelForLayer(const string& layerName,
                                                           const string& fontPath,
                                                           int kerning,
                                                           bool caps,
                                                           bool useLayout)
{

      for( int i = 0; i < CLOUDS_HUD_ALL; i++ ){
        
        SVGMesh* textMesh = layers[(CloudsHUDLayerSet)i]->svg.getMeshByID( layerName );
        
        if( textMesh == NULL ){
            continue;
        }
          
        textMesh->visible = false;
        
        float maxHeight = textMesh->bounds.height;
        
        CloudsHUDLabel *newLabel = new CloudsHUDLabel();
        // make a layout
        if(layerName == "BioTextBox"){
            ofxFTGLSimpleLayout *newLayout = new ofxFTGLSimpleLayout();
            newLayout->loadFont( fontPath, 10 );
            newLayout->setLineLength( 999 );
            newLabel->setup( newLayout, textMesh->bounds );
        }
        else{
            int fontSize = getFontSizeForMesh( textMesh );
            ofxFTGLFont *newFont = new ofxFTGLFont();
            newFont->setLetterSpacing(kerning * .08);
            newFont->loadFont( fontPath, fontSize );
            newLabel->setup( newFont, textMesh->bounds );
        }
        // make a label
        newLabel->caps = caps;
          
        hudLabelMap[layerName] = newLabel;
        hudLayerLabels[(CloudsHUDLayerSet)i].push_back(newLabel);
        
        return newLabel;
          
    }

    ofLogError("CloudsHUDController::getLayoutForLayer") << "Mesh not found " << layerName;

    return NULL;
    
}

/*
ofxFTGLFont* CloudsHUDController::getFontForLayer(const string& layerName, const string& fontPath, int kerning) {

    for( int i=0; i<allLayers.size(); i++ ){
        SVGMesh* textMesh = allLayers[i]->svg.getMeshByID( layerName );
        
        
        if( textMesh != NULL ){
            textMesh->visible = false;
            
            float maxHeight = textMesh->bounds.height;
            int fontSize = getFontSizeForMesh( textMesh );
            
            //            cout << "The correct font size is " << fontSize << " << endl;
            
            // make a layout
            
            // make a label
            CloudsHUDLabel *newLabel = new CloudsHUDLabel();
            newLabel->setup( newFont, textMesh->bounds );
            hudLabelMap[layerName] = newLabel;
            
            return newFont;
        }
    }
    return NULL;
}
*/
int CloudsHUDController::getFontSizeForMesh( SVGMesh* textMesh ){
    if( !textMesh ){
        ofLogError("CloudsHUDController::getFontSizeForMesh") << "Text box not found";
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
	
    scrollUpBoundsScaled = getScaledRectangle(scrollUpBounds);
    scrollDownBoundsScaled = getScaledRectangle(scrollDownBounds);

    home.interactiveBounds = getScaledRectangle(home.bounds);
    home.update();
    if( home.wasActivated() ){
        if( !hudOpenMap[CLOUDS_HUD_PAUSE] ){
            pause();
        }
        else{
            unpause();
        }
    }
    
    if( hudOpenMap[CLOUDS_HUD_RESEARCH_LIST] ){
        updateScroll();
        
        /////TEMP
        fakeConfirmSelectionBounds.x = ofGetWidth()/2 - 150;
        fakeConfirmSelectionBounds.y = ofGetHeight()/2 - 75;
        fakeConfirmSelectionBounds.width = 300;
        fakeConfirmSelectionBounds.height = 150;
    }
    
    if( hudOpenMap[CLOUDS_HUD_RESEARCH_NAV]){
        updateResearchNavigation();
    }
    ///////////////////////////////

}

void CloudsHUDController::updateScroll(){
    if(bIsScrollDownPressed || bIsScrollUpPressed){
        float timeSincePress = ofGetElapsedTimef() - scrollPressedTime;
        bool scrolled = ofGetMousePressed() && ( ( bIsHoldScrolling && timeSincePress > .5) || (!bIsHoldScrolling && timeSincePress > .75) );
        if(scrolled){
            float newScrollPosition = currentResearchList->scrollPosition + scrollIncrement * (bIsScrollUpPressed ? -1 : 1);
            currentResearchList->scrollPosition = ofClamp(newScrollPosition, 0, currentResearchList->totalScrollHeight - researchScrollBounds.height);
            bIsHoldScrolling = true;
        }
    }
    
    
    for(int i = 0; i < currentResearchList->buttons.size(); i++){
        CloudsHUDResearchButton& b = currentResearchList->buttons[i];
        b.visible = b.top > currentResearchList->scrollPosition &&
                    b.top < currentResearchList->scrollPosition + currentResearchList->totalScrollHeight;
        if(b.visible){
            b.selectRect = getScaledRectangle( ofRectangle(researchScrollBounds.x,
                                                           b.top - currentResearchList->scrollPosition + hudLabelMap["ListTextBoxes"]->bounds.x,
                                                           researchScrollBounds.width, 15) );
        }
    }
    
    /*
    if( currentTab == CLOUDS_HUD_RESEARCH_TAB_TOPICS ){
        for(int i = 0; i < topicButtons.size(); i++){
            TopicButton& b = topicButtons[i];
            b.visible = b.top > scrollPosition && b.top < scrollPosition + totalScrollHeight;
            if(b.visible){
                b.selectRect = getScaledRectangle( ofRectangle(researchScrollBounds.x,
                                                               b.top - scrollPosition + hudLabelMap["ListPeopleTextBox"]->bounds.x,
                                                               researchScrollBounds.width, 15) );
            }
        }
    }
    else if( currentTab == CLOUDS_HUD_RESEARCH_TAB_PEOPLE){
        //TODO:
    }
    else{
        //TDOO:
    }
     */
}

bool CloudsHUDController::isItemSelected(){
    for(int i = 0; i < currentResearchList->buttons.size(); i++){
        if(currentResearchList->buttons[i].clicked){
            return true;
        }
    }
    return false;
}

string CloudsHUDController::getSelectedItem(){
    for(int i = 0; i < currentResearchList->buttons.size(); i++){
        if(currentResearchList->buttons[i].clicked){
            return currentResearchList->buttons[i].tag;
        }
    }
    return "";
}

bool CloudsHUDController::isItemConfirmed(){
    return fakeConfirmClicked;
}

void CloudsHUDController::clearSelection(){
    fakeConfirmClicked = false;    
}

void CloudsHUDController::updateResearchNavigation(){

    //get the backing
    hudLabelMap["MapTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_RESEARCH_NAV]->svg.getMeshByID("MapHoverBacking")->bounds;
    hudLabelMap["PeopleTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_RESEARCH_NAV]->svg.getMeshByID("PeopleHoverBacking")->bounds;
    hudLabelMap["VisualsTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_RESEARCH_NAV]->svg.getMeshByID("VSHoverBacking")->bounds;
    hudLabelMap["RSResetButtonTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_RESEARCH_LIST]->svg.getMeshByID("RSResetButtonBacking")->bounds;
    
    //set the interaction regions
    hudLabelMap["MapTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["MapTextBox"]->baseInteractiveBounds);
    hudLabelMap["PeopleTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["PeopleTextBox"]->baseInteractiveBounds);
    hudLabelMap["VisualsTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["VisualsTextBox"]->baseInteractiveBounds);
    hudLabelMap["RSResetButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["RSResetButtonTextBox"]->baseInteractiveBounds);
    
    
}
void CloudsHUDController::pause(){

    //TODO: save the current HUD state before pause

    
    //set up the sizing
    ofRectangle backingBounds = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreBackingHover")->bounds;
    float textRightEdge = hudLabelMap["ExploreTextBox"]->getRightEdge();
    float exploreMapWidth = (textRightEdge - backingBounds.x) + margin;
    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreBackingHover")->bounds.width = exploreMapWidth;
    float seeMoreX = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreBackingHover")->bounds.getMaxX();
    float seeMoreMaxX  = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("SeeMoreBackingHover")->bounds.getMaxX();
    float seeMoreWidth = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("SeeMoreBackingHover")->bounds.getWidth();
    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("SeeMoreBackingHover")->bounds.x = seeMoreX;
    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("SeeMoreBackingHover")->bounds.width = (seeMoreMaxX - seeMoreX);

    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreSeeMoreTitleDivide")->mesh.getVertices()[0].x = seeMoreX;
    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreSeeMoreTitleDivide")->mesh.getVertices()[1].x = seeMoreX;
    
    hudLabelMap["SeeMoreTextBox"]->bounds.x = seeMoreX + margin;

    hudLabelMap["ResetButtonTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ResetButtonBacking")->bounds;
    hudLabelMap["ExploreTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreBackingHover")->bounds;
    hudLabelMap["SeeMoreTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("SeeMoreBackingHover")->bounds;
    hudLabelMap["NextButtonTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_NEXT]->svg.getMeshByID("NextButtonBacking")->bounds;
    
    //set the interaction regions
    hudLabelMap["ResetButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ResetButtonTextBox"]->baseInteractiveBounds);
    hudLabelMap["ExploreTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ExploreTextBox"]->baseInteractiveBounds);
    hudLabelMap["SeeMoreTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["SeeMoreTextBox"]->baseInteractiveBounds);
    hudLabelMap["NextButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["NextButtonTextBox"]->baseInteractiveBounds);
 
    animateOff( CLOUDS_HUD_QUESTION );
    animateOn( CLOUDS_HUD_PAUSE );
    animateOn( CLOUDS_HUD_NEXT );

    bJustPaused = true;
    bJustUnpaused = false;

}

void CloudsHUDController::unpause(){
    animateOff( CLOUDS_HUD_PAUSE );
    animateOff( CLOUDS_HUD_QUESTION );
    animateOff( CLOUDS_HUD_LOWER_THIRD );

    bJustUnpaused = true;
    bJustPaused = false;
    
}

void CloudsHUDController::setTopics(const set<string>& topics){
    CloudsHUDResearchList& topicList = researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS];
    topicList.buttons.clear();
    topicList.buttons.resize(topics.size());
    int i = 0;
    for(set<string>::iterator it = topics.begin(); it != topics.end(); it++){
        topicList.buttons[i].top = i * scrollIncrement;
        topicList.buttons[i].tag = *it;
        topicList.buttons[i].label = *it;
        i++;
    }
    
    topicList.totalScrollHeight = topicList.buttons.back().top + scrollIncrement;
    topicList.scrollPosition = 0;
}

void CloudsHUDController::populateSpeakers(){
    CloudsHUDResearchList& peopleList = researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE];
    peopleList.buttons.clear();
    peopleList.buttons.resize(CloudsSpeaker::speakers.size());
    int i = 0;
    for(map<string,CloudsSpeaker>::iterator it = CloudsSpeaker::speakers.begin(); it != CloudsSpeaker::speakers.end(); it++){
        peopleList.buttons[i].top = i * scrollIncrement;
        peopleList.buttons[i].tag = it->first;
        peopleList.buttons[i].label = it->second.firstName + " " + it->second.lastName;
        i++;
    }
    
    peopleList.totalScrollHeight = peopleList.buttons.back().top + scrollIncrement;
    peopleList.scrollPosition = 0;
}

//TODO: visuals

void CloudsHUDController::mouseMoved(ofMouseEventArgs& args){

    for (map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        (it->second)->mouseMoved(ofVec2f(args.x,args.y));
    }
    
    if(hudOpenMap[CLOUDS_HUD_RESEARCH_LIST]){
        bIsScrollUpHover = scrollUpBoundsScaled.inside(args.x, args.y);
        bIsScrollDownHover = scrollDownBoundsScaled.inside(args.x, args.y);
        
        if(researchScrollBounds.inside(args.x, args.y)){
            
            for(int i = 0; i < currentResearchList->buttons.size(); i++){
                if(currentResearchList->buttons[i].visible){
                    currentResearchList->buttons[i].hovered = currentResearchList->buttons[i].selectRect.inside(args.x, args.y);
                }
            }
        }
        
        //TEMP
        fakeConfirmHovered = fakeConfirmSelectionBounds.inside(args.x, args.y);
    }

}

void CloudsHUDController::mousePressed(ofMouseEventArgs& args){
	
    for(map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        (it->second)->mousePressed(ofVec2f(args.x,args.y));
    }

    if( hudOpenMap[CLOUDS_HUD_HOME] && home.hitTest(args.x, args.y) ){
        home.activate();
    }
    
    if(hudOpenMap[CLOUDS_HUD_RESEARCH_LIST]){
        bIsScrollUpPressed = scrollUpBoundsScaled.inside(args.x, args.y);
        bIsScrollDownPressed = scrollDownBoundsScaled.inside(args.x, args.y);
        
        scrollPressedTime = ofGetElapsedTimef();
        if(researchScrollBounds.inside(args.x, args.y)){
            for(int i = 0; i < currentResearchList->buttons.size(); i++){
                currentResearchList->buttons[i].clicked = false;
                if(currentResearchList->buttons[i].visible){
                    currentResearchList->buttons[i].pressed = currentResearchList->buttons[i].selectRect.inside(args.x, args.y);
                }
            }
        }
        
        //TEMP
        fakeConfirmPressed = fakeConfirmSelectionBounds.inside(args.x, args.y);

    }
}

void CloudsHUDController::mouseReleased(ofMouseEventArgs& args){

    for (map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        (it->second)->mouseReleased(ofVec2f(args.x,args.y));
    }
    
    if(hudOpenMap[CLOUDS_HUD_RESEARCH_LIST]){
        if(bIsScrollUpPressed && scrollUpBoundsScaled.inside(args.x, args.y)){
            float newScrollPosition = currentResearchList->scrollPosition - scrollIncrement;
            currentResearchList->scrollPosition = ofClamp(newScrollPosition, 0, currentResearchList->totalScrollHeight - researchScrollBounds.height);
        }
        if(bIsScrollDownPressed && scrollDownBoundsScaled.inside(args.x, args.y)){
            float newScrollPosition = currentResearchList->scrollPosition + scrollIncrement;
            currentResearchList->scrollPosition = ofClamp(newScrollPosition, 0, currentResearchList->totalScrollHeight - researchScrollBounds.height);
        }        
        bIsHoldScrolling = false;
        
        if(researchScrollBounds.inside(args.x, args.y)){

            for(int i = 0; i < currentResearchList->buttons.size(); i++){
                if(currentResearchList->buttons[i].visible){
                   currentResearchList->buttons[i].clicked = currentResearchList->buttons[i].pressed &&
                                                             currentResearchList->buttons[i].selectRect.inside(args.x, args.y);
                    currentResearchList->buttons[i].pressed = false;
                }
            }
        }
        
        //TEMP
        fakeConfirmClicked = fakeConfirmPressed && fakeConfirmSelectionBounds.inside(args.x, args.y);
        fakeConfirmPressed = false;
    }
}

bool CloudsHUDController::isResetHit(){
    return hudLabelMap["ResetButtonTextBox"]->isClicked();
}

bool CloudsHUDController::isNextHit(){
    return hudLabelMap["NextButtonTextBox"]->isClicked();
}

bool CloudsHUDController::isExploreMapHit(){
    bool selected = hudLabelMap["ExploreTextBox"]->isClicked();
    if(selected) {
        currentTab = CLOUDS_HUD_RESEARCH_TAB_TOPICS;
        currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS];
    }
    return selected;
}

bool CloudsHUDController::isSeeMorePersonHit(){
    bool selected = hudLabelMap["SeeMoreTextBox"]->isClicked();
    if(selected) {
        currentTab = CLOUDS_HUD_RESEARCH_TAB_PEOPLE;
        currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE];
    }
    return selected;
}

bool CloudsHUDController::selectedMapTab(){
    bool selected = hudLabelMap["MapTextBox"]->isClicked();
    if(selected) {
        currentTab = CLOUDS_HUD_RESEARCH_TAB_TOPICS;
        currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS];
    }
    return selected;
}

bool CloudsHUDController::selectedPeopleTab(){
    bool selected = hudLabelMap["PeopleTextBox"]->isClicked();
    if(selected) {
        currentTab = CLOUDS_HUD_RESEARCH_TAB_PEOPLE;
        currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE];
    }
    return selected;
}

bool CloudsHUDController::selectedVisualsTab(){
    bool selected = hudLabelMap["VisualsTextBox"]->isClicked();
    if(selected) {
        currentTab = CLOUDS_HUD_RESEARCH_TAB_VISUALS;
        currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_VISUALS];
        
    }
    return selected;
}
    
void CloudsHUDController::setHudEnabled(bool enable){
	bDrawHud = enable;
}

bool CloudsHUDController::isHudEnabled(){
    return bDrawHud;
}

bool CloudsHUDController::isPaused(){
    return hudOpenMap[CLOUDS_HUD_PAUSE];
}

bool CloudsHUDController::didPause(){
    bool ret = bJustPaused;
    bJustPaused = false;
    return ret;
}

bool CloudsHUDController::didUnpause(){
    bool ret = bJustUnpaused;
    bJustUnpaused = false;
    return ret;
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

    for(int i = 0; i < CLOUDS_HUD_ALL; i++){
        drawLayer(CloudsHUDLayerSet(i));
    }
//	drawLayer(CLOUDS_HUD_HOME);
//	drawLayer(CLOUDS_HUD_QUESTION);
//	drawLayer(CLOUDS_HUD_LOWER_THIRD);
//	drawLayer(CLOUDS_HUD_PROJECT_EXAMPLE);
//	drawLayer(CLOUDS_HUD_PAUSE);
//	drawLayer(CLOUDS_HUD_NEXT);
//	drawLayer(CLOUDS_HUD_RESEARCH_LIST);
//	drawLayer(CLOUDS_HUD_RESEARCH_NAV);
//	drawLayer(CLOUDS_HUD_RESEARCH_SHUFFLE);

    if(hudOpenMap[CLOUDS_HUD_RESEARCH_LIST]){
        
        ofPushStyle();
        //test to see bound locations
        ofSetColor(255, 0, 0);
        ofRect(scrollUpBounds);
        ofRect(scrollDownBounds);
        
        
        //begin test for fake confirm
        if(isItemSelected()){
            if(fakeConfirmClicked){
                ofSetColor(255, 0, 0, 50);
            }
            else if(fakeConfirmPressed){
                ofSetColor(255, 200, 0, 50);
            }
            else if(fakeConfirmHovered) {
                ofSetColor(255, 20, 255, 50);
            }
            else{
                ofNoFill();
                ofSetColor(255,255,255,50);
            }
            ofRect(fakeConfirmSelectionBounds);
            
        }
        
        ofPopStyle();
        ///end test
        
        beginListStencil();
        drawList();
        
        
//        if( currentTab == CLOUDS_HUD_RESEARCH_TAB_TOPICS ){
//            drawTopicsList();
//        }
//        else if( currentTab == CLOUDS_HUD_RESEARCH_TAB_PEOPLE){
//            drawPeopleList();
//        }
//        else if( currentTab == CLOUDS_HUD_RESEARCH_TAB_VISUALS ){
//            draweVisualList();
//        }
        
        endListStencil();
    }

    
	if (hudOpenMap[CLOUDS_HUD_HOME]){
		home.draw();
    }
    
    for (map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        (it->second)->draw();
    }
    
    
	if(hudLabelMap["ResetButtonTextBox"]->isClicked()){
		ofSetColor(200,30,0,200);
		resetTriangle.draw();
	}

	ofPopMatrix();
	ofPopStyle();


}

void CloudsHUDController::drawLayer(CloudsHUDLayerSet layer){
    layers[layer]->draw();
}

void CloudsHUDController::beginListStencil(){

    glEnable(GL_SCISSOR_TEST);
    ofRectangle scissorRect = getScaledRectangle(researchScrollBounds);
    glScissor(scissorRect.x, ofGetHeight() - ( scissorRect.y + scissorRect.height) , scissorRect.width, scissorRect.height);
    
    ofPushMatrix();
    ofTranslate(0, -currentResearchList->scrollPosition);
    
}

void CloudsHUDController::endListStencil(){
    
    ofPopMatrix();
	glDisable(GL_SCISSOR_TEST);
    //ofPopStyle();
    
}

void CloudsHUDController::drawList(){
    ofPushStyle();
    for(int i = 0; i < currentResearchList->buttons.size(); i++){
        if(currentResearchList->buttons[i].visible){
            //TODO: better coloring system
            if(currentResearchList->buttons[i].clicked){
                ofSetColor(255, 180, 0);
            }
            else if(currentResearchList->buttons[i].pressed){
                ofSetColor(255, 180, 0, 200);
            
            }
            else if(currentResearchList->buttons[i].hovered){
                ofSetColor(255);
            }
            else{
                ofSetColor(255, 200);
            }
            
            //TODO: maybe different styles for different
            ResearchTopicListLabel->font-> drawString(currentResearchList->buttons[i].label,
                                              hudLabelMap["ListTextBoxes"]->bounds.x,
                                              hudLabelMap["ListTextBoxes"]->bounds.y + currentResearchList->buttons[i].top);
        }
    }
    ofPopStyle();
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

void CloudsHUDController::drawLayer3D(CloudsHUDLayer layer, ofCamera* cam, ofVec2f& offset){
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
	
    for (map<CloudsHUDLayerSet, CloudsHUDLayer* >::iterator it = layers.begin(); it != layers.end(); ++it) {
        if (layer == it->first || layer == CLOUDS_HUD_ALL) {
            hudOpenMap[it->first] = true;
            it->second->start();
        }
    }
    
    // animate in text, this is sub-optimal
    if( layer == CLOUDS_HUD_ALL ){
        for( map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it ){
            (it->second)->animateIn( true );
        }
    }
    else{
        for(int i = 0; i < hudLayerLabels[layer].size(); i++){
            hudLayerLabels[layer][i]->animateIn( true );
        }
    }
    /*
    else if( layer == CLOUDS_HUD_LOWER_THIRD ){
        hudLabelMap["BylineFirstNameTextBox_1_"]->animateIn( true );
        hudLabelMap["BylineLastNameTextBox"]->animateIn( true );
        hudLabelMap["BylineTopicTextBoxTop"]->animateIn( true );
        hudLabelMap["BylineTopicTextBoxBottom"]->animateIn( true );
//        hudLabelMap["BylineBodyCopyTextBox"]->animateIn( true );
    }
    else if( layer == CLOUDS_HUD_PROJECT_EXAMPLE ){
        hudLabelMap["ProjectExampleTextboxLeft"]->animateIn( true );
        hudLabelMap["ProjectExampleTextboxRight"]->animateIn( true );
        hudLabelMap["ProjectExampleTextBoxTop"]->animateIn( true );
    }
    else if( layer == CLOUDS_HUD_PAUSE ){
		hudLabelMap["ResetButtonTextBox"]->animateIn( true );
        hudLabelMap["ExploreTextBox"]->animateIn( true );
        hudLabelMap["SeeMoreTextBox"]->animateIn( true );
    }
    else if( layer == CLOUDS_HUD_NEXT){
        hudLabelMap["NextButtonTextBox"]->animateIn( true );
    }
    else if( layer == CLOUDS_HUD_QUESTION ){
        hudLabelMap["QuestionTextBox_1_"]->animateIn( true );
    }
    else if( layer == CLOUDS_HUD_RESEARCH_LIST){
        hudLabelMap["RSResetButtonTextBox"]->animateIn( true );
    }
    else if( layer == CLOUDS_HUD_RESEARCH_NAV){
        hudLabelMap["MapTextBox"]->animateIn( true );
        hudLabelMap["PeopleTextBox"]->animateIn( true );
        hudLabelMap["VisualsTextBox"]->animateIn( true );
    }
    */
}

void CloudsHUDController::animateOff(){
    animateOff(CLOUDS_HUD_ALL);
}

void CloudsHUDController::animateOff(CloudsHUDLayerSet layer){
    
    if (isPlaying) {
		isPlaying = false;
        videoPlayer.stop();
        videoPlayer.close();
    }

    for (map<CloudsHUDLayerSet, CloudsHUDLayer* >::iterator it = layers.begin(); it != layers.end(); ++it) {
        if (layer == it->first || layer == CLOUDS_HUD_ALL) {
            hudOpenMap[it->first] = false;
            it->second->close();
        }
    }
    
    // animate out text, this is sub-optimal
    if( layer == CLOUDS_HUD_ALL ){
        for( map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it ){
            (it->second)->animateOut();
        }
    }
    else{
        for(int i = 0; i < hudLayerLabels[layer].size(); i++){
            hudLayerLabels[layer][i]->animateOut();
        }
    }
    
    /*
    else if( layer == CLOUDS_HUD_LOWER_THIRD ){
        hudLabelMap["BylineFirstNameTextBox_1_"]->animateOut();
        hudLabelMap["BylineLastNameTextBox"]->animateOut();
        hudLabelMap["BylineTopicTextBoxTop"]->animateOut();
        hudLabelMap["BylineTopicTextBoxBottom"]->animateOut();
//        hudLabelMap["BylineBodyCopyTextBox"]->animateOut();
    }
    else if( layer == CLOUDS_HUD_PROJECT_EXAMPLE ){
        hudLabelMap["ProjectExampleTextboxLeft"]->animateOut();
        hudLabelMap["ProjectExampleTextboxRight"]->animateOut();
        hudLabelMap["ProjectExampleTextBoxTop"]->animateOut();
    }
    else if( layer == CLOUDS_HUD_QUESTION ){
        hudLabelMap["QuestionTextBox_1_"]->animateOut();
    }
    else if( layer == CLOUDS_HUD_PAUSE ){
		hudLabelMap["ResetButtonTextBox"]->animateOut();
        hudLabelMap["ExploreTextBox"]->animateOut();
        hudLabelMap["SeeMoreTextBox"]->animateOut();
    }
    else if( layer == CLOUDS_HUD_NEXT){
        hudLabelMap["NextButtonTextBox"]->animateOut();
    }
    else if( layer == CLOUDS_HUD_RESEARCH_LIST){
        hudLabelMap["RSResetButtonTextBox"]->animateOut();
    }
    else if( layer == CLOUDS_HUD_RESEARCH_NAV){
        hudLabelMap["MapTextBox"]->animateOut();
        hudLabelMap["PeopleTextBox"]->animateOut();
        hudLabelMap["VisualsTextBox"]->animateOut();
    }
     */
    
}

ofRectangle CloudsHUDController::getScaledRectangle(const ofRectangle& rect){
    ofRectangle outrect;
    outrect.x      = rect.x * scaleAmt + scaleOffset.x;
	outrect.y      = rect.y * scaleAmt + scaleOffset.y;
	outrect.width  = rect.width  * scaleAmt;
	outrect.height = rect.height * scaleAmt;
    return outrect;
}

