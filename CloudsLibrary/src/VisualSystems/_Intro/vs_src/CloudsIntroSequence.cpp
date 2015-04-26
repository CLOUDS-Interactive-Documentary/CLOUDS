//
//  IntroSequence.cpp
//  ComputerTicker
//
//  Created by James George on 5/26/13.
//
//

#include "CloudsIntroSequence.h"
#include "CloudsGlobal.h"
#include "CloudsEvents.h"
#include "CloudsPortalEvents.h"
#ifdef KINECT_INPUT
#include "CloudsInputKinectOSC.h"
#endif

#include "CloudsLocalization.h"

#ifdef MOUSE_INPUT
//for moving mouse position on idle
#include "ofAppGLFWWindow.h"
#endif

CloudsVisualSystemEvents CloudsIntroSequence::events;

string CloudsIntroSequence::getSystemName(){
	return "_Intro";
}

CloudsIntroSequence::CloudsIntroSequence(){
	getSelectLow();
	getSelectMid();
	getSelectHigh();
	getClick();
    
    menuItems.push_back(&researchMenuItem);
    menuItems.push_back(&playMenuItem);
    menuItems.push_back(&aboutMenuItem);
    menuItems.push_back(&newMenuItem);
    menuItems.push_back(&resumeMenuItem);
    for(int i = 0; i < menuItems.size(); i++){
        menuItems[i]->visible = false;
        menuItems[i]->hovered = false;
        menuItems[i]->pressed = false;
        menuItems[i]->clicked = false;
    }

}

void CloudsIntroSequence::selfSetDefaults(){

	selectedQuestion = NULL;
	showingQuestions = false;
	paused = false;
	currentFontSize = -1;
	currentFontExtrusion = -1;
	startedOnclick = false;
	caughtQuestion = NULL;
	titleNoisePosition = 0;
	titleNoiseSpeed = 0;
	hoveringTitle = false;
	currentTitleOpacity = 0;
	bQuestionDebug = false;
	firstQuestionStopped = false;
    
    introNodeChangeTime = 0;
    introNodesShown = true;
	kinectHelperAlpha = 0.0;
	nodeAlphaAttenuate = 1.0;
	
	introNodeOne.introNode = introNodeTwo.introNode = introNodeThree.introNode = true;
	introNodeOne.clickSound = introNodeTwo.clickSound = introNodeThree.clickSound = getClick();
	introNodeOne.selectSound = introNodeTwo.selectSound = getSelectHigh();
	introNodeThree.selectSound = getSelectMid();
	
	introNodeOne.multiplier   =  1;
	introNodeTwo.multiplier   = -1;
	introNodeThree.multiplier =  0;
	
	hintCursorEndPoint = ofVec2f(320,240);

    menuFontSize = currentMenuFontSize = 1;
    menuToolTipFontSize = currentMenuToolTipFontSize = 1;
    
    menuYOffset = 0;
    menuWidth = 500;
 
	promptTime = 0.0;
	promptShown = false;
	
	// Set question defaults.
	questionScale = 0.1f;
	helperFontSize = 14;
	questionZStopRange.min = 50;
	questionZStopRange.max = 300;
	perlinOffset = 0;
	newResumeSpace = 0;
    
	clickTextActive = false;
	clickTextActiveTime = 0;
	clickToBeginAlpha = 0;
	mouseLastMovedTime = 0;

	cameraSwingDamp = 0.0;

    warpCamera.setNearClip(.01);
	
	timeSinceLastPrompt = 0;
	
	cursorAlpha = 1.0;

	questionChannels.resize(4);
	channelPauseTime.resize(4);
	
#ifdef OCULUS_RIFT
    bCursorInCenter = false;
    startTimeCursorInCenter = 0;
#endif

}

void CloudsIntroSequence::selfSetup(){
	
	ofDisableArbTex();
	sprite.loadImage(getVisualSystemDataPath() + "images/dot.png");
	ofEnableArbTex();
	
	currentFontExtrusion = -1;
	currentFontSize = -1;
	
	introNodes.push_back( &introNodeOne );
	introNodes.push_back( &introNodeTwo );
	introNodes.push_back( &introNodeThree );

	reloadShaders();
}

void CloudsIntroSequence::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "arrange questions" && ((ofxUIButton*)e.widget)->getValue()){
		positionStartQuestions();
	}
	else if(e.widget->getName() == "generate tunnel" && ((ofxUIButton*)e.widget)->getValue()){
		generateTunnel();
	}
}

void CloudsIntroSequence::selfSetupCameraGui(){
	camGui->addSlider("camera fwd force", 0, 2, &cameraForwardSpeed);
	camGui->addSlider("camera wobble range", 0, 10, &camWobbleRange);
	camGui->addSlider("camera wobble speed", 0, 1., &camWobbleSpeed);
	camGui->addSlider("camera swing x", 0, 100., &cameraSwingRange.x);
	camGui->addSlider("camera swing y", 0, 100., &cameraSwingRange.y);
	camGui->addSlider("camera swing damp", 0, .4, &cameraSwingDamp);

	camGui->addToggle("hold camera", &paused);
}

void CloudsIntroSequence::guiSystemEvent(ofxUIEventArgs &e){
}

void CloudsIntroSequence::selfSetupRenderGui(){
	
	//	rdrGui->addSlider("Min Point Size", 0, 7, &pointSize.min);
	rdrGui->addSlider("Max Point Size", 0, 7, &pointSize.max);
	rdrGui->addSlider("Min Distance", 0, 500, &distanceRange.min);
	rdrGui->addSlider("Max Distance", 0, 500, &distanceRange.max);
	
	//rdrGui->addSlider("Chroma Max Distort", 0, 10, &maxChromaDistort);
	
	rdrGui->addSlider("Perlin Amplitude", 0, 10, &perlinAmplitude);
	rdrGui->addSlider("Perlin Density", 0, 50, &perlinDensity);
	rdrGui->addSlider("Perlin Speed", 0, .1, &perlinSpeed);
	
	//	rdrGui->addSlider("Font Size", 1, 10, &fontSize);
	//	rdrGui->addSlider("Font Extrusion", 0, 10, &fontExtrusion);
	
	rdrGui->addSlider("Wireframe Alpha", 0, 1.0, &wireframeAlpha);
}


void CloudsIntroSequence::selfSetupGuis(){
	
	questionGui = new ofxUISuperCanvas("QUESTIONS", gui);
	questionGui->copyCanvasStyle(gui);
	questionGui->copyCanvasProperties(gui);
	questionGui->setName("Questions");
	questionGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	questionGui->addToggle("Debug Tug", &bQuestionDebug);
	questionGui->addSlider("Scale", 0, 1, &questionScale);
	questionGui->addSlider("Wrap Distance", 100, 4000, &questionWrapDistance);
	questionGui->addRangeSlider("Question Z Range", 10, 300, &questionZStopRange.min, &questionZStopRange.max);
	questionGui->addSlider("Inner Radius", 2, 20, &questionTunnelInnerRadius);
	questionGui->addRangeSlider("Tug Distance", 10, 300, &questionTugDistance.min, &questionTugDistance.max);
	questionGui->addRangeSlider("Attenuate Distance", 10, 300,&questionAttenuateDistance.min,&questionAttenuateDistance.max);
	questionGui->addSlider("Question Pause Dur", 1.0, 15.0, &questionPauseDuration);
	
	questionGui->addButton("arrange questions", false);
	//	questionGui->addToggle("Custom Toggle", &customToggle);
	ofAddListener(questionGui->newGUIEvent, this, &CloudsIntroSequence::selfGuiEvent);
	
	guis.push_back(questionGui);
	guimap[questionGui->getName()] = questionGui;
	
	
	tunnelGui = new ofxUISuperCanvas("TUNNEL", gui);
	tunnelGui->copyCanvasStyle(gui);
	tunnelGui->copyCanvasProperties(gui);
	tunnelGui->setName("Tunnel");
	tunnelGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	tunnelGui->addSlider("Tunnel Width",  10, 50, &tunnelMax.x);
	tunnelGui->addSlider("Tunnel Height", 10, 50, &tunnelMax.y);
	tunnelGui->addSlider("Tunnel Depth",  100, 1000, &tunnelMax.z);
	
	tunnelGui->addSlider("Tunnel Tint H",  0, 1.0, &tint.r);
	tunnelGui->addSlider("Tunnel Tint S",  0, 1.0, &tint.g);
	tunnelGui->addSlider("Tunnel Tint B",  0, 1.0, &tint.b);
	
	tunnelGui->addSlider("Loose Rez X", 5, 20, &looseTunnelResolutionX);
	tunnelGui->addSlider("Loose Rez Z", 5, 100, &looseTunnelResolutionZ);
    
	tunnelGui->addButton("generate tunnel", false);
	
	ofAddListener(tunnelGui->newGUIEvent, this, &CloudsIntroSequence::selfGuiEvent);
	
	guis.push_back(tunnelGui);
	guimap[tunnelGui->getName()] = tunnelGui;
	
	typeGui = new ofxUISuperCanvas("TYPE", gui);
	typeGui->copyCanvasStyle(gui);
	typeGui->copyCanvasProperties(gui);
	typeGui->setName("Type");
	typeGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	typeGui->addIntSlider("Title Size", 2, 15, &titleFontSize);
	typeGui->addIntSlider("Title Extrude", 1, 5, &titleFontExtrude);
	typeGui->addSlider("Title Tracking", 0, 50, &titleTypeTracking);
	typeGui->addSlider("Title Offset", 0, 100, &titleTypeOffset);
	typeGui->addSlider("Title Opacity", .0, 1., &titleTypeOpacity);
	
	typeGui->addRangeSlider("Title Glow Range", 0., 1.0, &titleMinGlow, &titleMaxGlow);
	typeGui->addSlider("Title Noise Speed", 0, .1, &titleNoiseSpeed);
	typeGui->addSlider("Title Noise Dense", .001, 1., &titleNoiseDensity);
	typeGui->addSlider("Title Hover X", 0, 1.0, &titleRectWidth);
	typeGui->addSlider("Title Hover Y", 0, 1.0, &titleRectHeight);
	
	guis.push_back(typeGui);
	guimap[typeGui->getName()] = typeGui;
	
	
	introGui = new ofxUISuperCanvas("INTRO", gui);
	introGui->copyCanvasStyle(gui);
	introGui->copyCanvasProperties(gui);
	introGui->setName("Intro");
	introGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	//different ranges
#ifdef OCULUS_RIFT
	introGui->addSlider("Intro Node Size", 2, 7, &introNodeSize);
	introGui->addSlider("Intro Node X",    0, 400, &introNodeOffset.x); //mirrored along the axis
	introGui->addSlider("Intro Node Y",    0, 200, &introNodeOffset.y); //mirrored along the axis
	introGui->addSlider("Intro Node Z", -100, 100, &introNodeOffset.z); //mirrored along the axis
#else
	introGui->addSlider("Intro Node Size", 2, 100, &introNodeSize);
	introGui->addSlider("Intro Node X",    0, 1000, &introNodeOffset.x); //mirrored along the axis
	introGui->addSlider("Intro Node Y",    0, 1000, &introNodeOffset.y); //mirrored along the axis
	introGui->addSlider("Intro Node Z", -100, 100, &introNodeOffset.z); //mirrored along the axis
#endif
	introGui->addSlider("Intro Node Center Y", -100, 100, &introNodeYAdjust); //mirrored along the axis

	introGui->addSlider("Intro Node Hold Time", .5, 3, &introNodeHoldTime); //seconds
	
	guis.push_back(introGui);
	guimap[introGui->getName()] = introGui;
	
	
	helperTextGui = new ofxUISuperCanvas("HELPERTEXT", gui);
	helperTextGui->copyCanvasStyle(gui);
	helperTextGui->copyCanvasProperties(gui);
	helperTextGui->setName("Helpertext");
	helperTextGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	helperTextGui->addIntSlider("FONT SIZE", 4, 100, &helperFontSize);
	helperTextGui->addSlider("FONT SCALE", 0, 1.0, &helperFontScale);
	helperTextGui->addSlider("TRACKING", 0, 30, &helperFontTracking);
	helperTextGui->addSlider("Y POSITION", -200, 200, &helperFontY);
	
	guis.push_back(helperTextGui);
	guimap[helperTextGui->getName()] = helperTextGui;

    
	menuGui = new ofxUISuperCanvas("MENU", gui);
	menuGui->copyCanvasStyle(gui);
	menuGui->copyCanvasProperties(gui);
	menuGui->setName("Menu");
	menuGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
	menuGui->addIntSlider("FONT SIZE", 5, 20, &menuFontSize);
	menuGui->addIntSlider("TOOLTIP FONT SIZE", 5, 20, &menuToolTipFontSize);
	menuGui->addSlider("Y OFFSET", 0, 300, &menuYOffset);
	menuGui->addSlider("TOTAL WIDTH", 200, 1000, &menuWidth);
	menuGui->addSlider("NEW RESUME SPACE", 0, 200, &newResumeSpace);

	guis.push_back(menuGui);
	guimap[menuGui->getName()] = menuGui;
    
}

void CloudsIntroSequence::selfPresetLoaded(string presetPath){
	generateTunnel();
	warpCamera.setPosition(0, 0, 0);
	warpCamera.lookAt(ofVec3f(0,0,tunnelMax.z));
	positionStartQuestions();
}

void CloudsIntroSequence::reloadShaders(){
	tunnelShader.load(getVisualSystemDataPath() + "shaders/IntroTunnel");
	typeShader.load(getVisualSystemDataPath() + "shaders/IntroType");
	CloudsPortal::reloadShader();
}

void CloudsIntroSequence::selfUpdate(){
	
	if(!startedOnclick && timeline->getIsPlaying()){
		timeline->stop();
	}
	
	updateCamera();
	updateWaiting();
	updateQuestions();
	updateTitle();
    updateMenu();
}

void CloudsIntroSequence::updateCamera(){
	
	if(selectedQuestion == NULL){
		ofVec2f wobble = ofVec2f(ofSignedNoise(100 + ofGetElapsedTimef()*camWobbleSpeed),
								 ofSignedNoise(200 + ofGetElapsedTimef()*camWobbleSpeed)) * camWobbleRange;
		if(!paused){
			warpCamera.dolly(-cameraForwardSpeed);
			warpCamera.setPosition(wobble.x, wobble.y, warpCamera.getPosition().z);
		}
		else{
			warpCamera.setPosition(wobble.x, wobble.y, 0);
		}
		ofVec2f camSwing = ofVec2f(ofMap(GetCloudsInputPosition().x, 0, getCanvasWidth(), cameraSwingRange.x, -cameraSwingRange.x, true ),
								   ofMap(GetCloudsInputPosition().y, 0, getCanvasHeight(), cameraSwingRange.y, -cameraSwingRange.y, true));

		curCameraSwing.interpolate(camSwing,powf(cameraSwingDamp, 2.0));

		warpCamera.lookAt( ofVec3f(curCameraSwing.x, curCameraSwing.y, warpCamera.getPosition().z + 50) );
	}
	else {
		float percentZoomed = powf(ofMap(ofGetElapsedTimef(), selectedQuestionTime, selectedQuestionTime + 2.0, 0.0, 1.0, true),2.);
		warpCamera.setPosition(selectQuestionStartPos.interpolate(selectedQuestion->hoverPosition, percentZoomed));
	}
}

void CloudsIntroSequence::updateWaiting(){
	
    // Trigger start manually
    if (startedOnclick) {
		nodeAlphaAttenuate = MAX(0,nodeAlphaAttenuate-0.02);
		clickToBeginAlpha *= .99;
		clickToBeginAlpha -= .001;
		clickToBeginAlpha = MAX(0,clickToBeginAlpha);
		return;
	}

	for(int i = 0; i < introNodes.size(); i++){
		updateIntroNodePosition(*introNodes[i]);
	}
	
	for(int i = 0; i < introNodes.size(); i++){
		updateIntroNodeInteraction(*introNodes[i]);
		//bail if we haven't completed this part of the sequence
		if(!introNodes[i]->finished){
			break;
		}
	}
	
	if(introNodeThree.finished){
		startedOnclick = true;
		timeline->play();
	}

	#ifdef OCULUS_RIFT
	
	#elif defined(KINECT_INPUT)
	k4w::ViewerState viewerState = ((CloudsInputKinectOSC*)GetCloudsInput().get())->viewerState;
	if(startQuestions.size() > 0 && viewerState != k4w::ViewerState_None){
		if(!promptShown && ofGetElapsedTimef() - timeSinceLastPrompt > 8){
			if(viewerState == k4w::ViewerState_OutOfRange){
//				CloudsPortalEventArgs args("MOVE CLOSER TO THE DISPLAY");
//				ofNotifyEvent(events.portalHoverBegan, args);
			}
			else if(viewerState == k4w::ViewerState_PresentIdle){
//				CloudsPortalEventArgs args("EXTEND YOUR HAND TO BEGIN");
//				ofNotifyEvent(events.portalHoverBegan, args);
			}
			timeSinceLastPrompt = ofGetElapsedTimef();
			promptShown = true;
			kinectHelperTargetAlpha = 1.0;
		}
		else if(promptShown && ofGetElapsedTimef() - timeSinceLastPrompt > 4){
			CloudsPortalEventArgs args("");
			ofNotifyEvent(events.portalHoverEnded, args);
			timeSinceLastPrompt = ofGetElapsedTimef();
			promptShown = false;
			kinectHelperTargetAlpha = 0.0;
		}
	}
	 
	#else
	if(startQuestions.size() > 0){
		if(!promptShown && ofGetElapsedTimef() - timeSinceLastPrompt > 10){
//			CloudsPortalEventArgs args("CLICK TO BEGIN");
//			ofNotifyEvent(events.portalHoverBegan, args);
			timeSinceLastPrompt = ofGetElapsedTimef();
			promptShown = true;

		}
		else if(promptShown && ofGetElapsedTimef() - timeSinceLastPrompt > 4){
			CloudsPortalEventArgs args("");
			ofNotifyEvent(events.portalHoverEnded, args);
			timeSinceLastPrompt = ofGetElapsedTimef();
			promptShown = false;
		}
	}
	#endif
	
	#ifdef MOUSE_INPUT
	if(clickTextActive){
		float timeSinceActive = ofGetElapsedTimef() - clickTextActiveTime;
		float timeSinceMouse = ofGetElapsedTimef() - mouseLastMovedTime;
		clickToBeginAlpha = ofMap(timeSinceActive,
								  .5,1.5,
								  0.0,1.0,true) *
								  ofMap(currentTitleOpacity,titleTypeOpacity,.9,0.3,.7,true);
        #ifdef TARGET_WIN32
        if(timeSinceMouse > 4.){
			clickToBeginAlpha *= ofMap(timeSinceMouse, 4.0, 5.0, 1.0, 0.0,true);
			if(timeSinceMouse > 5.0){
				clickTextActive = false;
				glfwSetCursorPos( ofAppGLFWWindow::windowP, getCanvasWidth()/2, getCanvasHeight()/2);
			}
		}
        #endif
	}
	#endif
}


void CloudsIntroSequence::updateIntroNodePosition(CalibrationNode& node){
	
	node.baseOffset = introNodeOffset;
	node.titleTypeOffset = titleTypeOffset;
	node.holdTime = introNodeHoldTime;
	node.centerYAdjust = introNodeYAdjust;
	node.updateWorldPosition();
	#ifdef OCULUS_RIFT
	node.activationDistance = questionTugDistance;
	#else
	node.activationDistance = ofRange(introNodeSize*3, introNodeSize*5);
	node.worldPosition.x += getCanvasWidth()/2;
	node.worldPosition.y += getCanvasHeight()/2;
	#endif
	node.updateScreenPosition();
	
}

void CloudsIntroSequence::updateIntroNodeInteraction(CalibrationNode& node){
	
	node.updateInteraction();
}

void CloudsIntroSequence::updateTitle(){
	
	if(!extrudedTitleText.isLoaded() ||
	   currentFontSize != titleFontSize ||
	   currentFontExtrusion != titleFontExtrude)
	{
		currentFontSize = titleFontExtrude;
		currentFontExtrusion = titleFontExtrude;
        //TODO replace with geo
		extrudedTitleText.loadFont(GetCloudsDataPath() + "font/materiapro_light.ttf", titleFontSize, currentFontExtrusion);
	}
	
	titleRect = ofRectangle(0,0,titleRectWidth * getSharedRenderTarget().getWidth(),
								titleRectHeight * getSharedRenderTarget().getHeight() );
	titleRect.alignTo( ofPoint(getCanvasWidth()/2, getCanvasHeight()/2) );
	hoveringTitle = titleRect.inside(GetCloudsInputX(), GetCloudsInputY());
	
	//	cout << "title rect is " << titleRect.getTopLeft() << " " << titleRect.getBottomLeft() << endl;
	//	cout << "hovering? " << (hoveringTitle ? "YES" : "NO" ) << endl;
	
	titleNoisePosition += titleNoiseSpeed;
	float hoverTitleOpacity;
	if(hoveringTitle || (startedOnclick && timeline->getIsPlaying()) ){
		hoverTitleOpacity = .9;
	}
	else{
		hoverTitleOpacity = titleTypeOpacity;
	}
	
	currentTitleOpacity += (hoverTitleOpacity-currentTitleOpacity)*.05;
}

void CloudsIntroSequence::updateMenu(){
    
	if(!menuFont.isLoaded() || currentMenuFontSize != menuFontSize){
		menuFont.loadFont(GetFontPath(), menuFontSize	);
        currentMenuFontSize = menuFontSize;
    }
	if(!menuToolTipFont.isLoaded() || currentMenuToolTipFontSize != menuToolTipFontSize){
		menuToolTipFont.loadFont(GetFontPath(), menuFontSize	);
        currentMenuToolTipFontSize = menuToolTipFontSize;
    }
    
    float menuHeight    = menuFont.stringHeight("W");
    float researchWidth = menuFont.stringWidth( GetTranslationForString("RESEARCH") );
    float playWidth     = menuFont.stringWidth( GetTranslationForString("PLAY") );
    float aboutWidth    = menuFont.stringWidth( GetTranslationForString("ABOUT") );
    float newWidth      = menuFont.stringWidth( GetTranslationForString("NEW") );
    float resumeWidth   = menuFont.stringWidth( GetTranslationForString("RESUME") );
    
    float menuTop = getCanvasHeight() / 2 + menuYOffset;
    researchMenuItem.bounds.x = getCanvasWidth() / 2 - menuWidth / 2;
    researchMenuItem.bounds.y = menuTop;
    researchMenuItem.bounds.width = researchWidth;
    researchMenuItem.bounds.height = menuHeight;

    playMenuItem.bounds.x = getCanvasWidth() / 2 - playWidth / 2;
    playMenuItem.bounds.y = menuTop;
    playMenuItem.bounds.width = playWidth;
    playMenuItem.bounds.height = menuHeight;

    aboutMenuItem.bounds.x = getCanvasWidth() / 2 + menuWidth/ 2 - aboutWidth;
    aboutMenuItem.bounds.y = menuTop;
    aboutMenuItem.bounds.width = aboutWidth;
    aboutMenuItem.bounds.height = menuHeight;
    
    float newResumeWidth = newWidth + resumeWidth + newResumeSpace;
    newMenuItem.bounds.x = getCanvasWidth() / 2 - newResumeWidth / 2;
    newMenuItem.bounds.y = menuTop;
    newMenuItem.bounds.width = newWidth;
    newMenuItem.bounds.height = menuHeight;
    
    resumeMenuItem.bounds.x = getCanvasWidth() / 2 + newResumeWidth/2 - resumeWidth;
    resumeMenuItem.bounds.y = menuTop;
    resumeMenuItem.bounds.width = researchWidth;
    resumeMenuItem.bounds.height = menuHeight;

}

void CloudsIntroSequence::updateQuestions(){

	for(int i = 0; i < startQuestions.size(); i++){
		CloudsPortal& curQuestion = startQuestions[i];
		curQuestion.scale = questionScale;
		curQuestion.update();
		
		if(curQuestion.hoverPosition.z < warpCamera.getPosition().z){
			curQuestion.hoverPosition.z += questionWrapDistance;
		}
		
		float slowDownFactor = 0.0;
		//hold the questions
		if(questionChannels[ curQuestion.tunnelQuadrantIndex ]){
			//let it go with time
			slowDownFactor = powf(ofMap(ofGetElapsedTimef(),
											  channelPauseTime[curQuestion.tunnelQuadrantIndex] + questionPauseDuration,
											  channelPauseTime[curQuestion.tunnelQuadrantIndex] + questionPauseDuration+2, 1.0, 0.0, true), 2.0);
			if(slowDownFactor == 0.0){
				questionChannels[ curQuestion.tunnelQuadrantIndex ] = false;
			}
			
		}
		else{
			float distanceFromCamera = (curQuestion.hoverPosition.z - warpCamera.getPosition().z);
			//if it's in front of the stop range
			if(distanceFromCamera > questionZStopRange.min){
				slowDownFactor = powf(ofMap(distanceFromCamera, questionZStopRange.min, questionZStopRange.max, 1.0, 0.0, true), 2.0);
//				curQuestion.hoverPosition.z += slowDownFactor * cameraForwardSpeed;
				if(slowDownFactor > .9){
					//pause this node and all the ones behind it
                    if(!firstQuestionStopped){
                        firstQuestionStoppedTime = ofGetElapsedTimef();
                        firstQuestionStopped = true;
                    }
					questionChannels[curQuestion.tunnelQuadrantIndex] = true;
					channelPauseTime[curQuestion.tunnelQuadrantIndex] = ofGetElapsedTimef();
				}
			}
		}
		
		if(&curQuestion == caughtQuestion){
			slowDownFactor = 1.0;
		}
		
		curQuestion.hoverPosition.z += cameraForwardSpeed * slowDownFactor;

		if(curQuestion.hoverPosition.z - warpCamera.getPosition().z < questionZStopRange.max || &curQuestion == caughtQuestion){
#ifdef OCULUS_RIFT
            ofVec3f screenPos = getOculusRift().worldToScreen(curQuestion.hoverPosition, true);
			ofRectangle viewport = getOculusRift().getOculusViewport();
            float distanceToQuestion = ofDist(screenPos.x, screenPos.y,viewport.getCenter().x, viewport.getCenter().y);
#else
            ofVec2f mouseNode = cursor;
			float distanceToQuestion = startQuestions[i].screenPosition.distance(mouseNode);
#endif
			if(selectedQuestion == NULL && caughtQuestion == NULL){
				if( distanceToQuestion < questionTugDistance.max ){
					if(distanceToQuestion < questionTugDistance.min){
						caughtQuestion = &curQuestion;
						if (caughtQuestion->startHovering()) {
							getClick()->setPosition(0);
							getClick()->play();
                        }
					}
				}
			}
			
			//we have a caught question make sure it's still close
			else if(caughtQuestion == &curQuestion){

				if( caughtQuestion->isSelected() && !bQuestionDebug && selectedQuestion == NULL){
					getSelectLow()->setPosition(0);
					getSelectLow()->play();

					selectedQuestion = caughtQuestion;
					selectedQuestionTime = ofGetElapsedTimef();
					selectQuestionStartPos = warpCamera.getPosition();
					selectQuestionStartRot = warpCamera.getOrientationQuat();

					CloudsPortalEventArgs args( GetTranslationForString( ofToUpper(selectedQuestion->question) ) );
					ofNotifyEvent(events.portalHoverBegan, args);
					
				}
				else if(distanceToQuestion > questionTugDistance.max && selectedQuestion == NULL){
					caughtQuestion->stopHovering();
					caughtQuestion = NULL;
                    if(firstQuestionStopped){
                        firstQuestionStoppedTime = ofGetElapsedTimef();
                    }
				}
			}
		}
	}
    
    if (caughtQuestion != NULL) {
        // move the sticky cursor towards the caught question
        stickyCursor.interpolate(caughtQuestion->screenPosition - ofVec2f(bleed,bleed)*.5, 0.2f);
    }
    else {
        stickyCursor.interpolate(cursor, 0.5f);
    }
}


void CloudsIntroSequence::setStartQuestions(vector<CloudsClip*>& possibleStartQuestions){

	selectedQuestion = NULL;
	startQuestions.clear();
	
	for(int i = 0; i < possibleStartQuestions.size(); i++){
		
		CloudsPortal q;
		q.cam = &warpCamera;
		q.bLookAtCamera = true;
//		q.font = &questionFont;
		q.clip = possibleStartQuestions[i];
		q.topic = q.clip->getAllTopicsWithQuestion()[0];
		q.question = q.clip->getQuestionForTopic(q.topic);

		q.setup();
		
		startQuestions.push_back(q);
	}
	timeSinceLastPrompt = ofGetElapsedTimef();
	positionStartQuestions();
}

void CloudsIntroSequence::generateTunnel(){

	// loose tunnel, with lines
	tunnelMeshLoose.clear();
	
	float looseTunnelStepZ = tunnelMax.z / looseTunnelResolutionZ;
	float looseTunnelStepY = tunnelMax.y / looseTunnelResolutionX;
	float looseTunnelStepX = tunnelMax.x / looseTunnelResolutionX;
	
	float tightTunnelStepZ = looseTunnelStepZ/2.0;
	float tightTunnelStepY = looseTunnelStepY/2.0;
	float tightTunnelStepX = looseTunnelStepX/2.0;

	for(float z = 0; z < tunnelMax.z; z += looseTunnelStepZ){
		//add invisible connector point
		tunnelMeshLoose.addColor(ofFloatColor(0,0));
		tunnelMeshLoose.addVertex(ofVec3f(-tunnelMax.x,-tunnelMax.y, z));
		
		//draw the top
		for(float x = -tunnelMax.x; x <= tunnelMax.x; x += tightTunnelStepX){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(x,-tunnelMax.y,z));
		}
		//draw right side
		for(float y = -tunnelMax.y; y <= tunnelMax.y; y += tightTunnelStepY){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(tunnelMax.x,y,z));
		}
		//draw bottom
		for(float x = tunnelMax.x; x >= -tunnelMax.x; x -= tightTunnelStepX){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(x,tunnelMax.y,z));
		}
		//draw the left side
		for(float y = tunnelMax.y; y >= -tunnelMax.y; y -= tightTunnelStepY){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(-tunnelMax.x,y,z));
		}
		tunnelMeshLoose.addColor(ofFloatColor(0,0));
		tunnelMeshLoose.addVertex(ofVec3f(-tunnelMax.x,-tunnelMax.y, z));
	}
	tunnelMeshLoose.setMode(OF_PRIMITIVE_LINE_STRIP);
	
	
	// tight tunnel, dots only
	tunnelMeshTight.clear();
	for(float z = 0; z < tunnelMax.z; z += tightTunnelStepZ){
		//draw the top
		for(float x = -tunnelMax.x; x <= tunnelMax.x; x += tightTunnelStepX){
			tunnelMeshTight.addVertex(ofVec3f(x,-tunnelMax.y,z));
		}
		//draw right side
		for(float y = -tunnelMax.y; y <= tunnelMax.y; y += tightTunnelStepY){
			tunnelMeshTight.addVertex(ofVec3f(tunnelMax.x,y,z));
		}
		//draw bottom
		for(float x = tunnelMax.x; x >= -tunnelMax.x; x -= tightTunnelStepX){
			tunnelMeshTight.addVertex(ofVec3f(x,tunnelMax.y,z));
		}
		//draw the left side
		for(float y = tunnelMax.y; y >= -tunnelMax.y; y -= tightTunnelStepY){
			tunnelMeshTight.addVertex(ofVec3f(-tunnelMax.x,y,z));
		}
	}	
}

void CloudsIntroSequence::positionStartQuestions(){

	//new way with sets of 4
	srand(ofGetSeconds());
	random_shuffle(startQuestions.begin(), startQuestions.end());
	
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].tunnelQuadrantIndex = i % 4;
		//push the top ones down a bit and push out the side ones
		float attenuator = (i % 2 == 0) ? .7 : 1.3;
		startQuestions[i].hoverPosition = ofVec3f(0, questionTunnelInnerRadius * attenuator, 0);
		startQuestions[i].hoverPosition.rotate(i%4 * .25 * 360, ofVec3f(0,0,1));
		startQuestions[i].hoverPosition.z = 200 + tunnelMax.z*.25 + i * (1.0*questionWrapDistance / startQuestions.size() );
	}
}

bool CloudsIntroSequence::istStartQuestionHovering(){
	return caughtQuestion != NULL;
}

string CloudsIntroSequence::getQuestionText(){
	if(selectedQuestion != NULL){
		return selectedQuestion->question;
	}
	else if(caughtQuestion != NULL){
		return caughtQuestion->question;
	}
	return "";
}

bool CloudsIntroSequence::userHasBegun(){
    return startedOnclick;
}

bool CloudsIntroSequence::isStartQuestionSelected(){
	return selectedQuestion != NULL;
}

void CloudsIntroSequence::autoSelectQuestion(){
	selectedQuestion = &startQuestions[ ofRandom(startQuestions.size()) ];
}

CloudsPortal* CloudsIntroSequence::getSelectedQuestion(){
	return selectedQuestion;
}

void CloudsIntroSequence::selfDrawBackground(){
	if(bQuestionDebug){
		ofPushStyle();
		
		for(int i = 0; i < startQuestions.size(); i++){
			float alpha = .1;
			if(startQuestions[i].hoverPosition.z - warpCamera.getPosition().z < questionZStopRange.min){
				alpha = .8;
			}
			
			if(startQuestions[i].hovering){
				ofFill();
			}
			else{
				ofNoFill();
			}
			
			ofSetColor(startQuestions[i].hovering ? ofColor::green : ofColor::yellow, alpha*255);
			ofCircle(startQuestions[i].screenPosition, questionTugDistance.min);
			ofNoFill();
			ofSetColor(ofColor::white, alpha*255);
			ofCircle(startQuestions[i].screenPosition, questionTugDistance.max);
			
		}
		ofPopStyle();
	}
}

void CloudsIntroSequence::selfDrawDebug(){

}

void CloudsIntroSequence::timelineBangEvent(ofxTLBangEventArgs& args){
	
	CloudsVisualSystem::timelineBangEvent(args);
	if(args.flag == "LOOP_END"){
		ofxTLFlags* flags = (ofxTLFlags*)timeline->getTrack("Intro-Outro");
		ofxTLFlag* flag = flags->getFlagWithKey("LOOP_START");
		if(flag != NULL){
			timeline->stop();
			timeline->setCurrentTimeMillis(flag->time);
			timeline->play();
		}
		
		showingQuestions = true;
	}
	
	if(args.flag == "PROMPT"){
		CloudsPortalEventArgs args("SELECT A QUESTION TO BEGIN");
//		ofNotifyEvent(events.portalHoverBegan, args);
	}
}

void CloudsIntroSequence::selfDraw(){
  
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
	glEnable(GL_POINT_SMOOTH);
	glDisable(GL_DEPTH_TEST);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	drawCloudsType();
#ifdef OCULUS_RIFT
	drawIntroNodes();
#endif
	drawTunnel();
	drawPortals();
	drawHelperType();
}

void CloudsIntroSequence::drawTunnel(){
	ofPushStyle();
	
	tunnelShader.begin();
	tunnelShader.setUniform1f("minPointSize", pointSize.min);
	tunnelShader.setUniform1f("maxPointSize", pointSize.max);
	tunnelShader.setUniform1f("minDistance", distanceRange.min);
	
	tunnelShader.setUniform1f("maxDistance", distanceRange.max + MAX(1.0 - cursorAlpha, currentTitleOpacity) * 120.);
	
	tunnelShader.setUniform1f("cameraZ", warpCamera.getPosition().z);
	tunnelShader.setUniform1f("tunnelDepth", tunnelMax.z);
	tunnelShader.setUniform1f("noiseAmplitude", perlinAmplitude);
	tunnelShader.setUniform1f("noiseDensity", perlinDensity);
	perlinOffset += perlinSpeed;
	tunnelShader.setUniform1f("noisePosition", perlinOffset);
	tunnelShader.setUniform1f("colorAttenuate", 1.0);
	ofFloatColor tinted = ofFloatColor::fromHsb(tint.r, tint.g, tint.b);
	tunnelShader.setUniform4f("tint",tinted.r,tinted.g,tinted.b, 1.0);
	
	ofSetColor(255);
	ofEnablePointSprites();
	tunnelShader.setUniformTexture("sprite", sprite, 0);
	tunnelShader.setUniform1f("useSprite", 1);
	tunnelMeshTight.drawVertices();
	ofDisablePointSprites();
	
	ofSetLineWidth(1);
	tunnelShader.setUniform1f("colorAttenuate", wireframeAlpha);
	tunnelShader.setUniform1f("useSprite", 0);
	tunnelMeshLoose.draw();
	
	tunnelShader.end();
	
	ofPopStyle();
}

void CloudsIntroSequence::drawPortals(){
	ofPushStyle();
    ofEnableAlphaBlending();
	
	CloudsPortal::shader.begin();
	
	CloudsPortal::shader.setUniform1i("doAttenuate", 1);
	CloudsPortal::shader.setUniform1f("minDistance", questionAttenuateDistance.min);
	CloudsPortal::shader.setUniform1f("maxDistance", questionAttenuateDistance.max);
	
    ofSetColor(255);
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].draw();
	}
	
	CloudsPortal::shader.end();
	
	ofPopStyle();
}

void CloudsIntroSequence::drawCloudsType(){
	ofPushMatrix();
	ofPushStyle();
	
	ofRotate(180, 0, 0, 1);
	
	ofEnableAlphaBlending();
	ofSetColor(255, currentTitleOpacity*255);
	ofTranslate(0, 0, titleTypeOffset );
	extrudedTitleText.setLetterSpacing( titleTypeTracking );
	float width  = extrudedTitleText.stringWidth("CLOUDS");
	float height = extrudedTitleText.stringHeight("CLOUDS");
	
	typeShader.begin();
	typeShader.setUniform1f("noisePosition",titleNoisePosition);
	typeShader.setUniform1f("noiseDensity",titleNoiseDensity);
	typeShader.setUniform1f("glowMin", titleMinGlow);
	typeShader.setUniform1f("glowMax", titleMaxGlow);

	extrudedTitleText.drawString("CLOUDS", -width/2, height/2);
	
	typeShader.end();

	ofPopStyle();
	ofPopMatrix();
}

void CloudsIntroSequence::drawHelperType(){

	ofPushStyle();
	glDisable(GL_DEPTH_TEST);
	ofDisableLighting();
    
	if(!helperFont.isLoaded() || currentHelperFontSize != helperFontSize){
		//helperFont.loadFont(GetCloudsDataPath() + "font/Blender-BOOK.ttf", helperFontSize);
#ifdef OCULUS_RIFT
		helperFont.loadFont(GetFontPath(), helperFontSize-2	); //hack!
#else
		helperFont.loadFont(GetFontPath(), helperFontSize	); //hack!
#endif		
		currentHelperFontSize = helperFontSize;
	}

	string helpHoverText;
	ofVec3f basePosition(0,0,0);
	float helperTextOpacity = 0.0;
	float scaleModifier = 1.0;// * ofGetMouseX() / ofGetWidth();

	
	#ifdef OCULUS_RIFT
	if(!startedOnclick){
		if(introNodeThree.hover || introNodeTwo.finished){
			helpHoverText = "< " + GetTranslationForString("LOOK CENTER");
			basePosition = introNodeTwo.worldPosition;
			helperTextOpacity = powf(ofMap(ofGetElapsedTimef(),
										   CalibrationNode::nodeActivatedTime,
										   CalibrationNode::nodeActivatedTime+.8,0.0,.8,true), 2.) * (1.0 - introNodeThree.percentComplete);
		}
		else if(introNodeTwo.hover || introNodeOne.finished){
			helpHoverText = GetTranslationForString("LOOK RIGHT") + " >";
			basePosition = introNodeOne.worldPosition;
			helperTextOpacity = powf(ofMap(ofGetElapsedTimef(),
										   CalibrationNode::nodeActivatedTime,
										   CalibrationNode::nodeActivatedTime+.8,0.0,.8,true), 2.);
		}
		else {
			helpHoverText = "< " + GetTranslationForString("LOOK LEFT");
			basePosition = introNodeThree.worldPosition;
			helperTextOpacity = (currentTitleOpacity - titleTypeOpacity) * (1.0 - introNodeOne.percentComplete);
		}
		helperFont.setLetterSpacing(helperFontTracking);
	}

	#endif
	
	if(caughtQuestion != NULL){
		basePosition = caughtQuestion->hoverPosition;
		helpHoverText = GetTranslationForString( caughtQuestion->question );
		helperTextOpacity = ofMap(caughtQuestion->hoverPercentComplete, 0.0, .05, 0.0, 1.0, true);

		scaleModifier = .5;
		helperFont.setLetterSpacing(helperFontTracking*.1);
	}

    //draw the text
	if(helpHoverText != ""){
        ofPushMatrix();
		helpHoverText = ofToUpper(helpHoverText);
		
		float hoverTextWidth = helperFont.stringWidth(helpHoverText);
		float hoverTextWidth2,questionTextHeight2;
		string secondLine;
		bool twoLines = hoverTextWidth > 500;
		if(helpHoverText.find("\n") != string::npos){
			twoLines = true;
			vector<string> split = ofSplitString(helpHoverText, "\n", true,true);
			helpHoverText = split[0];
			secondLine = split[1];
			hoverTextWidth = helperFont.stringWidth(helpHoverText);
			hoverTextWidth2 = helperFont.stringWidth(secondLine);
            
//            cout << "QUESTION " << helpHoverText << " " << secondLine << endl;
		}
		else if(twoLines){
			vector<string> pieces = ofSplitString(helpHoverText, " ", true,true);
			vector<string> firstHalf;
			vector<string> secondHalf;
			int halfsize = pieces.size() / 2;
			firstHalf.insert(firstHalf.begin(), pieces.begin(), pieces.begin() + halfsize);
			secondHalf.insert(secondHalf.begin(), pieces.begin() + halfsize, pieces.end());
			helpHoverText = ofJoinString(firstHalf, " ");
			secondLine = ofJoinString(secondHalf, " ");
			hoverTextWidth  = helperFont.stringWidth(helpHoverText);
			hoverTextWidth2 = helperFont.stringWidth(secondLine);
		}
		float hoverTextHeight = helperFont.stringHeight(helpHoverText);
		
        //basePosition = ofVec3f(0,0,warpCamera.getPosition().z + questionZStopRange.max);

		#ifdef OCULUS_RIFT
		getOculusRift().multBillboardMatrix( basePosition );
		#else
		ofTranslate(basePosition);
		#endif
		ofRotate(180, 0, 0, 1); //flip around
		ofScale(scaleModifier*helperFontScale,
				scaleModifier*helperFontScale,
				scaleModifier*helperFontScale);
		
		ofSetColor(255,255*helperTextOpacity);
		
        bool showAbove = !bUseOculusRift && caughtQuestion != NULL && caughtQuestion->tunnelQuadrantIndex == 2;
		int yOffsetMult = (showAbove) ? -1 : 1;
		//helperFont.drawString(helpHoverText, -hoverTextWidth/2, yOffsetMult * (helperFontY - hoverTextHeight/2) );
        
//        cout << "helper text opacity " << helperTextOpacity << endl;
//        cout << "helper font y " << helperFontY << endl;
		if(twoLines){
            if(showAbove){
//                cout << "drawing " << helpHoverText << " w " << hoverTextWidth << " h " <<  helperFontY + hoverTextHeight*1.5 << endl;
//                cout << "drawing " << secondLine << " w " << hoverTextWidth << " h " << hoverTextHeight << endl;
                helperFont.drawString(helpHoverText, -hoverTextWidth*.5, yOffsetMult * (helperFontY + hoverTextHeight*1.5) );
                helperFont.drawString(secondLine, -hoverTextWidth2*.5, yOffsetMult * (helperFontY - hoverTextHeight*.5));
            }
            else{
//                cout << "drawing " << secondLine << " w " << hoverTextWidth << " h " <<  hoverTextHeight << endl;
//                cout << "drawing " << helpHoverText << " w " << hoverTextWidth << " h " << hoverTextHeight << endl;
                helperFont.drawString(secondLine, -hoverTextWidth2*.5, yOffsetMult * (helperFontY + hoverTextHeight*1.5) );
                helperFont.drawString(helpHoverText, -hoverTextWidth*.5, yOffsetMult * (helperFontY - hoverTextHeight*.5));
            }
		}
        else{
            helperFont.drawString(helpHoverText, -hoverTextWidth*.5, yOffsetMult * (helperFontY - hoverTextHeight*.5));
        }
		ofPopMatrix();
	}
    
    if(firstQuestionStopped){
        ofPushMatrix();
        
        float questionhintAlpha = ofMap(ofGetElapsedTimef(),
                                        firstQuestionStoppedTime, firstQuestionStoppedTime+2,
                                        0.0, .2, true) * (1.0-helperTextOpacity);
        
        float hintTextWidth  = helperFont.stringWidth(GetTranslationForString("SELECT A QUESTION"));
		float hintTextHeight = helperFont.stringHeight(GetTranslationForString("SELECT A QUESTION"));
		basePosition = ofVec3f(0,0,warpCamera.getPosition().z + questionZStopRange.max);
#ifdef OCULUS_RIFT
		getOculusRift().multBillboardMatrix( basePosition );
#else
		ofTranslate(basePosition);
#endif
		ofRotate(180, 0, 0, 1); //flip around
		ofScale(helperFontScale*.8,
				helperFontScale*.8,
				helperFontScale*.8);
        
        ofSetColor(255, 255*questionhintAlpha);
		helperFont.drawString(GetTranslationForString("SELECT A QUESTION"), -hintTextWidth*.5, hintTextHeight*.5 );

        if(caughtQuestion != NULL){
            float questionHoldAlpha = ofMap(caughtQuestion->hoverPercentComplete, .2, .3, 0.0, .2, true);
            ofSetColor(255, 255*questionHoldAlpha);
#ifdef MOUSE_INPUT
			string textPrompt = GetTranslationForString("CLICK TO SELECT");
//            string textPrompt = GetTranslationForString("");
#else
			string textPrompt = GetTranslationForString("HOLD TO SELECT");
#endif
            hintTextWidth = helperFont.stringWidth(textPrompt);
            hintTextHeight = helperFont.stringWidth(textPrompt);
            helperFont.drawString(textPrompt, -hintTextWidth*.5, hintTextHeight*.5 );
        }
        
        ofPopMatrix();
    }

    ofEnableLighting();
	glEnable(GL_DEPTH_TEST);
	ofPopStyle();

}

void CloudsIntroSequence::drawIntroNodes(){

	ofPushStyle();
    float extraAttenuate = 1.0;
    
#ifdef KINECT_INPUT
	k4w::ViewerState viewerState = ((CloudsInputKinectOSC*)GetCloudsInput().get())->viewerState;
    if(viewerState < k4w::ViewerState_PresentIdle && introNodesShown){
        introNodesShown = false;
        introNodeChangeTime = ofGetElapsedTimef();
    }
	if(viewerState >= k4w::ViewerState_PresentIdle && !introNodesShown){
        introNodesShown = true;
        introNodeChangeTime = ofGetElapsedTimef();
    }
    
    extraAttenuate = ofMap(ofGetElapsedTimef(),
                           introNodeChangeTime, introNodeChangeTime+.5,
                           0.0, 1.0, true);
    //if(!introNodesShown) 1.0 - extraAttenuate;
    
    //cout << "show nodes " << introNodesShown << " attenuate " << endl;
    if(!introNodesShown){
        return;
    }
    
#endif
    
	for(int i = 0; i < introNodes.size(); i++){
		introNodes[i]->nodeAlphaAttenuate = nodeAlphaAttenuate;
		introNodes[i]->nodeBaseSize = introNodeSize;
		introNodes[i]->tint = ofFloatColor::fromHsb(tint.r, tint.g, tint.b);
		introNodes[i]->tint.a = 200*nodeAlphaAttenuate*extraAttenuate;
		
		introNodes[i]->draw();
		
		if(!introNodes[i]->finished){
			break;
		}
	}
	
	ofPopStyle();	
}

void CloudsIntroSequence::drawCursors(){
	cursorAlpha = 1.0;
	if(!startedOnclick){
		cursorAlpha = ofMap(clickToBeginAlpha, 0, .3, 0.0, 1.0, true);
	}
    map<int, CloudsInteractionEventArgs>& inputPoints = GetCloudsInputPoints();
    for (map<int, CloudsInteractionEventArgs>::iterator it = inputPoints.begin(); it != inputPoints.end(); ++it) {
        if (it->second.primary) {
            // override primaryCursorMode
            selfDrawCursor(stickyCursor, it->second.dragged, caughtQuestion? CURSOR_MODE_DRAW : CURSOR_MODE_CAMERA, it->second.focus, cursorAlpha);
        }
        else {
            selfDrawCursor(it->second.position, it->second.dragged, secondaryCursorMode, it->second.focus, cursorAlpha);
        }
    }
}

void CloudsIntroSequence::selfDrawOverlay(){

#if defined(MOUSE_INPUT)
	
    drawMenu();
    
//	ofPushStyle();
////	string helpHoverText = GetTranslationForString("CLICK TO BEGIN");
//	string helpHoverText = GetTranslationForString("");
//	float helperTextOpacity = clickToBeginAlpha;
//	ofSetColor(255,helperTextOpacity*255);
//	helperFont.setLetterSpacing(titleTypeTracking*.7);
//	float centerX = ofGetWidth()/2  - helperFont.stringWidth(helpHoverText)/2;
//	float centerY = ofGetHeight()/2 - helperFont.stringHeight(helpHoverText)/2;
//	helperFont.drawString(helpHoverText, centerX, centerY + helperFont.stringHeight(helpHoverText)*10);
//	ofPopStyle();
    
    
#elif defined(OCULUS_RIFT)
	//no overlay
#else
	//kinect mode
	drawIntroNodes();
#endif
    
}

void CloudsIntroSequence::drawMenu(){

    ofPushStyle();
    
    ofSetColor(255);

    menuFont.drawString(GetTranslationForString("RESEARCH"), researchMenuItem.bounds.x, researchMenuItem.bounds.y);
    menuFont.drawString(GetTranslationForString("PLAY"),     playMenuItem.bounds.x, playMenuItem.bounds.y);
    menuFont.drawString(GetTranslationForString("ABOUT"),    aboutMenuItem.bounds.x, aboutMenuItem.bounds.y);
    
    ofPopStyle();
    
}

void CloudsIntroSequence::selfPostDraw(){
	
	CloudsVisualSystem::selfPostDraw();
#ifdef KINECT_INPUT
	//TODO: Move this to the general system drawing
	if(kinectHelperAlpha > 0.0){
		kinectHelperAlpha += (kinectHelperTargetAlpha-kinectHelperAlpha)*.05;
		ofPushStyle();
		ofSetColor(255,kinectHelperAlpha);
		((CloudsInputKinectOSC*) GetCloudsInput().get() )->debug(0,0, 200, 100);
	}
#endif

}

void CloudsIntroSequence::selfBegin(){
	timeline->stop();
	startedOnclick = false;
	selectedQuestion = NULL;
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].stopHovering();
	}

}

void CloudsIntroSequence::selfEnd(){
	
}

void CloudsIntroSequence::selfExit(){
	
}

void CloudsIntroSequence::selfKeyPressed(ofKeyEventArgs & args){

	if(args.key == 'q'){
		if(selectedQuestion != NULL){
			selectedQuestion->stopHovering();
		}
		selectedQuestion = NULL;
	}
	if(args.key == 'R'){
		reloadShaders();
	}

}

void CloudsIntroSequence::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsIntroSequence::selfMouseDragged(ofMouseEventArgs& data){
    // use CloudsInput directly to get the z value too.
    cursor.set(GetCloudsInput()->getPosition());
}

void CloudsIntroSequence::selfMouseMoved(ofMouseEventArgs& data){
    // use CloudsInput directly to get the z value too.
    cursor.set(GetCloudsInput()->getPosition());
	mouseLastMovedTime = ofGetElapsedTimef();
#ifdef MOUSE_INPUT
	if(!clickTextActive && startQuestions.size() > 0){
		clickTextActive = true;
		clickTextActiveTime = mouseLastMovedTime;
	}
#endif
}

void CloudsIntroSequence::selfMousePressed(ofMouseEventArgs& data){
#if defined(MOUSE_INPUT)
	if(!startedOnclick && startQuestions.size() > 0){
		//startedOnclick  = true;
		timeline->play();		
	}
	else{
		for(int i = 0; i < startQuestions.size(); i++){
			startQuestions[i].mousePressed(data);
		}
	}
#endif
}

void CloudsIntroSequence::selfMouseReleased(ofMouseEventArgs& data){
	
}


void CloudsIntroSequence::guiRenderEvent(ofxUIEventArgs &e){
	
}
