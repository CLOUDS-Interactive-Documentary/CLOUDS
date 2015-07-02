//
//  CloudsHUDController.cpp
//  CloudsPointcloudViewer
//
//  Created by James George on 12/14/13.
//
//

#include "CloudsHUDController.h"

#include "CloudsGlobal.h"
#include "CloudsLocalization.h"

#include "CloudsInput.h"
#include "CloudsClip.h"
#include "CloudsHUDLayer.h"
#include "CloudsVisualSystem.h"

bool listSort(CloudsHUDResearchButton b, CloudsHUDResearchButton a){
    return b.label < a.label;
}

CloudsHUDController::CloudsHUDController(){

    bSkipAVideoFrame = false;
    bDrawHud = true;
    bProjectExampleDisplayed = false;
    transitionFade = 0.0;
    
    visualLevelChanged = false;
    setToPretty = false;
    historyCleared = false;
    
    bQuestionDisplayed = false;
    bJustPaused = false;
    bJustUnpaused = false;
    bActJustStarted = false;
    bVisualSystemDisplayed = false;
    bClipIsPlaying = false;
    bResearchTransitioning = false;
    bItemSelectionChanged = false;
    
    currentPreviewImage = &topicMapPreview;
    currentTab = CLOUDS_HUD_RESEARCH_TAB_TOPICS;

    researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS].scrollPosition = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS].totalScrollHeight = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE].scrollPosition = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE].totalScrollHeight = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_VISUALS].scrollPosition = 0;
    researchLists[CLOUDS_HUD_RESEARCH_TAB_VISUALS].totalScrollHeight = 0;
    
    currentResearchList = &researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS];
 
	isPlaying = false;
    
    scaleAmt = 1.0;
    margin = 40;
    bPaused = false;
    waitingMedia = NULL;
    bSetup = false;

}

void CloudsHUDController::setup(){

    bSetup = true;
	buildLayerSets();
    calculateFontSizes();

#ifdef MOUSE_INPUT
	ofAddListener(ofEvents().mouseMoved,this, &CloudsHUDController::mouseMoved, OF_EVENT_ORDER_BEFORE_APP);
	ofAddListener(ofEvents().mousePressed,this, &CloudsHUDController::mousePressed, OF_EVENT_ORDER_BEFORE_APP);
	ofAddListener(ofEvents().mouseReleased,this, &CloudsHUDController::mouseReleased, OF_EVENT_ORDER_BEFORE_APP);
    ofAddListener(ofEvents().mouseScrolled,this, &CloudsHUDController::mouseScrolled, OF_EVENT_ORDER_BEFORE_APP);
#endif

#ifdef VHX_MEDIA
    parseVHXIds();
#endif
    
	home.setup();
    backersFont.loadFont(GetFontBuffer(), 10);
    
    topicMapPreview.loadImage(GetCloudsDataPath() + "HUD/TopicMapPreview.jpg");
    peopleMapPreview.loadImage(GetCloudsDataPath() + "HUD/PeopleMapPreview.jpg");
	 
}

void CloudsHUDController::showProjectExample(const string& videoPath, bool forceOn)
{
	if(isPlaying){
        videoPlayer.stop();
    }
    
    isPlaying = videoPlayer.loadMovie(videoPath);
    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    videoPlayer.play();

    bSkipAVideoFrame = true;

    if( forceOn ){
        animateOn( CLOUDS_HUD_PROJECT_EXAMPLE );
    }
}

#ifdef VHX_MEDIA
void CloudsHUDController::parseVHXIds(){
    if (!vhxProjectExampleIds.empty()) {
        for (map<string, CloudsMedia *>::iterator it = vhxProjectExampleIds.begin(); it != vhxProjectExampleIds.end(); ++it) {
            delete it->second;
        }
    }
    vhxProjectExampleIds.clear();
    
    map<string, string> idMap;
    ParseVHXIds(GetCloudsDataPath() + "vhx/examples.csv", idMap);
    
    for (map<string, string>::iterator it = idMap.begin(); it != idMap.end(); ++it) {
        CloudsMedia *media = new CloudsMedia();
        media->vhxId = it->second;
        media->hasMediaAsset = true;
        vhxProjectExampleIds[it->first] = media;
    }
    cout << "FOUND " << vhxProjectExampleIds.size() << " MEDIA IDs" << endl;
}

void CloudsHUDController::vhxRequestComplete(CloudsVHXEventArgs& args){
    if(!args.success){
        ofLogError("CloudsHUDController::vhxRequestComplete") << "VHX request failed";
        return;
    }
    
    ofRemoveListener(waitingMedia->completeEvent, this, &CloudsHUDController::vhxRequestComplete);
    
    showProjectExample(args.result, true);

}
#endif


void CloudsHUDController::buildLayerSets(){
	
	//configure layers
    for(int i = 0; i < CLOUDS_HUD_ALL; i++){
        CloudsHUDLayerSet layerSet = CloudsHUDLayerSet(i);
        CloudsHUDLayer* layer = new CloudsHUDLayer();
        layer->load(GetCloudsDataPath() + "HUD/SVG/" + filenameForLayer(layerSet));
        layers[layerSet] = layer;
        allLayers.push_back( layer );
    }
                    
    for( int i = 0; i < allLayers.size(); i++ ){

		for( int s = 0; s < allLayers[i]->svg.getMeshes().size(); s++){
			ofVboMesh& m = allLayers[i]->svg.getMeshes()[s].mesh;
			for(int v = 0; v < m.getNumVertices(); v++){
				m.addNormal(ofVec3f(ofRandomuf(),0,0));
			}
		}
        
        allLayers[i]->duration = 1.;
    }
    
    home.bounds = layers[CLOUDS_HUD_HOME]->svg.getMeshByID("HomeButtonFrame")->bounds;
    home.bounds.scaleFromCenter(1.5);
    
    bioBounds = layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("BioFrame")->bounds;
    
    svgVideoBounds = layers[CLOUDS_HUD_PROJECT_EXAMPLE]->svg.getMeshByID("ProjectExampleFrame")->bounds;
	videoBounds = svgVideoBounds;
    
    researchScroller.scrollBounds     = layers[CLOUDS_RESEARCH]->svg.getMeshByID("ListBacking")->bounds;
    researchScroller.scrollUpBounds   = layers[CLOUDS_RESEARCH]->svg.getMeshByID("ListScrollUpBacking")->bounds;
    researchScroller.scrollDownBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("ListScrollDownBacking")->bounds;
    
    aboutScroller.scrollBounds        = layers[CLOUDS_ABOUT_BACKERS]->svg.getMeshByID("BackersList1TextBox")->bounds.getUnion(
                                        layers[CLOUDS_ABOUT_BACKERS]->svg.getMeshByID("BackersList2TextBox")->bounds.getUnion(
                                        layers[CLOUDS_ABOUT_BACKERS]->svg.getMeshByID("BackersList3TextBox")->bounds));

    aboutScroller.scrollUpBounds      = layers[CLOUDS_ABOUT_BACKERS]->svg.getMeshByID("BackersScrollUpBacking")->bounds;
    aboutScroller.scrollDownBounds    = layers[CLOUDS_ABOUT_BACKERS]->svg.getMeshByID("BackersScrollDownBacking")->bounds;
    
    layers[CLOUDS_ABOUT_BACKERS]->svg.getMeshByID("BackersScrollUpBacking")->visible = false;
    layers[CLOUDS_ABOUT_BACKERS]->svg.getMeshByID("BackersScrollDownBacking")->visible = false;

    
    hudBounds.set(0, 0, allLayers[0]->svg.getWidth(), allLayers[0]->svg.getHeight() );
    
    //ensure the lines are always up
    layers[CLOUDS_ABOUT_MAIN]->bForceHover = true;
    
    layers[CLOUDS_HUD_HOME]->bForceHover = true;
    layers[CLOUDS_HUD_PAUSE]->bForceHover = true;
    layers[CLOUDS_HUD_PROJECT_EXAMPLE]->bForceHover = true;
    layers[CLOUDS_RESEARCH]->bForceHover = true;
    layers[CLOUDS_RESEARCH_RESUME]->bForceHover = true;
    
    layers[CLOUDS_RESEARCH_TOPIC]->bForceHover = true;
    layers[CLOUDS_RESEARCH_PPL]->bForceHover = true;
    layers[CLOUDS_RESEARCH_VS]->bForceHover = true;
    layers[CLOUDS_ABOUT_CONTACT]->bForceHover = true;

    layers[CLOUDS_ABOUT_SETTINGS_EXPANDED]->bForceHover = true;
}

string CloudsHUDController::filenameForLayer(CloudsHUDLayerSet layer){
    switch (layer) {
        case CLOUDS_HUD_HOME:
            return "CLOUDS_HUD_HOME.svg";
        case CLOUDS_HUD_QUESTION:
            return "CLOUDS_HUD_QUESTION.svg";
        case CLOUDS_HUD_LOWER_THIRD:
            return "CLOUDS_HUD_LOWER_THIRD.svg";
        case CLOUDS_HUD_PROJECT_EXAMPLE:
            return "CLOUDS_HUD_PROJECT_EXAMPLE.svg";
        case CLOUDS_HUD_PAUSE:
            return "CLOUDS_HUD_PAUSE.svg";
        case CLOUDS_HUD_NEXT:
            return "CLOUDS_HUD_NEXT.svg";
        case CLOUDS_RESEARCH:
            return "CLOUDS_RESEARCH.svg";
        case CLOUDS_RESEARCH_SHUFFLE:
            return "CLOUDS_RESEARCH_SHUFFLE.svg";
        case CLOUDS_RESEARCH_RESUME:
            return "CLOUDS_RESEARCH_RESUME.svg";
        case CLOUDS_RESEARCH_TOPIC:
            return "CLOUDS_RESEARCH_TOPIC.svg";
        case CLOUDS_RESEARCH_PPL:
            return "CLOUDS_RESEARCH_PPL.svg";
        case CLOUDS_RESEARCH_VS:
            return "CLOUDS_RESEARCH_VS.svg";
        case CLOUDS_ABOUT_BACKERS:
            return "CLOUDS_ABOUT_BACKERS.svg";
        case CLOUDS_ABOUT_CAST:
            return "CLOUDS_ABOUT_CAST.svg";
        case CLOUDS_ABOUT_CREDITS:
            return "CLOUDS_ABOUT_CREDITS.svg";
        case CLOUDS_ABOUT_INFO:
            return "CLOUDS_ABOUT_INFO.svg";
        case CLOUDS_ABOUT_MAIN:
            return "CLOUDS_ABOUT_MAIN.svg";
        case CLOUDS_ABOUT_CONTACT:
            return "CLOUDS_ABOUT_CONTACT.svg";
        case CLOUDS_ABOUT_SETTINGS:
            return "CLOUDS_ABOUT_SETTINGS.svg";
        case CLOUDS_ABOUT_SETTINGS_EXPANDED:
            return "CLOUDS_ABOUT_SETTINGS_EXPANDED.svg";
        default:
            break;
    }
    
    ofLogError("CloudsHUDController::filenameForLayer") << "No filename for layer " << layer;
    return "ERROR!";
}

void CloudsHUDController::calculateFontSizes(){
    // temporary allocate
    int minFontSize = 1;
    int maxFontSize = 70;
#ifdef OCULUS_RIFT
	ofBuffer& thinFontBuffer = GetFontBuffer();
	ofBuffer& bookFontBuffer = GetBoldFontBuffer();
#else
	ofBuffer& thinFontBuffer = GetThinFontBuffer();
    ofBuffer& bookFontBuffer = GetFontBuffer();
#endif
    
    for(int i = minFontSize; i < maxFontSize; i++){
        ofxFTGLFont *tmp = new ofxFTGLFont();
        tmp->loadFont(bookFontBuffer , i );
        tempFontList.push_back( tmp );
    }
    
    //BIO
    ////first name
    getLabelForLayer("BylineFirstNameTextBox_1_", thinFontBuffer, 50, true);
    ////last name
    getLabelForLayer("BylineLastNameTextBox", thinFontBuffer, 50, true);
    ////title
    getLabelForLayer("BylineTopicTextBoxBottom", bookFontBuffer);
    ////location
    getLabelForLayer("BylineTopicTextBoxTop", bookFontBuffer);
    getLabelForLayer("VSCreditsTextBoxTop", bookFontBuffer);
    getLabelForLayer("VSCreditsTextBoxBottom", bookFontBuffer);
    getLabelForLayer("QuestionTextBox_1_", bookFontBuffer);
    
    getLabelForLayer("ProjectExampleTextboxLeft", bookFontBuffer);
    getLabelForLayer("ProjectExampleTextboxRight", bookFontBuffer);
    getLabelForLayer("ProjectExampleTextBoxTop", bookFontBuffer);
    
    //pause
    getLabelForLayer("ExploreTextBox", bookFontBuffer);
    getLabelForLayer("SeeMoreTextBox", bookFontBuffer);
    getLabelForLayer("NextButtonTextBox", bookFontBuffer);
    getLabelForLayer("BioTitleTextBox", bookFontBuffer);
#ifdef OCULUS_RIFT
    getLabelForLayer("BioTextBox", bookFontBuffer,35,false,true, 15); //use layout
#else
    getLabelForLayer("BioTextBox", bookFontBuffer,35,false,true); //use layout
#endif
    getLabelForLayer("ResetButtonTextBox", bookFontBuffer);
    //research stuff
    ResearchTopicListLabel = getLabelForLayer("ListTextBoxes", bookFontBuffer);
    
    //research navigation
    getLabelForLayer("MapTextBox", bookFontBuffer);
    getLabelForLayer("PeopleTextBox", bookFontBuffer);
    getLabelForLayer("VisualsTextBox", bookFontBuffer);
    getLabelForLayer("RSResetButtonTextBox", bookFontBuffer);
    getLabelForLayer("ShuffleButtonTextBox", bookFontBuffer);
    getLabelForLayer("ResumeButtonTextBox", bookFontBuffer);

    //research floating hovers
    getLabelForLayer("PeopleSelectBylineTextBox", bookFontBuffer, 35,false,true);
    getLabelForLayer("PeopleSelectPlayTextBox", bookFontBuffer);
    getLabelForLayer("PeopleSelectNameTextBox", bookFontBuffer);
    
    getLabelForLayer("TopicSelectTextBox", bookFontBuffer);
    getLabelForLayer("TopicSelectPlayTextBox", bookFontBuffer);
    
    //about text boxes
    getLabelForLayer("NavAboutTextBox", bookFontBuffer);
    getLabelForLayer("NavCastTextBox", bookFontBuffer);
    getLabelForLayer("NavCreditsTextBox", bookFontBuffer);
    getLabelForLayer("NavBackersTextBox", bookFontBuffer);
    
    getLabelForLayer("ExitButtonTextBox", bookFontBuffer);
    
    getLabelForLayer("SupportButtonTextBox", bookFontBuffer);
    getLabelForLayer("GithubButtonTextBox", bookFontBuffer);
    
    getLabelForLayer("AboutTextBox", bookFontBuffer, 35, false,true, 12);
    getLabelForLayer("AboutTitleTextBox", thinFontBuffer);
    
    getLabelForLayer("CastTitleTextBox", thinFontBuffer);
    getLabelForLayer("CastList1TextBox", bookFontBuffer, 35,false,true);
    getLabelForLayer("CastList2TextBox", bookFontBuffer, 35,false,true);
    getLabelForLayer("CastList3TextBox", bookFontBuffer, 35,false,true);

    getLabelForLayer("CreditsTitleTextBox", thinFontBuffer);
    getLabelForLayer("CreditsList1TextBox", bookFontBuffer, 35, false,true, 10);
    getLabelForLayer("CreditsList2TextBox", bookFontBuffer, 35, false,true, 10);
    getLabelForLayer("CreditsList3TextBox", bookFontBuffer, 35, false,true, 10);
    
    getLabelForLayer("BackersCopyTextBox", bookFontBuffer, 35, false,true, 10);
    getLabelForLayer("BackersList1TextBox", bookFontBuffer, 35, false,true, 10);
    getLabelForLayer("BackersList2TextBox", bookFontBuffer, 35, false,true, 10);
    getLabelForLayer("BackersList3TextBox", bookFontBuffer, 35, false,true, 10);

    getLabelForLayer("BackersScrollUpSpace", thinFontBuffer);
    getLabelForLayer("BackersScrollDownSpace", thinFontBuffer);
    
    //settings
    //getLabelForLayer("SettingsTextBox", bookFontBuffer);
    getLabelForLayer("SettingsTextBoxExpanded", bookFontBuffer);
    getLabelForLayer("SettingsFastTextBox", bookFontBuffer);
    getLabelForLayer("SettingsPrettyTextBox", bookFontBuffer);
    getLabelForLayer("SettingsClearHistoryTextBox", bookFontBuffer);
    
    getLabelForLayer("SettingsHelperTextBox", bookFontBuffer, 35, false,true, 8);
    getLabelForLayer("SettingsClearHelperTextBox", bookFontBuffer, 35, false,true, 8);

    //cheat for scroll
    getLabelForLayer("ListScrollUpArrowSpace", thinFontBuffer);
    getLabelForLayer("ListScrollDownArrowSpace", thinFontBuffer);


    // cleanup!
    for(int i = 0; i < tempFontList.size(); i++){
        delete tempFontList[i];
    }
    tempFontList.clear();
    
    
    researchScroller.scrollIncrement = hudLabelMap["ListTextBoxes"]->bounds.height * 1.5;
    aboutScroller.scrollIncrement = hudLabelMap["ListTextBoxes"]->bounds.height * 1.5;
    
    hudLabelMap["MapTextBox"]->setText(GetTranslationForString("MAP"), false);
    hudLabelMap["PeopleTextBox"]->setText(GetTranslationForString("PEOPLE"), false);
    hudLabelMap["VisualsTextBox"]->setText(GetTranslationForString("VISUALS"), false);
    hudLabelMap["RSResetButtonTextBox"]->setText(GetTranslationForString("QUIT"), false);
    hudLabelMap["ResumeButtonTextBox"]->setText(GetTranslationForString("RESUME"), false); 
    
    hudLabelMap["ResetButtonTextBox"]->setText(GetTranslationForString("QUIT"), false);
    hudLabelMap["NextButtonTextBox"]->setText(GetTranslationForString("NEXT"), false);
    hudLabelMap["ExploreTextBox"]->setText(GetTranslationForString("EXPLORE THE MAP"), false);
    hudLabelMap["SeeMoreTextBox"]->setText(GetTranslationForString("SEE MORE OF THIS PERSON"), false); //todo dynmic name

    hudLabelMap["TopicSelectPlayTextBox"]->setText(GetTranslationForString("PLAY"), false);
    hudLabelMap["PeopleSelectPlayTextBox"]->setText(GetTranslationForString("PLAY"), false);
    
    hudLabelMap["ShuffleButtonTextBox"]->setText(GetTranslationForString("SHUFFLE ALL"), false); //todo dynmic name
    hudLabelMap["BioTitleTextBox"]->setText(GetTranslationForString("BIO"), false);
    hudLabelMap["BioTextBox"]->layout->setLineLength(hudLabelMap["BioTextBox"]->bounds.width);
    
    hudLabelMap["PeopleSelectBylineTextBox"]->layout->setLineLength(hudLabelMap["PeopleSelectBylineTextBox"]->bounds.width);

    hudLabelMap["NavAboutTextBox"]->setText(GetTranslationForString("ABOUT"), false);
    hudLabelMap["NavCastTextBox"]->setText(GetTranslationForString("CAST"), false);
    hudLabelMap["NavCreditsTextBox"]->setText(GetTranslationForString("CREDITS"), false);
    hudLabelMap["NavBackersTextBox"]->setText(GetTranslationForString("SUPPORTERS"), false);
    
    hudLabelMap["ExitButtonTextBox"]->setText(GetTranslationForString("DONE"), false);
    hudLabelMap["SupportButtonTextBox"]->setText(GetTranslationForString("CONTACT US"), false);
    hudLabelMap["GithubButtonTextBox"]->setText(GetTranslationForString("VIEW SOURCE"), false);

    ///////////////////////////
    hudLabelMap["AboutTextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/about.txt").getText(), false);
    hudLabelMap["AboutTextBox"]->layout->setLineLength(hudLabelMap["AboutTextBox"]->bounds.width);
    hudLabelMap["AboutTitleTextBox"]->setText(GetTranslationForString("ABOUT"), false);

    
    hudLabelMap["CastTitleTextBox"]->setText(GetTranslationForString("CAST"), false);
    hudLabelMap["CastList1TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/cast1.txt").getText(), false);
    hudLabelMap["CastList2TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/cast2.txt").getText(), false);
    hudLabelMap["CastList3TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/cast3.txt").getText(), false);
    
    hudLabelMap["CreditsTitleTextBox"]->setText(GetTranslationForString("CREDITS"), false);
    hudLabelMap["CreditsList1TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/credits1.txt").getText(), false);
    hudLabelMap["CreditsList2TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/credits2.txt").getText(), false);
    hudLabelMap["CreditsList3TextBox"]->setText(ofBufferFromFile(GetCloudsDataPath() + "about/credits3.txt").getText(), false);
    
    ///// SETTING ABOUT
    hudLabelMap["SettingsTextBoxExpanded"]->setText(GetTranslationForString("SETTINGS"), false);
    hudLabelMap["SettingsFastTextBox"]->setText(GetTranslationForString("FAST"), false);
    hudLabelMap["SettingsPrettyTextBox"]->setText(GetTranslationForString("PRETTY"), false);
    hudLabelMap["SettingsHelperTextBox"]->setText("Fast for quick framerates, Pretty for best graphics.", false);
    hudLabelMap["SettingsHelperTextBox"]->layout->setLineLength(hudLabelMap["SettingsHelperTextBox"]->bounds.width);
    
    hudLabelMap["SettingsClearHistoryTextBox"]->setText(GetTranslationForString("CLEAR HISTORY"), false);
    hudLabelMap["SettingsClearHelperTextBox"]->setText("Start over at the beginning.", false);
    hudLabelMap["SettingsClearHelperTextBox"]->layout->setLineLength(hudLabelMap["SettingsClearHelperTextBox"]->bounds.width);

    ///////////////////////////
    
    ofBuffer backersFile = ofBufferFromFile(GetCloudsDataPath() + "about/backers.txt");
    string columns[3];
    float leftPos[3];
    
    leftPos[0] = hudLabelMap["BackersList1TextBox"]->bounds.x;
    leftPos[1] = hudLabelMap["BackersList2TextBox"]->bounds.x;
    leftPos[2] = hudLabelMap["BackersList3TextBox"]->bounds.x;
    float top  = hudLabelMap["BackersList1TextBox"]->bounds.y;

    int i = 0;

    while(!backersFile.isLastLine()){
        CloudsHUDBacker b;
        b.pos.x   = leftPos[ i % 3 ];
        b.pos.y   = top + (i/3) * aboutScroller.scrollIncrement;
        b.visible = false;
        b.backer  = backersFile.getNextLine();
        backers.push_back(b);
        i++;
    }
    
    aboutScroller.totalScrollHeight = (backers.size() / 3) * aboutScroller.scrollIncrement + aboutScroller.scrollIncrement;

    hudLabelMap["BackersCopyTextBox"]->setText("Supported by The Frank-Ratchye STUDIO for Creative Inquiry, Microsoft, Eyebeam, Yamaguchi Center for Arts and Media, NYU ITP, and our faithful Kickstarter backers:", false);
    hudLabelMap["BackersCopyTextBox"]->layout->setLineLength(hudLabelMap["BackersCopyTextBox"]->bounds.width);
 
    hudLabelMap["MapTextBox"]->tab = true;
    hudLabelMap["PeopleTextBox"]->tab = true;
    hudLabelMap["VisualsTextBox"]->tab = true;
    
    hudLabelMap["NavAboutTextBox"]->tab   = true;
    hudLabelMap["NavCastTextBox"]->tab    = true;
    hudLabelMap["NavCreditsTextBox"]->tab = true;
    hudLabelMap["NavBackersTextBox"]->tab = true;
    
    //dynamic backings
    //
    setupBacking("QuestionTextBox_1_", CLOUDS_HUD_QUESTION, "QuestionBacking");
    setupBacking("BylineTopicTextBoxBottom", CLOUDS_HUD_LOWER_THIRD, "BylineTopicBackingBottom");
    setupBacking("BylineTopicTextBoxTop", CLOUDS_HUD_LOWER_THIRD, "BylineTopicBackingTop");

    setupBacking("VSCreditsTextBoxTop", CLOUDS_HUD_LOWER_THIRD, "VSCreditsBackingTop");
    setupBacking("VSCreditsTextBoxBottom", CLOUDS_HUD_LOWER_THIRD, "VSCreditsBackingBottom");

    //triangle ticks
    attachTriangleToLabel(hudLabelMap["NextButtonTextBox"], CLOUDS_HUD_NEXT, "NextButtonArrowSpace", CLOUDS_HUD_TRIANGLE_RIGHT);
    
    attachTriangleToLabel(hudLabelMap["ExploreTextBox"], CLOUDS_HUD_PAUSE, "ExploreButtonArrowSpace", CLOUDS_HUD_TRIANGLE_UP);
    attachTriangleToLabel(hudLabelMap["SeeMoreTextBox"], CLOUDS_HUD_PAUSE, "SeeMoreButtonArrowSpace", CLOUDS_HUD_TRIANGLE_UP);
    attachTriangleToLabel(hudLabelMap["ResetButtonTextBox"], CLOUDS_HUD_PAUSE, "ResetButtonSpace", CLOUDS_HUD_TRIANGLE_X);
    
    attachTriangleToLabel(hudLabelMap["RSResetButtonTextBox"], CLOUDS_RESEARCH, "RSResetButtonArrowSpace", CLOUDS_HUD_TRIANGLE_X);
    attachTriangleToLabel(hudLabelMap["ResumeButtonTextBox"], CLOUDS_RESEARCH_RESUME, "ResumeButtonArrowSpace", CLOUDS_HUD_TRIANGLE_RIGHT);
    
    attachTriangleToLabel(hudLabelMap["TopicSelectPlayTextBox"], CLOUDS_RESEARCH_TOPIC, "TopicSelectPlayArrowSpace", CLOUDS_HUD_TRIANGLE_RIGHT);
    attachTriangleToLabel(hudLabelMap["PeopleSelectPlayTextBox"], CLOUDS_RESEARCH_PPL, "PeopleSelectPlayArrowSpace", CLOUDS_HUD_TRIANGLE_RIGHT);
    
    attachTriangleToLabel(hudLabelMap["ListScrollUpArrowSpace"], CLOUDS_RESEARCH, "ListScrollUpArrowSpace", CLOUDS_HUD_TRIANGLE_UP);
    attachTriangleToLabel(hudLabelMap["ListScrollDownArrowSpace"], CLOUDS_RESEARCH, "ListScrollDownArrowSpace", CLOUDS_HUD_TRIANGLE_DOWN);
    
    attachTriangleToLabel(hudLabelMap["BackersScrollUpSpace"], CLOUDS_ABOUT_BACKERS, "BackersScrollUpSpace", CLOUDS_HUD_TRIANGLE_UP);
    attachTriangleToLabel(hudLabelMap["BackersScrollDownSpace"],CLOUDS_ABOUT_BACKERS, "BackersScrollDownSpace", CLOUDS_HUD_TRIANGLE_DOWN);

    attachTriangleToLabel(hudLabelMap["ExitButtonTextBox"], CLOUDS_ABOUT_MAIN, "ExitButtonArrowSpace", CLOUDS_HUD_TRIANGLE_NONE);
    attachTriangleToLabel(hudLabelMap["SupportButtonTextBox"], CLOUDS_ABOUT_CONTACT, "SupportButtonArrowSpace", CLOUDS_HUD_TRIANGLE_RIGHT);
    attachTriangleToLabel(hudLabelMap["GithubButtonTextBox"], CLOUDS_ABOUT_CONTACT, "GithubButtonArrowSpace", CLOUDS_HUD_TRIANGLE_RIGHT);
    
    attachTriangleToLabel(hudLabelMap["SettingsTextBoxExpanded"], CLOUDS_ABOUT_SETTINGS, "SettingsArrowFrame", CLOUDS_HUD_TRIANGLE_DOWN);
    
    if(GetGraphicsQualityLevel() == PRETTY){
        setToPretty = true;
        attachTriangleToLabel(hudLabelMap["SettingsPrettyTextBox"], CLOUDS_ABOUT_SETTINGS_EXPANDED, "SettingsPrettyCheckBox", CLOUDS_HUD_TRIANGLE_X);
        attachTriangleToLabel(hudLabelMap["SettingsFastTextBox"], CLOUDS_ABOUT_SETTINGS_EXPANDED, "SettingsFastCheckBox", CLOUDS_HUD_TRIANGLE_NONE);
    }
    else{
        setToPretty = false;
        attachTriangleToLabel(hudLabelMap["SettingsPrettyTextBox"], CLOUDS_ABOUT_SETTINGS_EXPANDED, "SettingsPrettyCheckBox", CLOUDS_HUD_TRIANGLE_NONE);
        attachTriangleToLabel(hudLabelMap["SettingsFastTextBox"], CLOUDS_ABOUT_SETTINGS_EXPANDED, "SettingsFastCheckBox", CLOUDS_HUD_TRIANGLE_X);
    }
    hudLabelMap["ExploreTextBox"]->makeArrowPositionDynamic();
    hudLabelMap["SeeMoreTextBox"]->makeArrowPositionDynamic();
    
    currentPreviewSelection = hudLabelMap["ExploreTextBox"];

    
    //hide RESET
#ifdef OCULUS_RIFT
    hudLabelMap["ResetButtonTextBox"]->setText("", false);
    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ResetButtonBacking")->visible = false;
    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ResetButtonFrame")->visible = false;
    attachTriangleToLabel(hudLabelMap["ResetButtonTextBox"], CLOUDS_HUD_PAUSE, "ResetButtonSpace", CLOUDS_HUD_TRIANGLE_NONE);
    attachTriangleToLabel(hudLabelMap["ExploreTextBox"], CLOUDS_HUD_PAUSE, "ExploreButtonArrowSpace", CLOUDS_HUD_TRIANGLE_NONE);
    attachTriangleToLabel(hudLabelMap["SeeMoreTextBox"], CLOUDS_HUD_PAUSE, "SeeMoreButtonArrowSpace", CLOUDS_HUD_TRIANGLE_NONE);
    hudLabelMap["ExploreTextBox"]->setText("", false);
    hudLabelMap["SeeMoreTextBox"]->setText("", false);
    layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreSeeMoreTitleDivide")->visible = false;
#endif
}

void CloudsHUDController::setupBacking(string labelName, CloudsHUDLayerSet layer, string backingName){
    
    hudLabelMap[labelName]->bDynamicBacking = true;
    hudLabelMap[labelName]->dynamicBackingMesh = &layers[layer]->svg.getMeshByID(backingName)->mesh;
    hudLabelMap[labelName]->dynamicBackingBounds = layers[layer]->svg.getMeshByID(backingName)->bounds;
    
    hudLabelMap[labelName]->setDynamicMargin();
}

CloudsHUDLabel* CloudsHUDController::getLabelForLayer(const string& layerName,
                                                      ofBuffer& fontBuffer,
                                                      int kerning,
                                                      bool caps,
                                                      bool useLayout,
                                                      int layoutFontSize)
{
    
    for(int i = 0; i < CLOUDS_HUD_ALL; i++ ){
        
        SVGMesh* textMesh = layers[(CloudsHUDLayerSet)i]->svg.getMeshByID( layerName );
        
        if( textMesh == NULL ){
            continue;
        }
        
        textMesh->visible = false;
        
        float maxHeight = textMesh->bounds.height;
        
        CloudsHUDLabel *newLabel = new CloudsHUDLabel();
        // make a layout
        if(useLayout){
            ofxFTGLSimpleLayout *newLayout = new ofxFTGLSimpleLayout();
            newLayout->loadFont( fontBuffer, layoutFontSize);
            newLayout->setLineLength( 999 );
            newLabel->setup( newLayout, textMesh->bounds );
        }
        else{

            int fontSize = getFontSizeForMesh( textMesh );
            if( layerName == "ListTextBoxes"){
                fontSize -= 2;
            }
            
            ofxFTGLFont *newFont = new ofxFTGLFont();
            newFont->setLetterSpacing(kerning * .08);
            newFont->loadFont( fontBuffer, fontSize );
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

int CloudsHUDController::getFontSizeForMesh( SVGMesh* textMesh ){
    if( !textMesh ){
        ofLogError("CloudsHUDController::getFontSizeForMesh") << "Text box not found";
        return 0;
    }
    
    int fontSize = 0;
    float textBoxHeight = textMesh->bounds.height;
    
    for( int k=0; k<tempFontList.size()-1; k++){
        if(tempFontList[k]->getStringBoundingBox("W",0,0).height <= textBoxHeight &&
           tempFontList[k+1]->getStringBoundingBox("W",0,0).height > textBoxHeight )
        {
            fontSize = 1 + k;
            break;
        }
    }
    //give it a boost!
#ifdef OCULUS_RIFT
    if(ofToLower(textMesh->id).find("name") == string::npos){
        fontSize += 10;
    }
    else{
        fontSize += 5;
    }
#endif
    return fontSize;
}

void CloudsHUDController::attachTriangleToLabel(CloudsHUDLabel* label,
                                                CloudsHUDLayerSet layer,
                                                string triangleLayerName,
                                                CloudsHUDTriangleDirection direction)
{
    SVGMesh* triangleMesh = layers[layer]->svg.getMeshByID( triangleLayerName );
    if(triangleMesh == NULL){
        ofLogError("CloudsHUDController::attachTriangleToLabel") << "Couldn't find traingle " << triangleLayerName << " on layer " << filenameForLayer(layer);
        return;
    }

    triangleMesh->visible = false;
    label->hasTriangle = true;
    
    ofVec2f a = triangleMesh->bounds.getTopLeft();
    ofVec2f b = triangleMesh->bounds.getTopRight();
    ofVec2f c = triangleMesh->bounds.getBottomRight();
    ofVec2f d = triangleMesh->bounds.getBottomLeft();

    ofVec2f ab = a.getInterpolated(b, .5);
    ofVec2f bc = b.getInterpolated(c, .5);
    ofVec2f cd = c.getInterpolated(d, .5);
    ofVec2f da = d.getInterpolated(a, .5);
    float width = b.x - a.x;
    label->triangleMesh.clear();
    
    switch(direction){
        case CLOUDS_HUD_TRIANGLE_UP:
            label->triangleMesh.addVertex(ab);
            label->triangleMesh.addVertex(c);
            label->triangleMesh.addVertex(d);
            label->triangleMesh.setMode(OF_PRIMITIVE_TRIANGLES);
            break;
            
        case CLOUDS_HUD_TRIANGLE_RIGHT:
            label->triangleMesh.addVertex(bc);
            label->triangleMesh.addVertex(d);
            label->triangleMesh.addVertex(a);
            label->triangleMesh.setMode(OF_PRIMITIVE_TRIANGLES);
            break;
            
        case CLOUDS_HUD_TRIANGLE_DOWN:
            label->triangleMesh.addVertex(cd);
            label->triangleMesh.addVertex(a);
            label->triangleMesh.addVertex(b);
            label->triangleMesh.setMode(OF_PRIMITIVE_TRIANGLES);
            break;
            
        case CLOUDS_HUD_TRIANGLE_LEFT:
            label->triangleMesh.addVertex(da);
            label->triangleMesh.addVertex(b);
            label->triangleMesh.addVertex(c);
            label->triangleMesh.setMode(OF_PRIMITIVE_TRIANGLES);
            break;
            
        case CLOUDS_HUD_TRIANGLE_X:
            label->triangleMesh.addVertex(a);
            label->triangleMesh.addVertex(c);
            label->triangleMesh.addVertex(b);
            label->triangleMesh.addVertex(d);
            label->triangleMesh.setMode(OF_PRIMITIVE_LINES);
            break;
            
        case CLOUDS_HUD_TRIANGLE_SQUARE:
            label->triangleMesh.addVertex(da - ofVec2f(0,width*.5));
            label->triangleMesh.addVertex(bc - ofVec2f(0,width*.5));
            label->triangleMesh.addVertex(da + ofVec2f(0,width*.5));
            
            label->triangleMesh.addVertex(bc - ofVec2f(0,width*.5));
            label->triangleMesh.addVertex(bc + ofVec2f(0,width*.5));
            label->triangleMesh.addVertex(da + ofVec2f(0,width*.5));
            label->triangleMesh.setMode(OF_PRIMITIVE_TRIANGLES);
            break;
            
        default:
            break;
    }

}

void CloudsHUDController::actBegan(CloudsActEventArgs& args){
	bDrawHud = true;
	bActJustStarted = true;
    bVisualSystemDisplayed = false;
    home.deactivate();
    hudLabelMap["VSCreditsTextBoxTop"]->setText("", false);
    hudLabelMap["VSCreditsTextBoxBottom"]->setText("", false);
}

void CloudsHUDController::actEnded(CloudsActEventArgs& args){

    clearClip();
    //clearVisualSystem();
    
    animateOff( CLOUDS_HUD_HOME );
	animateOff( CLOUDS_HUD_LOWER_THIRD );
	animateOff( CLOUDS_HUD_PROJECT_EXAMPLE );
	animateOff( CLOUDS_HUD_PAUSE );
    
    bPaused = false;
    layers[CLOUDS_HUD_LOWER_THIRD]->bForceHover = false;
    
    bVisualSystemDisplayed = false;
    bActJustStarted = false;
    bClipIsPlaying = false;
    
}
//////////TODO: these need to animate out
void CloudsHUDController::clearQuestion(){
    if(hudLabelMap["QuestionTextBox_1_"] == NULL){
        return;
    }
	hudLabelMap["QuestionTextBox_1_"]->animateOut(true);
    animateOff(CLOUDS_HUD_QUESTION);

    bQuestionDisplayed = false;
}

void CloudsHUDController::clearClip(){
    hudLabelMap["BylineFirstNameTextBox_1_"]->animateOut(true);
    hudLabelMap["BylineLastNameTextBox"]->animateOut(true);
    hudLabelMap["BylineTopicTextBoxBottom"]->animateOut(true);
    hudLabelMap["BylineTopicTextBoxTop"]->animateOut(true);
    hudLabelMap["BioTextBox"]->animateOut(true);

    bClipIsPlaying = false;
    if(!bClipIsPlaying && !bVisualSystemDisplayed){
        animateOff(CLOUDS_HUD_LOWER_THIRD);
    }
    if(bProjectExampleDisplayed){
        videoPlayer.stop();
        videoPlayer.close();
    }
}
////////////////////

void CloudsHUDController::clearVisualSystem(){

    hudLabelMap["VSCreditsTextBoxTop"]->animateOut(true);
    hudLabelMap["VSCreditsTextBoxBottom"]->animateOut(true);

    bVisualSystemDisplayed = false;

    if(!bClipIsPlaying && !bVisualSystemDisplayed){
        animateOff(CLOUDS_HUD_LOWER_THIRD);
    }
}

void CloudsHUDController::clipBegan(CloudsClipEventArgs& args){
	respondToClip(args.chosenClip);
}

void CloudsHUDController::clipEnded(){
    //TODO: make this more reliable
    //clearClip();
}

void CloudsHUDController::visualSystemBegan(CloudsVisualSystemEventArgs& args){
    respondToSystem(args.preset);
}

void CloudsHUDController::visualSystemEnded(CloudsVisualSystemEventArgs& args){

    clearVisualSystem();
    if(bActJustStarted && bClipIsPlaying){
        animateOn(CLOUDS_HUD_LOWER_THIRD);
        animateOn(CLOUDS_HUD_HOME);
        animateOn(CLOUDS_HUD_NEXT);
    }
    bActJustStarted = false;
    
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
	
    if(!bVisualSystemDisplayed){
        bActJustStarted = false;
    }
    
    currentSpeaker = CloudsSpeaker::speakers[ clip->person ];
    
    populateLowerThird(currentSpeaker.firstName, currentSpeaker.lastName, currentSpeaker.location2, currentSpeaker.title, currentSpeaker.byline2 );
    
    if(!bActJustStarted){
        if(!hudOpenMap[CLOUDS_HUD_LOWER_THIRD] ){
            animateOn( CLOUDS_HUD_LOWER_THIRD );
        }
        
        if(!hudOpenMap[CLOUDS_HUD_HOME]){
            animateOn( CLOUDS_HUD_HOME );
        }
        
        if( !hudOpenMap[CLOUDS_HUD_NEXT] ){
            animateOn( CLOUDS_HUD_NEXT );
        }
    }
    
    if(clip->hasProjectExample){
        cout << "LOADING PROJECT EXAMPLE" << endl;
    }
    //PROJECT EXAMPLE
    if(!bVisualSystemDisplayed && clip->hasProjectExample && clip->projectExample.exampleVideos.size() ){
        CloudsProjectExample example = clip->projectExample;
        string videoPath = example.exampleVideos[ (int)ofRandom(0, example.exampleVideos.size()) ];
        
        hudLabelMap["ProjectExampleTextboxLeft"]->setText( example.creatorName, false );
        //hudLabelMap["ProjectExampleTextboxRight"]->setText( textRight, forceOn );
        hudLabelMap["ProjectExampleTextBoxTop"]->setText( example.title, false );

#ifdef VHX_MEDIA
        string mediaKey = videoPath;
        TrimVHXId(mediaKey);
        if(vhxProjectExampleIds.find(mediaKey) == vhxProjectExampleIds.end()){
            ofLogError("CloudsHUDController::respondToClip") << "Couldn't find project example on VHX " << mediaKey;
        }
        else{
            if(waitingMedia != NULL){
                ofRemoveListener(waitingMedia->completeEvent, this, &CloudsHUDController::vhxRequestComplete);
            }
            
            waitingMedia = vhxProjectExampleIds[mediaKey];
            
            ofAddListener(waitingMedia->completeEvent, this, &CloudsHUDController::vhxRequestComplete);
            waitingMedia->fetchVhxSourceUrl();
        }
#else
        showProjectExample( videoPath, true );
#endif
        bProjectExampleDisplayed = true;
    }
    else{
        animateOff(CLOUDS_HUD_PROJECT_EXAMPLE);
        bProjectExampleDisplayed = false;
    }
    
    bClipIsPlaying = true;
 
}

void CloudsHUDController::respondToSystem(const CloudsVisualSystemPreset& preset){
    
    populateVisualSystem(preset.credits.title, preset.credits.line1, preset.credits.line2 );
    
    if(!bActJustStarted && !hudOpenMap[CLOUDS_HUD_LOWER_THIRD] ){
        animateOn( CLOUDS_HUD_LOWER_THIRD );
    }
    
    bVisualSystemDisplayed = true;
    bProjectExampleDisplayed = false;
    animateOff(CLOUDS_HUD_PROJECT_EXAMPLE);
}

void CloudsHUDController::questionHoverOn(const string& question,bool animate){
	populateQuestion(question, true, animate);
}

void CloudsHUDController::populateQuestion(const string& question, bool forceOn, bool animate){
    if(question == "RESUME THE STORY"){
        return;
    }
    
    if(question == ""){
        clearQuestion();
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
        bQuestionDisplayed = true;
	}
}

//BIO
void CloudsHUDController::populateLowerThird(const string& firstName,
                                             const string& lastName,
                                             const string& title,
                                             const string& location,
                                             const string& textbox)
{
 
    CloudsHUDLabel* firstNameLabel  = hudLabelMap["BylineFirstNameTextBox_1_"];
    CloudsHUDLabel* lastNameLabel  = hudLabelMap["BylineLastNameTextBox"];
    
    bool forceOn = hudOpenMap[CLOUDS_HUD_LOWER_THIRD];
    firstNameLabel->setText( firstName, forceOn );
    lastNameLabel->setText( lastName, forceOn );
    
    int rightEdge = MAX(firstNameLabel->getRightEdge(), lastNameLabel->getRightEdge());

    //move these over to float left of name
    CloudsHUDLabel* locationLabel = hudLabelMap["BylineTopicTextBoxTop"];
    CloudsHUDLabel* titleLabel = hudLabelMap["BylineTopicTextBoxBottom"];
    locationLabel->bounds.x = rightEdge + margin;
    titleLabel->bounds.x = rightEdge + margin;
    
    locationLabel->setText( location, forceOn );
    titleLabel->setText( title, forceOn );

    CloudsHUDLabel* vsTopBox  = hudLabelMap["VSCreditsTextBoxTop"];
    CloudsHUDLabel* vsBottomBox  = hudLabelMap["VSCreditsTextBoxBottom"];
 
    rightEdge = MAX(locationLabel->getRightEdge(), titleLabel->getRightEdge());
    vsTopBox->bounds.x = rightEdge + margin;
    vsBottomBox->bounds.x = rightEdge + margin;
    
    vsTopBox->updateDynamicSize();
    vsBottomBox->updateDynamicSize();
    
    hudLabelMap["BioTextBox"]->setText(textbox, false);

}

void CloudsHUDController::populateVisualSystem(const string& title,
                                               const string& creditLine1,
                                               const string& creditLine2)
{
    
    CloudsHUDLabel* topBox  = hudLabelMap["VSCreditsTextBoxTop"];
    CloudsHUDLabel* bottomBox  = hudLabelMap["VSCreditsTextBoxBottom"];

    //if the HUD is open, force the text on
    bool forceOn = hudOpenMap[CLOUDS_HUD_LOWER_THIRD];

    //no title, clear it...
    if(title == ""){
        topBox->setText("");
        bottomBox->setText("");
    }
    //split the title and bylien to 2 lines
    else if(creditLine2 == ""){
        topBox->setText("Visuals: " + title, forceOn );
        bottomBox->setText("by " + creditLine1, forceOn );
    }
    //put the byline on the top with the name
    else{
        topBox->setText("Visuals: " + title + " by " + creditLine1, forceOn );
        bottomBox->setText(creditLine2, forceOn );
    }
}

void CloudsHUDController::setResearchClickAnchor(ofVec2f anchor){
    //TODO if we want to know where the target is on screen
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
    
#ifdef OCULUS_RIFT
	float scaleToWidth  = 1920;
	float scaleToHeight = 1080;
#else
	float scaleToWidth  = ofGetWidth();
	float scaleToHeight = ofGetHeight();
#endif
    
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

    hudLabelMap["NextButtonTextBox"]->baseInteractiveBounds = layers[CLOUDS_HUD_NEXT]->svg.getMeshByID("NextButtonBacking")->bounds;
    hudLabelMap["NextButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["NextButtonTextBox"]->baseInteractiveBounds);
 	
    researchScroller.scrollUpBoundsScaled   = getScaledRectangle(researchScroller.scrollUpBounds);
    researchScroller.scrollDownBoundsScaled = getScaledRectangle(researchScroller.scrollDownBounds);
    researchScroller.scrollBoundsScaled     = getScaledRectangle(researchScroller.scrollBounds);

    aboutScroller.scrollUpBoundsScaled   = getScaledRectangle(aboutScroller.scrollUpBounds);
    aboutScroller.scrollDownBoundsScaled = getScaledRectangle(aboutScroller.scrollDownBounds);
    aboutScroller.scrollBoundsScaled     = getScaledRectangle(aboutScroller.scrollBounds);
    
    home.bAnimateHoverRadar = layers[CLOUDS_HUD_LOWER_THIRD]->isHovering();
    home.animatedHoverStartTime = layers[CLOUDS_HUD_LOWER_THIRD]->hoverStartTime;

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
    
    if( hudOpenMap[CLOUDS_HUD_PAUSE]){
        if(hudLabelMap["ExploreTextBox"]->isHovered()){
            currentPreviewImage = &topicMapPreview;
            currentPreviewSelection = hudLabelMap["ExploreTextBox"];
        }
        else if(hudLabelMap["SeeMoreTextBox"]->isHovered()){
            currentPreviewImage = &peopleMapPreview;
            currentPreviewSelection = hudLabelMap["SeeMoreTextBox"];
        }
        
    }
    
    if( hudOpenMap[CLOUDS_RESEARCH] ){
        updateScroll();
        updateResearchNavigation();
    }
    
    if( hudOpenMap[CLOUDS_ABOUT_MAIN] ){
        aboutScroller.update();
//        cout << "scroll pos " << aboutScroller.scrollPosition << endl;
        updateAboutNavigation();
    }
    
    ///////////////////////////////

}

void CloudsHUDController::updateScroll(){
    
    researchScroller.update();
    currentResearchList->scrollPosition = researchScroller.scrollPosition;
    
    for(int i = 0; i < currentResearchList->buttons.size(); i++){
        CloudsHUDResearchButton& b = currentResearchList->buttons[i];
        b.visible = b.top >= currentResearchList->scrollPosition &&
                    b.top <= currentResearchList->scrollPosition + researchScroller.scrollBounds.height - researchScroller.scrollIncrement;
        
        if(b.visible){
            b.selectRect = ofRectangle(researchScroller.scrollBounds.x,
                                       researchScroller.scrollBounds.y + b.top - currentResearchList->scrollPosition,
                                       researchScroller.scrollBounds.width, 15);
            
            b.selectRectScaled = getScaledRectangle( b.selectRect );
        }
    }
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
    if(currentTab == CLOUDS_HUD_RESEARCH_TAB_TOPICS){
        return hudLabelMap["TopicSelectTextBox"]->isClicked();
    }
    else if(currentTab == CLOUDS_HUD_RESEARCH_TAB_PEOPLE){
        return hudLabelMap["PeopleSelectPlayTextBox"]->isClicked();
    }
    else if(currentTab == CLOUDS_HUD_RESEARCH_TAB_VISUALS){
        return false; // TODO;
    }
    
    return false;
}

bool CloudsHUDController::didItemSelectionChange(){
    bool ret = bItemSelectionChanged;
    bItemSelectionChanged = false;
    return ret;
}

void CloudsHUDController::clearSelection(){
    animateOff(CLOUDS_RESEARCH_TOPIC);
    animateOff(CLOUDS_RESEARCH_PPL);
    animateOff(CLOUDS_RESEARCH_VS);
}

void CloudsHUDController::updateResearchNavigation(){

    
    //set the interaction regions
    hudLabelMap["MapTextBox"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("MapHoverBacking")->bounds;
    hudLabelMap["MapTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["MapTextBox"]->baseInteractiveBounds);
    
    hudLabelMap["PeopleTextBox"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("PeopleHoverBacking")->bounds;
    hudLabelMap["PeopleTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["PeopleTextBox"]->baseInteractiveBounds);
    
    hudLabelMap["VisualsTextBox"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("VSHoverBacking")->bounds;
    hudLabelMap["VisualsTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["VisualsTextBox"]->baseInteractiveBounds);
    
    hudLabelMap["RSResetButtonTextBox"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("RSResetButtonBacking")->bounds;
    hudLabelMap["RSResetButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["RSResetButtonTextBox"]->baseInteractiveBounds);

    hudLabelMap["ListScrollUpArrowSpace"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("ListScrollUpBacking")->bounds;
    hudLabelMap["ListScrollUpArrowSpace"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ListScrollUpArrowSpace"]->baseInteractiveBounds);
    
    hudLabelMap["ListScrollDownArrowSpace"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH]->svg.getMeshByID("ListScrollDownBacking")->bounds;
    hudLabelMap["ListScrollDownArrowSpace"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ListScrollDownArrowSpace"]->baseInteractiveBounds);
    
    hudLabelMap["ResumeButtonTextBox"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH_RESUME]->svg.getMeshByID("ResumeButtonBacking")->bounds;
    hudLabelMap["ResumeButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ResumeButtonTextBox"]->baseInteractiveBounds);

    hudLabelMap["TopicSelectTextBox"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH_TOPIC]->svg.getMeshByID("TopicSelectBacking")->bounds;
    hudLabelMap["TopicSelectTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["TopicSelectTextBox"]->baseInteractiveBounds);

    hudLabelMap["PeopleSelectPlayTextBox"]->baseInteractiveBounds = layers[CLOUDS_RESEARCH_PPL]->svg.getMeshByID("PeopleSelectBacking")->bounds;
    hudLabelMap["PeopleSelectPlayTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["PeopleSelectPlayTextBox"]->baseInteractiveBounds);
    
    
    hudLabelMap["MapTextBox"]->tabSelected = currentTab == CLOUDS_HUD_RESEARCH_TAB_TOPICS;
    hudLabelMap["PeopleTextBox"]->tabSelected = currentTab == CLOUDS_HUD_RESEARCH_TAB_PEOPLE;
    hudLabelMap["VisualsTextBox"]->tabSelected = currentTab == CLOUDS_HUD_RESEARCH_TAB_VISUALS;
}

void CloudsHUDController::showAbout(){

    animateOn(CLOUDS_ABOUT_MAIN);
    animateOn(CLOUDS_ABOUT_SETTINGS);
    animateOn(CLOUDS_ABOUT_CONTACT);
    
    //TOOD: maybe save the last tab ...
    currentAboutTab = CLOUDS_HUD_ABOUT_TAB_INFO;
    animateOn(CLOUDS_ABOUT_INFO);
    animateOff(CLOUDS_ABOUT_CAST);
    animateOff(CLOUDS_ABOUT_CREDITS);
    animateOff(CLOUDS_ABOUT_BACKERS);
}

void CloudsHUDController::hideAbout(){
    animateOff(CLOUDS_ABOUT_MAIN);
    animateOff(CLOUDS_ABOUT_SETTINGS);
    animateOff(CLOUDS_ABOUT_CONTACT);
    
    animateOff(CLOUDS_ABOUT_INFO);
    animateOff(CLOUDS_ABOUT_CAST);
    animateOff(CLOUDS_ABOUT_CREDITS);
    animateOff(CLOUDS_ABOUT_BACKERS);
    animateOff(CLOUDS_ABOUT_SETTINGS);
    animateOff(CLOUDS_ABOUT_SETTINGS_EXPANDED);
}

bool CloudsHUDController::visualLevelDidChange(){
    bool ret = visualLevelChanged;
    visualLevelChanged = false;
    return ret;
}

bool CloudsHUDController::requestClearHistory(){
    bool ret = historyCleared;
    historyCleared = false;
    return ret;
}

void CloudsHUDController::setHasHistory(bool hasHistory){
    if(hasHistory){
        attachTriangleToLabel(hudLabelMap["SettingsClearHistoryTextBox"], CLOUDS_ABOUT_SETTINGS_EXPANDED,
                              "SettingsClearHistoryIndicatorBox", CLOUDS_HUD_TRIANGLE_SQUARE);
    }
    else{
        attachTriangleToLabel(hudLabelMap["SettingsClearHistoryTextBox"], CLOUDS_ABOUT_SETTINGS_EXPANDED,
                              "SettingsClearHistoryIndicatorBox", CLOUDS_HUD_TRIANGLE_NONE);
        
    }
}

void CloudsHUDController::updateAboutNavigation(){
    
    if(hudLabelMap["NavAboutTextBox"]->isClicked()){
        currentAboutTab = CLOUDS_HUD_ABOUT_TAB_INFO;
        animateOn(CLOUDS_ABOUT_INFO);
        animateOff(CLOUDS_ABOUT_CAST);
        animateOff(CLOUDS_ABOUT_CREDITS);
        animateOff(CLOUDS_ABOUT_BACKERS);
    }
    else if(hudLabelMap["NavCastTextBox"]->isClicked()){
        animateOff(CLOUDS_ABOUT_INFO);
        animateOn(CLOUDS_ABOUT_CAST);
        animateOff(CLOUDS_ABOUT_CREDITS);
        animateOff(CLOUDS_ABOUT_BACKERS);
        currentAboutTab = CLOUDS_HUD_ABOUT_TAB_CAST;
    }
    else if(hudLabelMap["NavCreditsTextBox"]->isClicked()){
        animateOff(CLOUDS_ABOUT_INFO);
        animateOff(CLOUDS_ABOUT_CAST);
        animateOn(CLOUDS_ABOUT_CREDITS);
        animateOff(CLOUDS_ABOUT_BACKERS);
        currentAboutTab = CLOUDS_HUD_ABOUT_TAB_CREDITS;
    }
    else if(hudLabelMap["NavBackersTextBox"]->isClicked()){
        animateOff(CLOUDS_ABOUT_INFO);
        animateOff(CLOUDS_ABOUT_CAST);
        animateOff(CLOUDS_ABOUT_CREDITS);
        animateOn(CLOUDS_ABOUT_BACKERS);
        currentAboutTab = CLOUDS_HUD_ABOUT_TAB_BACKERS;
    }
    else if(hudLabelMap["SupportButtonTextBox"]->isClicked()){
        ofLaunchBrowser("mailto:support@cloudsdocumentary.com");
    }
    else if(hudLabelMap["GithubButtonTextBox"]->isClicked()){
        ofLaunchBrowser("https://github.com/CLOUDS-Interactive-Documentary/CLOUDS");
    }
    
    if(hudLabelMap["SettingsTextBoxExpanded"]->isClicked()){
        if(hudOpenMap[CLOUDS_ABOUT_SETTINGS_EXPANDED]){
            animateOff(CLOUDS_ABOUT_SETTINGS_EXPANDED);
            attachTriangleToLabel(hudLabelMap["SettingsTextBoxExpanded"], CLOUDS_ABOUT_SETTINGS, "SettingsArrowFrame", CLOUDS_HUD_TRIANGLE_DOWN);
        }
        else{
            animateOn(CLOUDS_ABOUT_SETTINGS_EXPANDED);
            attachTriangleToLabel(hudLabelMap["SettingsTextBoxExpanded"], CLOUDS_ABOUT_SETTINGS, "SettingsArrowFrame", CLOUDS_HUD_TRIANGLE_UP);
        }
    }
    
    if(hudOpenMap[CLOUDS_ABOUT_SETTINGS_EXPANDED]){
        if(hudLabelMap["SettingsPrettyTextBox"]->isClicked() && !setToPretty){
            attachTriangleToLabel(hudLabelMap["SettingsPrettyTextBox"], CLOUDS_ABOUT_SETTINGS_EXPANDED, "SettingsPrettyCheckBox", CLOUDS_HUD_TRIANGLE_X);
            attachTriangleToLabel(hudLabelMap["SettingsFastTextBox"], CLOUDS_ABOUT_SETTINGS_EXPANDED, "SettingsFastCheckBox", CLOUDS_HUD_TRIANGLE_NONE);
            setToPretty = true;
            visualLevelChanged = true;
        }
        if(hudLabelMap["SettingsFastTextBox"]->isClicked() && setToPretty){
            attachTriangleToLabel(hudLabelMap["SettingsPrettyTextBox"], CLOUDS_ABOUT_SETTINGS_EXPANDED, "SettingsPrettyCheckBox", CLOUDS_HUD_TRIANGLE_NONE);
            attachTriangleToLabel(hudLabelMap["SettingsFastTextBox"], CLOUDS_ABOUT_SETTINGS_EXPANDED, "SettingsFastCheckBox", CLOUDS_HUD_TRIANGLE_X);
            setToPretty = false;
            visualLevelChanged = true;
        }
        if(hudLabelMap["SettingsClearHistoryTextBox"]->isClicked()){
            setHasHistory(false);
            historyCleared = true;
        }
    }
    hudLabelMap["NavAboutTextBox"]->baseInteractiveBounds   = layers[CLOUDS_ABOUT_MAIN]->svg.getMeshByID("AboutHoverBacking")->bounds;
    hudLabelMap["NavAboutTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["NavAboutTextBox"]->baseInteractiveBounds);

    hudLabelMap["NavCastTextBox"]->baseInteractiveBounds   = layers[CLOUDS_ABOUT_MAIN]->svg.getMeshByID("NavCastHoverBacking")->bounds;
    hudLabelMap["NavCastTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["NavCastTextBox"]->baseInteractiveBounds);

    hudLabelMap["NavCreditsTextBox"]->baseInteractiveBounds   = layers[CLOUDS_ABOUT_MAIN]->svg.getMeshByID("NavCreditsHoverBacking")->bounds;
    hudLabelMap["NavCreditsTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["NavCreditsTextBox"]->baseInteractiveBounds);

    hudLabelMap["NavBackersTextBox"]->baseInteractiveBounds   = layers[CLOUDS_ABOUT_MAIN]->svg.getMeshByID("NavBackersHoverBacking")->bounds;
    hudLabelMap["NavBackersTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["NavBackersTextBox"]->baseInteractiveBounds);
    
    hudLabelMap["ExitButtonTextBox"]->baseInteractiveBounds   = layers[CLOUDS_ABOUT_MAIN]->svg.getMeshByID("ExitButtonBacking")->bounds;
    hudLabelMap["ExitButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ExitButtonTextBox"]->baseInteractiveBounds);

    hudLabelMap["BackersScrollUpSpace"]->baseInteractiveBounds   = layers[CLOUDS_ABOUT_BACKERS]->svg.getMeshByID("BackersScrollUpBacking")->bounds;
    hudLabelMap["BackersScrollUpSpace"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["BackersScrollUpSpace"]->baseInteractiveBounds);

    hudLabelMap["BackersScrollDownSpace"]->baseInteractiveBounds   = layers[CLOUDS_ABOUT_BACKERS]->svg.getMeshByID("BackersScrollDownBacking")->bounds;
    hudLabelMap["BackersScrollDownSpace"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["BackersScrollDownSpace"]->baseInteractiveBounds);
    
    hudLabelMap["SettingsTextBoxExpanded"]->baseInteractiveBounds   = layers[CLOUDS_ABOUT_SETTINGS]->svg.getMeshByID("SettingsBackingExpanded")->bounds;
    hudLabelMap["SettingsTextBoxExpanded"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["SettingsTextBoxExpanded"]->baseInteractiveBounds);

    hudLabelMap["SettingsTextBoxExpanded"]->baseInteractiveBounds   = layers[CLOUDS_ABOUT_SETTINGS]->svg.getMeshByID("SettingsBackingExpanded")->bounds;
    hudLabelMap["SettingsTextBoxExpanded"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["SettingsTextBoxExpanded"]->baseInteractiveBounds);

    hudLabelMap["SupportButtonTextBox"]->baseInteractiveBounds   = layers[CLOUDS_ABOUT_CONTACT]->svg.getMeshByID("SupportButtonBacking")->bounds;
    hudLabelMap["SupportButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["SupportButtonTextBox"]->baseInteractiveBounds);
    
    hudLabelMap["GithubButtonTextBox"]->baseInteractiveBounds   = layers[CLOUDS_ABOUT_CONTACT]->svg.getMeshByID("GithubButtonBacking")->bounds;
    hudLabelMap["GithubButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["GithubButtonTextBox"]->baseInteractiveBounds);
    
    if(hudOpenMap[CLOUDS_ABOUT_SETTINGS_EXPANDED]){
        hudLabelMap["SettingsFastTextBox"]->baseInteractiveBounds
                    = layers[CLOUDS_ABOUT_SETTINGS_EXPANDED]->svg.getMeshByID("SettingsFastBacking")->bounds;
        hudLabelMap["SettingsFastTextBox"]->scaledInteractiveBounds
                    = getScaledRectangle(hudLabelMap["SettingsFastTextBox"]->baseInteractiveBounds);

        hudLabelMap["SettingsPrettyTextBox"]->baseInteractiveBounds
                    = layers[CLOUDS_ABOUT_SETTINGS_EXPANDED]->svg.getMeshByID("SettingsPrettyBacking")->bounds;
        hudLabelMap["SettingsPrettyTextBox"]->scaledInteractiveBounds
                    = getScaledRectangle(hudLabelMap["SettingsPrettyTextBox"]->baseInteractiveBounds);

        hudLabelMap["SettingsClearHistoryTextBox"]->baseInteractiveBounds
                    = layers[CLOUDS_ABOUT_SETTINGS_EXPANDED]->svg.getMeshByID("SettingsClearBacking")->bounds;
        hudLabelMap["SettingsClearHistoryTextBox"]->scaledInteractiveBounds
                    = getScaledRectangle(hudLabelMap["SettingsClearHistoryTextBox"]->baseInteractiveBounds);
    }
    
    hudLabelMap["NavAboutTextBox"]->tabSelected  = currentAboutTab  == CLOUDS_HUD_ABOUT_TAB_INFO;
    hudLabelMap["NavCastTextBox"]->tabSelected   = currentAboutTab  == CLOUDS_HUD_ABOUT_TAB_CAST;
    hudLabelMap["NavCreditsTextBox"]->tabSelected = currentAboutTab == CLOUDS_HUD_ABOUT_TAB_CREDITS;
    hudLabelMap["NavBackersTextBox"]->tabSelected = currentAboutTab == CLOUDS_HUD_ABOUT_TAB_BACKERS;
    
    for(int i = 0; i < backers.size(); i++){
        backers[i].visible = backers[i].pos.y >= aboutScroller.scrollBounds.y + aboutScroller.scrollPosition &&
                             backers[i].pos.y <= aboutScroller.scrollBounds.y + aboutScroller.scrollPosition + aboutScroller.scrollBounds.height;
    }
    
}

void CloudsHUDController::pause(){
    if(bPaused){
        return;
    }
    bPaused = true;
    
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
    
    //set the interaction regions
    hudLabelMap["ResetButtonTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ResetButtonTextBox"]->baseInteractiveBounds);
    hudLabelMap["ExploreTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["ExploreTextBox"]->baseInteractiveBounds);
    hudLabelMap["SeeMoreTextBox"]->scaledInteractiveBounds = getScaledRectangle(hudLabelMap["SeeMoreTextBox"]->baseInteractiveBounds);
 
#ifndef OCULUS_RIFT
    animateOff( CLOUDS_HUD_QUESTION );
#endif
    animateOff( CLOUDS_HUD_NEXT );
    if(bProjectExampleDisplayed){
        animateOff( CLOUDS_HUD_PROJECT_EXAMPLE );
        isPlaying = true;
    }

    home.forceActive();
    animateOn( CLOUDS_HUD_HOME );
    animateOn( CLOUDS_HUD_PAUSE );
    animateOn( CLOUDS_HUD_LOWER_THIRD );
    layers[CLOUDS_HUD_LOWER_THIRD]->bForceHover = true;
    
    bJustPaused = true;
    bJustUnpaused = false;

}

void CloudsHUDController::unpause(bool force){
    if( !force && !bPaused ){
        return;
    }
    bPaused = false;

    animateOff( CLOUDS_HUD_PAUSE );
    if(bQuestionDisplayed){
        animateOn( CLOUDS_HUD_QUESTION );
    }
    if(bProjectExampleDisplayed){
        animateOn( CLOUDS_HUD_PROJECT_EXAMPLE );
    }
    
    animateOn(CLOUDS_HUD_HOME);
    if(bClipIsPlaying || bVisualSystemDisplayed){
        animateOn(CLOUDS_HUD_LOWER_THIRD);
    }
    
    layers[CLOUDS_HUD_LOWER_THIRD]->bForceHover = false;
    
    animateOn( CLOUDS_HUD_NEXT );

    bJustUnpaused = true;
    bJustPaused = false;
    
}

void CloudsHUDController::enteringVisuals(){
    hudLabelMap["NextButtonTextBox"]->setText(GetTranslationForString("STOP"), false);
    attachTriangleToLabel(hudLabelMap["NextButtonTextBox"], CLOUDS_HUD_NEXT, "NextButtonArrowSpace", CLOUDS_HUD_TRIANGLE_SQUARE);
}

void CloudsHUDController::exitingVisuals(){
    hudLabelMap["NextButtonTextBox"]->setText(GetTranslationForString("NEXT"), false);
    attachTriangleToLabel(hudLabelMap["NextButtonTextBox"], CLOUDS_HUD_NEXT, "NextButtonArrowSpace", CLOUDS_HUD_TRIANGLE_RIGHT);
}

void CloudsHUDController::setTopics(const set<string>& topics){
    CloudsHUDResearchList& topicList = researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS];
    topicList.buttons.clear();
    topicList.buttons.resize(topics.size());
    
    int i = 0;
    for(set<string>::iterator it = topics.begin(); it != topics.end(); it++){
        topicList.buttons[i].tag = *it;
        topicList.buttons[i].label = ofToUpper(*it);
        topicList.buttons[i].parentTab = CLOUDS_HUD_RESEARCH_TAB_TOPICS;
        i++;
    }
    
    sort(topicList.buttons.begin(), topicList.buttons.end(), listSort);
    
    for(i = 0; i < topicList.buttons.size(); i++){
        topicList.buttons[i].top = i * researchScroller.scrollIncrement;
    }


    topicList.totalScrollHeight = topicList.buttons.back().top + researchScroller.scrollIncrement;
    topicList.scrollPosition = 0;
}

void CloudsHUDController::populateSpeakers(){
    CloudsHUDResearchList& peopleList = researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE];
    peopleList.buttons.clear();

    int i = 0;
    for(map<string,CloudsSpeaker>::iterator it = CloudsSpeaker::speakers.begin(); it != CloudsSpeaker::speakers.end(); it++){
        if(it->second.voiceOverOnly){
            continue;
        }
        peopleList.buttons.push_back(CloudsHUDResearchButton());
        peopleList.buttons[i].tag = it->first;
        peopleList.buttons[i].label = it->second.firstName + " " + it->second.lastName;
        peopleList.buttons[i].parentTab = CLOUDS_HUD_RESEARCH_TAB_PEOPLE;
        i++;
    }
    
    sort(peopleList.buttons.begin(), peopleList.buttons.end(), listSort);
    
    for(i = 0; i < peopleList.buttons.size(); i++){
        peopleList.buttons[i].top = i * researchScroller.scrollIncrement;
    }
    
    peopleList.totalScrollHeight = peopleList.buttons.back().top + researchScroller.scrollIncrement;
    peopleList.scrollPosition = 0;
}

void CloudsHUDController::setVisuals(map<string, CloudsVisualSystemCredit>& visuals){
    CloudsHUDResearchList& visualsList = researchLists[CLOUDS_HUD_RESEARCH_TAB_VISUALS];
    visualsList.buttons.clear();
    visualsList.buttons.resize(visuals.size());

    int i = 0;
    for(map<string, CloudsVisualSystemCredit>::iterator it = visuals.begin(); it != visuals.end(); it++){
        visualsList.buttons[i].tag = it->first;
        visualsList.buttons[i].label = ofToUpper(it->second.title);
        visualsList.buttons[i].parentTab = CLOUDS_HUD_RESEARCH_TAB_VISUALS;
        i++;
    }
    
    sort(visualsList.buttons.begin(), visualsList.buttons.end(), listSort);
    
    for(i = 0; i < visualsList.buttons.size(); i++){
        visualsList.buttons[i].top = i * researchScroller.scrollIncrement;
    }
    visualsList.totalScrollHeight = visualsList.buttons.back().top + researchScroller.scrollIncrement;
    visualsList.scrollPosition = 0;
}

void CloudsHUDController::setVisitedTopics(set<string> topics){
    updateVisited(researchLists[CLOUDS_HUD_RESEARCH_TAB_TOPICS], topics);
}
void CloudsHUDController::setVisitedPeople(set<string> people){
    updateVisited(researchLists[CLOUDS_HUD_RESEARCH_TAB_PEOPLE], people);
}
void CloudsHUDController::setVisitedVisuals(set<string> visuals){
    updateVisited(researchLists[CLOUDS_HUD_RESEARCH_TAB_VISUALS], visuals);
}
void CloudsHUDController::updateVisited(CloudsHUDResearchList& list, set<string> s){
    for(int i = 0; i < list.buttons.size(); i++){
        list.buttons[i].visited = s.find(list.buttons[i].tag) != s.end();
    }
}

void CloudsHUDController::mouseMoved(ofMouseEventArgs& args){
    args.canceled = false;
    
    for (map<string, CloudsHUDLabel*>::iterator it = hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        args.canceled |= it->second->mouseMoved(ofVec2f(args.x,args.y));
    }
    
    for(map<CloudsHUDLayerSet, CloudsHUDLayer*>::iterator it = layers.begin(); it != layers.end(); it++){
        if(it->second->isOpen()){
            if(getScaledRectangle( it->second->svg.getBounds()).inside(args.x,args.y)){
                it->second->hoverOn();
            }
            else {
                it->second->hoverOff();
            }
        }
    }
 
    if(hudOpenMap[CLOUDS_HUD_PAUSE] &&
       getScaledRectangle( layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreSeeMoreVisualBacking")->bounds).inside(args.x,args.y) )
    {
        currentPreviewSelection->forceHover();
    }
    else{
        currentPreviewSelection->unforceHover();
    }
    
    if(hudOpenMap[CLOUDS_RESEARCH]){
        
        args.canceled |= researchScroller.mouseMoved(ofVec2f(args.x,args.y));
        
        if(researchScroller.scrollBoundsScaled.inside(args.x, args.y)){
            args.canceled = true;
            for(int i = 0; i < currentResearchList->buttons.size(); i++){
                if(currentResearchList->buttons[i].visible){
                    currentResearchList->buttons[i].hovered = currentResearchList->buttons[i].selectRectScaled.inside(args.x, args.y);
                }
            }
        }
        else{
            for(int i = 0; i < currentResearchList->buttons.size(); i++){
                currentResearchList->buttons[i].hovered = false;
            }
        }
    }
    
    if(hudOpenMap[CLOUDS_ABOUT_BACKERS]){
        aboutScroller.mouseMoved(ofVec2f(args.x,args.y));
    }
}

void CloudsHUDController::mousePressed(ofMouseEventArgs& args){

    args.canceled = false;

    for(map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        args.canceled |= (it->second)->mousePressed(ofVec2f(args.x,args.y));
    }

    if(( hudOpenMap[CLOUDS_HUD_HOME] && home.hitTest(args.x, args.y) ) ||
       ( hudOpenMap[CLOUDS_HUD_LOWER_THIRD] && getScaledRectangle( layers[CLOUDS_HUD_LOWER_THIRD]->svg.getBounds()).inside(args.x, args.y)) )
    {
        home.activate();
    }
    
    if(hudOpenMap[CLOUDS_HUD_PAUSE] &&
       getScaledRectangle( layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreSeeMoreVisualBacking")->bounds).inside(args.x,args.y) )
    {
        currentPreviewSelection->forcePress();
    }
    else{
        currentPreviewSelection->unforcePress();
    }
    
    if(hudOpenMap[CLOUDS_RESEARCH]){
        
        args.canceled |= researchScroller.mousePressed(ofVec2f(args.x,args.y));
        
        if(researchScroller.scrollBoundsScaled.inside(args.x, args.y)){
            args.canceled = true;
            for(int i = 0; i < currentResearchList->buttons.size(); i++){
                currentResearchList->buttons[i].clicked = false;
                if(currentResearchList->buttons[i].visible){
                    currentResearchList->buttons[i].pressed = currentResearchList->buttons[i].selectRectScaled.inside(args.x, args.y);
                }
            }
        }
    }
    
    if(hudOpenMap[CLOUDS_ABOUT_BACKERS]){
        aboutScroller.mousePressed(ofVec2f(args.x,args.y));
    }

}

void CloudsHUDController::mouseReleased(ofMouseEventArgs& args){

    args.canceled = false;

    if(bActJustStarted && !hudOpenMap[CLOUDS_HUD_HOME] && (bVisualSystemDisplayed || bClipIsPlaying)) {
        bActJustStarted = false;
        animateOn(CLOUDS_HUD_LOWER_THIRD);
        animateOn(CLOUDS_HUD_HOME);
        animateOn(CLOUDS_HUD_NEXT);
    }
 
    for (map<string, CloudsHUDLabel*>::iterator it=hudLabelMap.begin(); it!= hudLabelMap.end(); ++it){
        args.canceled |= (it->second)->mouseReleased(ofVec2f(args.x,args.y));
    }
    
    if(hudOpenMap[CLOUDS_RESEARCH]){
        
        researchScroller.mouseReleased(ofVec2f(args.x,args.y));
        
        if(researchScroller.scrollBoundsScaled.inside(args.x, args.y)){
            
            args.canceled = true;
            
            for(int i = 0; i < currentResearchList->buttons.size(); i++){
                if(currentResearchList->buttons[i].visible){
                    currentResearchList->buttons[i].clicked = currentResearchList->buttons[i].pressed &&
                                                              currentResearchList->buttons[i].selectRectScaled.inside(args.x, args.y);
                    currentResearchList->buttons[i].pressed = false;
                    if(currentResearchList->buttons[i].clicked){
                        selectButton(currentResearchList->buttons[i]);
                        bItemSelectionChanged = true;
                    }
                }
            }
        }
    }
    
    if(hudOpenMap[CLOUDS_HUD_PAUSE]){
        if(getScaledRectangle( layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreSeeMoreVisualBacking")->bounds).inside(args.x,args.y) ){
            currentPreviewSelection->forceClick();
        }
    }
    
    if(hudOpenMap[CLOUDS_ABOUT_BACKERS]){
        aboutScroller.mouseReleased(ofVec2f(args.x,args.y));
    }

}

void CloudsHUDController::mouseScrolled(ofMouseEventArgs& args){

    if(hudOpenMap[CLOUDS_RESEARCH]){
        researchScroller.mouseScrolled(args.y);
    }
    if(hudOpenMap[CLOUDS_ABOUT_BACKERS]){
        aboutScroller.mouseScrolled(args.y);
    }
}


void CloudsHUDController::unselectButtons(){
    animateOff(CLOUDS_RESEARCH_PPL);
    animateOff(CLOUDS_RESEARCH_TOPIC);
}

void CloudsHUDController::selectButton(const CloudsHUDResearchButton& button){
    if(button.parentTab == CLOUDS_HUD_RESEARCH_TAB_TOPICS){
        bool forceOn = hudOpenMap[CLOUDS_RESEARCH_TOPIC];
        hudLabelMap["TopicSelectTextBox"]->setText(button.label, forceOn);
        if(currentTab == button.parentTab && !bResearchTransitioning){
            animateOn(CLOUDS_RESEARCH_TOPIC);
        }
    }
    else if(button.parentTab == CLOUDS_HUD_RESEARCH_TAB_PEOPLE){
        bool forceOn = hudOpenMap[CLOUDS_RESEARCH_PPL];
        hudLabelMap["PeopleSelectNameTextBox"]->setText(CloudsSpeaker::speakers[button.tag].firstName + " " +
                                                        CloudsSpeaker::speakers[button.tag].lastName, forceOn);
        hudLabelMap["PeopleSelectBylineTextBox"]->setText(CloudsSpeaker::speakers[button.tag].byline1, forceOn);

        //resize the bottom box
        float stringHeight = hudLabelMap["PeopleSelectBylineTextBox"]->layout->stringHeight(CloudsSpeaker::speakers[button.tag].byline1);
        SVGMesh* frameMesh = layers[CLOUDS_RESEARCH_PPL]->svg.getMeshByID("PeopleSelectFrame");
        SVGMesh* backingMesh = layers[CLOUDS_RESEARCH_PPL]->svg.getMeshByID("PeopleSelectBacking");
        
        float frameMargin = hudLabelMap["PeopleSelectNameTextBox"]->bounds.y - frameMesh->bounds.y;
        float newFrameY = hudLabelMap["PeopleSelectBylineTextBox"]->bounds.y + stringHeight + frameMargin;
        frameMesh->mesh.getVertices()[3].y = newFrameY;
        frameMesh->mesh.getVertices()[4].y = newFrameY;
        frameMesh->mesh.getVertices()[5].y = newFrameY;
        frameMesh->mesh.getVertices()[6].y = newFrameY;
        
        backingMesh->mesh.getVertices()[2].y = newFrameY;
        backingMesh->mesh.getVertices()[4].y = newFrameY;
        backingMesh->mesh.getVertices()[5].y = newFrameY;
        backingMesh->bounds.height = newFrameY - backingMesh->bounds.y;
        
        if(currentTab == button.parentTab && !bResearchTransitioning){
            animateOn(CLOUDS_RESEARCH_PPL);
        }
    }
    else if(button.parentTab == CLOUDS_HUD_RESEARCH_TAB_VISUALS){
        //taken care of in the vidual system
    }
}

bool CloudsHUDController::isResetHit(){
    return hudLabelMap["ResetButtonTextBox"]->isClicked();
}

bool CloudsHUDController::isResearchResetHit(){
    return hudLabelMap["RSResetButtonTextBox"]->isClicked();
}

bool CloudsHUDController::isNextHit(){
    return hudLabelMap["NextButtonTextBox"]->isClicked();
}

bool CloudsHUDController::isResumeActHit(){
    return hudLabelMap["ResumeButtonTextBox"]->isClicked();
}

void CloudsHUDController::setSeeMoreName(string name){
#ifndef OCULUS_RIFT
    hudLabelMap["SeeMoreTextBox"]->setText(name, false);
#endif
}

void CloudsHUDController::selectTopic(string topic){
    selectItem(CLOUDS_HUD_RESEARCH_TAB_TOPICS, topic);
}

void CloudsHUDController::selectPerson(string personID){
    selectItem(CLOUDS_HUD_RESEARCH_TAB_PEOPLE, personID);
}

void CloudsHUDController::selectVisual(string visualName){
    selectItem(CLOUDS_HUD_RESEARCH_TAB_VISUALS, visualName);
}

void CloudsHUDController::selectItem(CloudsHUDResearchTab tab, string itemID){
    CloudsHUDResearchList& list = researchLists[tab];
    for(int i = 0; i < list.buttons.size(); i++){
        list.buttons[i].clicked = false;
        list.buttons[i].hovered = false;
    }

    //select top item
    if(itemID == ""){
//        list.scrollPosition = MIN(list.buttons[0].top, list.totalScrollHeight - researchScroller.scrollBounds.height);
//        selectButton(list.buttons[0]);
        unselectButtons();
        return;
    }

    for(int i = 0; i < list.buttons.size(); i++){
        if(list.buttons[i].tag == itemID){
            list.buttons[i].clicked = true;
            if(!list.buttons[i].visible){
                researchScroller.scrollPosition = list.scrollPosition =
                    MIN(list.buttons[i].top, list.totalScrollHeight - researchScroller.scrollBounds.height);
            }
            selectButton(list.buttons[i]);
            return;
        }
    }
    ofLogError("CloudsHUDController::selectItem") << "Didn't find selected name " << itemID;
}

bool CloudsHUDController::isExploreMapHit(){
    bool selected = hudLabelMap["ExploreTextBox"]->isClicked();
    if(selected) {
        bResearchTransitioning = true;
        nextTab = CLOUDS_HUD_RESEARCH_TAB_TOPICS;
    }
    return selected;
}

bool CloudsHUDController::isSeeMorePersonHit(){
    bool selected = hudLabelMap["SeeMoreTextBox"]->isClicked();
    if(selected) {
        bResearchTransitioning = true;
        nextTab = CLOUDS_HUD_RESEARCH_TAB_PEOPLE;        
    }
    return selected;
}

void CloudsHUDController::preselectMap(){
    bResearchTransitioning = true;
    nextTab = CLOUDS_HUD_RESEARCH_TAB_TOPICS;
}

bool CloudsHUDController::selectedMapTab(){

    if(currentTab == CLOUDS_HUD_RESEARCH_TAB_TOPICS || bResearchTransitioning) return false;
    
    bool selected = hudLabelMap["MapTextBox"]->isClicked();
    if(selected) {
        clearSelection();
        bResearchTransitioning = true;
        nextTab = CLOUDS_HUD_RESEARCH_TAB_TOPICS;
    }
    return selected;
}

bool CloudsHUDController::selectedPeopleTab(){

    if(currentTab == CLOUDS_HUD_RESEARCH_TAB_PEOPLE || bResearchTransitioning) return false;
 
    bool selected = hudLabelMap["PeopleTextBox"]->isClicked();
    if(selected) {
        clearSelection();
        bResearchTransitioning = true;
        nextTab = CLOUDS_HUD_RESEARCH_TAB_PEOPLE;
    }
    return selected;
}

bool CloudsHUDController::selectedVisualsTab(){
    
    if(currentTab == CLOUDS_HUD_RESEARCH_TAB_VISUALS || bResearchTransitioning) return false;
    
    bool selected = hudLabelMap["VisualsTextBox"]->isClicked();
    if(selected) {
        clearSelection();
        bResearchTransitioning = true;
        nextTab = CLOUDS_HUD_RESEARCH_TAB_VISUALS;
    }
    return selected;
}

void CloudsHUDController::researchTransitionFinished(){
    if(bResearchTransitioning){
        currentTab = nextTab;
        currentResearchList = &researchLists[currentTab];
        bResearchTransitioning = false;
        researchScroller.scrollPosition = currentResearchList->scrollPosition;
        researchScroller.totalScrollHeight = currentResearchList->totalScrollHeight;
        if(!isItemSelected()){
            //default to top
            selectItem(currentTab, "");
        }
    }
}

bool CloudsHUDController::aboutClosed(){
    bool closed = hudLabelMap["ExitButtonTextBox"]->isClicked();
    if(closed){
        hideAbout();
    }
    return closed;

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

void CloudsHUDController::togglePause(){
    if(isPaused()){
        unpause();
    }
    else{
        pause();
    }
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

	if(!bSetup){
        return;
    }
    
    if( !bDrawHud ){
        return;
	}
    
	ofPushStyle();
	ofPushMatrix();
    ofDisableLighting();
	ofEnableAlphaBlending();
#ifdef OCULUS_RIFT
    ofSetLineWidth(4);
#else
    ofTranslate( (ofGetWindowSize() - getSize() ) * 0.5 );
    ofSetLineWidth(1);

#endif
    ofScale( scaleAmt, scaleAmt );
    
    if( videoPlayer.isPlaying() && !bSkipAVideoFrame && hudOpenMap[CLOUDS_HUD_PROJECT_EXAMPLE] ){
        ofPushStyle();
        ofSetColor(255, 255, 255, 255*0.7);
        videoPlayer.draw( videoBounds );
        ofPopStyle();
    }

    for(map<CloudsHUDLayerSet, CloudsHUDLayer*>::iterator it = layers.begin(); it != layers.end(); it++){
        it->second->draw();
    }
    
    for(map<string, CloudsHUDLabel*>::iterator it = hudLabelMap.begin(); it != hudLabelMap.end(); ++it){
        it->second->draw();
    }

	if(hudOpenMap[CLOUDS_HUD_HOME]){
		home.draw();
    }
    
    if(hudOpenMap[CLOUDS_RESEARCH]){
        drawList();
    }
    if(hudOpenMap[CLOUDS_ABOUT_BACKERS]){
        drawBackersList();
    }
    if (hudOpenMap[CLOUDS_HUD_PAUSE]) {

        ofPushStyle();
        if(currentPreviewSelection != NULL && currentPreviewImage != NULL){
#ifdef OCULUS_RIFT
            ofSetColor(255,255 * .8);
#else
            ofSetColor(255, ofMap(currentPreviewSelection->hoverAlpha, 0, 1.0, .3, 1.0, true) * 255 * .8);
#endif
            currentPreviewImage->draw(layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreSeeMoreVisualBacking")->bounds);
        }
        else{
            ofSetColor(255, 255 * .3);
            topicMapPreview.draw(layers[CLOUDS_HUD_PAUSE]->svg.getMeshByID("ExploreSeeMoreVisualBacking")->bounds);
        }
        ofPopStyle();
    }
    
    ofEnableLighting();
	ofPopMatrix();
	ofPopStyle();
}

void CloudsHUDController::drawList(){
    ofPushMatrix();
    ofTranslate(0, -currentResearchList->scrollPosition);
 
    ofPushStyle();
    for(int i = 0; i < currentResearchList->buttons.size(); i++){
        if(currentResearchList->buttons[i].visible){

            ofColor textColor;
            if(currentResearchList->buttons[i].clicked){
                textColor = CloudsColorTextActive;
            }
            else if(currentResearchList->buttons[i].pressed){
                textColor = CloudsColorTextSelected;
            }
            else if(currentResearchList->buttons[i].hovered){
                textColor = CloudsColorTextHover;
            }
            else if(currentResearchList->buttons[i].visited){
                textColor = CloudsColorTextDeactivated;
            }
            else{
                textColor = CloudsColorTextStatic;
            }
            
            textColor.a *= transitionFade;
            
            ofSetColor(textColor);
            float drawXPos = hudLabelMap["ListTextBoxes"]->bounds.x;
            float drawYPos = researchScroller.scrollBounds.y +
                             currentResearchList->buttons[i].top +
                             researchScroller.scrollIncrement * .5;
            
            ResearchTopicListLabel->font->drawString(currentResearchList->buttons[i].label, drawXPos, drawYPos);
            
        }
    }
    ofPopStyle();
    
    ofPopMatrix();
}

void CloudsHUDController::drawBackersList(){
    ofPushMatrix();
    ofTranslate(0, -aboutScroller.scrollPosition);

    for(int i = 0; i < backers.size(); i++){
        if(backers[i].visible){
            backersFont.drawString(backers[i].backer, backers[i].pos.x, backers[i].pos.y);
        }
    }
    ofPopMatrix();
}



void CloudsHUDController::animateOn(CloudsHUDLayerSet layer){
#ifdef OCULUS_RIFT
    if(layer == CLOUDS_HUD_NEXT){
        return;
    }
#endif
    
    for (map<CloudsHUDLayerSet, CloudsHUDLayer* >::iterator it = layers.begin(); it != layers.end(); ++it) {
        if (layer == it->first || layer == CLOUDS_HUD_ALL) {
            hudOpenMap[it->first] = true;
            it->second->start();
        }
    }
    
    // animate in text
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
}

void CloudsHUDController::animateOff(){
    animateOff(CLOUDS_HUD_ALL);
}

void CloudsHUDController::animateOff(CloudsHUDLayerSet layer){
    
    if(!bSetup){
        return;
    }
    
    if (isPlaying && (layer == CLOUDS_HUD_PROJECT_EXAMPLE  || layer == CLOUDS_HUD_ALL)) {
		isPlaying = false;
    }

    if(layer == CLOUDS_HUD_HOME || layer == CLOUDS_HUD_ALL){
        home.deactivate();
    }
    
    if(layer == CLOUDS_HUD_ALL){
        bPaused = false;
        layers[CLOUDS_HUD_LOWER_THIRD]->bForceHover = false;
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
}

ofRectangle CloudsHUDController::getScaledRectangle(const ofRectangle& rect){
    ofRectangle outrect;
    outrect.x      = rect.x * scaleAmt + scaleOffset.x;
	outrect.y      = rect.y * scaleAmt + scaleOffset.y;
	outrect.width  = rect.width  * scaleAmt;
	outrect.height = rect.height * scaleAmt;
    return outrect;
}

