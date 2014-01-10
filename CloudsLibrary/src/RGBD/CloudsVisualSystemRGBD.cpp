//TODO: replace string keys with RGBDTransitionType
//TODO: confirm where does duration get set



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
	
	transitionOutOption = OutLeft;
	
	questionLifeSpan = 3;
	transitioning = transitioningIn = transitioningOut = false;
	
	transitionInStart.setPosition( 0, 0, -1000 );
	transitionInStart.rotate( 180, ofVec3f(0, 1, 0 ) );
	
	transitionOutLeft.setPosition( 0, 0, -1001 );
	transitionOutLeft.rotate( 180, ofVec3f(0, 1, 0 ) );
	
	transitionOutRight.setPosition( 0, 0, -1001 );
	transitionOutRight.rotate( 180, ofVec3f(0, 1, 0 ) );
	
	transitionCam.useArrowKeys = true;
	transitionTarget = &transitionOutLeft;
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
	
	
	caughtPortal = NULL;
	selectedPortal = NULL;
	
	//transition editing
	placingTransitionNodes = false;
	bSaveTransition = false;
	transitionCamTargetNode = NULL;
	
	currentTransitionType = "FlyThrough";
	
	bMoveTransitionCameraUp = bMoveTransitionCameraDown = false;
	
	//IF we move this before setup(NOT selfSetup) we can have the option of whether or not to load it to the gui
	loadTransitionOptions("Transitions");
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetup(){
	
	portals.push_back(&leftPortal);
	portals.push_back(&rightPortal);
	
	leftPortal.setup();
	rightPortal.setup();
	
	leftPortal.cam = &cloudsCamera;
	rightPortal.cam = &cloudsCamera;
	
	leftPortal.bLookAtCamera = true;
	rightPortal.bLookAtCamera = true;

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
	
	bTransitionsAddedToGui = false;
	
	//IF we move this before setup(NOT selfSetup) we can have the option of whether or not to load it to the gui
	loadTransitionOptions("Transitions");
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

void CloudsVisualSystemRGBD::setTransitionNodes( string type, string option )
{
	if(transitionMap.find(type) != transitionMap.end())
	{
		TransitionInfo ti;
		ofQuaternion q;
		
		//just in case... maybe this fails a little more gracefully
		if(transitionMap[type].find( option ) != transitionMap[type].end())
		{
			ti = transitionMap[type][option];
		}else{
			ti = transitionMap[type]["default"];
		}
		
		transitionInStart.setPosition( ti.inStartPos + translatedHeadPosition );
		q.set( ti.inQuat );
		transitionInStart.setOrientation( q );
		
		transitionOutLeft.setPosition( ti.outLeftPos + translatedHeadPosition);
		q.set( ti.outLeftQuat );
		transitionOutLeft.setOrientation( q );
		
		transitionOutRight.setPosition( ti.outRightPos + translatedHeadPosition);
		q.set( ti.outRightQuat );
		transitionOutRight.setOrientation( q );
		
		cout << "transitions set to: " + type + " : "+ option << endl;
		return;
	}
	
	cout << "couldn't find ["+type+"]["+option+"] inf the transitionMap" << endl;
}

void CloudsVisualSystemRGBD::setTransitionNodes( RGBDTransitionType transitionType, string option)
{
	//TODO: dow we want to use more then one option per type? if so, should we pass in a string(or enum) to identify it?
	switch (transitionType) {
		case CloudsVisualSystem::TWO_DIMENSIONAL:
			setTransitionNodes("TWO_DIMENSIONAL", option);
			break;
			
		case CloudsVisualSystem::WHIP_PAN:
			setTransitionNodes("WHIP_PAN", option);
			break;
			
		case CloudsVisualSystem::FLY_THROUGH:
			setTransitionNodes("FLY_THROUGH", option);
			break;
			
		default:
			setTransitionNodes("WHIP_PAN", "default");
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

	questionGui->addToggle("DEBUG PORTALS", &bPortalDebugOn);
	questionGui->addSlider("PORTAL SCALE", .01, .5, &portalScale);
	
	questionGui->addSlider("HOVER X",    0, 500, &portalBaseHover.x);
	questionGui->addSlider("HOVER Y", -500, 500, &portalBaseHover.y);
	questionGui->addSlider("HOVER Z",    -200, 200, &portalBaseHover.z);
	//in pixels
	questionGui->addRangeSlider("PORTAL SELECT DISTANCE", 20, 200,
								&portalTugMinDistance, &portalTugMaxDistance);

	
	guis.push_back(questionGui);
	guimap[meshGui->getName()] = questionGui;
	
	
	//this is here becuase it needs to be loaded to add the transitions to the gui before setup(if we want)
	loadTransitionOptions( "Transitions" );
	addTransionEditorsToGui();
	
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
	
	if( placingTransitionNodes )
	{
		transitionCam.applyTranslation = transitionCam.applyRotation = !cursorIsOverGUI();
		
		if(bLookThroughIn)
		{
			lookThroughTransitionIn();
			bLookThroughIn = false;
		}
		else if(bLookThroughOutLeft)
		{
			lookThroughTransitionOutLeft();
			bLookThroughOutLeft = false;
		}
		else if(bLookThroughOutRight)
		{
			lookThroughTransitionOutRight();
			bLookThroughOutRight = false;
		}
		
		else if(bResetLookThoughs)
		{
			bResetLookThoughs = false;
			transitionCamTargetNode = NULL;
			resetTransitionNodes();
		}
		
		if(bMoveTransitionCameraUp)
		{
			bMoveTransitionCameraUp = false;
			transitionCam.move(0, 5, 0);
		}
		else if(bMoveTransitionCameraDown)
		{
			bMoveTransitionCameraDown = false;
			transitionCam.move(0, -5, 0);
		}
		
		if(transitionCamTargetNode)
		{
			transitionCamTargetNode->setPosition( transitionCam.getPosition() );
			transitionCamTargetNode->setOrientation( transitionCam.getOrientationQuat() );
		}

	}
	else{
		cloudsCamera.driftNoiseSpeed = caughtPortal ? 0 : attenuatedCameraDrift;
	}
	
	
	if(bSaveTransition)
	{
		bSaveTransition = false;
	
		saveTransitionSettings(ofSystemTextBoxDialog("Saving " + currentTransitionType + ": ", "option name") );
	}
}

//save and load transitions
//------------------------------------------------------------------------

void CloudsVisualSystemRGBD::resetTransitionNodes()
{
	transitionInStart.resetTransform();
	transitionOutLeft.resetTransform();
	transitionOutRight.resetTransform();
	
	ofVec3f offset(0,-50,-150);
	
	transitionInStart.rotate(180, 0, 1, 0);
	transitionOutLeft.rotate(180, 0, 1, 0);
	transitionOutRight.rotate(180, 0, 1, 0);
	
	transitionInStart.setPosition(translatedHeadPosition + offset);
	transitionOutLeft.setPosition(translatedHeadPosition + offset);
	transitionOutRight.setPosition(translatedHeadPosition + offset);
	
}

void CloudsVisualSystemRGBD::loadTransitionOptions(string filename)
{
	//we want to clear here, right?
	clearTransitionMap();
	
	//load the option data
	string path =GetCloudsDataPath() + "transitions/" + filename + ".xml";
	
	//cout << path << endl;
	
	ofxXmlSettings *XML = new ofxXmlSettings();
	XML->loadFile( path );
	
	int numTypes = XML->getNumTags("TRANSITION_TYPE");
	
	
	for(int i=0; i<numTypes; i++){
		
		XML->pushTag("TRANSITION_TYPE", i);
		
		string typeName = XML->getValue("NAME", "NULL", 0);
		transitionMap[typeName];
		
		
		//cout << typeName << endl;
		
		int numOptions = XML->getNumTags("OPTION");
		for(int j=0; j<numOptions; j++)
		{
			XML->pushTag("OPTION", j);
			string optionName = XML->getValue("NAME", "NULL", 0);
			
			
			//cout << optionName << endl;
			
			transitionMap[typeName][optionName];
			TransitionInfo* ti = &transitionMap[typeName][optionName];
			
			ti->transitionName = typeName;
			ti->optionName = optionName;

			XML->pushTag("InStartPos");
			ti->inStartPos.set( XML->getValue("x", 1 ), XML->getValue("y", 0. ), XML->getValue("z", 0. ) );
			XML->popTag();

			XML->pushTag("InStartQuat");
			ti->inQuat.set( XML->getValue("x", 0. ), XML->getValue("y", 0. ), XML->getValue("z", 0. ), XML->getValue("w", 0. ) );
			XML->popTag();
			
			XML->pushTag("OutLeftPos");
			ti->outLeftPos.set( XML->getValue("x", 1. ), XML->getValue("y", 0. ), XML->getValue("z", 0. ) );
			XML->popTag();

			XML->pushTag("OutLeftQuat");
			ti->outLeftQuat.set( XML->getValue("x", 0. ), XML->getValue("y", 0. ), XML->getValue("z", 0. ), XML->getValue("w", 0. ) );
			XML->popTag();

			XML->pushTag("OutRightPos");
			ti->outRightPos.set( XML->getValue("x", 1. ), XML->getValue("y", 0. ), XML->getValue("z", 0. ) );
			XML->popTag();

			XML->pushTag("OutRightQuat");
			ti->outRightQuat.set( XML->getValue("x", 0. ), XML->getValue("y", 0. ), XML->getValue("z", 0. ), XML->getValue("w", 0. ) );
			XML->popTag();

			
			XML->popTag();
		}
		
		//cout << "--------" << endl;
		
		XML->popTag();
	}
	delete XML;
	
	if(bTransitionsAddedToGui)
	{
		addTransionEditorsToGui();
	}
}

void CloudsVisualSystemRGBD::saveTransitionSettings(string optionName )
{
	//add/replace the transition option
	if(transitionsGuis[currentTransitionType]->getWidget(optionName) == NULL)
	{
		ofxUIRadio* r = (ofxUIRadio*)transitionsGuis[currentTransitionType]->getWidget("optionRadio");
		r->addToggle(transitionsGuis[currentTransitionType]->addToggle(optionName, true));
		
		transitionsGuis[currentTransitionType]->autoSizeToFitWidgets();
	}
	
	transitionMap[currentTransitionType][optionName];
	transitionMap[currentTransitionType][optionName].inStartPos = transitionInStart.getPosition() - translatedHeadPosition;
	transitionMap[currentTransitionType][optionName].inQuat = transitionInStart.getOrientationQuat();
	
	transitionMap[currentTransitionType][optionName].outLeftPos = transitionOutLeft.getPosition() - translatedHeadPosition;
	transitionMap[currentTransitionType][optionName].outLeftQuat = transitionOutLeft.getOrientationQuat().asVec4();
	
	transitionMap[currentTransitionType][optionName].outRightPos = transitionOutRight.getPosition() - translatedHeadPosition;
	transitionMap[currentTransitionType][optionName].outRightQuat = transitionOutRight.getOrientationQuat().asVec4();
	
	//loop through the transitionMap and build our XML
	ofxXmlSettings *XML = new ofxXmlSettings();
	ofVec4f q;
	int transitionTypeIndex = 0;
	for (auto &it: transitionMap)
	{
		//save info for eac option type
		XML->addTag("TRANSITION_TYPE");
		XML->pushTag("TRANSITION_TYPE", transitionTypeIndex);
		XML->setValue("INDEX", transitionTypeIndex );
		transitionTypeIndex++;
		
		XML->setValue("NAME", it.first );
		
		//save our various options for each type
		int optionIndex = 0;
		for (auto &option: it.second)
		{
			XML->addTag("OPTION");
			XML->pushTag("OPTION", optionIndex );
			optionIndex++;
			
			//write our specific option's data
			XML->setValue("INDEX", optionIndex );
			XML->setValue("NAME", option.first );
			
			XML->addTag("InStartPos");
			XML->pushTag("InStartPos");
			XML->setValue("x", option.second.inStartPos.x);
			XML->setValue("y", option.second.inStartPos.y);
			XML->setValue("z", option.second.inStartPos.z);
			XML->popTag();
			
			XML->addTag("OutRightPos");
			XML->pushTag("OutRightPos");
			XML->setValue("x", option.second.outRightPos.x);
			XML->setValue("y", option.second.outRightPos.y);
			XML->setValue("z", option.second.outRightPos.z);
			XML->popTag();
			
			XML->addTag("OutLeftPos");
			XML->pushTag("OutLeftPos");
			XML->setValue("x", option.second.outLeftPos.x);
			XML->setValue("y", option.second.outLeftPos.y);
			XML->setValue("z", option.second.outLeftPos.z);
			XML->popTag();
			
			q = option.second.inQuat.asVec4();
			XML->addTag("InStartQuat");
			XML->pushTag("InStartQuat");
			XML->setValue("x", q.x);
			XML->setValue("y", q.y);
			XML->setValue("z", q.z);
			XML->setValue("w", q.w);
			XML->popTag();
			
			q = option.second.outLeftQuat.asVec4();
			XML->addTag("OutLeftQuat");
			XML->pushTag("OutLeftQuat");
			XML->setValue("x", q.x);
			XML->setValue("y", q.y);
			XML->setValue("z", q.z);
			XML->setValue("w", q.w);
			XML->popTag();
			
			q = option.second.outRightQuat.asVec4();
			XML->addTag("OutRightQuat");
			XML->pushTag("OutRightQuat");
			XML->setValue("x", q.x);
			XML->setValue("y", q.y);
			XML->setValue("z", q.z);
			XML->setValue("w", q.w);
			XML->popTag();
			
			XML->popTag();
		}
		XML->popTag();
	}
	
	XML->saveFile( GetCloudsDataPath() +  "transitions/Transitions.xml" );
	delete XML;
}

void CloudsVisualSystemRGBD::StopEditTransitionMode()
{
	placingTransitionNodes = false;
}


void CloudsVisualSystemRGBD::addTransionEditorsToGui()
{
	//add/update the guis for our transitions
	for (auto &it: transitionMap)
	{
		cout << it.first << endl;
		if(transitionsGuis.find(it.first) == transitionsGuis.end())
		{
			addTransitionGui(it.first);
		}
	}
	bTransitionsAddedToGui = true;
}

void CloudsVisualSystemRGBD::clearTransitionMap()
{
	for (auto &i: transitionMap)
	{
		i.second.clear();
	}
	transitionMap.clear();
}

void CloudsVisualSystemRGBD::addTransitionGui(string guiName)
{
	cout << "addTransitionGui: " << guiName<< endl;
	
	//get out transition info
	map<string, TransitionInfo>* ti = &transitionMap[guiName];
	
	//create the canvas
	ofxUISuperCanvas* t = new ofxUISuperCanvas(guiName, gui);
	t->copyCanvasStyle(gui);
	t->copyCanvasProperties(gui);
	t->setName(guiName);
	t->setWidgetFontSize(OFX_UI_FONT_SMALL);
	t->addSpacer();
	
	//look through IN/OUT
	t->addToggle("drawTransitionNodes", &drawTransitionNodes);
	t->addButton("DriveIn", false )->setColorBack(ofColor(0,155,155));
	t->addButton("DriveOutLeft", false )->setColorBack(ofColor(155,155,0));
	t->addButton("DriveOutRight", false )->setColorBack(ofColor(155,0,155));
	t->addButton("resetNodes", &bResetLookThoughs );
	
	t->addSpacer();
	t->addToggle("moveUp", &bMoveTransitionCameraUp);
	t->addToggle("moveDown", &bMoveTransitionCameraDown);
	
	//saving & edit
	t->addSpacer();
	t->addToggle("Edit", placingTransitionNodes);
	t->addToggle("save", &bSaveTransition );
	
	//slider for scrubbing animation
	t->addSlider("scrubIn", 0, 1, &transitionScrubIn);
	t->addSlider("scrubOut", 0, 1, &transitionScrubOut);
	
//	//Left/Right toggles
//	t->addSpacer();
//	t->addToggle("LEFT", true);
//	t->addToggle("RIGHT", false);

	//ADD OUR OPTION TOGGLES
	t->addSpacer();
	t->addLabel("OPTIONS");
	vector<string> tempNames;
	ofxUIRadio* r = t->addRadio("optionRadio", tempNames);
	for(auto &it: *ti)
	{
		r->addToggle(t->addToggle(it.first, false));
	}
	
	//ADD LISTENERS ETC.
	transitionsGuis[guiName] = t;
	guis.push_back(transitionsGuis[guiName]);
	guimap[transitionsGuis[guiName]->getName()] = transitionsGuis[guiName];
	ofAddListener(transitionsGuis[guiName]->newGUIEvent, this, &CloudsVisualSystemRGBD::selfGuiEvent);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::addQuestion(CloudsClip& questionClip, string topic, string question){

	CloudsPortal* rportal = ofRandomuf() ? &leftPortal : &rightPortal;
	
	if(rportal->question != ""){
		//swap and override for certain so we keep the newest!
		rportal = rportal == &leftPortal ? &rightPortal : &leftPortal;
	}
	
	rportal->question = question;
	rportal->topic = topic;
	rportal->clip = questionClip;
	
	/*
	 //TODO RE ADD
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
	*/
	
//	questions.push_back(q);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::updateQuestions(){
	
	leftPortal.hoverPosition  = portalBaseHover + translatedHeadPosition;
	rightPortal.hoverPosition = portalBaseHover*ofVec3f(-1,0,0) + translatedHeadPosition;

	leftPortal.scale = portalScale;
	rightPortal.scale = portalScale;
	leftPortal.lookTarget = cloudsCamera.getPosition();
	rightPortal.lookTarget = cloudsCamera.getPosition();

	for(int i = 0; i < portals.size(); i++){
		
		portals[i]->update();
		
		#ifdef OCULUS_RIFT
		ofVec3f screenPos = getOculusRift().worldToScreen(portals[i]->hoverPosition, true);
        ofRectangle viewport = getOculusRift().getOculusViewport();
		float distanceToQuestion = ofDist(screenPos.x, screenPos.y,
										  viewport.getCenter().x, viewport.getCenter().y);
		#else
		ofVec2f mouseNode(GetCloudsInputX(),GetCloudsInputY());
		float distanceToQuestion = portals[i]->screenPosition.distance(mouseNode);
		#endif
		
		if(caughtPortal == NULL){
			if( distanceToQuestion < portalTugMaxDistance) {
				if(distanceToQuestion < portalTugMinDistance) {
					caughtPortal = portals[i];
					caughtPortal->startHovering();
				}
			}
		}
		//we have a caught question make sure it's still close
		else if(caughtPortal == portals[i]){
			//we went over the timer distance! zoooom!!!
			if(caughtPortal->isSelected() ){
				selectedPortal = caughtPortal;
			}
			//let it go
			else if(distanceToQuestion > portalTugMaxDistance){
				caughtPortal->stopHovering();
				caughtPortal = NULL;
			}
		}
	}
		/*
        if( selectedPortal == NULL && portals[i]->isSelected() ){
            selectedPortal = portals[i];
//			selectedPortal->lockHover = true;
			break;
        }

		if(caughtPortal == NULL){
			portals[i]->enableHover();
		
			if(questions[i]->hovering){
				caughtPortal = portals[i];
			}
		}
		
		if(caughtPortal != NULL) {
			if(portals[i] == caughtPortal){
				if(!caughtPortal->hovering){
					caughtPortal = NULL;
				}
			}
			else {
				portals[i]->disableHover();
			}
		}
		 */
//	}

	//TODO reconsider destroying portals
//	if(selectedPortal != NULL){
//		for(int i = questions.size()-1; i >= 0; i--){
//			if(selectedQuestion != questions[i]){
//				if(!questions[i]->isDestroyed){
//					questions[i]->destroy();
//				}
//				else if(questions[i]->destroyFadeoutTime < ofGetElapsedTimef()){
//					delete questions[i];
//					questions.erase(questions.begin() + i);
//				}
//			}
//		}
//	}
}

void CloudsVisualSystemRGBD::setSelectedQuestion(){

//    if(questions.size() > 0){
//        selectedQuestion = questions[0];
//    }
//    else{
//        cout << "No questions!" << endl;
//    }
}

void CloudsVisualSystemRGBD::clearQuestions(){
	
//	cout << "Clearing questions!" << endl;
	
//    selectedQuestion = NULL;
//	caughtQuestion = NULL;
//    for (int i = 0; i<questions.size(); i++) {
//        delete questions[i];
//    }
//    questions.clear();
//
}



//JG NEW TRANSITION STUBS<----- James, I love these! thank you, Lars

void CloudsVisualSystemRGBD::startCurrentTransitionOut()
{
	//transition to the left or right based on relative posiiton
	setOutOption((cloudsCamera.getPosition().x - translatedHeadPosition.x) > 0 ? OutLeft : OutRight);
	
	//transitionEase = ofxTween::easeOut;
	transitionEase = ofxTween::easeIn;
	transitioning = true;
	
	cloudsCamera.setTransitionStartNode( &cloudsCamera.mouseBasedNode );
	cloudsCamera.setTransitionTargetNode( transitionOutOption == OutLeft? &transitionOutLeft : &transitionOutRight );
}

void CloudsVisualSystemRGBD::startCurrentTransitionIn()
{
	//transitionEase = ofxTween::easeIn;
	transitionEase = ofxTween::easeOut;//are we sure we don't want easeInOut?
	transitioning = true;
	
	cloudsCamera.setTransitionStartNode( &transitionInStart );
	cloudsCamera.setTransitionTargetNode( &cloudsCamera.mouseBasedNode );
}

void CloudsVisualSystemRGBD::startTransitionOut(RGBDTransitionType transitionType, string option)
{
	//set the in/out nodes
	setTransitionNodes( transitionType, option );
	
	//transition to the left or right based on relative posiiton
	setOutOption((cloudsCamera.getPosition().x - translatedHeadPosition.x) > 0 ? OutLeft : OutRight);
	
	//transitionEase = ofxTween::easeOut;
	transitionEase = ofxTween::easeIn;
	transitioning = true;
	
	cloudsCamera.setTransitionStartNode( &cloudsCamera.mouseBasedNode );
	cloudsCamera.setTransitionTargetNode( transitionOutOption == OutLeft? &transitionOutLeft : &transitionOutRight );
}

void CloudsVisualSystemRGBD::startTransitionIn(RGBDTransitionType transitionType, string option)
{
	//set the in/out nodes
	setTransitionNodes(transitionType, option);
	
	//transitionEase = ofxTween::easeIn;
	transitionEase = ofxTween::easeOut;//are we sure we don't want easeInOut?
	transitioning = true;
	
	cloudsCamera.setTransitionStartNode( &transitionInStart );
	cloudsCamera.setTransitionTargetNode( &cloudsCamera.mouseBasedNode );
}

void CloudsVisualSystemRGBD::updateTransition(float percentComplete)
{
	if(transitioning)
	{
		float easedPercent = ofxTween::map(percentComplete, 0, 1, 0, 1, true, ofxEasingCubic(), transitionEase );//ofxEasingSine
		cloudsCamera.setTransitionPercent( easedPercent );
		
		cout <<"TRANSITIONING : easedValue = "<< easedPercent << endl;
	}
}

void CloudsVisualSystemRGBD::transtionFinished()
{
	cout << "transtionFinished()" <<endl;
	
	cloudsCamera.targetNode = NULL;
	cloudsCamera.startNode = NULL;
	transitioning = false;
}

void CloudsVisualSystemRGBD::setOutOption( OutOption outOption )
{
	switch (outOption) {
		case OutLeft:
			cout << "setOutOption: OutLeft" << endl;
			transitionOutOption = OutLeft;
			break;
			
		case OutRight:
			cout << "setOutOption: OutRight" << endl;
			transitionOutOption = OutRight;
			break;
			
		default:
			break;
	}
}



void CloudsVisualSystemRGBD::updateTransition(){
	
//	if(transitioning){
//		//get our mixing value by mapping currentTime to the transition start and end time
//		float t = ofGetElapsedTimef();
//		float x = ofxTween::map(t, transitionStartTime, transitionEndTime, 0, 1, true, transitionEase );
//		
//		if(t >= transitionEndTime ){
//			cout << "CloudsVisualSystemRGBD: transition ended " << ofGetElapsedTimef() << endl << endl;
//			transitioning = false;
//			cloudsCamera.targetNode = NULL;
//			cloudsCamera.startNode = NULL;
//		}
//		
//		transitionVal = x;
//		cloudsCamera.setTransitionPercent( transitionVal );
//	}
}

void CloudsVisualSystemRGBD::lookThroughTransitionIn(){
	
	transitionCamTargetNode = &transitionInStart;
	
	transitionCam.setPosition( transitionInStart.getPosition() );
	transitionCam.setOrientation( transitionInStart.getOrientationQuat() );
	transitionCam.movedManually();
	
	//transitionCam.positionChanged = transitionCam.rotationChanged = true;
	
//	transitionCam.positionChanged = transitionCam.rotationChanged = true;
	

//	ofEventArgs args;
//	transitionCam.update(args);
}

void CloudsVisualSystemRGBD::lookThroughTransitionOutLeft(){
	
	transitionCamTargetNode = &transitionOutLeft;
	
	transitionCam.setPosition( transitionOutLeft.getPosition() );
	transitionCam.setOrientation( transitionOutLeft.getOrientationQuat() );
	transitionCam.movedManually();
}

void CloudsVisualSystemRGBD::lookThroughTransitionOutRight()
{
	transitionCamTargetNode = &transitionOutRight;
	
	transitionCam.setPosition( transitionOutRight.getPosition() );
	transitionCam.setOrientation( transitionOutRight.getOrientationQuat() );
	transitionCam.movedManually();
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
	
	rightPortal.question = "";
	leftPortal.question = "";
	
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
	
//	for(int i = 0; i < questions.size(); i++){
//		ofBox( questions[i]->position, 3);
//	}
	
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
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofEnableBlendMode(OF_BLENDMODE_SCREEN);
        
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
		
		ofMultMatrix( transitionOutLeft.getLocalTransformMatrix() );
		ofScale( .25, .25, 1 );
		ofBox(0, 0, 0, 100);
		
		ofPopMatrix();
		
		
		ofSetColor( 0, 255, 255 );
		ofPushMatrix();
		
		ofMultMatrix( transitionInStart.getLocalTransformMatrix() );
		ofScale( .25, .25, 1.);
		ofBox(0, 0, 0, 100);
		
		ofPopMatrix();
		
		
		ofSetColor( 255, 0, 255 );
		ofPushMatrix();
		
		ofMultMatrix( transitionOutRight.getLocalTransformMatrix() );
		ofScale( .25, .25, 1.);
		ofBox(0, 0, 0, 100);
		
		ofPopMatrix();
		
		ofPopStyle();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	drawQuestions();

}

void CloudsVisualSystemRGBD::drawQuestions(){


	glDisable(GL_DEPTH_TEST);
	CloudsPortal::shader.begin();
	CloudsPortal::shader.setUniform1i("doAttenuate", 0);
	if(leftPortal.question != "" || bPortalDebugOn){
		leftPortal.draw();
	}
	if(rightPortal.question != "" || bPortalDebugOn){
		rightPortal.draw();
	}
	CloudsPortal::shader.end();
	
	glEnable(GL_DEPTH_TEST);

//	CloudsQuestion::startShader();
//	CloudsQuestion::shader.setUniform1f("attenuateFade", 0.0);
//	ofFloatColor baseColor  = ofFloatColor::fromHsb(questionBaseHSB.r, questionBaseHSB.g, questionBaseHSB.b);
//	ofFloatColor hoverColor = ofFloatColor::fromHsb(questionHoverHSB.r, questionHoverHSB.g, questionHoverHSB.b);
//	CloudsQuestion::shader.setUniform4f("color",baseColor.r,baseColor.g,baseColor.b,.7);
//	CloudsQuestion::shader.setUniform4f("selectedColor",hoverColor.r,hoverColor.g,hoverColor.b,.7);
//	for(int i = 0; i < questions.size(); i++){
//		questions[i]->draw();
//	}
//	CloudsQuestion::endShader();
//	glPointSize(1);
}

void CloudsVisualSystemRGBD::selfDrawOverlay() {
//	ofPushStyle();	
//	for(int i = 0; i < questions.size(); i++){
//		questions[i]->drawOverlay();
//	}
	//This will be replaced with the HUD
//    cloudsCaption.drawOverlay();
	//test overlay
//	ofSetColor(0,0,0,0);
//	ofRect(20, 20, 300,300);
//	ofPopStyle();
}

void CloudsVisualSystemRGBD::selfExit(){
	
}

void CloudsVisualSystemRGBD::selfBegin(){
	cloudsCamera.jumpToPosition();
}

void CloudsVisualSystemRGBD::selfEnd(){
	
}

bool CloudsVisualSystemRGBD::isQuestionSelectedAndClipDone(){
    return selectedPortal != NULL && getRGBDVideoPlayer().isDone();
}

bool CloudsVisualSystemRGBD::isQuestionSelected(){
	return selectedPortal != NULL;
}

CloudsPortal* CloudsVisualSystemRGBD::getSelectedQuestion(){
 //   return selectedQuestion;
}

void CloudsVisualSystemRGBD::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		
		loadShader();
		
//		particulateController.reloadShaders();
//		CloudsQuestion::reloadShader();
//		rgbdShader.load( GetCloudsDataPath() + "shaders/rgbdcombined" );
	}
}

void CloudsVisualSystemRGBD::selfKeyReleased(ofKeyEventArgs & args){

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
	refreshMesh  = true;
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfGuiEvent(ofxUIEventArgs &e)
{
	//transtions gui
	string name = e.getName();
	string parentName = e.widget->getParent()->getName();
	int kind = e.getKind();
	
	ofxUIWidget* parent = e.widget->getParent();
	
	
	for(auto &guiIt: transitionsGuis)
	{
		
		//TRANSITION OPTIONS
		ofxUIWidget* r = guiIt.second->getWidget("optionRadio");
		if(r == parent && e.getToggle()->getValue() )
		{
			transitionCamTargetNode = NULL;
			setTransitionNodes(guiIt.first, name);
			
			ofxUIToggle* in = (ofxUIToggle*)guiIt.second->getWidget("DriveIn");
			in->setColorBack(ofColor(0,155,155));
			in->setColorFill(ofColor(0,155,155));
			
			ofxUIToggle* out = (ofxUIToggle*)guiIt.second->getWidget("DriveOutLeft");
			out->setColorBack(ofColor(155,155,0));
			out->setColorFill(ofColor(155,155,0));
			
			
			ofxUIToggle* outRight = (ofxUIToggle*)guiIt.second->getWidget("DriveOutRight");
			outRight->setColorBack(ofColor(155,0,155));
			outRight->setColorFill(ofColor(155,0,155));
		}
		
		//used to switch which type we're editing
		if(parent == guiIt.second || parent == r )
		{
			currentTransitionType = guiIt.first;
			guiIt.second->setColorBack(ofColor(55,55,55));
		}
		else
		{
			guiIt.second->setColorBack(ofColor(100,0,0));
			ofxUIToggle* t = (ofxUIToggle*)guiIt.second->getWidget("Edit");
			t->setValue(false);
		}
		
		if( name == "Edit" && parent == guiIt.second)
		{
			guiIt.second->setColorBack(ofColor(55,55,55));
			currentTransitionType = guiIt.first;
			placingTransitionNodes = e.getToggle()->getValue();
		}
		else if(parent != guiIt.second && currentTransitionType != guiIt.first)
		{
			guiIt.second->setColorBack(ofColor(100,0,0));
			((ofxUIToggle*)guiIt.second->getWidget("Edit"))->setValue(false);
		}
		
		//saving and loading 
		if(currentTransitionType == guiIt.first)
		{
			if(parent == guiIt.second)
			{
				if(name == "save" && e.getToggle()->getValue() == true)
				{
					bSaveTransition = true;
				}
				else if(name == "LEFT" && e.getToggle()->getValue())
				{
					setOutOption( OutLeft );
					((ofxUIToggle*)guiIt.second->getWidget("RIGHT"))->setValue( false );
				}
				else if(name == "RIGHT" && e.getToggle()->getValue())
				{
					setOutOption( OutRight );
					((ofxUIToggle*)guiIt.second->getWidget("LEFT"))->setValue( false );
				}
				
				else if(name == "scrubIn")
				{
					StopEditTransitionMode();
					
					//transitionEase = ofxTween::easeIn;
					transitionEase = ofxTween::easeOut;//are we sure we don't want easeInOut?
					transitioning = true;
					
					cloudsCamera.setTransitionStartNode( &transitionInStart );
					cloudsCamera.setTransitionTargetNode( &cloudsCamera.mouseBasedNode );
					
					updateTransition(transitionScrubIn);
					transitioning = false;
				}
				
				else if(name == "scrubOut")
				{
					StopEditTransitionMode();
					
					//transition to the left or right based on relative posiiton
					setOutOption((cloudsCamera.getPosition().x - translatedHeadPosition.x) > 0 ? OutLeft : OutRight);
					
					//transitionEase = ofxTween::easeOut;
					transitionEase = ofxTween::easeIn;
					transitioning = true;
					
					cloudsCamera.setTransitionStartNode( &cloudsCamera.mouseBasedNode );
					cloudsCamera.setTransitionTargetNode( transitionOutOption == OutLeft? &transitionOutLeft : &transitionOutRight );
					
					updateTransition(transitionScrubOut);
					transitioning = false;
				}

				else if( name == "DriveIn")
				{
					//enter edit mode
					((ofxUIToggle*)guiIt.second->getWidget("Edit"))->setValue(true);
					placingTransitionNodes = true;
					
					//set look though bools
					bLookThroughIn = true;
					bLookThroughOutLeft = false;
					bLookThroughOutRight = false;
					
					//update gui colors
					e.widget->setColorFill(ofColor(0,255,255));
					e.widget->setColorBack(ofColor(0,255,255));
					
					ofxUIToggle* outLeft = (ofxUIToggle*)guiIt.second->getWidget("DriveOutLeft");
					outLeft->setColorBack(ofColor(155,155,0));
					outLeft->setColorFill(ofColor(155,155,0));
					
					ofxUIToggle* outRight = (ofxUIToggle*)guiIt.second->getWidget("DriveOutRight");
					outRight->setColorBack(ofColor(155,0,155));
					outRight->setColorFill(ofColor(155,0,155));
				}
				else if( name == "DriveOutLeft")
				{
					//enter edit mode
					((ofxUIToggle*)guiIt.second->getWidget("Edit"))->setValue(true);
					placingTransitionNodes = true;
					
					//set look though bools
					bLookThroughIn = false;
					bLookThroughOutLeft = true;
					bLookThroughOutRight = false;
					
					//update gui colors
					e.widget->setColorFill(ofColor(255,255,0));
					e.widget->setColorBack(ofColor(255,255,0));
					
					ofxUIToggle* in = (ofxUIToggle*)guiIt.second->getWidget("DriveIn");
					in->setColorBack(ofColor(0,155,155));
					in->setColorFill(ofColor(0,155,155));
					
					ofxUIToggle* outRight = (ofxUIToggle*)guiIt.second->getWidget("DriveOutRight");
					outRight->setColorBack(ofColor(155,0,155));
					outRight->setColorFill(ofColor(155,0,155));
				}
				
				else if( name == "DriveOutRight")
				{
					//enter edit mode
					((ofxUIToggle*)guiIt.second->getWidget("Edit"))->setValue(true);
					placingTransitionNodes = true;
					
					//set look though bools
					bLookThroughIn = false;
					bLookThroughOutLeft = false;
					bLookThroughOutRight = true;
					
					//update gui colors
					e.widget->setColorFill(ofColor(255,0,255));
					e.widget->setColorBack(ofColor(255,0,255));
					
					ofxUIToggle* in = (ofxUIToggle*)guiIt.second->getWidget("DriveIn");
					in->setColorBack(ofColor(0,155,155));
					in->setColorFill(ofColor(0,155,155));
					
					ofxUIToggle* outLeft = (ofxUIToggle*)guiIt.second->getWidget("DriveOutLeft");
					outLeft->setColorBack(ofColor(155,155,0));
					outLeft->setColorFill(ofColor(155,155,0));
				}
			}
		}
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

