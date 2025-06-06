//
//  CloudsVisualSystemOcean.cpp
//  VSCloudsRezanator
//
//  Created by James George on 5/31/13.
//
//

#include "ofxAudioDecoderTonic.h"

#include "CloudsVisualSystemOcean.h"

using namespace Tonic;

CloudsVisualSystemOcean::CloudsVisualSystemOcean(){
	windSpeed = 32;
    oceanTileSizeX = 200;
    oceanTileSizeY = 200;
	currentLookRot = 0;
	currentSteerRot = 0;
	maxLookUpRot = 90;
	maxLookDownRot = 90;
	
	needsRegenerate = false;

	drawOcean = true;
	depthTesting = false;
}

string CloudsVisualSystemOcean::getSystemName(){
	return "Ocean";
}
void CloudsVisualSystemOcean::selfSetup(){
	needsRegenerate = true;
    
    tonicSamples.push_back(TonicSample("Vocal_harmonic_high_shorter.mp3"));
    tonicSamples.push_back(TonicSample("vocal_harmony_bass.mp3"));

    // sound
    gain = 0;
    #ifdef TONIC_SOUNDS
    synth.setOutputGen(buildSynth());
    #endif
}
void CloudsVisualSystemOcean::selfSetDefaults(){
    primaryCursorMode = CURSOR_MODE_CAMERA;
    secondaryCursorMode = CURSOR_MODE_INACTIVE;
}
void CloudsVisualSystemOcean::selfPresetLoaded(string presetPath){
	generateOcean();
}

void CloudsVisualSystemOcean::generateOcean(){
	needsRegenerate = false;
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
    guimap[oceanCameraGui->getName()] = oceanCameraGui;
    
	oceanCamera.ocean = &ocean;
	
	blendMode = OF_BLENDMODE_ALPHA;
    
    // sound
    soundGui = new ofxUISuperCanvas("OCEAN Sound", gui);
	soundGui->copyCanvasStyle(gui);
	soundGui->copyCanvasProperties(gui);
	soundGui->setName("OCEAN Sound");
	soundGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    soundGui->addSlider("Gain", 0, 1, &gain);
    soundGui->addToggle(tonicSamples[0].soundFile, &tonicSamples[0].playSample);
    soundGui->addToggle(tonicSamples[1].soundFile, &tonicSamples[1].playSample);
    
	guis.push_back(soundGui);
	guimap[soundGui->getName()] = soundGui;
    ofAddListener(soundGui->newGUIEvent, this, &CloudsVisualSystemOcean::selfGuiEvent);
}

void CloudsVisualSystemOcean::selfUpdate(){
    
    volumeControl.value(gain);
	
	if(	needsRegenerate){
		generateOcean();
	}
	
	if(useOceanCam){
		getCameraRef().dolly(cameraSpeed);
		ocean.cameraPosition = getCameraRef().getPosition();
		float mouseXPercent = 1.*GetCloudsInputX() / getCanvasWidth();
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
        //MA: changed ofGetHeight() to getCanvasHeight()
		float mouseYPercent = 1. * GetCloudsInputY() / getCanvasHeight();
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
    
	//glPushAttrib(GL_POINT_BIT | GL_POLYGON_BIT | GL_FOG_BIT | GL_DEPTH_BITS);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glPointSize(pointSize);
	glEnable(GL_CULL_FACE);
#if defined(OCULUS_RIFT) && defined(TARGET_OSX)
	glCullFace(GL_BACK);
#else
    glCullFace(GL_FRONT);
#endif
	glDisable(GL_LINE_SMOOTH);
//	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_FOG);
	glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
	glFogi(GL_FOG_MODE, GL_EXP);

//	float FogCol[3]={0.8f,0.8f,0.8f}; // Define a nice light grey
//	glFogfv(GL_FOG_COLOR, FogCol);     // Set the fog color
	glFogf(GL_FOG_DENSITY, powf(fogDensity,2));

	mat->begin();

	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetLineWidth(1.0);
	if(depthTesting){
		GLfloat fogColor[4] = {
			(GLfloat)(bgColor.r/255.),
			(GLfloat)(bgColor.g/255.),
			(GLfloat)(bgColor.b/255.), (GLfloat)(1.0)
		};
		glFogfv (GL_FOG_COLOR, fogColor);
		ofEnableDepthTest();
	}
	else{
		GLfloat fogColor[4] = {0.0, 0.0, 0.0, 1.0};
		glFogfv (GL_FOG_COLOR, fogColor);
		ofDisableDepthTest();
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
	ofPopStyle();
	mat->end();
	
	glPopAttrib();
}

void CloudsVisualSystemOcean::selfExit(){
	
}

void CloudsVisualSystemOcean::selfBegin(){
    // sound
    ofAddListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemOcean::audioRequested);
    
    for (int i=0; i<tonicSamples.size(); i++)
    {
        if (tonicSamples[i].playSample) {
            tonicSamples[i].soundTrigger.trigger();
        }
    }
}

void CloudsVisualSystemOcean::selfEnd(){
    // remove sound listener
    volumeControl.value(0);
    ofRemoveListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemOcean::audioRequested);
	
}

void CloudsVisualSystemOcean::selfKeyPressed(ofKeyEventArgs & args){

	if(args.key == 'R'){
		generateOcean();
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
    
    for (int i=0; i<2; i++)
    {
        if (e.widget->getName() == tonicSamples[i].soundFile) {
            ofxUIToggle* toggle = static_cast<ofxUIToggle*>(e.widget);
            tonicSamples[i].playSample = toggle->getValue();
            if (toggle->getValue() == true) {
                tonicSamples[i].soundTrigger.trigger();
            }
        }
    }
}

void CloudsVisualSystemOcean::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOcean::guiSystemEvent(ofxUIEventArgs &e){
	
}

//void CloudsVisualSystemOcean::reloadShader(){
////	cout << "Reloading ocean shader" << endl;
////	oceanShader.load(getVisualSystemDataPath() + "shaders/ocean");
//}

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

#ifdef TONIC_SOUNDS
Tonic::Generator CloudsVisualSystemOcean::buildSynth()
{
    string strDir = GetCloudsDataPath(true)+"sound/textures/";
    ofDirectory sdir(strDir);
    
    SampleTable samples[2];
    
    for(int i=0; i<tonicSamples.size();i++){
        string strAbsPath = ofToDataPath(strDir + "/" + tonicSamples[i].soundFile, true);
        samples[i] = ofxAudioDecoderTonic(strAbsPath);
    }
    
    Generator sampleGen1 = BufferPlayer().setBuffer(samples[0]).loop(1).trigger(tonicSamples[0].soundTrigger);
    Generator sampleGen2 = BufferPlayer().setBuffer(samples[1]).loop(1).trigger(tonicSamples[1].soundTrigger);
    
    return (sampleGen1 * 1.0f + sampleGen2 * 1.0f) * volumeControl;
}
#endif

void CloudsVisualSystemOcean::audioRequested(ofAudioEventArgs& args)
{
    #ifdef TONIC_SOUNDS
    synth.fillBufferOfFloats(args.buffer, args.bufferSize, args.nChannels);
    #endif
}



