
#include "CloudsVisualSystemRGBD.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
map<string, int> CloudsVisualSystemRGBD::appearances;

//--------------------------------------------------------------
string CloudsVisualSystemRGBD::getSystemName(){
	return "RGBD";
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetup(){
	currentFlowPosition = 0;
	visualSystemFadeValue = 1.0;
	
	drawMesh = false;
	
	questionLifeSpan = 3;

	rgbdShader.load( getDataPath() + "shaders/rgbdcombined" );
	CloudsQuestion::reloadShader();

	lastPercentChanceOfPoint = 0;
	lastTriangulationXStep = 0;
	lastTriangulationYStep = 0;
	
	percentChanceOfPoint = .5;
	triangulationXStep = 1.5;
	triangulationYStep = 1.5;
	
	generatePointGrid();
	generateScanlines();
	generateTriangulation();
	
	particulateController.setParticleCount(20000);
	particulateController.setShaderDirectory(getDataPath() + "shaders/GPUParticles/");
	particulateController.setup();
	
	cloudsCamera.setup();
	cloudsCamera.lookTarget = ofVec3f(0,25,0);
	setCurrentCamera(cloudsCamera);
	
//	generator.setup();
	
	displayFont.loadFont(getDataPath() + "font/materiapro_light.ttf", 14);
	
	//TODO: do this elsewhere
	transitioning = transitioningIn = transitioningOut = false;
	
	transitionInStart.setPosition( 0, 0, -1000 );
	transitionInStart.rotate( 180, ofVec3f(0, 1, 0 ) );
	
	transitionOutTarget.setPosition( 0, 0, -1001 );
	transitionOutTarget.rotate( 180, ofVec3f(0, 1, 0 ) );
	
	transitionCam.useArrowKeys = true;
	
	transitionTarget = &transitionOutTarget;
	
	drawTransitionNodes = false;
	
	transitionCam.setup();
	
    captionFontSize = 12;
    rebuildCaptionFont();


//	enum RGBDTransitionType
//  {
//	TWO_DIMENSIONAL = 0,
//	FLY_THROUGH = 1,
//	WHIP_PAN = 2,
//	RGBD = 3
//  };

}

void CloudsVisualSystemRGBD::rebuildCaptionFont(){
    if(bUseOculusRift){
        captionFont.loadFont(getDataPath() + "font/MateriaPro_Regular.ttf", captionFontSize);
    }
    else{
        captionFont.loadFont(getDataPath() + "font/materiapro_light.ttf", captionFontSize);
    }
}

void CloudsVisualSystemRGBD::setTransitionNodes( RGBDTransitionType transitionType ){
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
	
	meshGui = new ofxUISuperCanvas("MESH", gui);
	meshGui->copyCanvasStyle(gui);
    meshGui->copyCanvasProperties(gui);
    meshGui->setName("Mesh");
    meshGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	meshGui->addToggle("DRAW CLOUD", &drawCloud);
	
	meshGui->addSlider("CLOUD SCALE", .001,  1.0, &pointcloudScale);
	meshGui->addSlider("CLOUD OFFSET",   0, -800, &pointcloudOffsetZ);
	
	meshGui->addLabel("MESH");
	meshGui->addToggle("DRAW MESH", &drawMesh);
	meshGui->addSlider("MESH ALPHA", 0, 1.0f, &meshAlpha);
	meshGui->addSlider("SKIN MULTIPLIER", 0, 1.0f, &skinMultiplier);
	meshGui->addSlider("EYE MULTIPLIER", 0, 1.0f, &eyeMultiplier);
	
	meshGui->addLabel("POINTS");
	meshGui->addToggle("DRAW POINTS", &drawPoints);
	meshGui->addSlider("SIZE MAX", 0, 3, &pointSizeMax);
	meshGui->addSlider("SIZE MIN", 0, 3, &pointSizeMin);
	meshGui->addSpacer();
	meshGui->addSlider("P GRID ALPHA", 0, 1.0, &pointGridAlpha);
	meshGui->addSlider("P GRID VERT SPACE", 1, 10, &pointVerticalSpace);
	meshGui->addSlider("P GRID HORI SPACE", 1, 10, &pointHorizontalSpace);
	meshGui->addSpacer();
	meshGui->addSlider("NUM RANDOM POINTS", 0, 500000.0f, &numRandomPoints);
	meshGui->addSlider("RANDOM ALPHA", 0, 1.0f, &randomPointAlpha);
	
	meshGui->addLabel("LINES");
	meshGui->addToggle("DRAW LINES", &drawScanlines);
	meshGui->addSlider("VERT LINE SPACE", .5, 12, &scanlineSimplify.x);
	meshGui->addSlider("VERT LINE ALPHA", 0, 1.0, &verticalScanlineAlpha);
	meshGui->addSlider("VERT LINE THICKNESS", 0, 2.0, &verticalScanlineThickness);
	meshGui->addSpacer();
	meshGui->addSlider("HORIZ LINE SPACE", .5, 12, &scanlineSimplify.y);
	meshGui->addSlider("HORIZ LINE ALPHA", 0, 1.0, &horizontalScanlineAlpha);
	meshGui->addSlider("HORIZ LINE THICKNESS", 0, 2.0, &horizontalScanlineThickness);
	
	meshGui->addLabel("FLOW");
	meshGui->addSlider("CLOUD FLOW", -5, 5, &cloudFlow);
	
	meshGui->addSlider("LIGHT_OFFSET_Y", -100, 100, &lightOffsetY);
	meshGui->addSlider("LIGHT_OFFSET_Z", 0, 100, &lightOffsetZ);

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
	
	connectorGui = new ofxUISuperCanvas("CONNECTORS", gui);
	connectorGui->copyCanvasStyle(gui);
	connectorGui->copyCanvasProperties(gui);
	connectorGui->setName("connectors");
	connectorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
//	connectorGui->addSlider("Num Particles", 50, 64*64, &generator.numParticles);
//	connectorGui->addToggle("Draw Connections", &generator.drawConnections);
//	connectorGui->addSlider("Min Connection Distance", 1, 100, &generator.minDistance);
//	connectorGui->addSlider("Boundary Size", 100, 1000, &generator.boundarySize);
//	connectorGui->addSlider("Max Connections", 1, 10, &generator.maxConnections);
	
	guis.push_back(connectorGui);
	guimap[connectorGui->getName()] = connectorGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfUpdate(){
	
	if(drawCloud){
		if(refreshScanlineMesh){
			generateScanlines();
		}
		
		if(refreshPointcloud){
			generatePointGrid();
		}
		
		if(numRandomPoints != randomPoints.getNumVertices()){
			generateRandomPoints();
		}
		
		if(refreshTriangulation){
			generateTriangulation();
		}
		
		currentFlowPosition += cloudFlow;
		getRGBDVideoPlayer().flowPosition = currentFlowPosition;
	}

	if(drawParticulate){
		
		particulateController.birthPlace = translatedHeadPosition;
		
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		particulateController.getPoints().color = ofFloatColor::fromHsb(pointColor.x, pointColor.y, pointColor.z);
		particulateController.getPoints().color.a = pointColor.w;
		
		particulateController.update();
	}
	
	updateQuestions();
	updateTransition();
    
    cloudsCaption.update();
	
	if(currentCamera == &transitionCam){
		transitionCamTargetNode->setPosition( transitionCam.getPosition() );
		transitionCamTargetNode->setOrientation( transitionCam.getOrientationQuat() );
	}
	else{
		cloudsCamera.driftNoiseSpeed = caughtQuestion ? 0 : attenuatedCameraDrift;
	}

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
		
        if(selectedQuestion == NULL && questions[i]->isSelected() && ofGetMousePressed()){
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

    if(questions.size() > 0)    {
        selectedQuestion = questions[0];
    }
    else{
        cout << "No questions!" << endl;
    }
    
}

void CloudsVisualSystemRGBD::clearQuestions(){
	
	cout << "Clearing questions!" << endl;
	
    selectedQuestion = NULL;
	caughtQuestion = NULL;
    for (int i = 0; i<questions.size(); i++) {
        delete questions[i];
    }
    questions.clear();

}

void CloudsVisualSystemRGBD::updateTransition(){
	
	if(transitioning){
		//get our mixing value by mapping currentTime to the transition start and end time
		float t = ofGetElapsedTimef();
		float x = ofxTween::map(t, transitionStartTime, transitionEndTime, 0, 1, true, transitionEase );
		
		if(t >= transitionEndTime ){
			cout <<"CloudsVisualSystemRGBD: transition ended "<< ofGetElapsedTimef() << endl << endl;
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
	
	cout << endl <<endl <<endl <<endl;
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
	
	setCurrentCamera( transitionCam );
	
}

void CloudsVisualSystemRGBD::lookThroughTransitionOut(){
	
	transitionCamTargetNode = &transitionOutTarget;
	
	transitionCam.setPosition( transitionOutTarget.getPosition() );
	transitionCam.setOrientation( transitionOutTarget.getOrientationQuat() );
	
	setCurrentCamera( transitionCam );
}


//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generatePointGrid(){
	
	pointGrid.clear();
	
	pointVerticalSpace = MAX(pointVerticalSpace, 1);
	pointHorizontalSpace = MAX(pointHorizontalSpace, 1);
	for(float y = 0; y <= 480; y+= pointVerticalSpace){
		for(float x = 0; x <= 640; x+= pointHorizontalSpace){
			pointGrid.addVertex(ofVec3f(x,y,0));
		}
	}
//        ofRange(clip1.start,clip1.end).intersects(ofRange(clip2.start,clip2.end))

    //of
	pointGrid.clearIndices();
	int x = 0;
	int y = 0;
	
	int gw = ceil(640 / pointHorizontalSpace);
	int w = gw*pointHorizontalSpace;
	int h = 480;
	
	for (float ystep = 0; ystep < h-pointVerticalSpace; ystep += pointVerticalSpace){
		for (float xstep = 0; xstep < w-pointHorizontalSpace; xstep += pointHorizontalSpace){
			ofIndexType a,b,c;
			
			a = x+y*gw;
			b = (x+1)+y*gw;
			c = x+(y+1)*gw;
			pointGrid.addIndex(a);
			pointGrid.addIndex(b);
			pointGrid.addIndex(c);
            
			a = (x+1)+(y+1)*gw;
			b = x+(y+1)*gw;
			c = (x+1)+(y)*gw;
			pointGrid.addIndex(a);
			pointGrid.addIndex(b);
			pointGrid.addIndex(c);
			
			x++;
		}
		
		y++;
		x = 0;
	}

//	pointGrid.setMode(OF_PRIMITIVE_POINTS);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generateRandomPoints(){
	if(numRandomPoints == 0){
		randomPoints.clear();
	}
	else if(numRandomPoints < randomPoints.getNumVertices() ){
		randomPoints.getVertices().erase(randomPoints.getVertices().begin(), randomPoints.getVertices().begin() + (randomPoints.getNumVertices() - numRandomPoints) );
	}
	while(numRandomPoints > randomPoints.getNumVertices()){
		randomPoints.addVertex( ofVec3f(ofRandom(640),ofRandom(480),0) );
	}
	
	randomPoints.setMode(OF_PRIMITIVE_POINTS);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::generateScanlines(){

	verticalScanLines.clear();
	horizontalScanLines.clear();
	
	if(scanlineSimplify.x <= 0) scanlineSimplify.x = 1;
	if(scanlineSimplify.y <= 0) scanlineSimplify.y = 1;
	
	int height = 480;
	int width = 640;
	
	//VERTICAL
	for (float xstep = 0; xstep <= width-scanlineSimplify.x; xstep += scanlineSimplify.x){
		for (float ystep = 0; ystep <= height-scanlineSimplify.y; ystep += scanlineSimplify.y){
			verticalScanLines.addVertex( ofVec3f(xstep, ystep, 0) );
			verticalScanLines.addVertex( ofVec3f(xstep, ystep+scanlineSimplify.y, 0) );
		}
	}
	
	//HORIZONTAL
	for (float ystep = 0; ystep <= height-scanlineSimplify.y; ystep += scanlineSimplify.y){
		for (float xstep = 0; xstep <= width-scanlineSimplify.x; xstep += scanlineSimplify.x){
			
//			float ystepOffset = ofRandom(-scanlineSimplify.y/4,scanlineSimplify.y/4);
			
//			horizontalScanLines.addColor(ofFloatColor(ofRandom(1.)));
			ofVec3f stepA = ofVec3f(xstep, ystep, 0);
			ofVec3f stepB = ofVec3f(xstep+scanlineSimplify.x, ystep, 0);
			ofVec3f mid = (stepA + stepB) / 2.0;
			
			horizontalScanLines.addNormal( mid );
			horizontalScanLines.addColor( ofFloatColor( stepB.x/640,stepB.y/480, stepB.x/640,stepB.y/480) );
			horizontalScanLines.addVertex( stepA);
			
			horizontalScanLines.addNormal( mid );
			horizontalScanLines.addColor( ofFloatColor( stepA.x/640,stepA.y/480, stepA.x/640,stepA.y/480 ) );
			horizontalScanLines.addVertex( stepB);
		}
	}

	verticalScanLines.setMode( OF_PRIMITIVE_LINES );
	horizontalScanLines.setMode( OF_PRIMITIVE_LINES );
	
	refreshScanlineMesh = false;
}

void CloudsVisualSystemRGBD::speakerChanged(){

	//check speaker vars
//	this->speakerFirstName = speakerFirstName;
//	this->speakerLastName = speakerLastName;
//	this->quoteName = quoteName;
    
	// Add an appearance for this speaker.
    string key = speakerFirstName + " " + speakerLastName;
    
    if (appearances.find(key) == appearances.end()) {
        appearances[key] = 1;
    }
    else {
        appearances[key]++;
    }
    
    cout << "CloudsVisualSystemRGBD::speakerChanged " << speakerFirstName << " " << speakerLastName << ": " << quoteName << " (" << appearances[key] << ")" << endl;
    
    if (appearances[key] == 1) {
        cloudsCaption.font = &captionFont;
        cloudsCaption.caption = key;
        cloudsCaption.isEnabled = true;
        cloudsCaption.begin();
    }
    else {
        cloudsCaption.isEnabled = false;
    }
}

void CloudsVisualSystemRGBD::generateTriangulation(){
	

	if(percentChanceOfPoint == lastPercentChanceOfPoint ||
	   triangulationXStep == lastTriangulationXStep ||
	   triangulationYStep == lastTriangulationYStep)
	{
		return;
	}

	delaunay.reset();

	for(float y = 0; y < 480; y += triangulationYStep){
		for(float x = 0; x < 640; x += triangulationXStep){
			if(ofRandomuf() < percentChanceOfPoint){
				delaunay.addPoint(ofVec2f(x,y));
			}
		}
	}
	
	lastPercentChanceOfPoint = percentChanceOfPoint;
	lastTriangulationXStep = triangulationXStep;
	lastTriangulationYStep = triangulationYStep;

	delaunay.triangulate();
	
	ofMesh& dmesh = delaunay.triangleMesh;
	triangulation.clear();
	for(int i = 0; i < dmesh.getNumIndices(); i += 3){
		
		ofVec3f& a = dmesh.getVertices()[ dmesh.getIndices()[i  ] ];
		ofVec3f& b = dmesh.getVertices()[ dmesh.getIndices()[i+1] ];
		ofVec3f& c = dmesh.getVertices()[ dmesh.getIndices()[i+2] ];
		
		ofVec3f center = (a+b+c)/3.0;
		
		triangulation.addColor( ofFloatColor(b.x/640., b.y/480., c.x/640., c.y/480.) );
		triangulation.addNormal(center);
		triangulation.addVertex(a);

		triangulation.addColor( ofFloatColor(a.x/640., a.y/480., c.x/640., c.y/480.) );
		triangulation.addNormal(center);
		triangulation.addVertex(b);
		
		triangulation.addColor( ofFloatColor(a.x/640., a.y/480., b.x/640., b.y/480.) );
		triangulation.addNormal(center);
		triangulation.addVertex(c);
	
	}
	
	refreshTriangulation = false;
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
	
	ofPushMatrix();
	
	if(drawCloud && hasSpeaker){

//		cout << "RGBD DRAW" << endl;
		
		ofPushStyle();
		ofPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		
		//Enable smooth lines and screen blending
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		
		setupRGBDTransforms();
		
		rgbdShader.begin();
		getRGBDVideoPlayer().setupProjectionUniforms(rgbdShader);
		
//		cout << "base multiplier " << getRGBDVideoPlayer().getFadeIn() * getRGBDVideoPlayer().getFadeOut() << endl;
		rgbdShader.setUniform1f("fadeValue", 1.0);
//		rgbdShader.setUniform1f("fadeValue", getRGBDVideoPlayer().getFadeIn() * getRGBDVideoPlayer().getFadeOut() );
		float transitionValue = 1.0 - getRGBDVideoPlayer().getFadeIn() * getRGBDVideoPlayer().getFadeOut() * visualSystemFadeValue;
		ofxEasingCubic cub;
		rgbdShader.setUniform1f("triangleContract", ofxTween::map(transitionValue, 0, 1.0, 0, 1.0, true, cub, ofxTween::easeOut));
		rgbdShader.setUniform1f("eyeMultiplier", 0.0);
		rgbdShader.setUniform1f("skinMultiplier", 0.0);
		rgbdShader.setUniform1f("baseMultiplier", 1.0);
		
		rgbdShader.setUniform3f("headPosition",
								getRGBDVideoPlayer().headPosition.x,
								-getRGBDVideoPlayer().headPosition.y,
								getRGBDVideoPlayer().headPosition.z);
		
		rgbdShader.setUniform3f("lightPosition",
								getRGBDVideoPlayer().headPosition.x,
								getRGBDVideoPlayer().headPosition.y+lightOffsetY*100,
								getRGBDVideoPlayer().headPosition.z+lightOffsetZ*100);
		
		
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
		ofDisableAlphaBlending();
		//ofEnableAlphaBlending();
		ofEnableBlendMode(OF_BLENDMODE_SCREEN);
		if(drawMesh){
			
			ofPushStyle();
			ofSetColor(255, 255, 255);
			rgbdShader.setUniform1f("isMeshed", 1);
			rgbdShader.setUniform1f("headAttenuateMix", 1.);
			rgbdShader.setUniform1f("flowPosition", 0);
			glEnable(GL_DEPTH_TEST);
			
//			glDepthFunc(GL_LEQUAL);
			glEnable(GL_CULL_FACE);
			//if(bUseOculusRift){
		//		glCullFace(GL_FRONT);
		//	}
		//	else{
				glCullFace(GL_BACK);
		//	}
			
			triangulation.draw();

			glDisable(GL_CULL_FACE);
			ofPopStyle();
		}
			
		
		glDisable(GL_DEPTH_TEST);
		ofEnableBlendMode(OF_BLENDMODE_ADD);
//		glDepthFunc(GL_LESS);

		if(drawPoints){
			rgbdShader.setUniform1f("flowPosition", currentFlowPosition);
			rgbdShader.setUniform1f("isMeshed", 0);
			rgbdShader.setUniform1f("headAttenuateMix", 0.);
			//draw the points
			glPointSize(pointSizeMin);
			ofSetColor(255*randomPointAlpha);
			randomPoints.draw();
			ofSetColor(255*pointGridAlpha);
			pointGrid.drawVertices();
		}
		
		rgbdShader.setUniform2f("simplify", scanlineSimplify.x, scanlineSimplify.y);

		//draw the lines
		if(drawScanlines){
			rgbdShader.setUniform1f("flowPosition", 0.0);
			rgbdShader.setUniform1f("isMeshed", 1.0);
			rgbdShader.setUniform1f("headAttenuateMix", .0);

			ofSetLineWidth(horizontalScanlineThickness);
			horizontalScanLines.draw();

//			rgbdShader.setUniform1f("flowPosition", 0);
//			ofSetLineWidth(verticalScanlineThickness);
//			ofSetColor(255*verticalScanlineAlpha);
//			verticalScanLines.draw();
		}
		
		//subtractive wirerame
		//glDisable(GL_DEPTH_TEST);
		ofPushMatrix();
		ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
		glPushAttrib(GL_POLYGON_BIT);
		glPolygonOffset(1, 0);
//		triangulation.drawWireframe();
		glPopAttrib();
		ofEnableAlphaBlending();
		ofPopMatrix();

//		connectionGenerator.draw();
//		generator.draw();
		
		rgbdShader.end();
				
		glPopAttrib();
		ofPopMatrix();
		ofPopStyle();
	}
		
	if(drawParticulate){
		glEnable(GL_DEPTH_TEST);
		particulateController.draw();
	}
	
	ofPopMatrix();
	

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
    
    cloudsCaption.drawOverlay();

	//test overlay
	ofSetColor(0,0,0,0);
	ofRect(20, 20, 300,300);
	ofPopStyle();
}

void CloudsVisualSystemRGBD::selfExit(){
	
}

void CloudsVisualSystemRGBD::selfBegin(){
	
	cloudsCamera.jumpToPosition();
    
    cloudsCaption.begin();
}

void CloudsVisualSystemRGBD::selfEnd(){
	
}

bool CloudsVisualSystemRGBD::isQuestionSelectedAndClipDone(){
    return  selectedQuestion != NULL && getRGBDVideoPlayer().isDone();
}

CloudsQuestion* CloudsVisualSystemRGBD::getSelectedQuestion(){
    return selectedQuestion;
}

void CloudsVisualSystemRGBD::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		particulateController.reloadShaders();
		CloudsQuestion::reloadShader();
		rgbdShader.load( getDataPath() + "shaders/rgbdcombined" );
	}
	
	if(args.key == 'v' && currentCamera != &transitionCam ){
		lookThroughTransitionIn();
	}
	
	if(args.key == 'V' && currentCamera != &transitionCam ){
		lookThroughTransitionOut();
	}
	
}

void CloudsVisualSystemRGBD::selfKeyReleased(ofKeyEventArgs & args){
	if(args.key == 'V' || args.key == 'v' ){
		
		printTransitionNodes();
		
		setCurrentCamera(cloudsCamera);
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
	refreshScanlineMesh = true;
	refreshPointcloud = true;
	refreshTriangulation = true;
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "VERT LINE SPACE"){
		refreshScanlineMesh = true;
	}
	else if(e.widget->getName() == "HORIZ LINE SPACE"){
		refreshScanlineMesh = true;
	}
	else if(e.widget->getName() == "P GRID VERT SPACE"){
		refreshPointcloud = true;
	}
	else if(e.widget->getName() == "P GRID HORI SPACE"){
		refreshPointcloud = true;
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

