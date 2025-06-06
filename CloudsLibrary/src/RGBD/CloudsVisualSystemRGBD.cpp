
#include "CloudsVisualSystemRGBD.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsGlobal.h"
#include "CloudsLocalization.h"

CloudsVisualSystemEvents CloudsVisualSystemRGBD::events;

//--------------------------------------------------------------
CloudsVisualSystemRGBD::CloudsVisualSystemRGBD(){

	visualSystemFadeValue = 0.0;
    questionToReplace = 0;	
	transitionTarget = NULL;
	bTransitionIn = bTransitionOut = false;
	bLookThroughIn = bLookThroughOutLeft = bLookThroughOutRight = false;
	bSaveTransition = false;
	
	transitionCamTargetNode = NULL;
	
	bTransitionsAddedToGui = false;
	transitionOutOption = OutLeft;
	bMoveTransitionCameraUp = bMoveTransitionCameraDown = false;
	transitionScrubIn = transitionScrubOut = 0;
	
	//MESH
	globalMeshGui = NULL;
	drawRGBD = false;
	edgeAttenuate = edgeAttenuateExponent = skinBrightness = 0;
	
	//LINES
	linesGui = NULL;
    lineVertexCount = 0;
	drawLines = false;
	lineAlpha = lineThickness = lineSpacing =
		lineDensityVariance = lineGranularity = 
			lineFlowPosition = lineFlowSpeed = lineHeadOverlap = 0.0f;
	linesFlowUp = false;
    lineColorBoost = lineSkinBoost = lineMaxActuatorRetract = lineRandomOffset = 0.0f;
	refreshLines = false;
	
    
	//MESH
	meshGui = NULL;
    meshVertexCount = 0;
	drawMesh = false;
	meshAlpha = xSimplify = ySimplify = randomVariance = 
		meshFaceMinRadius = meshFaceFalloff = meshRetractionFalloff = meshForceGeoRetraction = 
		meshColorBoost = meshSkinBoost = meshMaxActuatorRetract = 0;
    refreshMesh = false;

    //FILL
	fillGui = NULL;
    bEnableFill = false;
	fillAlpha = fillFaceFalloff = fillRetractionFalloff = fillFaceMinRadius = 0;

    ///OCCLUSION
	occlusionGui = NULL;
    bDrawOcclusion = drawOcclusionDebug = false;

    occlusionVertexCount = occlusionXSimplify = occlusionYSimplify = 
		occlusionMeshFaceMinRadius = occlusionMeshFaceFalloff = occlusionMeshRetractionFalloff = 0;
    refreshOcclusion = false;
    
    ///ACTUATORS
    actuatorSpinPosition = actuatorSpinSpeed = actuatorShineAngle = 0;

	cameraGui  = backgroundMeshGui = questionGui = actuatorGui = NULL;
    
	drawParticulate = 0;
	attenuatedCameraDrift = 0;
    particleCount = 0;
    
	portalScale = 0;
	minDistanceToQuestion = 0;
	bPortalDebugOn = false;

	portalToClear = NULL;
	questionFontSize = currentQuestionFontSize = questionFontScale = questionYOffset = questionFontTracking = questionFontSplitWidth = 0;
	
	caughtPortal = selectedPortal = NULL;
	
	cullFace = GL_FRONT;    
	placingTransitionNodes = false;
	drawTransitionNodes = false;
	transitioning, transitioningIn, transitioningOut, bResetLookThoughs, bResetRight,bResetLeft,bResetIn = false;
	transitionStartTime, transitionEndTime, transitionStartVal, transitionTargetVal = 0;
	
	transitionType = TWO_DIMENSIONAL;
	
	transitionVal = 0.0;
    paused = false;
}

//--------------------------------------------------------------
string CloudsVisualSystemRGBD::getSystemName(){
	return "RGBD";
}

void CloudsVisualSystemRGBD::selfSetDefaults(){
	visualSystemFadeValue = 1.0;
	portalToClear = NULL;
	
	drawRGBD = true;
	#ifdef OCULUS_RIFT
		cullFace = GL_BACK;//GL_FRONT;
	#else
		cullFace = GL_BACK;
	#endif

	transitionOutOption = OutLeft;
	
//	questionLifeSpan = 3;
	transitioning = transitioningIn = transitioningOut = false;
	
	transitionInStart.setPosition( 0, 0, -1000 );
	transitionInStart.rotate( 180, ofVec3f(0, 1, 0 ) );
	
	transitionOutLeft.setPosition( 0, 0, -1001 );
	transitionOutLeft.rotate( 180, ofVec3f(0, 1, 0 ) );
	
	transitionOutRight.setPosition( 0, 0, -1001 );
	transitionOutRight.rotate( 180, ofVec3f(0, 1, 0 ) );
	
#ifdef HAS_GAMECAM
	transitionCam.useArrowKeys = true;
#endif
	transitionTarget = &transitionOutLeft;
	drawTransitionNodes = false;
	
	bTransitionIn = bTransitionOut = false;
	
	bTransitionsAddedToGui = false;

    meshColorBoost = .0;
    meshSkinBoost  = .0;
    lineColorBoost = .0;
    lineSkinBoost  = .0;
    
	actuatorSpinPosition = 0;
	edgeAttenuate = 0.;
	skinBrightness = 0.;
	   
	drawLines = true;
	lineAlpha = .5;
	lineThickness	= 1.0;
	lineFlowPosition = 0.;
	lineFlowSpeed = 0.;
    lineMaxActuatorRetract = 0.0;
	linesFlowUp = false;
	refreshLines = true;
	
	drawMesh = true;
	xSimplify = 2.0;
	ySimplify = 2.0;
	randomVariance = 1.0;

	meshFaceMinRadius = 0.0;
	meshFaceFalloff = 0.0;
	meshRetractionFalloff = 1.0;
	meshForceGeoRetraction = .0;
	meshMaxActuatorRetract = 0.0;
    
    questionToReplace = 0;
    
    bEnableFill = false;
	fillFaceFalloff = 0.0;
	fillRetractionFalloff = 0.0;
    fillFaceMinRadius = 0.0;

    particleCount = 3000;
    
    bDrawOcclusion = true;
    occlusionVertexCount = 0;
   	occlusionXSimplify = 4.;
	occlusionYSimplify = 4.;
    refreshOcclusion = true;
	
    lineRandomOffset = 0.;
    
	caughtPortal = NULL;
	selectedPortal = NULL;
	
	//transition editing
	placingTransitionNodes = false;
	bSaveTransition = false;
	transitionCamTargetNode = NULL;
	
    drawOcclusionDebug = false;
    
	currentTransitionType = "FlyThrough";
	
	bMoveTransitionCameraUp = bMoveTransitionCameraDown = false;
	
    drawParticulate = false;
    
	//IF we move this before setup(NOT selfSetup) we can have the option of whether or not to load it to the gui
	loadTransitionOptions("Transitions");
    
    pointLayer1.setDefaults();
    pointLayer2.setDefaults();
    
    primaryCursorMode = CURSOR_MODE_CAMERA;
    secondaryCursorMode = CURSOR_MODE_INACTIVE;
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetup(){
	
    portals.clear();
	portals.push_back(&leftPortal);
	portals.push_back(&rightPortal);

	leftPortal.setup();
	rightPortal.setup();
	
	leftPortal.cam = &cloudsCamera;
	rightPortal.cam = &cloudsCamera;
	
	leftPortal.bLookAtCamera = true;
	rightPortal.bLookAtCamera = true;
	
#ifdef OCULUS_RIFT
   	portals.push_back(&resetPortal);
	resetPortal.setup();
	resetPortal.cam = &cloudsCamera;
	//resetPortal.cam = &getOculusRift().getTransformedCamera();
	resetPortal.bLookAtCamera = true;
	resetPortal.question = "QUIT";
#endif
    
	loadShader();
	
    pointLayer1.pointShader = &pointShader;
    pointLayer2.pointShader = &pointShader;

	generateLines();
	generateMesh();
		
	voxelMesh.setup();
	
	cloudsCamera.setup();
	cloudsCamera.lookTarget = ofVec3f(0,25,0);
			
#ifdef HAS_GAMECAM
	transitionCam.setup();
	transitionCam.applyTranslation = true;
	transitionCam.applyRotation = true;
#endif
	
	
	//IF we move this before setup(NOT selfSetup) we can have the option of whether or not to load it to the gui
	loadTransitionOptions("Transitions");
}
void CloudsVisualSystemRGBD::playTestVideo(CloudsClip* clip){
	getRGBDVideoPlayer().setup(
		clip->combinedVideoPath, 
		clip->combinedCalibrationXMLPath);
	getRGBDVideoPlayer().swapAndPlay();
}

void CloudsVisualSystemRGBD::playTestVideo(int videoIndex){


	string sourceFile, filePathMov,filePathXml, subtitlesPathSrt;

	//if(videoIndex == 0){
	//	filePathXml = GetCloudsDataPath(true) + "media/Aaron_collaboration.xml";
	//	sourceFile  ="https://d2jijm80bx6z70.cloudfront.net/clouds/videos/20914/99835/Aaron_dreaming_together.mov?response-content-disposition=attachment;filename=%22Aaron-dreaming-together%20720p.mp4%22&Expires=1428346924&Signature=f6zxAfopdpwYe859FQ0kH80i46at6jphJntU8JM8Fr0Cfz~mzEzLErFROg9Vl-C1O0~~YvzAkElCl9Oifw8mbBVENy00~qW0CWX2TNclXXdRi-PPTdQYs7d3uQuhNslfyRD9qWAgpOYcsvZyyMVrAi0KtMuQrNnvSMglX80qFI2sG6XrWk9BWQJxG5iS-KWSBwjTGJuOX9giIIreKIjpRzBy2AyiajISiUtydmmP1D6MQH12ZjGexobIV4OPuxB34yEmUJMyxuqydAjgYp-aGAfQtN3mcbXiZGL9Yo88ktKFh5oDX-NDXiliIPtWLeaIJRJFh1kFDbEv1RzbH~SA~Q__&Key-Pair-Id=APKAIKJG7ZVXLCEDZ7RQ";
	//	filePathMov = sourceFile;
	//	setupSpeaker("Kyle", "McDonald","");
	//}
	//else{
		filePathXml = GetCloudsDataPath() + "clipxml/Aaron_collaboration.xml";
		sourceFile  = GetCloudsMediaPath() + "media/Aaron_collaboration";
		filePathMov = sourceFile+".mov";
		setupSpeaker("Kyle", "McDonald","");
	//}
	//if(ofFile::doesFileExist(filePathMov)){
        getRGBDVideoPlayer().setup(filePathMov, filePathXml, subtitlesPathSrt);
		getRGBDVideoPlayer().swapAndPlay();
	//}
	//else{
	//	ofSystemAlertDialog("Test Video " + filePathMov + " does not exist");
	//}
}

void CloudsVisualSystemRGBD::loadShader(){
//	cout << "loading point shader " << endl;
	if(!pointShader.load(getVisualSystemDataPath() + "shaders/rgbdPoints")){
		ofLogError("CloudsVisualSystemRGBD::loadShader") << "Points failed";
	}
//	cout << "loading line shader " << endl;
	if(!lineShader.load( getVisualSystemDataPath() + "shaders/rgbdLines")){
		ofLogError("CloudsVisualSystemRGBD::loadShader") << "Points failed";
	}
//	cout << "loading mesh shader " << endl;
	if(!meshShader.load( getVisualSystemDataPath() + "shaders/rgbdMesh")){
		ofLogError("CloudsVisualSystemRGBD::loadShader") << "Points failed";
	}
//    cout << "loading occlusion shader " << endl;
    if(!occlusionShader.load( getVisualSystemDataPath() + "shaders/rgbdOcclusion")){
		ofLogError("CloudsVisualSystemRGBD::loadShader") << "Points failed";
	}
}

void CloudsVisualSystemRGBD::setTransitionNodes( string type, string option )
{
	currentTransitionType = type;
	TransitionInfo ti;
	ofQuaternion q;
	
	//just in case... maybe this fails a little more gracefully
	if(transitionMap[type].find( option ) != transitionMap[type].end()){
		ti = transitionMap[type][option];
	}
	else{
		ti = transitionMap[type]["default"];
	}
	
	if(type == "QUESTION") {
		q.set( getCameraRef().getOrientationQuat() );
		
		transitionInStart.setPosition( ti.inStartPos + translatedHeadPosition );
		transitionInStart.setOrientation( q );
		
//		ofVec3f leftExtraFlying = (leftPortal.hoverPosition - transitionInStart.getPosition() ).normalized() * 100;
//		ofVec3f rightExtraFlying = (rightPortal.hoverPosition - transitionInStart.getPosition() ).normalized() * 100;
		ofVec3f leftExtraFlying;
		ofVec3f rightExtraFlying;
		
		transitionOutLeft.setPosition(rightPortal.hoverPosition + leftExtraFlying);
		transitionOutLeft.setOrientation( q );
		
		transitionOutRight.setPosition(leftPortal.hoverPosition + rightExtraFlying);
		transitionOutRight.setOrientation( q );
        #ifdef OCULUS_RIFT
		transitionOutReset.setPosition(resetPortal.hoverPosition);
		transitionOutReset.setOrientation( q );
        #endif
	}
	else if(transitionMap.find(type) != transitionMap.end()){	
		transitionInStart.setPosition( ti.inStartPos + translatedHeadPosition );
		q.set( ti.inQuat );
		transitionInStart.setOrientation( q );
		
		transitionOutLeft.setPosition( ti.outLeftPos + translatedHeadPosition);
		q.set( ti.outLeftQuat );
		transitionOutLeft.setOrientation( q );
		
		transitionOutRight.setPosition( ti.outRightPos + translatedHeadPosition);
		q.set( ti.outRightQuat );
		transitionOutRight.setOrientation( q );
		
//		cout << "transitions set to: " + type + " : "+ option << endl;
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
			
		case CloudsVisualSystem::QUESTION :
			setTransitionNodes("QUESTION", option);
			break;
			
		default:
			setTransitionNodes("FLY_THROUGH", "default");
			break;
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetupGuis(){

    //////////////////MESH
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
	pointcloudGuis.push_back(globalMeshGui);
	
    //////////////////POINTS
    ofxUISuperCanvas* pointsGui1 = pointLayer1.createGui(gui, "Points 1");
	guis.push_back(pointsGui1);
	guimap[pointsGui1->getName()] = pointsGui1;
    pointcloudGuis.push_back(pointsGui1);
	
    ofxUISuperCanvas* pointsGui2 = pointLayer2.createGui(gui, "Points 2");
	guis.push_back(pointsGui2);
	guimap[pointsGui2->getName()] = pointsGui2;
	pointcloudGuis.push_back(pointsGui2);
    //////////////////
    
    //////////////////LINES
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
	linesGui->addSlider("Line Color Boost", 0, 1.0, &lineColorBoost);
	linesGui->addSlider("Line Skin Boost", 0, 1.0, &lineSkinBoost);
    linesGui->addSpacer();
	linesGui->addSlider("Line Thickness", 0, 3.0, &lineThickness);
	linesGui->addSlider("Line Spacing", 1., 5., &lineSpacing);
	linesGui->addSlider("Line Face Overlap", 0., 1.0, &lineHeadOverlap);
	linesGui->addSlider("Line Granularity", 1., 10.0, &lineGranularity);
	linesGui->addSlider("Line Flow", 0, 1.0, &lineFlowSpeed);
	linesGui->addToggle("Lines Flow Up", &linesFlowUp);
    linesGui->addSlider("Max Actuator Retr", 0.0, 1.0, &lineMaxActuatorRetract);
    linesGui->addSlider("Line Random Offset", 0.0, 3.0, &lineRandomOffset);
    
	ofAddListener(linesGui->newGUIEvent, this, &CloudsVisualSystemRGBD::selfGuiEvent);
	guis.push_back(linesGui);
	guimap[linesGui->getName()] = linesGui;
	pointcloudGuis.push_back(linesGui);
    //////////////////
	
    //////////////////MESH
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
	meshGui->addSlider("Mesh Color Boost", 0, 1.0, &meshColorBoost);
	meshGui->addSlider("Mesh Skin Boost", 0, 1.0, &meshSkinBoost);
    meshGui->addSpacer();
	meshGui->addSlider("X Simplify", 1., 16., &xSimplify);
	meshGui->addSlider("Y Simplify", 1., 16., &ySimplify);
	meshGui->addSlider("Random Variance", 0, 10.0, &randomVariance);
	meshGui->addSlider("Face Min Radius", 0, 600., &meshFaceMinRadius);
	meshGui->addSlider("Face Falloff", 0, 600., &meshFaceFalloff);
	meshGui->addSlider("Edge Geo Retraction", 0, 1.0, &meshRetractionFalloff);
	meshGui->addSlider("Force Geo Retraction", 0, 1.0, &meshForceGeoRetraction);
    meshGui->addSlider("Max Actuator Retr", 0.0, 1.0, &meshMaxActuatorRetract);
    
	ofAddListener(meshGui->newGUIEvent, this, &CloudsVisualSystemRGBD::selfGuiEvent);
	guis.push_back(meshGui);
	guimap[meshGui->getName()] = meshGui;
	pointcloudGuis.push_back(meshGui);
    //////////////////MESH
    
    //////////////////FILL
	fillGui = new ofxUISuperCanvas("FILL", gui);
	fillGui->copyCanvasStyle(gui);
	fillGui->copyCanvasProperties(gui);
	fillGui->setName("Fill");
	fillGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	toggle = fillGui->addToggle("ENABLE", &bEnableFill);
	toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
	fillGui->resetPlacer();
	fillGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
	fillGui->addWidgetToHeader(toggle);
    
	fillGui->addSlider("Mesh Alpha", 0., 1.0, &fillAlpha);
	fillGui->addSlider("Face Min Radius", 0, 600., &fillFaceMinRadius);
	fillGui->addSlider("Face Falloff", 0, 600., &fillFaceFalloff);
    fillGui->addSlider("Edge Geo Retraction", 0, 1.0, &fillRetractionFalloff);
    
	ofAddListener(fillGui->newGUIEvent, this, &CloudsVisualSystemRGBD::selfGuiEvent);
	guis.push_back(fillGui);
	guimap[fillGui->getName()] = fillGui;
	pointcloudGuis.push_back(fillGui);
    //////////////////FILL
    
    ////////////////// OCCLUSION
	occlusionGui = new ofxUISuperCanvas("OCCLUSION", gui);
	occlusionGui->copyCanvasStyle(gui);
	occlusionGui->copyCanvasProperties(gui);
	occlusionGui->setName("Occlusion");
	occlusionGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	toggle = occlusionGui->addToggle("ENABLE", &bDrawOcclusion);
	toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
	occlusionGui->resetPlacer();
	occlusionGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
	occlusionGui->addWidgetToHeader(toggle);

    occlusionGui->addToggle("OcclusionDebug", &drawOcclusionDebug);
	occlusionGui->addSlider("Occl X Simplify", 1., 8., &occlusionXSimplify);
	occlusionGui->addSlider("Occl Y Simplify", 1., 8., &occlusionYSimplify);
	occlusionGui->addSlider("Face Min Radius", 0, 600., &occlusionMeshFaceMinRadius);
	occlusionGui->addSlider("Face Falloff", 0, 600., &occlusionMeshFaceFalloff);
    occlusionGui->addSlider("Edge Geo Retraction", 0, 1.0, &occlusionMeshRetractionFalloff);
    
	ofAddListener(occlusionGui->newGUIEvent, this, &CloudsVisualSystemRGBD::selfGuiEvent);
	guis.push_back(occlusionGui);
	guimap[occlusionGui->getName()] = occlusionGui;
	pointcloudGuis.push_back(occlusionGui);
    ////////////////// OCCLUSION
    
	
    ////////////////// ACTUATORS
    actuatorGui = new ofxUISuperCanvas("ACTUATORS", gui);
	actuatorGui->copyCanvasStyle(gui);
    actuatorGui->copyCanvasProperties(gui);
    actuatorGui->setName("Actuators");
    actuatorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

    actuatorGui->addSlider("SPIN SPEED",    0,  5.0, &actuatorSpinSpeed);
    actuatorGui->addSlider("SHINE ANGLE", -90, 90.0, &actuatorShineAngle);
    
    guis.push_back(actuatorGui);
	guimap[actuatorGui->getName()] = actuatorGui;
	pointcloudGuis.push_back(actuatorGui);
    ////////////////// ACTUATORS
    
    ////////////////// CAMERA
	cameraGui = new ofxUISuperCanvas("CAMERA", gui);
	cameraGui->copyCanvasStyle(gui);
	cameraGui->copyCanvasProperties(gui);
	cameraGui->setName("Camera");
	cameraGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	cameraGui->addLabel("OFFSETS");
	cameraGui->addSlider("FRONT DISTANCE", 50, 200, &cloudsCamera.frontDistance);
	cameraGui->addSlider("SIDE DISTANCE", 20, 500, &cloudsCamera.sideDistance);
	cameraGui->addSlider("SIDE PULLBACK", -500, 500, &cloudsCamera.sidePullback);
	cameraGui->addSlider("LIFT RANGE", 0, 100, &cloudsCamera.liftRange);
	cameraGui->addSlider("LIFT AMOUNT", 10, 200, &cloudsCamera.liftAmount);
	cameraGui->addSlider("DROP AMOUNT", 0, 200, &cloudsCamera.dropAmount);
	cameraGui->addSlider("DRIFT ANGLE", 0, 200, &cloudsCamera.maxDriftAngle);
	cameraGui->addSlider("DRIFT DENSITY", 0, 1.0, &cloudsCamera.driftNoiseDensity);
	cameraGui->addSlider("DRIFT SPEED", 0, .01, &attenuatedCameraDrift);
    cameraGui->addSlider("DAMPING", .001, .1, &cloudsCamera.damp);
    cameraGui->addRangeSlider("FOV ZOOM", 40, 120, &cloudsCamera.zoomFOVRange.min, &cloudsCamera.zoomFOVRange.max);

	guis.push_back(cameraGui);
	guimap[cameraGui->getName()] = cameraGui;
		
	////////////////// BACKGROUND PARTICLES
	backgroundMeshGui = new ofxUISuperCanvas("BACKGROUNDMESH", gui);
	backgroundMeshGui->copyCanvasStyle(gui);
	backgroundMeshGui->copyCanvasProperties(gui);
	backgroundMeshGui->setName("Backgroundmesh");
	backgroundMeshGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	backgroundMeshGui->addToggle("draw points", &voxelMesh.drawPoints);
	backgroundMeshGui->addToggle("draw lines", &voxelMesh.drawLines);
	backgroundMeshGui->addSlider("point size", 1.0, 4.0, &voxelMesh.pointSize);
	backgroundMeshGui->addSlider("line width", 1.0, 4.0, &voxelMesh.lineWidth);
	
	backgroundMeshGui->addIntSlider("num voxels", 10, 100, &voxelMesh.numVoxels);
	backgroundMeshGui->addSlider("voxel spacing", 10, 100, &voxelMesh.voxelWidth);
	
	backgroundMeshGui->addSlider("sphere radius",  100, 800, &voxelMesh.sphereRadius);
	backgroundMeshGui->addSlider("sphere percent", -2.0, 2.0, &voxelMesh.spherePercent);
	
	backgroundMeshGui->addRangeSlider("distance range", 0.0, 2.0, &voxelMesh.minDistance, &voxelMesh.maxDistance);
	
	backgroundMeshGui->addSlider("twist speed x",  0, .001, &voxelMesh.twistSpeedX);
	backgroundMeshGui->addSlider("twist factor x", 0, 1.0, &voxelMesh.twistFactorX);
	backgroundMeshGui->addSlider("twist speed y",  0, .001, &voxelMesh.twistSpeedY);
	backgroundMeshGui->addSlider("twist factor y", 0, 1.0, &voxelMesh.twistFactorY);
	
	backgroundMeshGui->addSlider("noise distort x", 0, 1.0, &voxelMesh.noiseDistort.x);
	backgroundMeshGui->addSlider("noise distort y", 0, 1.0, &voxelMesh.noiseDistort.y);
	backgroundMeshGui->addSlider("noise distort z", 0, 1.0, &voxelMesh.noiseDistort.z);
	backgroundMeshGui->addSlider("noise distort radial", 0, 1.0, &voxelMesh.noiseDistort.w);
	backgroundMeshGui->addSlider("noise density", 0, 0.2, &voxelMesh.noiseDensity);
	backgroundMeshGui->addRangeSlider("center decay", 0, 1.0, &voxelMesh.centerDecayMinRadius, &voxelMesh.centerDecayMaxRadius);
	backgroundMeshGui->addSlider("noise speed", 0, 0.1, &voxelMesh.noiseSpeed);
	
	guis.push_back(backgroundMeshGui);
	guimap[backgroundMeshGui->getName()] = backgroundMeshGui;
	backgroundGuis.push_back(backgroundMeshGui);
	
    ////////////////// QUESTIONS
	questionGui = new ofxUISuperCanvas("QUESTIONS", gui);
	questionGui->copyCanvasStyle(gui);
    questionGui->copyCanvasProperties(gui);
    questionGui->setName("Questions");
    questionGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

	questionGui->addToggle("DEBUG PORTALS", &bPortalDebugOn);
	questionGui->addSlider("PORTAL SCALE", .01, .5, &portalScale);
	questionGui->addIntSlider("QUESTION FONT SIZE", 4, 20, &questionFontSize);
	questionGui->addSlider("HOVER X",    0, 500, &portalBaseHover.x);
	questionGui->addSlider("HOVER Y", -500, 500, &portalBaseHover.y);
	questionGui->addSlider("HOVER Z", -200, 200, &portalBaseHover.z);
	questionGui->addRangeSlider("PORTAL SELECT DISTANCE", 20, 400,
								&portalTugDistance.min, &portalTugDistance.max);
	questionGui->addSlider("QUESTION FONT SCALE", 0, 1.0, &questionFontScale);
	questionGui->addSlider("QUESTION FONT Y SHIFT", -200.0, 200.0, &questionYOffset);
	questionGui->addSlider("QUESTION FONT TRACKING", 1.0, 20, &questionFontTracking);
	questionGui->addSlider("QUESTION FONT SPLIT WIDTH", 100, 500.0, &questionFontSplitWidth);
	
#ifdef OCULUS_RIFT
	questionGui->addSlider("RESET Y", -300, 300, &resetHoverPosition.y);
	questionGui->addSlider("RESET Z", -500, 500, &resetHoverPosition.z);
#endif
	
	guis.push_back(questionGui);
	guimap[questionGui->getName()] = questionGui;
	
	//this is here becuase it needs to be loaded to add the transitions to the gui before setup(if we want)
	loadTransitionOptions( "Transitions" );
	addTransionEditorsToGui();
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfUpdate(){


#ifdef OCULUS_RIFT
	float curQuestionSelectFade = (caughtPortal == NULL ? 1.0 : powf(ofMap(caughtPortal->hoverPercentComplete,
																		0.0, .2, 1.0, 0.0, true), 2.0));
	questionSelectFade += (curQuestionSelectFade - questionSelectFade) * .1;
#else
    questionSelectFade = 1.0;
#endif
    
//	if(!transitioning){
//		playerScaleModifier = 1.0;
//	}

	float transitionValue = getRGBDTransitionValue();
	//cout << "TRANSITION IS " << transitionValue << endl;

	pointLayer1.visualSystemFadeValue = getRGBDTransitionValue();
    pointLayer2.visualSystemFadeValue = getRGBDTransitionValue();
    pointLayer1.update();
    pointLayer2.update();
    
	if(refreshLines){
		generateLines();
	}

	if(refreshMesh){
		generateMesh();
	}

    if(refreshOcclusion){
        generateOcclusion();
    }
    
	lineFlowPosition += powf(lineFlowSpeed,2.0);

	voxelMesh.center = cloudsCamera.lookTarget;
	voxelMesh.update();

	updateActuators();
    updateQuestions();
    
	
	if( placingTransitionNodes )
	{
		#ifdef HAS_GAMECAM
		transitionCam.applyTranslation = transitionCam.applyRotation = !cursorIsOverGUI();
		#endif
		
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
		else if(bResetIn)
		{
			bResetIn = false;
			transitionCamTargetNode = NULL;
			resetInTransitionNode();
		}
		else if(bResetLeft)
		{
			bResetLeft = false;
			transitionCamTargetNode = NULL;
			resetLeftTransitionNode();
		}
		else if(bResetRight)
		{
			bResetRight = false;
			transitionCamTargetNode = NULL;
			resetRightTransitionNode();
		}
		#ifdef HAS_GAMECAM
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
		#endif

	}
	else {
        //this stops the camera from drifting around as you move closer to the question
		//cloudsCamera.driftNoiseSpeed = caughtPortal ? 0 : attenuatedCameraDrift;
		if( portalTugDistance.max != portalTugDistance.center() ){
			float cameraDriftDamp = powf(ofMap(minDistanceToQuestion, portalTugDistance.max, portalTugDistance.center(),
											   1.0, 0,true), 2.0);
			cloudsCamera.driftNoiseSpeed = attenuatedCameraDrift * cameraDriftDamp;
		}
	}

	if(bSaveTransition) {
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

void CloudsVisualSystemRGBD::resetInTransitionNode()
{
	transitionInStart.resetTransform();
	
	ofVec3f offset(0,-50,-150);
	
	transitionInStart.rotate(180, 0, 1, 0);
	
	transitionInStart.setPosition(translatedHeadPosition + offset);
}

void CloudsVisualSystemRGBD::resetLeftTransitionNode()
{
	transitionOutLeft.resetTransform();
	
	ofVec3f offset(0,-50,-150);
	
	transitionOutLeft.rotate(180, 0, 1, 0);
	
	transitionOutLeft.setPosition(translatedHeadPosition + offset);
}

void CloudsVisualSystemRGBD::resetRightTransitionNode()
{
	transitionOutRight.resetTransform();
	
	ofVec3f offset(0,-50,-150);
	
	transitionOutRight.rotate(180, 0, 1, 0);
	
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
	t->addButton("ResetIn", &bResetIn);
	t->addButton("ResetLeft", &bResetLeft);
	t->addButton("ResetRight", &bResetRight);
	
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

void CloudsVisualSystemRGBD::loadBackgroundGUISFromName(string presetName){
	string presetPath = getVisualSystemDataPath()+"Presets/"+ presetName;
	for(int i = 0; i < backgroundGuis.size(); i++){
		string presetPathName = presetPath+"/"+backgroundGuis[i]->getName()+".xml";
		backgroundGuis[i]->loadSettings(presetPathName);
	}
}

void CloudsVisualSystemRGBD::loadPointcloudGUISFromName(string presetName){
	string presetPath = getVisualSystemDataPath() + "Presets/" + presetName;
	for(int i = 0; i < pointcloudGuis.size(); i++){
		string presetPathName = presetPath+"/"+pointcloudGuis[i]->getName()+".xml";
		pointcloudGuis[i]->loadSettings(presetPathName);
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::addQuestion(CloudsClip* questionClip, string topic, string question){
    
    //////////////QUEUE WAY
    CloudsPortal* testportal = NULL;
    if(leftPortal.question == ""){
        testportal = &leftPortal;
    }
    else if(rightPortal.question == ""){
        testportal = &rightPortal;
    }
    
	if(testportal != NULL){
        testportal->question = question;
        testportal->topic = topic;
        testportal->clip = questionClip;
    }
    else{
        QuestionQueue q;
        q.clip  = questionClip;
        q.topic = topic;
        q.question = question;
        questions.push_back(q);
    }

}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::updateActuators(){
    
    actuatorSpinPosition += actuatorSpinSpeed;
    
    ofVec3f vx = ofVec3f(1,0,0);
    ofVec3f vy = ofVec3f(0,1,0);
    ofVec3f vz = ofVec3f(0,0,1);
    
    //rotate 3 vecs down to shine angle then spready them out apart evenly, modulated by the spin position
    pointActuator = vx.getRotated(actuatorShineAngle, vz).getRotated(0 * 360/3.0 + actuatorSpinPosition, vy);
    lineActuator  = vx.getRotated(actuatorShineAngle, vz).getRotated(1 * 360/3.0 + actuatorSpinPosition, vy);
    meshActuator  = vx.getRotated(actuatorShineAngle, vz).getRotated(2 * 360/3.0 + actuatorSpinPosition, vy);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::updateQuestions(){
	
    if(paused){
        stickyCursor.interpolate(cursor, 0.5f);
        return;
    }
    
	leftPortal.hoverPosition  = portalBaseHover + translatedHeadPosition;
	rightPortal.hoverPosition = portalBaseHover*ofVec3f(-1,0,0) + translatedHeadPosition;
	leftPortal.scale  = portalScale;
	rightPortal.scale = portalScale;

	leftPortal.lookTarget  = cloudsCamera.getPosition();
	rightPortal.lookTarget = cloudsCamera.getPosition();

#ifdef OCULUS_RIFT
    resetPortal.scale = portalScale;
    resetPortal.lookTarget = cloudsCamera.getPosition();
	resetPortal.hoverPosition = resetHoverPosition + translatedHeadPosition;
#endif
    
    //default to far away
    minDistanceToQuestion = portalTugDistance.max;
	for(int i = 0; i < portals.size(); i++){
		
		portals[i]->update();
		#ifndef OCULUS_RIFT

        if( !portals[i]->onScreen || portals[i]->question == ""){
            continue;
		}

		ofVec2f mouseNode(GetCloudsInputX(),GetCloudsInputY());
		float distanceToQuestion = portals[i]->screenPosition.distance(mouseNode);

		#else
		
        if( portals[i]->question == ""){
            continue;
        }
	
		ofNode n;
		n.setOrientation(getOculusRift().getOrientationQuat());
		n.setPosition(getOculusRift().getTranslation());
		float dotproduct = n.getLookAtDir().dot(n.getPosition() - portals[i]->hoverPosition);
		//not on screen
		if(dotproduct < 0){
			continue;
		}
		//if(portals[i]->question == "QUIT"){
		//	cout << "dot " << dotproduct << endl;
		//}

		ofVec3f screenPos = getOculusRift().worldToScreen(portals[i]->hoverPosition);

        ofRectangle viewport = getOculusRift().getViewport();
		float distanceToQuestion = ofDist(screenPos.x, screenPos.y,
                                    viewport.getCenter().x, viewport.getCenter().y);
        
		//cout << "DISTANCE IS " << distanceToQuestion << endl;

		#endif
		
        ////////////
        //if(caughtPortal == &resetPortal) caughtPortal = NULL;
        ////////////
        
		if(caughtPortal == NULL){
			if( distanceToQuestion < portalTugDistance.max) {
				if(distanceToQuestion < portalTugDistance.min) {
					caughtPortal = portals[i];
                    /////NEW QUESTION WAY
					if (caughtPortal->startHovering()) {
						CloudsVisualSystem::getClick()->setPosition(0);
						CloudsVisualSystem::getClick()->play();
                    }
				}
			}
		}
		//we have a caught question make sure it's still close
		else if(caughtPortal == portals[i]){
			//we went over the timer distance! zoooom!!!
			if(caughtPortal->isSelected()){
				if(selectedPortal == NULL){
					selectedPortal = caughtPortal;
					CloudsPortalEventArgs args(getQuestionText());
					ofNotifyEvent(events.portalHoverBegan, args);
					CloudsVisualSystem::getSelectLow()->setPosition(0);
					CloudsVisualSystem::getSelectLow()->play();
					#ifdef CLOUDS_SCREENING
					portalToClear = selectedPortal;
					#endif
				}
			}
			//let it go
			else if(distanceToQuestion > portalTugDistance.max){
				caughtPortal->stopHovering();
				caughtPortal = NULL;				
			}
		}
        minDistanceToQuestion = MIN(distanceToQuestion, minDistanceToQuestion);
	}
    
    if (caughtPortal != NULL) {
        // move the sticky cursor towards the caught portal
		ofVec2f targetPosition = caughtPortal->screenPosition;
		if(bEnablePostFX){
			targetPosition -= ofVec2f(bleed,bleed)*.5;
		}
        stickyCursor.interpolate(targetPosition, 0.2f); //minue ten for bleed
    }
    else {
        stickyCursor.interpolate(cursor, 0.5f);
    }
}

void CloudsVisualSystemRGBD::clearQuestions(){
	
#ifdef CLOUDS_SCREENING
    //only clear when we have non-planted questions
    if(questions.size() > 3){
        rightPortal.question = "";
        leftPortal.question = "";
        questions.clear();
    }
#else
    rightPortal.question = "";
    leftPortal.question = "";
    questions.clear();
    
#endif
    
    leftPortal.clearSelection();
    rightPortal.clearSelection();
#ifdef OCULUS_RIFT
	resetPortal.clearSelection();
#endif
    selectedPortal = NULL;
    caughtPortal = NULL;
}

//JG NEW TRANSITION STUBS<----- James, I love these! thank you, Lars
void CloudsVisualSystemRGBD::startCurrentTransitionOut()
{
	if(currentTransitionType == "QUESTION")
	{
		transitionOutRight.setOrientation( getCameraRef().getOrientationQuat() );
		transitionOutLeft.setOrientation( getCameraRef().getOrientationQuat() );

		transitionOutRight.setPosition(leftPortal.hoverPosition);
		transitionOutLeft.setPosition(rightPortal.hoverPosition);
#ifdef OCULUS_RIFT
		transitionOutReset.setPosition(resetPortal.hoverPosition);
		transitionOutReset.setOrientation( getCameraRef().getOrientationQuat() );
#endif
	}
	
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
	transitionEase = ofxTween::easeOut;
	transitioning = true;
	
	cloudsCamera.setTransitionStartNode( &transitionInStart );
	cloudsCamera.setTransitionTargetNode( &cloudsCamera.mouseBasedNode );
}

void CloudsVisualSystemRGBD::startTransitionOut(RGBDTransitionType transitionType, string option){
	
	//set the in/out nodes
	setTransitionNodes( transitionType, option );
	
	//transition to the left or right based on relative posiiton
#ifdef OCULUS_RIFT
	if(selectedPortal != nullptr){
		setOutOption(selectedPortal == &leftPortal ? OutRight : OutLeft);
	}
	else{
		setOutOption( (cloudsCamera.getPosition().x - translatedHeadPosition.x) > 0 ? OutLeft : OutRight);
	}
#else
	setOutOption( (cloudsCamera.getPosition().x - translatedHeadPosition.x) > 0 ? OutLeft : OutRight);
#endif
	//transitionEase = ofxTween::easeOut;
	transitionEase = ofxTween::easeIn;
	transitioning = true;
	
	if(isResetSelected()){
		cloudsCamera.setTransitionStartNode( &cloudsCamera.mouseBasedNode );
		cloudsCamera.setTransitionTargetNode( &transitionOutReset );
	}
	else{
		cloudsCamera.setTransitionStartNode( &cloudsCamera.mouseBasedNode );
		cloudsCamera.setTransitionTargetNode( transitionOutOption == OutLeft? &transitionOutLeft : &transitionOutRight );
	}
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

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::updateTransition(float percentComplete)
{
	if(transitioning) {
		float easedPercent = ofxTween::map(percentComplete, 0, 1, 0, 1, true, ofxEasingCubic(), transitionEase );//ofxEasingSine
		cloudsCamera.setTransitionPercent( easedPercent );
		if(selectedPortal != NULL){
			playerScaleModifier = powf(ofMap(percentComplete,0,.8,1.0,0.0,true),2.0);
		}
		else{
			playerScaleModifier = 1.0;
		}

		float easedRotPercent = easedPercent * easedPercent;//ofxTween::map(percentComplete, .6, 1, 0, 1, true, ofxEasingCubic(), transitionEase );//ofxEasingSine
		cloudsCamera.setTransitionRotationPercent( easedRotPercent );
		
//		cout <<"TRANSITIONING : easedValue = "<< easedPercent << endl;
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::transtionFinished()
{
//	cout << "transtionFinished()" <<endl;
	
	cloudsCamera.targetNode = NULL;
	cloudsCamera.startNode = NULL;
	transitioning = false;
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::setOutOption( OutOption outOption )
{
	switch (outOption) {
		case OutLeft:
//			cout << "setOutOption: OutLeft" << endl;
			transitionOutOption = OutLeft;
			break;
			
		case OutRight:
//			cout << "setOutOption: OutRight" << endl;
			transitionOutOption = OutRight;
			break;
			
		default:
			break;
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::lookThroughTransitionIn(){
	
	transitionCamTargetNode = &transitionInStart;
	
#ifdef HAS_GAMECAM
	transitionCam.setPosition( transitionInStart.getPosition() );
	transitionCam.setOrientation( transitionInStart.getOrientationQuat() );
	transitionCam.movedManually();
#endif
	
	//transitionCam.positionChanged = transitionCam.rotationChanged = true;
//	transitionCam.positionChanged = transitionCam.rotationChanged = true;
//	ofEventArgs args;
//	transitionCam.update(args);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::lookThroughTransitionOutLeft(){
	
	transitionCamTargetNode = &transitionOutLeft;
	
#ifdef HAS_GAMECAM
	transitionCam.setPosition( transitionOutLeft.getPosition() );
	transitionCam.setOrientation( transitionOutLeft.getOrientationQuat() );
	transitionCam.movedManually();
#endif
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::lookThroughTransitionOutRight()
{
	transitionCamTargetNode = &transitionOutRight;
	
#ifdef HAS_GAMECAM
	transitionCam.setPosition( transitionOutRight.getPosition() );
	transitionCam.setOrientation( transitionOutRight.getOrientationQuat() );
	transitionCam.movedManually();
#endif
	
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generateLines(){
	
	lines.clear();

	if(lineGranularity <= 0) lineGranularity = 1;
	if(lineSpacing <= 0) lineSpacing = 1;
	
	int height = 480;
	int width = 640;
	
    ofMesh m;
	//HORIZONTAL
	for (float ystep = 0; ystep < height; ystep += lineSpacing){
		for (float xstep = 0; xstep <= width - lineGranularity; xstep += lineGranularity){
			
			ofVec3f stepA = ofVec3f(xstep, ystep, 0);
			ofVec3f stepB = ofVec3f(xstep+lineGranularity, ystep, 0);
			ofVec3f mid   = stepA.getInterpolated(stepB, .5) + ofVec3f(ofRandom(lineRandomOffset), 0, 0);
			
			m.addNormal( stepA-mid );
			m.addVertex( mid );
			
			m.addNormal( stepB-mid );
			m.addVertex( mid );
		}
	}
	
	lines.setMesh(m, GL_STATIC_DRAW);
	lineVertexCount = m.getNumVertices();
    
//	cout << "GENERATED LINES " << lineVertexCount << endl;

	refreshLines = false;
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generateMesh(){
		
	if(xSimplify <= 0) xSimplify = 1.0;
	if(ySimplify <= 0) ySimplify = 1.0;


	int x = 0;
	int y = 0;

	int gw = ceil(640. / xSimplify);
	int w = gw*xSimplify;
	int h = 480.;
	
	vector<ofVec3f> vertices;
	vector<int> indeces;
	for (float y = 0; y < 480; y += ySimplify){
		for (float x = 0; x < 640; x += xSimplify){
			vertices.push_back(ofVec3f(x + ofRandomf()*randomVariance,
									   y + ofRandomf()*randomVariance,0));
		}
	}
	

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
	
    ofMesh m;
	for(int i = 0; i < indeces.size(); i+=3){
		
		ofVec3f& a = vertices[ indeces[i+0] ];
		ofVec3f& b = vertices[ indeces[i+1] ];
		ofVec3f& c = vertices[ indeces[i+2] ];
		ofVec3f mid = (a+b+c)/3.;
		
		ofVec3f toA = a-mid;
		ofVec3f toB = b-mid;
		ofVec3f toC = c-mid;
		
		m.addNormal(toA);
		m.addColor(ofFloatColor(toB.x/640.,toB.y/480.,toC.x/640.,toC.y/480.));
		m.addVertex(mid);

		m.addNormal(toB);
		m.addColor(ofFloatColor(toA.x/640.,toA.y/480.,toC.x/640.,toC.y/480.));
		m.addVertex(mid);
		
		m.addNormal(toC);
		m.addColor(ofFloatColor(toA.x/640.,toA.y/480.,toB.x/640.,toB.y/480.));
		m.addVertex(mid);
	}
    
    meshVertexCount = m.getNumVertices();
    mesh.setMesh(m, GL_STATIC_DRAW);
	//m.setMode(OF_PRIMITIVE_TRIANGLES);
	refreshMesh = false;

//	cout << "GENERATED MESH " << meshVertexCount << endl;
}


void CloudsVisualSystemRGBD::generateOcclusion(){
	if(occlusionXSimplify <= 0) occlusionXSimplify = 1.0;
	if(occlusionYSimplify <= 0) occlusionYSimplify = 1.0;
    
    
	int x = 0;
	int y = 0;
    
	int gw = ceil(640. / occlusionXSimplify);
	int w = gw*occlusionXSimplify;
	int h = 480.;
	
	vector<ofVec3f> vertices;
	vector<int> indeces;

	for (float y = 0; y < 480; y += occlusionYSimplify){
		for (float x = 0; x < 640; x += occlusionXSimplify){
			vertices.push_back( ofVec3f(x,y,0) );
		}
	}
	
	for (float ystep = 0; ystep < h-occlusionYSimplify; ystep += occlusionYSimplify){
		for (float xstep = 0; xstep < w-occlusionXSimplify; xstep += occlusionXSimplify){
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
	
    ofMesh m;
	for(int i = 0; i < indeces.size(); i+=3){
		
		ofVec3f& a = vertices[ indeces[i+0] ];
		ofVec3f& b = vertices[ indeces[i+1] ];
		ofVec3f& c = vertices[ indeces[i+2] ];
		ofVec3f mid = (a+b+c)/3.;
		
		ofVec3f toA = a-mid;
		ofVec3f toB = b-mid;
		ofVec3f toC = c-mid;
		
		m.addNormal(toA);
		m.addColor(ofFloatColor(toB.x/640.,toB.y/480.,toC.x/640.,toC.y/480.));
		m.addVertex(mid);
        
		m.addNormal(toB);
		m.addColor(ofFloatColor(toA.x/640.,toA.y/480.,toC.x/640.,toC.y/480.));
		m.addVertex(mid);
		
		m.addNormal(toC);
		m.addColor(ofFloatColor(toA.x/640.,toA.y/480.,toB.x/640.,toB.y/480.));
		m.addVertex(mid);
	}
    
    occlusionVertexCount = m.getNumVertices();
    occlusion.setMesh(m, GL_STATIC_DRAW);
    refreshOcclusion = false;
    
	cout << "GENERATED OCCLUSION " << occlusionVertexCount << endl;
}

void CloudsVisualSystemRGBD::speakerChanged(){

	if(timeline != NULL){
       timeline->hide();
	}
    
	assignAvailableQuestion(leftPortal);
	assignAvailableQuestion(rightPortal);
}

void CloudsVisualSystemRGBD::assignAvailableQuestion(CloudsPortal& portal){
	if(questions.size() == 0){
		return;
	}

//we always want to do this now
//#ifndef CLOUDS_SCREENING
	if(&portal != caughtPortal && &portal != selectedPortal){
		portal.clip = questions.back().clip;
		portal.topic = questions.back().topic;
		portal.question = questions.back().question;
	}
	questions.pop_back();
//#endif
}

void CloudsVisualSystemRGBD::selfDrawBackground(){
	
}

void CloudsVisualSystemRGBD::selfDrawDebug(){
	ofDrawSphere(translatedHeadPosition, 10);
}

void CloudsVisualSystemRGBD::selfSceneTransformation(){

}

void CloudsVisualSystemRGBD::selfDraw(){
	
    #ifdef OCULUS_RIFT 
//	#ifdef CLOUDS_HUD
//    if (hud != NULL) {
//        hud->draw3D(getOculusRift().baseCamera);
//    }
//    #endif
    #endif
	
	ofPushStyle();
	ofPushMatrix();
    
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	
	if(!getRGBDVideoPlayer().playingVO && getRGBDVideoPlayer().getPlayer().isLoaded() && drawRGBD){
		
        ofEnableAlphaBlending();
        
		voxelMesh.draw();
		
		//Enable smooth lines and screen blending
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
        
		setupRGBDTransforms();
        
        if(bEnableFill){
            if(bDrawOcclusion){
                drawOcclusionLayer();
            }
			
			glEnable(GL_CULL_FACE);
            glCullFace(cullFace);
			meshShader.begin();
			getRGBDVideoPlayer().begin(meshShader);
            
			meshShader.setUniform1f("meshAlpha", fillAlpha);
			meshShader.setUniform1f("triangleExtend",
                                    getRGBDTransitionValue());
            
			meshShader.setUniform1f("meshRetractionFalloff",fillRetractionFalloff);
			meshShader.setUniform1f("headMinRadius", fillFaceMinRadius);
			meshShader.setUniform1f("headFalloff", fillFaceFalloff);
			meshShader.setUniform1f("edgeAttenuateBase",powf(edgeAttenuate,2.0));
			meshShader.setUniform1f("edgeAttenuateExponent",edgeAttenuateExponent);
			meshShader.setUniform1f("forceGeoRetraction",0.0);            
			meshShader.setUniform1f("colorBoost", meshColorBoost);
			meshShader.setUniform1f("skinBoost", meshSkinBoost);
			meshShader.setUniform1f("maxActuatorRetract", 1.0);
            
            mesh.draw(GL_TRIANGLES, 0, meshVertexCount);
			
			meshShader.end();
			getRGBDVideoPlayer().end();
			glDisable(GL_CULL_FACE);
        }
        
		if(drawMesh){
            
            if(bDrawOcclusion){
                drawOcclusionLayer();
            }
			
            if(bEnableFill){
                ofEnableBlendMode(OF_BLENDMODE_SCREEN);
            }
            
			glEnable(GL_CULL_FACE);
            glCullFace(cullFace);
            
			meshShader.begin();
			getRGBDVideoPlayer().begin(meshShader);
		
			meshShader.setUniform1f("meshAlpha", meshAlpha);
			meshShader.setUniform1f("triangleExtend",
										getRGBDTransitionValue());
            
			meshShader.setUniform1f("meshRetractionFalloff",meshRetractionFalloff);
			meshShader.setUniform1f("headMinRadius", meshFaceMinRadius);
			meshShader.setUniform1f("headFalloff", meshFaceFalloff);
			meshShader.setUniform1f("edgeAttenuateBase",powf(edgeAttenuate,2.0));
			meshShader.setUniform1f("edgeAttenuateExponent",edgeAttenuateExponent);
			meshShader.setUniform1f("forceGeoRetraction",meshForceGeoRetraction);
			meshShader.setUniform3f("actuatorDirection",
                                    meshActuator.x,
                                    meshActuator.y,
                                    meshActuator.z);
            
			meshShader.setUniform1f("colorBoost", meshColorBoost);
			meshShader.setUniform1f("skinBoost", meshSkinBoost);
			meshShader.setUniform1f("maxActuatorRetract", meshMaxActuatorRetract);
            
            mesh.draw(GL_TRIANGLES, 0, meshVertexCount);
			
			meshShader.end();
			getRGBDVideoPlayer().end();
			glDisable(GL_CULL_FACE);
		}
		
        if(!bDrawOcclusion){
            glDisable(GL_DEPTH_TEST);
        }
        
//		ofEnableBlendMode(OF_BLENDMODE_ADD);
        ofEnableBlendMode(OF_BLENDMODE_SCREEN);
        
		if(drawLines){
            
            if(bDrawOcclusion){
                glClear(GL_DEPTH_BUFFER_BIT);
                drawOcclusionLayer();
            }

			ofSetLineWidth(lineThickness);
			lineShader.begin();
			
            getRGBDVideoPlayer().flowPosition = lineFlowPosition * (linesFlowUp?-1:1);
            
			getRGBDVideoPlayer().begin(lineShader);
			
			lineShader.setUniform1f("lineExtend", getRGBDTransitionValue());
            lineShader.setUniform1f("headMinRadius", meshFaceMinRadius);
			lineShader.setUniform1f("headFalloff", meshFaceFalloff);
			lineShader.setUniform1f("edgeAttenuateBase",powf(edgeAttenuate,2.0));
			lineShader.setUniform1f("edgeAttenuateExponent",edgeAttenuateExponent);
			lineShader.setUniform1f("headOverlap",lineHeadOverlap);
			lineShader.setUniform1f("alpha", lineAlpha);
            lineShader.setUniform1f("colorBoost", lineColorBoost);
			lineShader.setUniform1f("skinBoost", lineSkinBoost);

			lineShader.setUniform3f("actuatorDirection",
                                    lineActuator.x,
                                    lineActuator.y,
                                    lineActuator.z);
            lineShader.setUniform1f("maxActuatorRetract", lineMaxActuatorRetract);
            
            lines.draw( ofGetGLPrimitiveMode(OF_PRIMITIVE_LINES), 0, lineVertexCount);
			
			lineShader.end();
			getRGBDVideoPlayer().end();
		}
		
        
		if(pointLayer1.drawPoints || pointLayer2.drawPoints){
            
            if(bDrawOcclusion){
                glClear(GL_DEPTH_BUFFER_BIT);
                drawOcclusionLayer();
            }
            
            pointLayer1.draw();
            pointLayer2.draw();

		}
	}
	
	if(drawParticulate){
		glEnable(GL_DEPTH_TEST);
//		particulateController.draw();
		
	}
	
	
	
	glPopAttrib();
	ofPopMatrix();
	ofPopStyle();
	


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
	
    if(!paused){
        drawQuestions();
    }
}

float CloudsVisualSystemRGBD::getRGBDTransitionValue(){

	return getRGBDVideoPlayer().getFadeIn()  *
           getRGBDVideoPlayer().getFadeOut() *
		   visualSystemFadeValue * questionSelectFade;
}

void CloudsVisualSystemRGBD::drawOcclusionLayer(){
    
	ofPushMatrix();
    if(!drawOcclusionDebug){
        float occludeZOffset = 5.44;
		//occludeZOffset = ofMap(ofGetMouseX(), 0, 500, -50,50, true);
		//cout << " Z occlude " << occludeZOffset << endl;
        ofTranslate(0, 0, occludeZOffset);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);  // We want depth test !
        glDepthFunc(GL_LESS);     // We want to get the nearest pixels
        glColorMask(0,0,0,0);     // Disable color, we only want depth.
        glDepthMask(GL_TRUE);     // Ask z writing
    }
    
    occlusionShader.begin();
    
    getRGBDVideoPlayer().begin(occlusionShader);
    
    occlusionShader.setUniform1f("triangleExtend", getRGBDTransitionValue());
    occlusionShader.setUniform1f("meshRetractionFalloff",occlusionMeshRetractionFalloff);
    occlusionShader.setUniform1f("headMinRadius", occlusionMeshFaceMinRadius);
    occlusionShader.setUniform1f("headFalloff", occlusionMeshFaceFalloff);
    
    occlusion.draw(GL_TRIANGLES, 0, occlusionVertexCount);
    
	getRGBDVideoPlayer().end();

    occlusionShader.end();
    
    if(!drawOcclusionDebug){
		glEnable(GL_DEPTH_TEST);  // We still want depth test
        glDepthFunc(GL_LEQUAL);   // EQUAL should work, too. (Only draw pixels if they are the closest ones)
        glColorMask(1,1,1,1);     // We want color this time
		//#ifndef OCULUS_RIFT
        glDepthMask(GL_FALSE);
		//#endif
    }
    
	ofPopMatrix();
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
#ifdef OCULUS_RIFT
    resetPortal.draw();
#endif
    
	CloudsPortal::shader.end();
	drawQuestionType();
	glEnable(GL_DEPTH_TEST);

}

//called either in draw for rift, or draw overlay for normal
void CloudsVisualSystemRGBD::drawQuestionType(){
	if(!questionFont.isLoaded() || currentQuestionFontSize != questionFontSize){
		questionFont.loadFont(GetFontBuffer(), questionFontSize);
		currentQuestionFontSize = questionFontSize;
	}
	
	string questionText = getQuestionText();
	if(questionText == "" || (caughtPortal == NULL && selectedPortal == NULL) ){
		return;
	}
	//cout << "Question Text " << questionText << endl;
	ofVec3f basePosition = caughtPortal != NULL ? caughtPortal->hoverPosition : selectedPortal->hoverPosition;
	float textOpacity = 1.0;
	questionFont.setLetterSpacing(questionFontTracking);
		
	float questionTextWidth = questionFont.stringWidth(questionText);
	float questionTextWidth2,questionTextHeight2;
	string secondLine;
	bool twoLines = questionTextWidth > questionFontSplitWidth;
	if(questionText.find("\n") != string::npos){
		twoLines = true;
		vector<string> split = ofSplitString(questionText, "\n", true,true);
		questionText = split[0];
		secondLine = split[1];
		questionTextWidth  = questionFont.stringWidth(questionText);
		questionTextWidth2 = questionFont.stringWidth(secondLine);
	}
	else if(twoLines){
		vector<string> pieces = ofSplitString(questionText, " ", true,true);
		vector<string> firstHalf;
		vector<string> secondHalf;
		int halfsize = pieces.size() / 2;
		firstHalf.insert(firstHalf.begin(), pieces.begin(), pieces.begin() + halfsize);
		secondHalf.insert(secondHalf.begin(), pieces.begin() + halfsize, pieces.end());
		questionText = ofJoinString(firstHalf, " ");
		secondLine = ofJoinString(secondHalf, " ");
		questionTextWidth  = questionFont.stringWidth(questionText);
		questionTextWidth2 = questionFont.stringWidth(secondLine);

	}
	float questionTextHeight = questionFont.stringHeight(questionText);

	ofPushMatrix();
	ofPushStyle();
	ofEnableAlphaBlending();
	ofDisableLighting();

	#ifdef OCULUS_RIFT
	getOculusRift().multBillboardMatrix( basePosition );
	#else
	ofNode n;
	n.setPosition( basePosition );
	n.lookAt(cloudsCamera.getPosition(), cloudsCamera.getUpDir());
	ofVec3f axis; float angle;
	n.getOrientationQuat().getRotate(angle, axis);
	// Translate the object to its position.
	ofTranslate( basePosition );
	// Perform the rotation.
	ofRotate(angle, axis.x, axis.y, axis.z);
	#endif
	
	ofRotate(180, 0, 0, 1); //flip around
	ofScale(questionFontScale,questionFontScale,questionFontScale);
	ofSetColor(255,255*textOpacity);

	questionFont.drawString(questionText, -questionTextWidth*.5, questionYOffset - questionTextHeight*.5);
	if(twoLines){
		questionFont.drawString(secondLine, -questionTextWidth2*.5, questionYOffset + questionTextHeight*1.5);
	}
	
	ofEnableLighting();
	ofPopStyle();
	ofPopMatrix();
}

void CloudsVisualSystemRGBD::drawCursors(){
    map<int, CloudsInteractionEventArgs>& inputPoints = GetCloudsInputPoints();
    for (map<int, CloudsInteractionEventArgs>::iterator it = inputPoints.begin(); it != inputPoints.end(); ++it) {
        if (it->second.primary) {
            // override primaryCursorMode
            selfDrawCursor(stickyCursor, it->second.dragged, caughtPortal? CURSOR_MODE_DRAW : CURSOR_MODE_CAMERA, it->second.focus);
        }
        else {
            selfDrawCursor(it->second.position, it->second.dragged, secondaryCursorMode, it->second.focus);
        }
    }
}

void CloudsVisualSystemRGBD::selfDrawOverlay() {
    if(bPortalDebugOn){
        ofPushStyle();
        
        for(int i = 0; i < portals.size(); i++){

            if(!portals[i]->onScreen){
                continue;
            }
            if(portals[i] == caughtPortal){
                ofFill();
            }
            else{
                ofNoFill();
            }
            ofSetColor(ofColor::salmon, 200);
            ofCircle(portals[i]->screenPosition.x, portals[i]->screenPosition.y, portalTugDistance.min);
            ofNoFill();
            ofSetColor(ofColor::steelBlue, 200);
            ofCircle(portals[i]->screenPosition.x, portals[i]->screenPosition.y, portalTugDistance.max);
        }
        
        ofPopStyle();
    }
}

void CloudsVisualSystemRGBD::selfExit(){

}

void CloudsVisualSystemRGBD::selfBegin(){
    
    bPortalDebugOn = false;
	cloudsCamera.jumpToPosition();
    
	if(timeline != NULL){
		timeline->hide();
	}
    
    //clear any previously selected portals
    caughtPortal = NULL;
    selectedPortal = NULL;
	playerScaleModifier = 1.0;

    //make sure any portals are clear
    leftPortal.clearSelection();
    rightPortal.clearSelection();
#ifdef OCULUS_RIFT
    resetPortal.clearSelection();
#endif
    
}

void CloudsVisualSystemRGBD::selfEnd(){

    if(portalToClear != NULL){
		if(questions.size() != 0){
			portalToClear->clip = questions[0].clip;
			portalToClear->topic = questions[0].topic;
			portalToClear->question = questions[0].question;
			questions.erase( questions.begin() );
			cout << "******ERASING QUESTIONS. SIZE IS NOW " << questions.size() << endl;
		}
        else{
			portalToClear->topic = "";
			portalToClear->question = "";
        }
		portalToClear = NULL;
	}

}

bool CloudsVisualSystemRGBD::isQuestionSelectedAndClipDone(){
    return selectedPortal != NULL && getRGBDVideoPlayer().isDone();
}

string CloudsVisualSystemRGBD::getQuestionText(){
    if(bPortalDebugOn){
        return "WHAT DOES IT FEEL LIKE TO CODE?";
    }
    else if(caughtPortal != NULL){
        return GetTranslationForString( ofToUpper(caughtPortal->question) );
    }
    else if(selectedPortal != NULL){
        return GetTranslationForString( ofToUpper(selectedPortal->question) );
    }
    return "";
}
//
//#ifdef CLOUDS_SCREENING
//bool CloudsVisualSystemRGBD::hasQuestionsRemaining(){
//	return !(questions.size() == 0 && leftPortal.question == "" && rightPortal.question == "");
//}
//
//void CloudsVisualSystemRGBD::clearQuestionQueue(){
//	questions.clear();
//	leftPortal.question  = "";
//	rightPortal.question = "";
//}
//
//#endif

vector<QuestionQueue>& CloudsVisualSystemRGBD::getQuestionQueue(){
    return questions;
}

void CloudsVisualSystemRGBD::removeQuestionFromQueue(CloudsClip* clip){
    for(int i = questions.size()-1; i >= 0; i--){
        if(questions[i].clip == clip){
            questions.erase(questions.begin() + i);
            return;
        }
    }
    
    ofLogError("CloudsVisualSystemRGBD::removeQuestionFromQueue") << "Failed to remove clip " << clip->getLinkName() << " from queue";
}

bool CloudsVisualSystemRGBD::isQuestionSelected(){
#ifdef OCULUS_RIFT
	return selectedPortal != NULL && selectedPortal != &resetPortal;
#else
    return selectedPortal != NULL;
#endif
}

CloudsPortal* CloudsVisualSystemRGBD::getSelectedQuestion(){
   return selectedPortal;
}

bool CloudsVisualSystemRGBD::isResetSelected(){
#ifdef OCULUS_RIFT
	return selectedPortal == &resetPortal;
#else
    return false;
#endif
}

void CloudsVisualSystemRGBD::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		
		loadShader();
		voxelMesh.reloadShaders();
		
	}
    
    if(args.key == 'P'){
        bPortalDebugOn = !bPortalDebugOn;
    }
}

void CloudsVisualSystemRGBD::selfKeyReleased(ofKeyEventArgs& args){

}

void CloudsVisualSystemRGBD::selfMouseDragged(ofMouseEventArgs& data){
    cursor.set(data.x, data.y, cursor.z);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfMouseMoved(ofMouseEventArgs& data){
    cursor.set(data.x, data.y, cursor.z);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfMousePressed(ofMouseEventArgs& data){
	#ifdef MOUSE_INPUT
	if(leftPortal.question != ""){
		leftPortal.mousePressed(data);
	}
	if(rightPortal.question != ""){
		rightPortal.mousePressed(data);
	}
	#endif
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
    refreshOcclusion = true;
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
	
	
	if(e.widget->getName() == "Line Spacing" ||
       e.widget->getName() == "Line Granularity" ||
       e.widget->getName() == "Line Random Offset")
    {
		refreshLines = true;
	}
	else if(e.widget->getName() == "X Simplify" ||
			e.widget->getName() == "Y Simplify" ||
			e.widget->getName() == "Random Variance")
	{
		refreshMesh = true;
	}
    else if(e.widget->getName() == "Occl X Simplify" ||
            e.widget->getName() == "Occl Y Simplify")
    {
        refreshOcclusion = true;
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
}
