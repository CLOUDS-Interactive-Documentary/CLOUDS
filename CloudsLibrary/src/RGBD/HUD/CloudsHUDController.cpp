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
    scrollPosition = 0;
    totalScrollHeight = 0;
    scrollPressedTime = 0;
    
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
	hudLabelMap["QuestionTextBox"]->setText("", false);
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

    CloudsHUDLayer* researchListLayer = new CloudsHUDLayer();
    researchListLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_RESEARCH_LIST");
    layers[CLOUDS_HUD_RESEARCH_LIST] = researchListLayer;
    allLayers.push_back( researchListLayer );
    
    CloudsHUDLayer* researchNavLayer = new CloudsHUDLayer();
    researchNavLayer->parseDirectory(GetCloudsDataPath() + "HUD/SVG/CLOUDS_RESEARCH_NAV");
    layers[CLOUDS_HUD_RESEARCH_NAV] = researchNavLayer;
    allLayers.push_back( researchNavLayer );
    
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
    
    QuestionTextBox             = getLayoutForLayer("QuestionTextBox", fontPath);

    ProjectExampleTextboxLeft   = getLayoutForLayer("ProjectExampleTextboxLeft", fontPath);
    ProjectExampleTextboxRight  = getLayoutForLayer("ProjectExampleTextboxRight", fontPath);
    ProjectExampleTextBoxTop    = getLayoutForLayer("ProjectExampleTextBoxTop", fontPath);
    
    //pause
    ExploreTextBox              = getLayoutForLayer("ExploreTextBox", fontPath);
    SeeMoreTextBox              = getLayoutForLayer("SeeMoreTextBox", fontPath);
    NextButtonTextBox           = getLayoutForLayer("NextButtonTextBox", fontPath);
    
    ResetButtonTextBox          = getLayoutForLayer("ResetButtonTextBox", fontPath);
    //research stuff
    ResearchTopicListFont       = getFontForLayer("ListPeopleTextBox", fontPath, 35);
    
    //research navigation
    ResearchMapTabFont          = getFontForLayer("MapTextBox", fontPath, 35);
    ResearchPeopleTabFont       = getFontForLayer("PeopleTextBox", fontPath, 35);
    ResearchVisualsTabFont      = getFontForLayer("VisualsTextBox", fontPath, 35);
    ResearchResetButtonFont     = getFontForLayer("ResearchResetButtonTextBox", fontPath, 35);
    
    scrollIncrement             = hudLabelMap["ListPeopleTextBox"]->bounds.height * 1.5;
 
    hudLabelMap["MapTextBox"]->setText(GetTranslationForString("MAP"), false);
    hudLabelMap["MapTextBox"]->clearTextOnAnimateOut = false;
    hudLabelMap["PeopleTextBox"]->setText(GetTranslationForString("PEOPLE"), false);
    hudLabelMap["PeopleTextBox"]->clearTextOnAnimateOut = false;
    hudLabelMap["VisualsTextBox"]->setText(GetTranslationForString("VISUALS"), false);
    hudLabelMap["VisualsTextBox"]->clearTextOnAnimateOut = false;
    hudLabelMap["ResearchResetButtonTextBox"]->setText(GetTranslationForString("RESET"), false); //this one may change...
    hudLabelMap["ResearchResetButtonTextBox"]->clearTextOnAnimateOut = false;
 
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
            float newScrollPosition = scrollPosition + scrollIncrement * (bIsScrollUpPressed ? -1 : 1);
            scrollPosition = ofClamp(newScrollPosition, 0, totalScrollHeight - researchScrollBounds.height);
            bIsHoldScrolling = true;
        }
    }
    
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
}

bool CloudsHUDController::isTopicSelected(){
    for(int i = 0; i < topicButtons.size(); i++){
        if(topicButtons[i].clicked){
            return true;
        }
    }
    return false;
}

bool CloudsHUDController::isTopicConfirmed(){
    //TODO: User needs to click on topic in the map, just return true for now
    for(int i = 0; i < topicButtons.size(); i++){
        topicButtons[i].clicked = false;
    }
    
    return true;
}

string CloudsHUDController::getSelectedTopic(){
    for(int i = 0; i < topicButtons.size(); i++){
        if(topicButtons[i].clicked){
            return topicButtons[i].topic;
        }
    }
    return "";
}

void CloudsHUDController::updateResearchNavigation(){

    //get the backing
    hudLabelMap["MapTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_RESEARCH_NAV]->svg.getMeshByID("MapHoverBacking")->bounds;
    hudLabelMap["PeopleTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_RESEARCH_NAV]->svg.getMeshByID("PeopleHoverAltColor")->bounds;
    hudLabelMap["VisualsTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_RESEARCH_NAV]->svg.getMeshByID("VSHoverAltColor")->bounds;
    hudLabelMap["ResearchResetButtonTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_RESEARCH_NAV]->svg.getMeshByID("ResearchResetButtonBacking")->bounds;
    
    //set the interaction regions
    hudLabelMap["MapTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["MapTextBox"]->baseInteractiveBounds);
    hudLabelMap["PeopleTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["PeopleTextBox"]->baseInteractiveBounds);
    hudLabelMap["VisualsTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["VisualsTextBox"]->baseInteractiveBounds);
    hudLabelMap["ResearchResetButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ResearchResetButtonTextBox"]->baseInteractiveBounds);
    

    
    //TODO:
    //hudLabelMap["ResearchResetButtonTextBox"]
    
}
void CloudsHUDController::pause(){

    //TODO: save the current HUD state before pause

    hudLabelMap["ResetButtonTextBox"]->setText(GetTranslationForString("RESET"));
    hudLabelMap["NextButtonTextBox"]->setText(GetTranslationForString("NEXT"));
    hudLabelMap["ExploreTextBox"]->setText(GetTranslationForString("EXPLORE THE MAP"));
    hudLabelMap["SeeMoreTextBox"]->setText(GetTranslationForString("SEE MORE OF THIS PERSON")); //todo dynmic name
    
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
    hudLabelMap["NextButtonTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("NextButtonBacking")->bounds;
    
    //set the interaction regions
    hudLabelMap["ResetButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ResetButtonTextBox"]->baseInteractiveBounds);
    hudLabelMap["ExploreTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ExploreTextBox"]->baseInteractiveBounds);
    hudLabelMap["SeeMoreTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["SeeMoreTextBox"]->baseInteractiveBounds);
    hudLabelMap["NextButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["NextButtonTextBox"]->baseInteractiveBounds);
 
    animateOff( CLOUDS_HUD_QUESTION );
    animateOn( CLOUDS_HUD_PAUSE );

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
    topicButtons.clear();
    topicButtons.resize(topics.size());
    int i = 0;
    for(set<string>::iterator it = topics.begin(); it != topics.end(); it++){
        topicButtons[i].top = i * scrollIncrement; //1.5 is an arbirtary margine
        topicButtons[i].topic = *it;
        i++;
    }
    
    totalScrollHeight = topicButtons.back().top + scrollIncrement;
}

void CloudsHUDController::mouseMoved(ofMouseEventArgs& args){

    for (map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        (it->second)->mouseMoved(ofVec2f(args.x,args.y));
    }
    
    if(hudOpenMap[CLOUDS_HUD_RESEARCH_LIST]){
        bIsScrollUpHover = scrollUpBoundsScaled.inside(args.x, args.y);
        bIsScrollDownHover = scrollDownBoundsScaled.inside(args.x, args.y);
        
        if(researchScrollBounds.inside(args.x, args.y)){
            for(int i = 0; i < topicButtons.size(); i++){
                if(topicButtons[i].visible){
                    topicButtons[i].hovered = topicButtons[i].selectRect.inside(args.x, args.y);
                }
            }
        }
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
            for(int i = 0; i < topicButtons.size(); i++){
                topicButtons[i].clicked = false;
                if(topicButtons[i].visible){
                    topicButtons[i].pressed = topicButtons[i].selectRect.inside(args.x, args.y);
                }
            }
        }
    }
}

void CloudsHUDController::mouseReleased(ofMouseEventArgs& args){

    for (map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        (it->second)->mouseReleased(ofVec2f(args.x,args.y));
    }
    
    if(hudOpenMap[CLOUDS_HUD_RESEARCH_LIST]){
        if(bIsScrollUpPressed && scrollUpBoundsScaled.inside(args.x, args.y)){
            float newScrollPosition = scrollPosition - scrollIncrement;
            scrollPosition = ofClamp(newScrollPosition, 0, totalScrollHeight - researchScrollBounds.height);
        }
        if(bIsScrollDownPressed && scrollDownBoundsScaled.inside(args.x, args.y)){
            float newScrollPosition = scrollPosition + scrollIncrement;
            scrollPosition = ofClamp(newScrollPosition, 0, totalScrollHeight - researchScrollBounds.height);
        }        
        bIsHoldScrolling = false;
        
        for(int i = 0; i < topicButtons.size(); i++){
            if(topicButtons[i].visible){
                topicButtons[i].clicked = topicButtons[i].pressed && topicButtons[i].selectRect.inside(args.x, args.y);
            }
        }
    }
}

bool CloudsHUDController::isResetHit(){
    return hudLabelMap["ResetButtonTextBox"]->isClicked();
}

bool CloudsHUDController::isNextHit(){
    return hudLabelMap["NextButtonTextBox"]->isClicked();
}

bool CloudsHUDController::isExploreMapHit(){
    return hudLabelMap["ExploreTextBox"]->isClicked();
}

bool CloudsHUDController::isSeeMorePersonHit(){
    return hudLabelMap["SeeMoreTextBox"]->isClicked();
}

bool CloudsHUDController::selectedMapTab(){
    return hudLabelMap["MapTextBox"]->isClicked();
}

bool CloudsHUDController::selectedPeopleTab(){
    return hudLabelMap["PeopleTextBox"]->isClicked();
}

bool CloudsHUDController::selectedVisualsTab(){
    return hudLabelMap["VisualsTextBox"]->isClicked();
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

	drawLayer(CLOUDS_HUD_HOME);
	drawLayer(CLOUDS_HUD_QUESTION);
	drawLayer(CLOUDS_HUD_LOWER_THIRD);
	drawLayer(CLOUDS_HUD_PROJECT_EXAMPLE);
	drawLayer(CLOUDS_HUD_PAUSE);
	drawLayer(CLOUDS_HUD_RESEARCH_LIST);
	drawLayer(CLOUDS_HUD_RESEARCH_NAV);

    if(hudOpenMap[CLOUDS_HUD_RESEARCH_LIST]){
        
        ofSetColor(255, 0, 0);
        ofRect(scrollUpBounds);
        ofRect(scrollDownBounds);
        ofSetColor(255);
        
        beginListStencil();
        
        if( currentTab == CLOUDS_HUD_RESEARCH_TAB_TOPICS ){
            drawTopicsList();
        }
        else if( currentTab == CLOUDS_HUD_RESEARCH_TAB_PEOPLE){
            drawPeopleList();
        }
        else{
            draweVisualList();
        }
        
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
    ofTranslate(0, -scrollPosition);
    
}

void CloudsHUDController::endListStencil(){
    
    ofPopMatrix();
	glDisable(GL_SCISSOR_TEST);
    //ofPopStyle();
    
}

void CloudsHUDController::drawTopicsList(){
    ofPushStyle();
    for(int i = 0; i < topicButtons.size(); i++){
        if(topicButtons[i].visible){
            //TODO: better coloring system
            if(topicButtons[i].clicked){
                ofSetColor(255, 180, 0);
            }
            else if(topicButtons[i].pressed){
                ofSetColor(255, 180, 0, 200);
            
            }
            else if(topicButtons[i].hovered){
                ofSetColor(255);
            }
            else{
                ofSetColor(255, 200);
            }
            ResearchTopicListFont->drawString(topicButtons[i].topic,
                                              hudLabelMap["ListPeopleTextBox"]->bounds.x,
                                              hudLabelMap["ListPeopleTextBox"]->bounds.y + topicButtons[i].top);
        }
    }
    ofPopStyle();
}

void CloudsHUDController::drawPeopleList(){
    
}

void CloudsHUDController::draweVisualList(){
    
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
    else if( layer == CLOUDS_HUD_LOWER_THIRD ){
        hudLabelMap["BylineFirstNameTextBox_1_"]->animateIn( true );
        hudLabelMap["BylineLastNameTextBox"]->animateIn( true );
        hudLabelMap["BylineTopicTextBoxTop"]->animateIn( true );
        hudLabelMap["BylineTopicTextBoxBottom"]->animateIn( true );
        hudLabelMap["BylineBodyCopyTextBox"]->animateIn( true );
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
        hudLabelMap["NextButtonTextBox"]->animateIn( true );
    }
    else if( layer == CLOUDS_HUD_QUESTION ){
        hudLabelMap["QuestionTextBox"]->animateIn( true );
    }
    else if( layer == CLOUDS_HUD_RESEARCH_NAV){
        hudLabelMap["MapTextBox"]->animateIn( true );
        hudLabelMap["PeopleTextBox"]->animateIn( true );
        hudLabelMap["VisualsTextBox"]->animateIn( true );
        hudLabelMap["ResearchResetButtonTextBox"]->animateIn( true );
    }
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
    else if( layer == CLOUDS_HUD_LOWER_THIRD ){
        hudLabelMap["BylineFirstNameTextBox_1_"]->animateOut();
        hudLabelMap["BylineLastNameTextBox"]->animateOut();
        hudLabelMap["BylineTopicTextBoxTop"]->animateOut();
        hudLabelMap["BylineTopicTextBoxBottom"]->animateOut();
        hudLabelMap["BylineBodyCopyTextBox"]->animateOut();
    }
    else if( layer == CLOUDS_HUD_PROJECT_EXAMPLE ){
        hudLabelMap["ProjectExampleTextboxLeft"]->animateOut();
        hudLabelMap["ProjectExampleTextboxRight"]->animateOut();
        hudLabelMap["ProjectExampleTextBoxTop"]->animateOut();
    }
    else if( layer == CLOUDS_HUD_QUESTION ){
        hudLabelMap["QuestionTextBox"]->animateOut();
    }
    else if( layer == CLOUDS_HUD_PAUSE ){
		hudLabelMap["ResetButtonTextBox"]->animateOut();
        hudLabelMap["ExploreTextBox"]->animateOut();
        hudLabelMap["SeeMoreTextBox"]->animateOut();
        hudLabelMap["NextButtonTextBox"]->animateOut();
    }
    else if( layer == CLOUDS_HUD_RESEARCH_NAV){
        hudLabelMap["MapTextBox"]->animateOut();
        hudLabelMap["PeopleTextBox"]->animateOut();
        hudLabelMap["VisualsTextBox"]->animateOut();
        hudLabelMap["ResearchResetButtonTextBox"]->animateOut();
    }
    
}

ofRectangle CloudsHUDController::getScaledRectangle(const ofRectangle& rect){
    ofRectangle outrect;
    outrect.x      = rect.x * scaleAmt + scaleOffset.x;
	outrect.y      = rect.y * scaleAmt + scaleOffset.y;
	outrect.width  = rect.width  * scaleAmt;
	outrect.height = rect.height * scaleAmt;
    return outrect;
}

