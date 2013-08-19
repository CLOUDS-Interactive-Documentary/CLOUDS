//
//  IntroSequence.cpp
//  ComputerTicker
//
//  Created by James George on 5/26/13.
//
//

#include "CloudsIntroSequence.h"
#include "ofxObjLoader.h"
#include "CloudsGlobal.h"

CloudsIntroSequence::CloudsIntroSequence(){
	selectedQuestion = NULL;
	showingQuestions = false;
	useDebugCamera = false;
}

CloudsIntroSequence::~CloudsIntroSequence(){
	
}

string CloudsIntroSequence::getSystemName(){
	return "_Intro";
}

void CloudsIntroSequence::selfSetup(){

	font.loadFont(getDataPath() + "font/materiapro_light.ttf", 14);
	
	perlinOffset = 0;
	
	camera.setup();
	camera.autosavePosition = true;
	camera.loadCameraPosition();
	
	ofxObjLoader::load(getVisualSystemDataPath() + "OBJ/ParticleCube_supertight.obj", tunnelMeshTight);
	ofxObjLoader::load(getVisualSystemDataPath() + "OBJ/ParticleCube_loose.obj", tunnelMeshLoose);

//	ofxObjLoader::load(getDataPath() + "intro/OBJ/CLOUDS_type_thin_02.obj",thinTypeMesh);
	
	ofxObjLoader::load(getVisualSystemDataPath() + "OBJ/CLOUDS_type_thick.obj",thickTypeMesh);
//	ofxObjLoader::load(getDataPath() + "intro/OBJ/CLOUDS_type_thin_02.obj",thinTypeMesh);
	thinTypeMesh.load(getVisualSystemDataPath() + "OBJ/CLOUDS_type_thin_02.ply");
	
	thinTypeMesh.clearColors();
		
	currentFontExtrusion = -1;
	currentFontSize = -1;
	
	reloadShaders();
}

void CloudsIntroSequence::selfPresetLoaded(string presetPath){
	tunnelMax = tunnelMin = tunnelMeshLoose.getVertices()[0];
	for(int i = 1; i < tunnelMeshLoose.getVertices().size(); i++){
		tunnelMax = ofVec3f(MAX(tunnelMax.x, tunnelMeshLoose.getVertices()[i].x),
							MAX(tunnelMax.y, tunnelMeshLoose.getVertices()[i].y),
							MAX(tunnelMax.z, tunnelMeshLoose.getVertices()[i].z));
		tunnelMin = ofVec3f(MIN(tunnelMin.x, tunnelMeshLoose.getVertices()[i].x),
							MIN(tunnelMin.y, tunnelMeshLoose.getVertices()[i].y),
							MIN(tunnelMin.z, tunnelMeshLoose.getVertices()[i].z));
	}
	
	cout << "Tunnel min is " << tunnelMin << " tunnel max is " << tunnelMax << endl;
	
	warpCamera.setPosition(0, 0, tunnelMin.z);
	warpCamera.lookAt(ofVec3f(0,0,0));
	
	positionStartQuestions();
}

void CloudsIntroSequence::reloadShaders(){
	tunnelShader.load(getVisualSystemDataPath() + "shaders/IntroTunnel");
	chroma.load("",getVisualSystemDataPath() + "shaders/BarrelChromaAb.fs");
}

void CloudsIntroSequence::selfUpdate(){
	
	camera.applyRotation = camera.applyTranslation = useDebugCamera && !cursorIsOverGUI();
	
	warpCamera.dolly(-cameraForwardSpeed);
	
	ofVec2f mouseNode(ofGetMouseX(),ofGetMouseY());
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].update();
		if(startQuestions[i].position.z < warpCamera.getPosition().z){
			startQuestions[i].position.z += questionWrapDistance;
		}
		
		if(startQuestions[i].position.z - warpCamera.getPosition().z < distanceRange.max){
			float distanceToQuestion = startQuestions[i].currentScreenPoint.distance(mouseNode);
			if(caughtQuestion == NULL){
				if( distanceToQuestion < questionTugMaxDistance){
					startQuestions[i].position.z += ofMap(distanceToQuestion, questionTugMaxDistance, questionTugMinDistance, 0, cameraForwardSpeed);
					//startQuestions[i].position.z += (1. - (distanceToQuestion / questionTugMinDistance) ) * cameraForwardSpeed;
					if(distanceToQuestion < questionTugMinDistance){
						caughtQuestion = &startQuestions[i];
						caughtQuestion->startHovering();
					}
				}
				
			}
			//we have a caught question make sure it's still close
			else if(caughtQuestion == &startQuestions[i]){
				startQuestions[i].position.z += cameraForwardSpeed;
				if( caughtQuestion->isSelected()){
					selectedQuestion = caughtQuestion;
				}
				else if(distanceToQuestion > questionTugMinDistance){
					caughtQuestion->stopHovering();
					caughtQuestion = NULL;
				}
			}
		}
	}
	
	if(currentFontSize != fontSize ||
	   currentFontExtrusion != fontExtrusion)
	{
		currentFontSize = fontSize;
		currentFontExtrusion = fontExtrusion;
//		cloudsTypeMesh.init(getDataPath() + "/font/materiapro_light.ttf", currentFontSize, currentFontExtrusion, "CLOUDS");
	}

}

void CloudsIntroSequence::setStartQuestions(vector<CloudsClip>& possibleStartQuestions){

	selectedQuestion = NULL;
	startQuestions.clear();
	
	for(int i = 0; i < possibleStartQuestions.size(); i++){
		
		CloudsQuestion q;
		q.cam = &warpCamera;
		q.font = &font;
		q.clip = possibleStartQuestions[i];
		q.topic = q.clip.getAllTopicsWithQuestion()[0];
		q.question = q.clip.getQuestionForTopic(q.topic);
		
		q.setup();
		
		startQuestions.push_back(q);
	}
}

void CloudsIntroSequence::positionStartQuestions(){
	//set the start questions along a random tunnel
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].position = ofVec3f(0,ofRandom(0, tunnelMax.y), 0);
		startQuestions[i].position.rotate(ofRandom(360), ofVec3f(0,0,1));
		startQuestions[i].position.z = tunnelMax.z + ofRandom(questionWrapDistance);
	}
}

bool CloudsIntroSequence::isStartQuestionSelected(){
	return selectedQuestion != NULL;
}

CloudsQuestion* CloudsIntroSequence::getSelectedQuestion(){
	return selectedQuestion;
}

void CloudsIntroSequence::selfDrawBackground(){

}

void CloudsIntroSequence::drawCloudsType(){
	ofPushMatrix();
	
	ofRotate(180, 0, 1, 0);
	ofScale(fontScale, fontScale, fontScale);
	//	ofSetColor(255);
	//	for(int i = 0; i < cloudsTypeMesh.letters.size(); i++){
	//		cloudsTypeMesh.letters[i].front.drawFaces();
	//	}
	
	thinTypeMesh.draw();
	
	//DRAW MESH
	//thickTypeMesh.drawWireframe();
	//ofEnableAlphaBlending();
	//ofSetColor(255, 100);
	//thickTypeMesh.draw();
	
	ofPopMatrix();
}

void CloudsIntroSequence::selfDrawDebug(){


}

void CloudsIntroSequence::timelineBangEvent(ofxTLBangEventArgs& args){
	//testing for now
	CloudsVisualSystem::timelineBangEvent(args);
//	if(args.flag == "TriggerQ"){
//		//selectedQuestion = &startQuestions[0];
//		showingQuestions = true;
//	}
}

void CloudsIntroSequence::selfDraw(){
	glEnable(GL_DEPTH_TEST);
	
	ofClear(0);
	
	ofEnableSmoothing();
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
	glEnable(GL_POINT_SMOOTH);
	
	drawCloudsType();
	
	ofPushStyle();
	
	tunnelShader.begin();
	tunnelShader.setUniform1f("minPointSize", pointSize.min);
	tunnelShader.setUniform1f("maxPointSize", pointSize.max);
	tunnelShader.setUniform1f("minDistance", distanceRange.min);
	tunnelShader.setUniform1f("maxDistance", distanceRange.max);
	
	tunnelShader.setUniform1f("noiseAmplitude", perlinAmplitude);
	tunnelShader.setUniform1f("noiseDensity", perlinDensity);
	perlinOffset += perlinSpeed;
	tunnelShader.setUniform1f("noisePosition", perlinOffset);
	
	ofSetColor(255);
	tunnelMeshTight.drawVertices();
	
	ofSetColor(255*wireframeAlpha);
	tunnelMeshLoose.drawWireframe();

	tunnelShader.end();
		
	ofPopStyle();
	
	ofPushStyle();
//	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofMesh debugMesh;
	ofSetColor(255);
	
	//	cout << "debug drawing " << startQuestions.size() << " questions" << endl;
	
	for(int i = 0; i < startQuestions.size(); i++){
		debugMesh.addColor(caughtQuestion == &startQuestions[i] ? ofFloatColor::red : ofFloatColor::white);
		debugMesh.addVertex(startQuestions[i].position);
//		ofDrawBitmapString(startQuestions[i].question, startQuestions[i].position);
//		cout << "drawing point at " << startQuestions[i].position << endl;
		
		
	}
	
	glPointSize(4);
	debugMesh.drawVertices();
	glPointSize(1);
	ofPopStyle();
	
}

void CloudsIntroSequence::selfDrawOverlay(){
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].drawOverlay();
	}
}

void CloudsIntroSequence::selfPostDraw(){
	chroma.begin();
	chroma.setUniform2f("resolution", ofGetWidth(),ofGetHeight());
	chroma.setUniform1f("max_distort", maxChromaDistort);
	CloudsVisualSystem::selfPostDraw();
	chroma.end();
}

void CloudsIntroSequence::selfExit(){
	
}

void CloudsIntroSequence::selfBegin(){
	
}

void CloudsIntroSequence::selfEnd(){
	
}

void CloudsIntroSequence::selfKeyPressed(ofKeyEventArgs & args){
}

void CloudsIntroSequence::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsIntroSequence::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "arrange questions" && ((ofxUIButton*)e.widget)->getValue()){
		positionStartQuestions();
	}
}

void CloudsIntroSequence::selfSetupSystemGui(){
	sysGui->addButton("reset debug camera", false);
	sysGui->addButton("reload shader", false);
}

void CloudsIntroSequence::selfSetupCameraGui(){
	camGui->addToggle("use debug camera", &useDebugCamera);
	camGui->addSlider("debug camera speed", 1, 20, &camera.speed);
	camGui->addSlider("camera fwd force", 0, 10, &cameraForwardSpeed);
}

void CloudsIntroSequence::guiSystemEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "reload shader" && ((ofxUIButton*)e.widget)->getValue()){
		cout << "Loaded shader" << endl;
		reloadShaders();
	}
	else if(e.widget->getName() == "reset debug camera" && ((ofxUIButton*)e.widget)->getValue()){
		camera.setPosition(0, 0, 0);
		camera.setOrientation(ofQuaternion());
		camera.rotate(180, ofVec3f(0,1,0));
		camera.setAnglesFromOrientation();

	}
}

void CloudsIntroSequence::selfSetupRenderGui(){
	
	rdrGui->addSlider("Min Point Size", 0, 7, &pointSize.min);
	rdrGui->addSlider("Max Point Size", 0, 7, &pointSize.max);
	rdrGui->addSlider("Min Distance", 0, 5000, &distanceRange.min);
	rdrGui->addSlider("Max Distance", 0, 10000, &distanceRange.max);

	rdrGui->addSlider("Chroma Max Distort", 0, 10, &maxChromaDistort);
	
	rdrGui->addSlider("Perlin Amplitude", 0, 10, &perlinAmplitude);
	rdrGui->addSlider("Perlin Density", 0, 50, &perlinDensity);
	rdrGui->addSlider("Perlin Speed", 0, .3, &perlinSpeed);
	
//	rdrGui->addSlider("Font Size", 1, 10, &fontSize);
//	rdrGui->addSlider("Font Extrusion", 0, 10, &fontExtrusion);
	rdrGui->addSlider("Font Scale", .1, 10, &fontScale);
	
	rdrGui->addSlider("Wireframe Alpha", 0, 1.0, &wireframeAlpha);
}


void CloudsIntroSequence::selfSetupGuis(){
	
	questionGui = new ofxUISuperCanvas("QUESTIONS", gui);
	questionGui->copyCanvasStyle(gui);
	questionGui->copyCanvasProperties(gui);
	questionGui->setName("Questions");
	questionGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	//	questionGui->addSlider("Start Z", 1, 1000, &questionStartZ);
	questionGui->addSlider("Wrap Distance", 100, 1000, &questionWrapDistance);
	questionGui->addSlider("Tug Min Distance", 10, 300, &questionTugMinDistance) ;
	questionGui->addSlider("Tug Max Distance", 10, 300, &questionTugMaxDistance) ;

	questionGui->addButton("arrange questions", false);
	//	questionGui->addToggle("Custom Toggle", &customToggle);
		ofAddListener(questionGui->newGUIEvent, this, &CloudsIntroSequence::selfGuiEvent);
	
	guis.push_back(questionGui);
	guimap[questionGui->getName()] = questionGui;
	
}

void CloudsIntroSequence::guiRenderEvent(ofxUIEventArgs &e){
	
}

