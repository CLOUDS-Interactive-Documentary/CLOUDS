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

//JG REMOVING THIS
CloudsVisualSystemEvents CloudsIntroSequence::events;

CloudsIntroSequence::CloudsIntroSequence(){
	selectedQuestion = NULL;
	showingQuestions = false;
	useDebugCamera = false;
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
}

CloudsIntroSequence::~CloudsIntroSequence(){
	
}

string CloudsIntroSequence::getSystemName(){
	return "_Intro";
}

void CloudsIntroSequence::selfSetup(){
    
    // Set question defaults.
    questionScale = 0.1f;
    questionFontSize = 12;
    questionLineLength = 300.0f;
    questionLineSpacing = 1.0f;
	questionMinZDistance = 50;
    rebuildQuestionFont();
	perlinOffset = 0;
	
    warpCamera.setNearClip(.01);
	
	ofDisableArbTex();
	sprite.loadImage(getVisualSystemDataPath() + "images/dot.png");
	ofEnableArbTex();
	
	currentFontExtrusion = -1;
	currentFontSize = -1;
	
	reloadShaders();
    
#ifdef OCULUS_RIFT
    bCursorInCenter = false;
    startTimeCursorInCenter = 0;
#endif

}

void CloudsIntroSequence::selfPresetLoaded(string presetPath){

	generateTunnel();
	warpCamera.setPosition(0, 0, 0);
	warpCamera.lookAt(ofVec3f(0,0,tunnelMax.z));
	positionStartQuestions();
}

void CloudsIntroSequence::reloadShaders(){
	tunnelShader.load(getVisualSystemDataPath() + "shaders/IntroTunnel");
	questionShader.load(getVisualSystemDataPath() + "shaders/Question");
	chroma.load("",getVisualSystemDataPath() + "shaders/BarrelChromaAb.fs");
	typeShader.load(getVisualSystemDataPath() + "shaders/IntroType");
	
	CloudsPortal::reloadShader();
}

void CloudsIntroSequence::rebuildQuestionFont(){
    if(bUseOculusRift){
        questionFont.loadFont(GetCloudsDataPath() + "font/MateriaPro_Regular.ttf", questionFontSize);
    }
    else{
        questionFont.loadFont(GetCloudsDataPath() + "font/materiapro_light.ttf", questionFontSize);
    }
    questionFont.setLineLength(questionLineLength);
    questionFont.setLineSpacing(questionLineSpacing);
}

void CloudsIntroSequence::selfUpdate(){
	
//	camera.applyRotation = camera.applyTranslation = useDebugCamera && !cursorIsOverGUI();
	
	if(!startedOnclick && timeline->getIsPlaying()){
		timeline->stop();
	}
	ofVec2f wobble = ofVec2f(ofSignedNoise(100 + ofGetElapsedTimef()*camWobbleSpeed),
							 ofSignedNoise(200 + ofGetElapsedTimef()*camWobbleSpeed)) * camWobbleRange;
	if(!paused){
		warpCamera.dolly(-cameraForwardSpeed);
		warpCamera.setPosition(wobble.x, wobble.y, warpCamera.getPosition().z);
	}
	else{
		warpCamera.setPosition(wobble.x, wobble.y, 0);
	}
	warpCamera.lookAt( ofVec3f(0, 0, warpCamera.getPosition().z + 50) );
    
#ifdef OCULUS_RIFT
    ofRectangle viewport = getOculusRift().getOculusViewport();

    // Trigger start manually
    if (!startedOnclick) {
        bool cursorNearCenter = cursor.distance(ofVec3f(viewport.getCenter().x, viewport.getCenter().y, cursor.z)) < 20;
        if (cursorNearCenter) {
            if (bCursorInCenter) {
                // already started, let's see if we've been there long enough
                if (ofGetElapsedTimef() - startTimeCursorInCenter > 1.25) {
                    ofMouseEventArgs args;
                    selfMousePressed(args);
                }
            }
            else {
                bCursorInCenter = true;
                startTimeCursorInCenter = ofGetElapsedTimef();
            }
        }
        else {
            bCursorInCenter = false;
            startTimeCursorInCenter = 0;
        }
    }
#endif
	
	for(int i = 0; i < startQuestions.size(); i++){
		
		startQuestions[i].scale = questionScale;
		startQuestions[i].update();
		
		if(startQuestions[i].hoverPosition.z < warpCamera.getPosition().z){
			startQuestions[i].hoverPosition.z += questionWrapDistance;
		}
		
		if(startQuestions[i].hoverPosition.z - warpCamera.getPosition().z < questionMinZDistance){
#ifdef OCULUS_RIFT
            ofVec3f screenPos = getOculusRift().worldToScreen(startQuestions[i].hoverPosition, true);
            float distanceToQuestion = ofDist(screenPos.x, screenPos.y,
                                              viewport.getCenter().x, viewport.getCenter().y);
#else
//			ofVec2f mouseNode(GetCloudsInputX(),GetCloudsInputY());
            ofVec2f mouseNode = cursor;
			float distanceToQuestion = startQuestions[i].screenPosition.distance(mouseNode);
#endif
			if(caughtQuestion == NULL){
				if( distanceToQuestion < questionTugDistance.max ){
					startQuestions[i].hoverPosition.z += ofMap(distanceToQuestion,
															   questionTugDistance.max, questionTugDistance.min,
															   0, cameraForwardSpeed);
					if(distanceToQuestion < questionTugDistance.min){
						caughtQuestion = &startQuestions[i];
						if (caughtQuestion->startHovering()) {
                            CloudsPortalEventArgs args(startQuestions[i], getQuestionText());
                            ofNotifyEvent(events.portalHoverBegan, args);
                        }
					}
				}
			}
			//we have a caught question make sure it's still close
			else if(caughtQuestion == &startQuestions[i]){
				startQuestions[i].hoverPosition.z += cameraForwardSpeed;
				if( caughtQuestion->isSelected() && !bQuestionDebug){
					selectedQuestion = caughtQuestion;
				}
				//TODO consider MAX distance here.....
				else if(distanceToQuestion > questionTugDistance.max){
					caughtQuestion->stopHovering();
					caughtQuestion = NULL;
                    CloudsPortalEventArgs args(startQuestions[i], getQuestionText());
                    ofNotifyEvent(events.portalHoverEnded, args);
				}
			}
		}
	}
	
	if(currentFontSize != titleFontSize ||
	   currentFontExtrusion != titleFontExtrude)
	{
		currentFontSize = titleFontExtrude;
		currentFontExtrusion = titleFontExtrude;
		extrudedTitleText.loadFont(GetCloudsDataPath() + "font/materiapro_light.ttf", titleFontSize, currentFontExtrusion);
	}
	
	titleRect = ofRectangle(0,0,titleRectWidth*ofGetWidth(),titleRectHeight*ofGetHeight());
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
	//currentTitleOpacity = hoverTitleOpacity;
    
    //always move the questions in the direction of the camera
    for(int i = 0; i < startQuestions.size(); i++){
        if(&startQuestions[i] != caughtQuestion){
            startQuestions[i].hoverPosition.z += cameraForwardSpeed*.75;
        }
    }
}

void CloudsIntroSequence::setStartQuestions(vector<CloudsClip>& possibleStartQuestions){

	selectedQuestion = NULL;
	startQuestions.clear();
	
	for(int i = 0; i < possibleStartQuestions.size(); i++){
		
		CloudsPortal q;
		q.cam = &warpCamera;
		q.font = &questionFont;
		q.clip = possibleStartQuestions[i];
		q.topic = q.clip.getAllTopicsWithQuestion()[0];
		q.question = q.clip.getQuestionForTopic(q.topic);

		q.setup();
		
		startQuestions.push_back(q);
	}
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

	//set the start questions along a random tunnel
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].hoverPosition = ofVec3f(0, ofRandom(questionTunnelInnerRadius, tunnelMax.y), 0);
		startQuestions[i].hoverPosition.rotate(ofRandom(360), ofVec3f(0,0,1));
		startQuestions[i].hoverPosition.z = tunnelMax.z*.5 + ofRandom(questionWrapDistance);
        // TODO: Figure out if orientToCenter() is still necessary
//		startQuestions[i].orientToCenter();
	}
	
}

bool CloudsIntroSequence::istStartQuestionHovering(){
	caughtQuestion != NULL;
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
			if(startQuestions[i].hoverPosition.z - warpCamera.getPosition().z < questionMinZDistance){
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
	//testing for now
	
	CloudsVisualSystem::timelineBangEvent(args);
	if(args.flag == "LOOP_END"){
		ofxTLFlags* flags = (ofxTLFlags*)timeline->getTrack("Intro-Outro");
		ofxTLFlag* flag = flags->getFlagWithKey("LOOP_START");
		if(flag != NULL){
			timeline->stop();
			timeline->setCurrentTimeMillis(flag->time);
			timeline->play();
		}
		
		//selectedQuestion = &startQuestions[0];
		showingQuestions = true;
	}
}

void CloudsIntroSequence::selfDraw(){
#if defined(OCULUS_RIFT) && defined(CLOUDS_APP)
    if (hud != NULL) {
        if(selectedQuestion != NULL){
            hud->draw3D(getOculusRift().baseCamera, ofVec2f(0, -selectedQuestion->screenPosition.y/2));
        }
        else if(caughtQuestion != NULL){
            hud->draw3D(getOculusRift().baseCamera, ofVec2f(0, -caughtQuestion->screenPosition.y/2));
        }
        else{
            hud->draw3D(getOculusRift().baseCamera);
        }
    }
#endif
    
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
	glEnable(GL_POINT_SMOOTH);
	glDisable(GL_DEPTH_TEST);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	drawCloudsType();
	
	ofPushStyle();
	
	tunnelShader.begin();
	tunnelShader.setUniform1f("minPointSize", pointSize.min);
	tunnelShader.setUniform1f("maxPointSize", pointSize.max);
	tunnelShader.setUniform1f("minDistance", distanceRange.min);

	if(bUseOculusRift){
		tunnelShader.setUniform1f("maxDistance", distanceRange.max + currentTitleOpacity * 120.);
	}
	else{
		tunnelShader.setUniform1f("maxDistance", distanceRange.max + currentTitleOpacity * 200.);
	}
	
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
	extrudedTitleText.setTracking( titleTypeTracking );
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

void CloudsIntroSequence::selfDrawOverlay(){
//	if(bUseOculusRift){
//		ofPushStyle();
//		for(int i = 0; i < startQuestions.size(); i++){
//			startQuestions[i].drawOverlay(true);
//		}
//		ofPopStyle();
//	}
}

void CloudsIntroSequence::selfPostDraw(){
//	chroma.begin();
//	chroma.setUniform2f("resolution", ofGetWidth(),ofGetHeight());
//	chroma.setUniform1f("max_distort", maxChromaDistort);
	CloudsVisualSystem::selfPostDraw();
//	chroma.end();
	if(!bUseOculusRift){
		//JG: MOVING TO HUD
//		ofPushStyle();
//		for(int i = 0; i < startQuestions.size(); i++){
//			startQuestions[i].drawOverlay();
//		}
//		ofPopStyle();
	}
}

void CloudsIntroSequence::selfExit(){
	
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

void CloudsIntroSequence::selfKeyPressed(ofKeyEventArgs & args){

	if(args.key == 'q'){
		//DEBUG selected question eject
		if(selectedQuestion != NULL){
			selectedQuestion->stopHovering();
		}
		selectedQuestion = NULL;
	}
	if(args.key == 'R'){
		reloadShaders();
	}
//    if (args.key == 'a') {
//        cursor.z += 0.1;
//    }
//    if (args.key == 'z') {
//        cursor.z -= 0.1;
//    }

}

void CloudsIntroSequence::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsIntroSequence::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfMouseMoved(ofMouseEventArgs& data)
{
    cursor.set(data.x, data.y, cursor.z);
}

void CloudsIntroSequence::selfMousePressed(ofMouseEventArgs& data){
	if(!startedOnclick){
		startedOnclick  = true;
		timeline->play();
	}
}

void CloudsIntroSequence::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "arrange questions" && ((ofxUIButton*)e.widget)->getValue()){
		positionStartQuestions();
	}
	else if(e.widget->getName() == "generate tunnel" && ((ofxUIButton*)e.widget)->getValue()){
		generateTunnel();
	}
    
    else if (e.widget->getName() == "Question Font Size" ||
             e.widget->getName() == "Question Line Length" ||
             e.widget->getName() == "Question Line Spacing") {
        rebuildQuestionFont();
    }
}

//void CloudsIntroSequence::pauseAtBeginning(){
//	warpCamera.setPosition(0, 0, 0);
//	warpCamera.lookAt(ofVec3f(0,0,tunnelMax.z));
//	paused = !paused;
//}

void CloudsIntroSequence::selfSetupSystemGui(){
	sysGui->addButton("reset debug camera", false);
	sysGui->addButton("reload shader", false);
}

void CloudsIntroSequence::selfSetupCameraGui(){
	camGui->addToggle("use debug camera", &useDebugCamera);
//	camGui->addSlider("debug camera speed", 1, 20, &camera.speed);
	camGui->addSlider("camera fwd force", 0, 2, &cameraForwardSpeed);
	camGui->addSlider("camera wobble range", 0, 10, &camWobbleRange);
	camGui->addSlider("camera wobble speed", 0, 1., &camWobbleSpeed);


	camGui->addToggle("hold camera", &paused);

}

void CloudsIntroSequence::guiSystemEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "reload shader" && ((ofxUIButton*)e.widget)->getValue()){
		cout << "Loaded shader" << endl;
		reloadShaders();
	}
	else if(e.widget->getName() == "reset debug camera" && ((ofxUIButton*)e.widget)->getValue()){
//		camera.setPosition(0, 0, 0);
//		camera.setOrientation(ofQuaternion());
//		camera.rotate(180, ofVec3f(0,1,0));
		//camera.setAnglesFromOrientation();
	}
}

void CloudsIntroSequence::selfSetupRenderGui(){
	
//	rdrGui->addSlider("Min Point Size", 0, 7, &pointSize.min);
	rdrGui->addSlider("Max Point Size", 0, 7, &pointSize.max);
	rdrGui->addSlider("Min Distance", 0, 500, &distanceRange.min);
	rdrGui->addSlider("Max Distance", 0, 500, &distanceRange.max);

	rdrGui->addSlider("Chroma Max Distort", 0, 10, &maxChromaDistort);
	
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
	questionGui->addSlider("Question Min Z", 10, 100, &questionMinZDistance);
	questionGui->addSlider("Inner Radius", 2, 20, &questionTunnelInnerRadius);
	questionGui->addRangeSlider("Tug Distance", 10, 300, &questionTugDistance.min, &questionTugDistance.max);
	questionGui->addRangeSlider("Attenuate Distance", 10, 300,&questionAttenuateDistance.min,&questionAttenuateDistance.max);
	
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
	tunnelGui->addSlider("Loose Rez Z", 5, 50, &looseTunnelResolutionZ);
//	tunnelGui->addSlider("Tight Rez Mult", 1, 5, &tightResMult);
    
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

	
}

void CloudsIntroSequence::guiRenderEvent(ofxUIEventArgs &e){
	
}

