
#include "CloudsVisualSystemRGBD.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
string CloudsVisualSystemRGBD::getSystemName(){
	return "RGBD";
}

void CloudsVisualSystemRGBD::selfSetDefaults(){
	visualSystemFadeValue = 1.0;

	drawRGBD = true;
	
	questionLifeSpan = 3;
	transitioning = transitioningIn = transitioningOut = false;
	
	transitionInStart.setPosition( 0, 0, -1000 );
	transitionInStart.rotate( 180, ofVec3f(0, 1, 0 ) );
	
	transitionOutTarget.setPosition( 0, 0, -1001 );
	transitionOutTarget.rotate( 180, ofVec3f(0, 1, 0 ) );
	
	transitionCam.useArrowKeys = true;
	transitionTarget = &transitionOutTarget;
	drawTransitionNodes = false;
	
//	captionFontSize = 12;
	
	edgeAttenuate = 0.;
	skinBrightness = 0.;
	
	drawPoints = true;
	refreshPointcloud = true;
	numRandomPoints = 20000;
	pointSize.min = 1.0;
	pointSize.max = 3.0;
	pointAlpha = 1.0;
	pointFlowPosition = 0.0;
	pointFlowSpeed = 0.0;
	pointsFlowUp = false;
	
	drawLines = true;
	lineAlpha = .5;
	lineThickness	= 1.0;
	lineFlowPosition = 0.;
	lineFlowSpeed = 0.;
	linesFlowUp = false;
	refreshLines = true;
	
	drawMesh = true;
	xSimplify = 2.0;
	ySimplify = 2.0;
	randomVariance = 1.0;

	meshFaceMinRadius = 0.0;
	meshFaceFalloff = 0.0;
	meshRetractionFalloff = 1.0;
	meshForceGeoRectraction = .0;
	
	
	//transition editing
	placingTransitionNodes = false;
	bSaveTransition = false;
	transitionCamTargetNode = NULL;

}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetup(){
	
	loadShader();
	
	generateLines();
	generatePoints();
	generateMesh();
		
//	particulateController.setParticleCount(20000);
//	particulateController.setShaderDirectory(GetCloudsDataPath() + "shaders/GPUParticles/");
//	particulateController.setup();
	
	cloudsCamera.setup();
	cloudsCamera.lookTarget = ofVec3f(0,25,0);
			
	displayFont.loadFont(GetCloudsDataPath() + "font/materiapro_light.ttf", 14);
	
	transitionCam.setup();
	transitionCam.applyTranslation = true;
	transitionCam.applyRotation = true;
	
//    rebuildCaptionFont();
	
	bTransitionIn = bTransitionOut = false;
	
	loadTransitionSettings("Transitions");
}

void CloudsVisualSystemRGBD::playTestVideo(){

	if(ofFile::doesFileExist("TestVideo/Lindsay_memes_2.mov")){
		getRGBDVideoPlayer().setup("TestVideo/Lindsay_memes_2.mov",
								   "TestVideo/Lindsay_memes_2.xml", 0, 0  );
		getRGBDVideoPlayer().swapAndPlay();
	}
}

void CloudsVisualSystemRGBD::loadShader(){
	cout << "loading point shader " << endl;
	pointShader.load(getVisualSystemDataPath() + "shaders/rgbdPoints");
	cout << "loading line shader " << endl;
	lineShader.load( getVisualSystemDataPath() + "shaders/rgbdLines");
	cout << "loading mesh shader " << endl;
	meshShader.load( getVisualSystemDataPath() + "shaders/rgbdMesh");
//	CloudsQuestion::reloadShader();
}

//void CloudsVisualSystemRGBD::rebuildCaptionFont(){
//    if(bUseOculusRift){
//        captionFont.loadFont(GetCloudsDataPath() + "font/MateriaPro_Regular.ttf", captionFontSize);
//    }
//    else{
//        captionFont.loadFont(GetCloudsDataPath() + "font/materiapro_light.ttf", captionFontSize);
//    }
//}

void CloudsVisualSystemRGBD::setTransitionNodes( RGBDTransitionType transitionType ){
	//TODO: SAVE THESE VALUES TO getVisualSystemDataPath()/transitionTargets/
	switch (transitionType) {
		case CloudsVisualSystem::TWO_DIMENSIONAL:
			transitionInStart.setPosition(-1.61592, -80.692, 36.2162);
			transitionInStart.setOrientation( ofQuaternion( 0.0497688, 0.837992, 0.542999, -0.0213495) );
			
			transitionOutTarget.setPosition(-1.61592, -80.692, 36.2162);
			transitionOutTarget.setOrientation(ofQuaternion( 0.0497688, 0.837992, 0.542999, -0.0213495) );
			
			break;
			
		case CloudsVisualSystem::WHIP_PAN:
			transitionInStart.setPosition(8.48001e-05, 0, -30);
			transitionInStart.setOrientation( ofQuaternion( -0.0344557, -0.751671, -0.0393931, 0.657458) );
			
			transitionOutTarget.setPosition(8.48001e-05, 0, -31);
			transitionOutTarget.setOrientation( ofQuaternion( -0.0278925, -0.613558, 0.0216942, -0.788858) );
			
			break;
			
			
		case CloudsVisualSystem::FLY_THROUGH:
			transitionInStart.setPosition(-334.72, 1.57074, -941.388);
			transitionInStart.setOrientation( ofQuaternion( -0.00373021, -0.990374, -0.135664, 0.0272313) );
			
			transitionOutTarget.setPosition(618.12, 1.75281, 614.075);
			transitionOutTarget.setOrientation( ofQuaternion( 0.0769274, -0.91773, 0.219058, 0.322284) );
			
			break;
			
		default:
			break;
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupGuis(){

	globalMeshGui = new ofxUISuperCanvas("GLOBAL MESH", gui);
	globalMeshGui->copyCanvasStyle(gui);
    globalMeshGui->copyCanvasProperties(gui);
    globalMeshGui->setName("GLOBAL MESH");
    globalMeshGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	ofxUIToggle *toggle;
	toggle = globalMeshGui->addToggle("ENABLE", &drawRGBD);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    globalMeshGui->resetPlacer();
    globalMeshGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    globalMeshGui->addWidgetToHeader(toggle);
	globalMeshGui->addSlider("Bottom Edge Falloff", 0, 1.0, &edgeAttenuate);
	globalMeshGui->addSlider("Bottom Edge Expo", 1, 5.0, &edgeAttenuateExponent);
		
	ofAddListener(globalMeshGui->newGUIEvent, this, &CloudsVisualSystemRGBD::selfGuiEvent);
	guis.push_back(globalMeshGui);
	guimap[globalMeshGui->getName()] = globalMeshGui;

	pointsGui = new ofxUISuperCanvas("POINTS", gui);
	pointsGui->copyCanvasStyle(gui);
    pointsGui->copyCanvasProperties(gui);
    pointsGui->setName("Points");
    pointsGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	toggle = pointsGui->addToggle("ENABLE", &drawPoints);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    pointsGui->resetPlacer();
    pointsGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    pointsGui->addWidgetToHeader(toggle);
	
	pointsGui->addSlider("Point Alpha", 0, 1.0, &pointAlpha);
	pointsGui->addIntSlider("Num Points", 0, 100000, &numRandomPoints);
	pointsGui->addRangeSlider("Point Size", 0.0, 3.0, &pointSize.min, &pointSize.max);
	pointsGui->addSlider("Point Face Overlap",0., 1.0, &pointHeadOverlap);
	pointsGui->addSlider("Point Flow", 0, 1.0, &pointFlowSpeed);
	pointsGui->addToggle("Points Flow Up", &pointsFlowUp);
	
	ofAddListener(pointsGui->newGUIEvent, this, &CloudsVisualSystemRGBD::selfGuiEvent);
	guis.push_back(pointsGui);
	guimap[pointsGui->getName()] = pointsGui;
	
	linesGui = new ofxUISuperCanvas("LINES", gui);
	linesGui->copyCanvasStyle(gui);
    linesGui->copyCanvasProperties(gui);
    linesGui->setName("Lines");
    linesGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	toggle = linesGui->addToggle("ENABLE", &drawLines);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    linesGui->resetPlacer();
    linesGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    linesGui->addWidgetToHeader(toggle);
	
	linesGui->addSlider("Line Alpha", 0, 1.0, &lineAlpha);
	linesGui->addSlider("Line Thickness", 0, 3.0, &lineThickness);
	linesGui->addSlider("Line Spacing", 1., 16., &lineSpacing);
	linesGui->addSlider("Line Face Overlap", 0., 1.0, &lineHeadOverlap);
	linesGui->addSlider("Line Granularity", 1., 10.0, &lineGranularity);
	linesGui->addSlider("Line Flow", 0, 1.0, &lineFlowSpeed);
	linesGui->addToggle("Lines Flow Up", &linesFlowUp);
	ofAddListener(linesGui->newGUIEvent, this, &CloudsVisualSystemRGBD::selfGuiEvent);
	guis.push_back(linesGui);
	guimap[linesGui->getName()] = linesGui;

	
	meshGui = new ofxUISuperCanvas("MESH", gui);
	meshGui->copyCanvasStyle(gui);
    meshGui->copyCanvasProperties(gui);
    meshGui->setName("Mesh");
    meshGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	toggle = meshGui->addToggle("ENABLE", &drawMesh);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    meshGui->resetPlacer();
    meshGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    meshGui->addWidgetToHeader(toggle);

	meshGui->addSlider("Mesh Alpha", 0., 1.0, &meshAlpha);
	meshGui->addSlider("X Simplify", 1., 16., &xSimplify);
	meshGui->addSlider("Y Simplify", 1., 16., &ySimplify);
	meshGui->addSlider("Random Variance", 0, 10.0, &randomVariance);
	meshGui->addSlider("Face Min Radius", 0, 600., &meshFaceMinRadius);
	meshGui->addSlider("Face Falloff", 0, 600., &meshFaceFalloff);
	meshGui->addSlider("Edge Geo Retraction", 0, 1.0, &meshRetractionFalloff);
	meshGui->addSlider("Force Geo Retraction", 0, 1.0, &meshForceGeoRectraction);
	ofAddListener(meshGui->newGUIEvent, this, &CloudsVisualSystemRGBD::selfGuiEvent);
	guis.push_back(meshGui);
	guimap[meshGui->getName()] = meshGui;
	
	cameraGui = new ofxUISuperCanvas("CAMERA", gui);
	cameraGui->copyCanvasStyle(gui);
    cameraGui->copyCanvasProperties(gui);
    cameraGui->setName("Camera");
    cameraGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	cameraGui->addLabel("OFFSETS");
	cameraGui->addSlider("FRONT DISTANCE", 50, 200, &cloudsCamera.frontDistance);
	cameraGui->addSlider("SIDE DISTANCE", 20, 200, &cloudsCamera.sideDistance);
	cameraGui->addSlider("SIDE PULLBACK", -200, 200, &cloudsCamera.sidePullback);
	cameraGui->addSlider("LIFT RANGE", 0, 100, &cloudsCamera.liftRange);
	cameraGui->addSlider("LIFT AMOUNT", 10, 200, &cloudsCamera.liftAmount);
	cameraGui->addSlider("DROP AMOUNT", 0, 200, &cloudsCamera.dropAmount);
	cameraGui->addSlider("DRIFT ANGLE", 0, 200, &cloudsCamera.maxDriftAngle);
	cameraGui->addSlider("DRIFT DENSITY", 0, 1.0, &cloudsCamera.driftNoiseDensity);
	cameraGui->addSlider("DRIFT SPEED", 0, .01, &attenuatedCameraDrift);
	
	guis.push_back(cameraGui);
	guimap[meshGui->getName()] = cameraGui;
	
	particleGui = new ofxUISuperCanvas("PARTICLE", gui);
	particleGui->copyCanvasStyle(gui);
    particleGui->copyCanvasProperties(gui);
    particleGui->setName("Particle");
    particleGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	particleGui->addToggle("DRAW PARTICLES", &drawParticulate);
	
	particleGui->addSlider("BIRTH RATE", 0, .01, &particulateController.birthRate);
	particleGui->addSlider("BIRTH SPREAD", 10, 10000, &particulateController.birthSpread);
	particleGui->addSlider("POINT SIZE THRESHOLD", 0, .01, &particulateController.getPoints().sizeThreshold);
	
	particleGui->addSlider("POINT COLOR H", 0, 1.0, &pointColor.x);
	particleGui->addSlider("POINT COLOR S", 0, 1.0, &pointColor.y);
	particleGui->addSlider("POINT COLOR V", 0, 1.0, &pointColor.z);
	particleGui->addSlider("POINT COLOR A", 0, 1.0, &pointColor.w);
	
	guis.push_back(particleGui);
	guimap[meshGui->getName()] = particleGui;
	
	questionGui = new ofxUISuperCanvas("QUESTIONS", gui);
	questionGui->copyCanvasStyle(gui);
    questionGui->copyCanvasProperties(gui);
    questionGui->setName("Questions");
    questionGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	//center point, max drift
	//questionGui->add2DPad("XZ",	ofVec3f(0, -400), ofVec3f(-200, 200), &questionXZ);
	questionGui->addSlider("Position X", 0, -400, &questionXZ.x);
	questionGui->addSlider("Position Z", -200, 200, &questionXZ.z);
	questionGui->addSlider("Drift Range", 40, 200, &questionDriftRange);
	questionGui->addSlider("Y Range", 40, 200, &questionYRange);
	questionGui->addSlider("Y Start", -50, 50, &questionYCenter);
	
	questionGui->addSlider("Base Color H", 0, 1., &questionBaseHSB.r);
	questionGui->addSlider("Base Color S", 0, 1., &questionBaseHSB.g);
	questionGui->addSlider("Base Color B", 0, 1., &questionBaseHSB.b);
	
	questionGui->addSlider("Hover Color H", 0, 1., &questionHoverHSB.r);
	questionGui->addSlider("Hover Color S", 0, 1., &questionHoverHSB.g);
	questionGui->addSlider("Hover Color B", 0, 1., &questionHoverHSB.b);

	CloudsQuestion::addQuestionVariables( questionGui );
	
	guis.push_back(questionGui);
	guimap[meshGui->getName()] = questionGui;
	
	
	//transitionEditorGui
	transitionEditorGui = new ofxUISuperCanvas("TRANSITIONEDITOR", gui);
	transitionEditorGui->copyCanvasStyle(gui);
    transitionEditorGui->copyCanvasProperties(gui);
    transitionEditorGui->setName("TransitionEditor");
    transitionEditorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	transitionEditorGui->addToggle("placingTransitionNodes", &placingTransitionNodes);
	
	transitionEditorGui->addSpacer();
	
	transitionEditorGui->addToggle("drawTransitionNodes", &drawTransitionNodes);
	
	transitionEditorGui->addSpacer();
		
	transitionEditorGui->addToggle("lookThoughIN", &bLookThroughIn )->setColorBack(ofColor(0,255,255));
	transitionEditorGui->addToggle("lookThoughOUT", &bLookThroughOut )->setColorBack(ofColor(255,255,0));
//	transitionEditorGui->addToggle("resetNodes", &bResetLookThoughs);
	
	transitionEditorGui->addSpacer();
	
	transitionEditorGui->addToggle("saveTransition", &bSaveTransition);
	transitionEditorGui->addSpacer();
	
	vector<string> transitionNames;
	for(auto &cur_pair: transitionMap) { transitionNames.push_back((cur_pair.first)); }
	
	transitionEditorGui->addLabel("transitionTypes")->setColorFill(ofColor(100));
	transitionEditorGui->addRadio("loadTransition", transitionNames);
	
	
	CloudsQuestion::addQuestionVariables( transitionEditorGui );
	
	guis.push_back(transitionEditorGui);
	guimap[transitionEditorGui->getName()] = transitionEditorGui;
	
	ofAddListener(transitionEditorGui->newGUIEvent, this, &CloudsVisualSystemRGBD::selfGuiEvent);
	
//	connectorGui = new ofxUISuperCanvas("CONNECTORS", gui);
//	connectorGui->copyCanvasStyle(gui);
//	connectorGui->copyCanvasProperties(gui);
//	connectorGui->setName("connectors");
//	connectorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
//	connectorGui->addSlider("Num Particles", 50, 64*64, &generator.numParticles);
//	connectorGui->addToggle("Draw Connections", &generator.drawConnections);
//	connectorGui->addSlider("Min Connection Distance", 1, 100, &generator.minDistance);
//	connectorGui->addSlider("Boundary Size", 100, 1000, &generator.boundarySize);
//	connectorGui->addSlider("Max Connections", 1, 10, &generator.maxConnections);
	
//	guis.push_back(connectorGui);
//	guimap[connectorGui->getName()] = connectorGui;
}

void CloudsVisualSystemRGBD::updateTransitionGui()
{
	//add any newly saved transitions to out radio
	ofxUIRadio* transitionRadio = (ofxUIRadio*)transitionEditorGui->getWidget("loadTransition");

	vector <string> existingNames;
	for(auto &it: transitionRadio->getToggles() )
	{
		existingNames.push_back( it->getName() );
	}
	
	int yPos = transitionRadio->getRect()->getMaxY();
	for(auto &it: transitionMap)
	{
		if( find(existingNames.begin(), existingNames.end(), it.first) == existingNames.end())
		{
			//cout << "add a new toggle here " << it.first << endl;
			ofxUIToggle* t = transitionEditorGui->addToggle(it.first, true);
			transitionRadio->addToggle(t);
			
			transitionEditorGui->autoSizeToFitWidgets();
		}
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfUpdate(){
	
	if(numRandomPoints != points.getNumVertices()){
		generatePoints();
	}
	
	if(refreshLines){
		generateLines();
	}

	if(refreshMesh){
		generateMesh();
	}

	lineFlowPosition += powf(lineFlowSpeed,2.0);
	pointFlowPosition += powf(pointFlowSpeed,2.0);
	
	if(drawParticulate){
		
//		particulateController.birthPlace = translatedHeadPosition;
		
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
//		particulateController.getPoints().color = ofFloatColor::fromHsb(pointColor.x, pointColor.y, pointColor.z);
//		particulateController.getPoints().color.a = pointColor.w;
//		
//		particulateController.update();
	}
	
	updateQuestions();
	updateTransition();
    
//    cloudsCaption.update();
	
	if( placingTransitionNodes )
	{
		transitionCam.applyTranslation = transitionCam.applyRotation = !cursorIsOverGUI();
		
		if(bLookThroughIn)
		{
			lookThroughTransitionIn();
			bLookThroughIn = false;
		}
		else if(bLookThroughOut)
		{
			lookThroughTransitionOut();
			bLookThroughOut = false;
		}
		
		if (bResetLookThoughs)
		{
			bResetLookThoughs = false;
			setTransitionNodes(activeTransition);
		}
		
		
		if(transitionCamTargetNode)
		{
			if(ofGetFrameNum()%30 == 0)
			{
//				cout << "wTF" << endl;
			}
			transitionCamTargetNode->setPosition( transitionCam.getPosition() );
			transitionCamTargetNode->setOrientation( transitionCam.getOrientationQuat() );
		}

	}
	else{
		cloudsCamera.driftNoiseSpeed = caughtQuestion ? 0 : attenuatedCameraDrift;
	}
	
	
	if(bSaveTransition)
	{
		bSaveTransition = false;
	
		saveTransitionSettings( ofSystemTextBoxDialog("save transition", "transitionType") );
		
		updateTransitionGui();
	}
}

//save and load transitions
//------------------------------------------------------------------------
void CloudsVisualSystemRGBD::loadTransitionSettings(string filename)
{	
	transitionMap.clear();
	
	ofxXmlSettings *XML = new ofxXmlSettings();
	XML->loadFile( GetCloudsDataPath() + "transitions/" + filename + ".xml" );
	
	int numTransitions = XML->getNumTags("TRANSITION");
	for(int i = 0; i < numTransitions; i++)
	{
		XML->pushTag("TRANSITION", i);
		string name = XML->getValue("Name", "NULL", 0);
		
		transitionMap[name].name = name;
		
		XML->pushTag("InStartPos");
		transitionMap[name].inStartPos.set( XML->getValue("x", 0. ), XML->getValue("y", 0. ), XML->getValue("z", 0. ) );
		XML->popTag();
		
		XML->pushTag("InStartQuat");
		transitionMap[name].inStartQuat.set( XML->getValue("x", 0. ), XML->getValue("y", 0. ), XML->getValue("z", 0. ), XML->getValue("w", 0. ) );
		XML->popTag();
		
		XML->pushTag("OutTargetPos");
		transitionMap[name].inStartPos.set( XML->getValue("x", 0. ), XML->getValue("y", 0. ), XML->getValue("z", 0. ) );
		XML->popTag();
		
		XML->pushTag("OutTargetQuat");
		transitionMap[name].inStartQuat.set( XML->getValue("x", 0. ), XML->getValue("y", 0. ), XML->getValue("z", 0. ), XML->getValue("w", 0. ) );
		XML->popTag();
		
		XML->popTag();
	}
	
	delete XML;
}

void CloudsVisualSystemRGBD::setTransitionNodes( string transitionName )
{
	if(transitionMap.find(activeTransition) != transitionMap.end())
	{
		transitionInStart.setPosition( transitionMap[activeTransition].inStartPos);// + translatedHeadPosition );
		transitionInStart.setOrientation( transitionMap[activeTransition].inStartQuat );
		
		transitionOutTarget.setPosition( transitionMap[activeTransition].outTargetPos);// + translatedHeadPosition );
		transitionOutTarget.setOrientation( transitionMap[activeTransition].outTargetQuat );
	}
}

void CloudsVisualSystemRGBD::saveTransitionSettings(string transitionName)
{
	//save positions relative to head position
	transitionMap[transitionName].name = transitionName;
	
	transitionMap[transitionName].inStartPos = transitionInStart.getPosition();// - translatedHeadPosition;
	transitionMap[transitionName].inStartQuat = transitionInStart.getOrientationQuat();
	
	transitionMap[transitionName].outTargetPos = transitionOutTarget.getPosition();// - translatedHeadPosition;
	transitionMap[transitionName].outTargetQuat = transitionOutTarget.getOrientationQuat().asVec4();
	
	
	//cout << "saving transitions settings " + transitionName << endl;
	ofxXmlSettings *XML = new ofxXmlSettings();
	
	int transitionIndex = 0;
	for(map<string, TransitionInfo>::iterator it=transitionMap.begin(); it != transitionMap.end(); it++)
	{	
		ofVec4f inQ = it->second.inStartQuat.asVec4();
		ofVec4f outQ = it->second.outTargetQuat.asVec4();
		
		cout << "inQ: " << inQ << endl;
		cout << "outQ: " << outQ << endl;
		
		XML->addTag("TRANSITION");
		XML->pushTag("TRANSITION", transitionIndex);
		transitionIndex++;
		
		XML->setValue("Name", it->second.name );

		XML->addTag("InStartPos");
		XML->pushTag("InStartPos");
		XML->setValue("x", it->second.inStartPos.x);
		XML->setValue("y", it->second.inStartPos.y);
		XML->setValue("z", it->second.inStartPos.z);
		XML->popTag();

		XML->addTag("InStartQuat");
		XML->pushTag("InStartQuat");
		XML->setValue("x", inQ.x);
		XML->setValue("y", inQ.y);
		XML->setValue("z", inQ.z);
		XML->setValue("w", inQ.w);
		XML->popTag();

		XML->addTag("OutTargetPos");
		XML->pushTag("OutTargetPos");
		XML->setValue("x", it->second.outTargetPos.x);
		XML->setValue("y", it->second.outTargetPos.y);
		XML->setValue("z", it->second.outTargetPos.z);
		XML->popTag();

		XML->addTag("OutTargetQuat");
		XML->pushTag("OutTargetQuat");
		XML->setValue("x", outQ.x);
		XML->setValue("y", outQ.y);
		XML->setValue("z", outQ.z);
		XML->setValue("w", outQ.w);
		XML->popTag();
		
		XML->popTag();
	}

	XML->saveFile(GetCloudsDataPath() + "transitions/Transitions.xml" );
	delete XML;
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::addFakeQuestion(vector<string> testPngFilePaths){
	CloudsQuestion* q = new CloudsQuestion();
	q->cam = &cloudsCamera;
	q->font = &displayFont;
	q->topic = "no topic";
	q->question = testPngFilePaths[0];
	q->testFiles = testPngFilePaths;
	
	ofVec3f startPosition = ofVec3f(questionXZ.x, questionYCenter, questionXZ.z)
	+ ofVec3f(ofRandom(-questionDriftRange,questionDriftRange),
			  ofRandom(-questionYRange,questionYRange),
			  ofRandom(-questionDriftRange,questionDriftRange));
	
	q->position = translatedHeadPosition + startPosition;
	q->birthTime = ofGetElapsedTimef();
	
	q->setup();
	
	questions.push_back(q);
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::addQuestion(CloudsClip& questionClip, string topic, string question){
	
	for(int i = 0; i < questions.size(); i++){
		if(questionClip.getID() == questions[i]->clip.getID()){
			//don't add duplicate questions
            ofLogError()<<"Duplicate question, ignoring"<<endl;
			return;
		}
	}
	
	CloudsQuestion* q = new CloudsQuestion();
	q->cam = &cloudsCamera;
	q->font = &displayFont;
	q->clip = questionClip;
	q->topic = topic;
	q->question = question;
	
	cout << "adding question " << q->clip.getLinkName() << " " << q->question << " " << q->topic << endl;
	
	ofVec3f startPosition = ofVec3f(questionXZ.x, questionYCenter, questionXZ.z)
								+ ofVec3f(ofRandom(-questionDriftRange,questionDriftRange),
										  ofRandom(-questionYRange,questionYRange),
										  ofRandom(-questionDriftRange,questionDriftRange));
	
	q->position = translatedHeadPosition + startPosition;
	q->birthTime = ofGetElapsedTimef();
	
	q->setup();
	
	questions.push_back(q);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::updateQuestions(){

	for(int i = questions.size()-1; i >= 0; i--){
	
		questions[i]->update();
		
        if(selectedQuestion == NULL && questions[i]->isSelected() && GetCloudsInputPressed()){
            selectedQuestion = questions[i];
			selectedQuestion->lockHover = true;
			break;
        }

		if(caughtQuestion == NULL){
			questions[i]->enableHover();
		
			if(questions[i]->hovering){
				caughtQuestion = questions[i];
			}
		}
		
		if(caughtQuestion != NULL) {
			if(questions[i] == caughtQuestion){
				if(!caughtQuestion->hovering){
					caughtQuestion = NULL;
				}
			}
			else {
				questions[i]->disableHover();
			}
		}
	}
	
	if(selectedQuestion != NULL){
		for(int i = questions.size()-1; i >= 0; i--){
			if(selectedQuestion != questions[i]){
				if(!questions[i]->isDestroyed){
					questions[i]->destroy();
				}
				else if(questions[i]->destroyFadeoutTime < ofGetElapsedTimef()){
					delete questions[i];
					questions.erase(questions.begin() + i);
				}
			}
		}
	}
}

void CloudsVisualSystemRGBD::setSelectedQuestion(){

    if(questions.size() > 0){
        selectedQuestion = questions[0];
    }
    else{
        cout << "No questions!" << endl;
    }
}

void CloudsVisualSystemRGBD::clearQuestions(){
	
//	cout << "Clearing questions!" << endl;
	
    selectedQuestion = NULL;
	caughtQuestion = NULL;
    for (int i = 0; i<questions.size(); i++) {
        delete questions[i];
    }
    questions.clear();

}

//JG NEW TRANSITION STUBS
void CloudsVisualSystemRGBD::startTransitionOut(RGBDTransitionType transitionType){
	
}
void CloudsVisualSystemRGBD::startTransitionIn(RGBDTransitionType transitionType){
	
}
void CloudsVisualSystemRGBD::updateTransition(float percentComplete){
	
}
void CloudsVisualSystemRGBD::transtionFinished(){
	
}
//JG END TRANSITION STUBES

void CloudsVisualSystemRGBD::updateTransition(){
	
	if(transitioning){
		//get our mixing value by mapping currentTime to the transition start and end time
		float t = ofGetElapsedTimef();
		float x = ofxTween::map(t, transitionStartTime, transitionEndTime, 0, 1, true, transitionEase );
		
		if(t >= transitionEndTime ){
			cout << "CloudsVisualSystemRGBD: transition ended " << ofGetElapsedTimef() << endl << endl;
			transitioning = false;
			cloudsCamera.targetNode = NULL;
			cloudsCamera.startNode = NULL;
		}
		
		transitionVal = x;
		cloudsCamera.setTransitionPercent( transitionVal );
	}
}

void CloudsVisualSystemRGBD::transition( float duration, float startTime )
{
	transitionStartTime = startTime;
	transitionEndTime = transitionStartTime + duration;
	
	transitioning = true;
}

void CloudsVisualSystemRGBD::transitionIn( ofNode& targetNode, float duration, float startTime )
{
	cout << "CloudsVisualSystemRGBD::TRANSITION In:: " << ofGetElapsedTimef() << endl;
	transition( duration, startTime );
	
	cloudsCamera.setTransitionStartNode( &transitionInStart );
	cloudsCamera.setTransitionTargetNode( &cloudsCamera.mouseBasedNode );
}

void CloudsVisualSystemRGBD::transitionOut( ofNode& startNode, float duration, float startTime ) {
	cout << "CloudsVisualSystemRGBD::TRANSITION OUT:: " << ofGetElapsedTimef() << endl;
	
	cloudsCamera.setTransitionStartNode( &cloudsCamera.mouseBasedNode );
	cloudsCamera.setTransitionTargetNode( &transitionOutTarget );
	
	transition( duration, startTime );
}

void CloudsVisualSystemRGBD::transitionIn( RGBDTransitionType transitionType, float duration, float startTime ) {
	setTransitionNodes(transitionType );
	transitionIn( transitionInStart, duration, startTime );
}

void CloudsVisualSystemRGBD::transitionOut( RGBDTransitionType transitionType, float duration, float startTime ) {
	
	setTransitionNodes( transitionType );
	transitionOut( transitionOutTarget, duration, startTime );
}

void CloudsVisualSystemRGBD::printTransitionNodes(){
	
	ofVec4f strtQuat = transitionInStart.getOrientationQuat()._v;
	
	ofVec4f endQuat = transitionOutTarget.getOrientationQuat()._v;
	
	cout << endl<<endl<<endl<<endl;
	cout << "case TRANSITION_TYPE:" << endl;
	cout << "	transitionInStart.setPosition(" << transitionInStart.getPosition() << ");"<< endl ;
	cout << "	transitionInStart.setOrientation( ofQuaternion( " << strtQuat << ") );"<< endl << endl;
	cout << "	transitionOutTarget.setPosition(" << transitionOutTarget.getPosition() << ");"<< endl ;
	cout << "	transitionOutTarget.setOrientation( ofQuaternion( " << endQuat << ") );"<< endl ;
	cout <<endl<<"	break;" << endl << endl<< endl<< endl;
}

void CloudsVisualSystemRGBD::lookThroughTransitionIn(){
	
	transitionCamTargetNode = &transitionInStart;
	
	transitionCam.setPosition( transitionInStart.getPosition() );
	transitionCam.setOrientation( transitionInStart.getOrientationQuat() );
	transitionCam.movedManually();
	//transitionCam.positionChanged = transitionCam.rotationChanged = true;
	
//	ofEventArgs args;
//	transitionCam.update(args);
}

void CloudsVisualSystemRGBD::lookThroughTransitionOut(){
	
	transitionCamTargetNode = &transitionOutTarget;
	
	transitionCam.setPosition( transitionOutTarget.getPosition() );
	transitionCam.setOrientation( transitionOutTarget.getOrientationQuat() );
	transitionCam.movedManually();
	
//	transitionCam.positionChanged = transitionCam.rotationChanged = true;
	
	ofEventArgs args;
	transitionCam.update(args);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generatePoints(){
	
	lines.setUsage( GL_STATIC_DRAW);
	
	if(numRandomPoints == 0){
		points.clear();
	}
	else if(numRandomPoints < points.getNumVertices() ){
		points.getVertices().erase(points.getVertices().begin(),
								   points.getVertices().begin() + (points.getNumVertices() - numRandomPoints) );
	}
	
	while(numRandomPoints > points.getNumVertices()){
		points.addVertex( ofVec3f(ofRandom(640),ofRandom(480),0) );
	}
	
	points.setMode(OF_PRIMITIVE_POINTS);

}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generateLines(){
	
	lines.clear();
	lines.setUsage(GL_STATIC_DRAW);

	if(lineGranularity <= 0) lineGranularity = 1;
	if(lineSpacing <= 0) lineSpacing = 1;
	
	int height = 480;
	int width = 640;
	
	//HORIZONTAL
	for (float ystep = 0; ystep <= height; ystep += lineSpacing){
		for (float xstep = 0; xstep <= width - lineGranularity; xstep += lineGranularity){
			
			ofVec3f stepA = ofVec3f(xstep, ystep, 0);
			ofVec3f stepB = ofVec3f(xstep+lineGranularity, ystep, 0);
			ofVec3f mid   = stepA.getInterpolated(stepB, .5);
			
			lines.addNormal( stepA-mid );
			lines.addVertex( mid );
			
			lines.addNormal( stepB-mid );
			lines.addVertex( mid );
		}
	}
	
	lines.setMode(OF_PRIMITIVE_LINES );
	
	refreshLines = false;
}


void CloudsVisualSystemRGBD::generateMesh(){
		
	if(xSimplify <= 0) xSimplify = 1.0;
	if(ySimplify <= 0) ySimplify = 1.0;

	int x = 0;
	int y = 0;

	int gw = ceil(640. / xSimplify);
	int w = gw*xSimplify;
	int h = 480.;
	
	vector<ofVec3f> vertices;
	
	for (float y = 0; y < 480; y += ySimplify){
		for (float x = 0; x < 640; x += xSimplify){
			vertices.push_back(ofVec3f(x + ofRandomf()*randomVariance,
									   y + ofRandomf()*randomVariance,0));
		}
	}
	
	vector<int> indeces;
	for (float ystep = 0; ystep < h-ySimplify; ystep += ySimplify){
		for (float xstep = 0; xstep < w-xSimplify; xstep += xSimplify){
			ofIndexType a,b,c;
			
			a = x+y*gw;
			b = (x+1)+y*gw;
			c = x+(y+1)*gw;
			indeces.push_back(a);
			indeces.push_back(b);
			indeces.push_back(c);
			
			a = (x+1)+(y+1)*gw;
			b = x+(y+1)*gw;
			c = (x+1)+(y)*gw;
			indeces.push_back(a);
			indeces.push_back(b);
			indeces.push_back(c);
			
			x++;
		}
		
		y++;
		x = 0;
	}
	
	mesh.clear();
	mesh.setUsage(GL_STATIC_DRAW);

	for(int i = 0; i < indeces.size(); i+=3){
		
		ofVec3f& a = vertices[ indeces[i+0] ];
		ofVec3f& b = vertices[ indeces[i+1] ];
		ofVec3f& c = vertices[ indeces[i+2] ];
		ofVec3f mid = (a+b+c)/3.;
		
		ofVec3f toA = a-mid;
		ofVec3f toB = b-mid;
		ofVec3f toC = c-mid;
		
		mesh.addNormal(toA);
		mesh.addColor(ofFloatColor(toB.x/640.,toB.y/480.,toC.x/640.,toC.y/480.));
		mesh.addVertex(mid);

		mesh.addNormal(toB);
		mesh.addColor(ofFloatColor(toA.x/640.,toA.y/480.,toC.x/640.,toC.y/480.));
		mesh.addVertex(mid);
		
		mesh.addNormal(toC);
		mesh.addColor(ofFloatColor(toA.x/640.,toA.y/480.,toB.x/640.,toB.y/480.));
		mesh.addVertex(mid);
	}
	
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);
	refreshMesh = false;
}

void CloudsVisualSystemRGBD::speakerChanged(){
	
	//check speaker vars
	//	this->speakerFirstName = speakerFirstName;
	//	this->speakerLastName = speakerLastName;
	//	this->quoteName = quoteName;
    
	// Add an appearance for this speaker.
//    string key = speakerFirstName + " " + speakerLastName;
//    
//    if (appearances.find(key) == appearances.end()) {
//        appearances[key] = 1;
//    }
//    else {
//        appearances[key]++;
//    }
//    
//    cout << "CloudsVisualSystemRGBD::speakerChanged " << speakerFirstName << " " << speakerLastName << ": " << quoteName << " (" << appearances[key] << ")" << endl;
    
//    if (appearances[key] == 1) {
//        cloudsCaption.font = &captionFont;
//        cloudsCaption.caption = key;
//        cloudsCaption.isEnabled = true;
//        cloudsCaption.begin();
//    }
//    else {
//        cloudsCaption.isEnabled = false;
//    }
}

void CloudsVisualSystemRGBD::selfDrawBackground(){
	
}

void CloudsVisualSystemRGBD::selfDrawDebug(){
	ofSphere(translatedHeadPosition, 10);
	
	ofVec3f questionOriginMax = ofVec3f(questionXZ.x, questionYCenter+questionYRange, questionXZ.z);
	ofVec3f questionOriginMin = ofVec3f(questionXZ.x, questionYCenter-questionYRange, questionXZ.z);
	ofSphere(translatedHeadPosition+questionOriginMax, 10);
	ofSphere(translatedHeadPosition+questionOriginMin, 10);
	
	ofPushStyle();
	ofPushMatrix();
	
	for(int i = 0; i < questions.size(); i++){
		ofBox( questions[i]->position, 3);
	}
	ofNoFill();
	ofTranslate(questionXZ.x, questionYCenter, questionXZ.y);
	ofRotate(90, 1, 0, 0);
	ofCircle(0, 0, questionDriftRange);
	
	ofPopStyle();
	ofPopMatrix();
}

void CloudsVisualSystemRGBD::selfSceneTransformation(){

}

void CloudsVisualSystemRGBD::selfDraw(){
	
	ofPushStyle();
	ofPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	


	if(!getRGBDVideoPlayer().playingVO && getRGBDVideoPlayer().getPlayer().isLoaded() && drawRGBD){
		
		//Enable smooth lines and screen blending
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		setupRGBDTransforms();
			
		if(drawMesh){
			
			meshShader.begin();
			getRGBDVideoPlayer().setupProjectionUniforms(meshShader);
		
			meshShader.setUniform1f("meshAlpha", meshAlpha);
			meshShader.setUniform1f("triangleExtend", getRGBDVideoPlayer().getFadeIn() * getRGBDVideoPlayer().getFadeOut() * visualSystemFadeValue);
			meshShader.setUniform1f("meshRetractionFalloff",meshRetractionFalloff);
			meshShader.setUniform1f("headMinRadius", meshFaceMinRadius);
			meshShader.setUniform1f("headFalloff", meshFaceFalloff);
			meshShader.setUniform1f("edgeAttenuateBase",powf(edgeAttenuate,2.0));
			meshShader.setUniform1f("edgeAttenuateExponent",edgeAttenuateExponent);
			meshShader.setUniform1f("forceGeoRectraction",meshForceGeoRectraction);
			mesh.draw();
			
			meshShader.end();
			//glDisable(GL_CULL_FACE);
		}
		
		glDisable(GL_DEPTH_TEST);
		ofEnableBlendMode(OF_BLENDMODE_ADD);
			
		if(drawLines){
			lineShader.begin();
			ofSetLineWidth(lineThickness);
			getRGBDVideoPlayer().flowPosition = lineFlowPosition * (linesFlowUp?-1:1);
			getRGBDVideoPlayer().setupProjectionUniforms(lineShader);
			
			lineShader.setUniform1f("lineExtend", getRGBDVideoPlayer().getFadeIn() * getRGBDVideoPlayer().getFadeOut() * visualSystemFadeValue);
			lineShader.setUniform1f("headMinRadius", meshFaceMinRadius);
			lineShader.setUniform1f("headFalloff", meshFaceFalloff);
			lineShader.setUniform1f("edgeAttenuateBase",powf(edgeAttenuate,2.0));
			lineShader.setUniform1f("edgeAttenuateExponent",edgeAttenuateExponent);
			lineShader.setUniform1f("headOverlap",lineHeadOverlap);
			lineShader.setUniform1f("alpha", lineAlpha);
			
			lines.draw();
			
			lineShader.end();
		}
			
		if(drawPoints){
			pointShader.begin();
			getRGBDVideoPlayer().flowPosition = pointFlowPosition * (pointsFlowUp?-1:1);
			getRGBDVideoPlayer().setupProjectionUniforms(pointShader);
			
			
			pointShader.setUniform1f("headMinRadius", meshFaceMinRadius);
			pointShader.setUniform1f("headFalloff", meshFaceFalloff);
			pointShader.setUniform1f("edgeAttenuateBase",powf(edgeAttenuate,2.0));
			pointShader.setUniform1f("edgeAttenuateExponent",edgeAttenuateExponent);
			pointShader.setUniform1f("headOverlap", pointHeadOverlap);
			pointShader.setUniform1f("pointSizeMin", pointSize.min);
			pointShader.setUniform1f("pointSizeMax", pointSize.max);
			pointShader.setUniform1f("alpha", pointAlpha * getRGBDVideoPlayer().getFadeIn() * getRGBDVideoPlayer().getFadeOut() );
			
			points.draw();
			
			pointShader.end();
		}
	}
	
	if(drawParticulate){
		glEnable(GL_DEPTH_TEST);
//		particulateController.draw();
	}
	
	glPopAttrib();
	ofPopMatrix();
	ofPopStyle();
	
//		rgbdShader.begin();
//		getRGBDVideoPlayer().setupProjectionUniforms(rgbdShader);
		
//		cout << "base multiplier " << getRGBDVideoPlayer().getFadeIn() * getRGBDVideoPlayer().getFadeOut() << endl;
//		rgbdShader.setUniform1f("fadeValue", 1.0);
//		rgbdShader.setUniform1f("fadeValue", getRGBDVideoPlayer().getFadeIn() * getRGBDVideoPlayer().getFadeOut() );
//		float transitionValue = 1.0 - getRGBDVideoPlayer().getFadeIn() * getRGBDVideoPlayer().getFadeOut() * visualSystemFadeValue;
//		ofxEasingCubic cub;
//		rgbdShader.setUniform1f("triangleContract", ofxTween::map(transitionValue, 0, 1.0, 0, 1.0, true, cub, ofxTween::easeOut));
//		rgbdShader.setUniform1f("eyeMultiplier", 0.0);
//		rgbdShader.setUniform1f("skinMultiplier", 0.0);
//		rgbdShader.setUniform1f("baseMultiplier", 1.0);
//		
//		rgbdShader.setUniform3f("headPosition",
//								getRGBDVideoPlayer().headPosition.x,
//								-getRGBDVideoPlayer().headPosition.y,
//								getRGBDVideoPlayer().headPosition.z);
//		
//		rgbdShader.setUniform3f("lightPosition",
//								getRGBDVideoPlayer().headPosition.x,
//								getRGBDVideoPlayer().headPosition.y+lightOffsetY*100,
//								getRGBDVideoPlayer().headPosition.z+lightOffsetZ*100);
		//		if(drawMesh){
		//			rgbdShader.setUniform1f("flowPosition", 0);
		//			rgbdShader.setUniform1f("eyeMultiplier", eyeMultiplier);
		//			rgbdShader.setUniform1f("skinMultiplier", skinMultiplier);
		//			rgbdShader.setUniform1f("baseMultiplier", meshAlpha);
		//
		////			sharedRenderer->setSimplification(ofVec2f(pointHorizontalSpace, pointVerticalSpace));
		//			rgbdShader.setUniform2f("simplify", pointHorizontalSpace, pointVerticalSpace);
		//
		//			ofPushStyle();
		//			glEnable(GL_DEPTH_TEST);
		//			glDepthFunc(GL_LEQUAL);
		//			glEnable(GL_CULL_FACE);
		//			glCullFace(GL_FRONT);
		//
		//			pointGrid.draw();
		//			ofTranslate(0,0,-3);
		//
		//			ofPopStyle();
		//
		//			rgbdShader.setUniform1f("eyeMultiplier", 0);
		//			rgbdShader.setUniform1f("skinMultiplier", 0);
		//			rgbdShader.setUniform1f("baseMultiplier", 1.0);
		//		}
		
		//set up the renderer so that any geometry within 640x480 space
		//can be prjected onto the pointcloud
//		ofDisableAlphaBlending();
//		//ofEnableAlphaBlending();
//		ofEnableBlendMode(OF_BLENDMODE_SCREEN);
//		if(drawMesh){
			
//			ofPushStyle();
//			ofSetColor(255, 255, 255);
//			rgbdShader.setUniform1f("isMeshed", 1);
//			rgbdShader.setUniform1f("headAttenuateMix", 1.);
//			rgbdShader.setUniform1f("flowPosition", 0);
//			glEnable(GL_DEPTH_TEST);
//			
////			glDepthFunc(GL_LEQUAL);
//			glEnable(GL_CULL_FACE);
//			//if(bUseOculusRift){
//		//		glCullFace(GL_FRONT);
//		//	}
//		//	else{
//				glCullFace(GL_BACK);
//		//	}
//			
//			triangulation.draw();
//
//			glDisable(GL_CULL_FACE);
//			ofPopStyle();
//		}
			
		
//		glDisable(GL_DEPTH_TEST);
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
////		glDepthFunc(GL_LESS);
//
//		if(drawPoints){
//			rgbdShader.setUniform1f("flowPosition", currentFlowPosition);
//			rgbdShader.setUniform1f("isMeshed", 0);
//			rgbdShader.setUniform1f("headAttenuateMix", 0.);
//			//draw the points
//			glPointSize(pointSizeMin);
//			ofSetColor(255*randomPointAlpha);
//			randomPoints.draw();
//			ofSetColor(255*pointGridAlpha);
//			pointGrid.drawVertices();
//		}
		
//		rgbdShader.setUniform2f("simplify", scanlineSimplify.x, scanlineSimplify.y);

//		//draw the lines
//		if(drawScanlines){
//			rgbdShader.setUniform1f("flowPosition", 0.0);
//			rgbdShader.setUniform1f("isMeshed", 1.0);
//			rgbdShader.setUniform1f("headAttenuateMix", .0);
//
//			ofSetLineWidth(horizontalScanlineThickness);
//			horizontalScanLines.draw();
//
////			rgbdShader.setUniform1f("flowPosition", 0);
////			ofSetLineWidth(verticalScanlineThickness);
////			ofSetColor(255*verticalScanlineAlpha);
////			verticalScanLines.draw();
//		}
		
		//subtractive wirerame
		//glDisable(GL_DEPTH_TEST);
//		ofPushMatrix();
//		ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
//		glPushAttrib(GL_POLYGON_BIT);
//		glPolygonOffset(1, 0);
////		triangulation.drawWireframe();
//		glPopAttrib();
//		ofEnableAlphaBlending();
//		ofPopMatrix();

//		connectionGenerator.draw();
//		generator.draw();
		
//		rgbdShader.end();
				
//		glPopAttrib();
//		ofPopMatrix();
//		ofPopStyle();
//	}
		

	
//	ofPopMatrix();
	
	//LARS TODO: add drawTransitionNodes to GUI
	if(drawTransitionNodes){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ofPushStyle();
		ofSetColor( 255, 255, 0 );
		ofPushMatrix();
		
		ofMultMatrix( transitionOutTarget.getLocalTransformMatrix() );
		ofScale( .25, .25, 1 );
		ofBox(0, 0, 0, 100);
		
		ofPopMatrix();
		
		
		ofSetColor( 0, 255, 255 );
		ofPushMatrix();
		
		ofMultMatrix( transitionInStart.getLocalTransformMatrix() );
		ofScale( .25, .25, 1.);
		ofBox(0, 0, 0, 100);
		
		ofPopMatrix();
		
		ofPopStyle();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	drawQuestions();

}

void CloudsVisualSystemRGBD::drawQuestions(){
	//TODO parameterize stuff
	glPointSize(3);

	CloudsQuestion::startShader();
	CloudsQuestion::shader.setUniform1f("attenuateFade", 0.0);
	ofFloatColor baseColor  = ofFloatColor::fromHsb(questionBaseHSB.r, questionBaseHSB.g, questionBaseHSB.b);
	ofFloatColor hoverColor = ofFloatColor::fromHsb(questionHoverHSB.r, questionHoverHSB.g, questionHoverHSB.b);
	CloudsQuestion::shader.setUniform4f("color",baseColor.r,baseColor.g,baseColor.b,.7);
	CloudsQuestion::shader.setUniform4f("selectedColor",hoverColor.r,hoverColor.g,hoverColor.b,.7);
	for(int i = 0; i < questions.size(); i++){
		questions[i]->draw();
	}
	CloudsQuestion::endShader();
	glPointSize(1);
}

void CloudsVisualSystemRGBD::selfDrawOverlay() {
	ofPushStyle();
	for(int i = 0; i < questions.size(); i++){
		questions[i]->drawOverlay();
	}
    
	//This will be replaced with the HUD
//    cloudsCaption.drawOverlay();

	//test overlay
//	ofSetColor(0,0,0,0);
//	ofRect(20, 20, 300,300);
	ofPopStyle();
}

void CloudsVisualSystemRGBD::selfExit(){
	
}

void CloudsVisualSystemRGBD::selfBegin(){
	cloudsCamera.jumpToPosition();
}

void CloudsVisualSystemRGBD::selfEnd(){
	
}

bool CloudsVisualSystemRGBD::isQuestionSelectedAndClipDone(){
    return selectedQuestion != NULL && getRGBDVideoPlayer().isDone();
}

bool CloudsVisualSystemRGBD::isQuestionSelected(){
	return selectedQuestion != NULL;
}

CloudsQuestion* CloudsVisualSystemRGBD::getSelectedQuestion(){
    return selectedQuestion;
}

void CloudsVisualSystemRGBD::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		
		loadShader();
		
//		particulateController.reloadShaders();
		
//		CloudsQuestion::reloadShader();
//		rgbdShader.load( GetCloudsDataPath() + "shaders/rgbdcombined" );
	}
	
//	if(args.key == 'v' && currentCamera != &transitionCam ){
//		lookThroughTransitionIn();
//	}
//	
//	if(args.key == 'V' && currentCamera != &transitionCam ){
//		lookThroughTransitionOut();
//	}
	
}

void CloudsVisualSystemRGBD::selfKeyReleased(ofKeyEventArgs & args){
	if(args.key == 'V' || args.key == 'v' ){
		
		printTransitionNodes();
		
//		setCurrentCamera(cloudsCamera);
	}
	

}

void CloudsVisualSystemRGBD::selfMouseDragged(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfMouseMoved(ofMouseEventArgs& data){
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfMousePressed(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfMouseReleased(ofMouseEventArgs& data){

}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfPresetLoaded( string presetName ){
	refreshLines = true;
	refreshMesh = true;
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfGuiEvent(ofxUIEventArgs &e)
{
	//transtions gui
	string name = e.getName();
	int kind = e.getKind();
	
	if(kind == OFX_UI_WIDGET_TOGGLE && e.getToggle()->getValue())
	{
		for(auto &it: transitionMap)
		{
			if(it.first == name)
			{
				activeTransition = name;
				 
				setTransitionNodes(name);
				
				transitionCamTargetNode = NULL;
			}
		}
	}
	else if(name == "lookThoughIN" && e.getToggle()->getValue())
	{
		bLookThroughIn = true;
		bLookThroughOut = false;
	}
	else if(name == "lookThoughOUT" && e.getToggle()->getValue())
	{
		bLookThroughIn = false;
		bLookThroughOut = true;
	}
	
	
	if(e.widget->getName() == "Line Spacing" || e.widget->getName() == "Line Granularity") {
		refreshLines = true;
	}
	else if(e.widget->getName() == "X Simplify" ||
			e.widget->getName() == "Y Simplify" ||
			e.widget->getName() == "Random Variance")
	{
		refreshMesh = true;
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupSystemGui(){
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::guiSystemEvent(ofxUIEventArgs &e){
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupRenderGui(){
	

}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::guiRenderEvent(ofxUIEventArgs &e){
	
//	cout << "GUI EVENT WITH WIDGET " << e.widget->getName();
	

}

