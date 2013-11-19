//
//  CloudsVisualSystemOcean.cpp
//  VSCloudsRezanator
//
//  Created by James George on 5/31/13.
//
//

#include "CloudsVisualSystemOcean.h"

CloudsVisualSystemOcean::CloudsVisualSystemOcean(){
	windSpeed = 32;
    oceanTileSizeX = 200;
    oceanTileSizeY = 200;
	currentLookRot = 0;
	currentSteerRot = 0;
	maxLookUpRot = 90;
	maxLookDownRot = 90;

	drawOcean = true;
	depthTesting = false;
}

string CloudsVisualSystemOcean::getSystemName(){
	return "Ocean";
}

void CloudsVisualSystemOcean::selfSetup(){
	generateOcean();
	
}

void CloudsVisualSystemOcean::selfPresetLoaded(string presetPath){
	generateOcean();
}

void CloudsVisualSystemOcean::generateOcean(){
	ocean.size = ofVec3f(int(oceanTileSizeX), 1.0, int(oceanTileSizeY));
    ocean.windSpeed = windSpeed;
    ocean.setup();
	
	renderer.setup(&ocean, 9, 9);
}

void CloudsVisualSystemOcean::selfSetupGuis(){
	
	oceanGui = new ofxUISuperCanvas("OCEAN STYLE", gui);
    oceanGui->copyCanvasStyle(gui);
    oceanGui->copyCanvasProperties(gui);
	
    oceanGui->setName("OceanSettings");
    oceanGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	oceanGui->addSlider("OCEAN SIZE X", 10, 1000, &oceanTileSizeX);
	oceanGui->addSlider("OCEAN SIZE Y", 10, 1000, &oceanTileSizeY);
	oceanGui->addSlider("WIND SPEED Y", 2, 1000, &windSpeed);
	
	oceanGui->addButton("REGENERATE", &shouldRegenerateOcean);
	
	oceanGui->addSlider("WAVE SPEED", 0, 10, &ocean.waveSpeed);
	oceanGui->addSlider("WAVE SCALE", 0, 100.0, &ocean.waveScale);
	oceanGui->addSlider("WAVE CHOPPINESS", 0, 20, &ocean.choppyScale);


	oceanGui->addLabel("RENDERING");
	oceanGui->addToggle("DRAW POINTS", &drawPoints);
	oceanGui->addSlider("POINT ALPHA", 0, 1.0, &pointAlpha);
	oceanGui->addSlider("POINT SIZE", .5, 4, &pointSize);
	
	oceanGui->addToggle("DRAW WIREFRAME", &drawWireframe);
	oceanGui->addSlider("WIREFRAME ALPHA", 0, 1.0, &wireframeAlpha);

	oceanGui->addToggle("DRAW OCEAN", &drawOcean);
	
	oceanGui->addSlider("OCEAN ALPHA", 0, 1.0, &oceanAlpha);
	oceanGui->addToggle("DEPTH TESTING", &depthTesting);
	oceanGui->addSlider("FOG DENSITY", 0, .3, &fogDensity);
	
	ofAddListener(oceanGui->newGUIEvent, this, &CloudsVisualSystemOcean::selfGuiEvent);
	
    guis.push_back(oceanGui);
    guimap[oceanGui->getName()] = oceanGui;
	
	oceanCameraGui = new ofxUISuperCanvas("OCEAN CAMERA", gui);
	oceanCameraGui->copyCanvasStyle(gui);
    oceanCameraGui->copyCanvasProperties(gui);

	oceanCameraGui->addToggle("USE BUOYANT CAM", &useOceanCam);
	oceanCameraGui->addSlider("BASE WIDTH", 2, 50, &oceanCamera.baseWidth);
	oceanCameraGui->addSlider("LIFT HEIGHT", 0, 100, &oceanCamera.lift);
	oceanCameraGui->addSlider("SPEED", 0, -1, &cameraSpeed);
	oceanCameraGui->addSlider("DAMPEN", 0, 1.0, &oceanCamera.dampening);
	oceanCameraGui->addSlider("MAX LOOK UP ROT", 0, 90, &maxLookUpRot);
	oceanCameraGui->addSlider("MAX LOOK DOWN ROT", 0, 90, &maxLookDownRot);
	
    guis.push_back(oceanCameraGui);
    guimap[oceanGui->getName()] = oceanCameraGui;

	oceanCamera.ocean = &ocean;
	
	blendMode = OF_BLENDMODE_ALPHA;
	
	reloadShader();
}

void CloudsVisualSystemOcean::selfUpdate(){

	if(useOceanCam){
		getCameraRef().dolly(cameraSpeed);
		ocean.cameraPosition = getCameraRef().getPosition();
		float mouseXPercent = 1.*ofGetMouseX() / ofGetWidth();
		if(mouseXPercent > .6){
			float rotateAmount = .6 - mouseXPercent;
			currentSteerRot += (rotateAmount - currentSteerRot)*.01;
		}
		else if(mouseXPercent < .4){
			float rotateAmount = .4 - mouseXPercent;
			currentSteerRot += (rotateAmount - currentSteerRot)*.01;
		}
		else{
			currentSteerRot *= .99;
		}
		oceanCamera.heading += currentSteerRot;
	}
	else{
		ocean.cameraPosition = ofVec3f(0,0,0);
	}
	ocean.setFrameNum( ofGetFrameNum() );
	
	
	//update the sim, view and cam
    ocean.update();
	renderer.update();
	oceanCamera.update();
		
	//now we can move the camera up and down based on mouse position
	if(useOceanCam){
		float mouseYPercent = 1.*ofGetMouseY() / ofGetHeight();
		if(mouseYPercent > .6){
			currentLookRot += (ofMap(mouseYPercent, .6, 1.0, 0, -maxLookUpRot, true) - currentLookRot) * .005;
		}
		else if( mouseYPercent < .4){
			currentLookRot += (ofMap(mouseYPercent, .4, 0.0, 0, maxLookDownRot, true) - currentLookRot) * .005;
		}
		else{
			currentLookRot *= .995;
		}
		
		getCameraRef().rotate(currentLookRot, getCameraRef().getSideDir());
	}
}

void CloudsVisualSystemOcean::selfDrawBackground(){
	
}

void CloudsVisualSystemOcean::selfDrawDebug(){
	if(!useOceanCam){
		oceanCamera.drawDebug();
	}	
}

void CloudsVisualSystemOcean::selfSceneTransformation(){
	
}

void CloudsVisualSystemOcean::selfDraw(){
	

	glPushAttrib(GL_POINT_BIT | GL_POLYGON_BIT | GL_FOG_BIT | GL_DEPTH_BITS);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glPointSize(pointSize);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
//	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_FOG);
	glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
	glFogi(GL_FOG_MODE, GL_EXP);

//	float FogCol[3]={0.8f,0.8f,0.8f}; // Define a nice light grey
//	glFogfv(GL_FOG_COLOR, FogCol);     // Set the fog color
	glFogf(GL_FOG_DENSITY, powf(fogDensity,2));

	mat->begin();

	if(depthTesting){
		GLfloat fogColor[4] = {bgColor.r/255.,bgColor.g/255.,bgColor.b/255., 1.0 };
		glFogfv (GL_FOG_COLOR, fogColor);
		glEnable(GL_DEPTH_TEST);
		ofEnableAlphaBlending();
	}
	else{
		GLfloat fogColor[4] = {0.0, 0.0, 0.0, 1.0};
		glFogfv (GL_FOG_COLOR, fogColor);
		glDisable(GL_DEPTH_TEST);
		ofEnableBlendMode(OF_BLENDMODE_ADD);
	}
	

	ofSetColor(255, pointAlpha*255);
	if(drawPoints) renderer.drawVertices();
	
	ofSetColor(255, wireframeAlpha*255);
	if(drawWireframe) renderer.drawWireframe();

	glPolygonOffset(-1, 0);
	ofSetColor(255, oceanAlpha*255);
	if(drawOcean){
		renderer.draw();
	}
	mat->end();
	
	//oceanShader.end();
	
	glPopAttrib();
	
	ofEnableAlphaBlending();
	

}

void CloudsVisualSystemOcean::selfExit(){
	
}

void CloudsVisualSystemOcean::selfBegin(){
	
}

void CloudsVisualSystemOcean::selfEnd(){
	
}

void CloudsVisualSystemOcean::selfKeyPressed(ofKeyEventArgs & args){

	if(args.key == 'R'){
		reloadShader();
	}
}

void CloudsVisualSystemOcean::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOcean::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOcean::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOcean::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOcean::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOcean::selfSetupGui(){
	
}

void CloudsVisualSystemOcean::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "REGENERATE" && ((ofxUIButton*)e.widget)->getValue() ){
		generateOcean();
	}
}

void CloudsVisualSystemOcean::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOcean::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemOcean::reloadShader(){
	cout << "Reloading ocean shader" << endl;
	oceanShader.load(getVisualSystemDataPath() + "shaders/ocean");
}

void CloudsVisualSystemOcean::selfSetupRenderGui(){
	vector<string> modes;
	modes.push_back("alpha");
	modes.push_back("add");
	modes.push_back("screen");
	
	//modes.push_back("sub");
	//modes.push_back("mult");
//	modes.push_back("disable");
	
	//rdrGui->addRadio("blending", modes);


}

void CloudsVisualSystemOcean::guiRenderEvent(ofxUIEventArgs &e){
	
}
