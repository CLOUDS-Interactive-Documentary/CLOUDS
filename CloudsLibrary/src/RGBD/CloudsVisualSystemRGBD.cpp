
#include "CloudsVisualSystemRGBD.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
string CloudsVisualSystemRGBD::getSystemName(){
	return "RGBD";
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::selfSetup(){
	currentFlowPosition = 0;
	
	drawMesh = false;
	
	rgbdShader.load( getDataPath() + "shaders/rgbdcombined" );

	generatePointGrid();
	generateScanlines();
	connectionGenerator.numParticles = 200;
	connectionGenerator.setMinDistance(40);
	connectionGenerator.setup();
	
	particulateController.setParticleCount(1e5);
	particulateController.setShaderDirectory(getDataPath() + "shaders/GPUParticles/");
	particulateController.setup();
	
	cloudsCamera.setup();
	cloudsCamera.lookTarget = ofVec3f(0,25,0);
	setCurrentCamera(cloudsCamera);
	
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
	
	
//	enum RGBDTransitionType
//  {
//	TWO_DIMENSIONAL = 0,
//	FLY_THROUGH = 1,
//	WHIP_PAN = 2,
//	RGBD = 3
//  };

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
	
	guis.push_back(questionGui);
	guimap[meshGui->getName()] = questionGui;
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
	
	if(currentCamera == &transitionCam){
		transitionCamTargetNode->setPosition( transitionCam.getPosition() );
		transitionCamTargetNode->setOrientation( transitionCam.getOrientationQuat() );
	}
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::addQuestion(CloudsClip& questionClip){
	CloudsQuestion* q = new CloudsQuestion();
	q->cam = &cloudsCamera;
	q->font = &displayFont;
	q->clip = questionClip;
	q->setup();
	
	questions.push_back(q);
}

//--------------------------------------------------------------
void CloudsVisualSystemRGBD::updateQuestions(){
	for(int i = 0; i < questions.size(); i++){
		questions[i]->position = translatedHeadPosition + ofVec3f(-cloudsCamera.sideDistance, 0, 0);
		questions[i]->update();
	}
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
	
//	ofQuaternion transitionEndRotation;
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
			
			float ystepOffset = ofRandom(-scanlineSimplify.y/4,scanlineSimplify.y/4);
			
			horizontalScanLines.addColor(ofFloatColor(ofRandom(1.)));
			horizontalScanLines.addVertex( ofVec3f(xstep, ystep+ystepOffset, 0) );
			
			horizontalScanLines.addColor(ofFloatColor(ofRandom(1.)));
			horizontalScanLines.addVertex( ofVec3f(xstep+scanlineSimplify.x, ystep+ystepOffset, 0) );
		}
	}

	verticalScanLines.setMode( OF_PRIMITIVE_LINES );
	horizontalScanLines.setMode( OF_PRIMITIVE_LINES );
	
	refreshScanlineMesh = false;
}

void CloudsVisualSystemRGBD::selfDrawBackground(){
	
}

void CloudsVisualSystemRGBD::selfDrawDebug(){
	ofSphere(translatedHeadPosition, 10);
}

void CloudsVisualSystemRGBD::selfSceneTransformation(){

}


void CloudsVisualSystemRGBD::selfDraw(){
	
	ofPushMatrix();
	
	
	//trnsition transformation
	ofMultMatrix( transitionMatrix );// <--- LB: I think this can go...
	
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

		ofEnableAlphaBlending();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		
		setupRGBDTransforms();
		
		rgbdShader.begin();
		getRGBDVideoPlayer().setupProjectionUniforms(rgbdShader);
		
		rgbdShader.setUniform1f("baseMultiplier", getRGBDVideoPlayer().getFadeIn() * getRGBDVideoPlayer().getFadeOut() );
		//set up the renderer so that any geometry within 640x480 space
		//can be prjected onto the pointcloud
		if(drawMesh){

			rgbdShader.setUniform1f("flowPosition", 0);
			rgbdShader.setUniform1f("eyeMultiplier", eyeMultiplier);
			rgbdShader.setUniform1f("skinMultiplier", skinMultiplier);
			rgbdShader.setUniform1f("baseMultiplier", meshAlpha);
			
//			sharedRenderer->setSimplification(ofVec2f(pointHorizontalSpace, pointVerticalSpace));
			rgbdShader.setUniform2f("simplify", pointHorizontalSpace, pointVerticalSpace);
			
			ofPushStyle();
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			pointGrid.draw();
			ofTranslate(0,0,-3);
			
			ofPopStyle();
			
			rgbdShader.setUniform1f("eyeMultiplier", 0);
			rgbdShader.setUniform1f("skinMultiplier", 0);
			rgbdShader.setUniform1f("baseMultiplier", 1.0);
			
		}
		
		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		rgbdShader.setUniform1f("flowPosition", currentFlowPosition);

		if(drawPoints){
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
			
			ofSetColor(255*horizontalScanlineAlpha);
			ofSetLineWidth(horizontalScanlineThickness);
			horizontalScanLines.draw();

			rgbdShader.setUniform1f("flowPosition", 0);
			ofSetLineWidth(verticalScanlineThickness);
			ofSetColor(255*verticalScanlineAlpha);
			verticalScanLines.draw();
		
		}
		

		rgbdShader.setUniform1f("flowPosition", 0);
//		connectionGenerator.draw();

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
	glPointSize(3);
	
	ofSetColor(ofColor(0,100,250), 100);
	for(int i = 0; i < questions.size(); i++){
		questions[i]->draw();
	}
	glPointSize(1);
}

void CloudsVisualSystemRGBD::selfDrawOverlay() {
	for(int i = 0; i < questions.size(); i++){
		questions[i]->drawOverlay();
	}
}

void CloudsVisualSystemRGBD::selfExit(){
	
}

void CloudsVisualSystemRGBD::selfBegin(){

}

void CloudsVisualSystemRGBD::selfEnd(){
	
}

void CloudsVisualSystemRGBD::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		particulateController.reloadShaders();
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

