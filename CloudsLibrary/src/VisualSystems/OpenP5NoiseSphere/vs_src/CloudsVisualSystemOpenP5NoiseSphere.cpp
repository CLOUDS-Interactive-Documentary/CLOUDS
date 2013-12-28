//
//  CloudsVisualSystemOpenP5NoiseSphere.cpp
//

#include "CloudsVisualSystemOpenP5NoiseSphere.h"


#include "CloudsRGBDVideoPlayer.h"

float * Hair::levelScaleLookUp = NULL;

float Hair::minNoiseScale = 0.5f;
float Hair::maxNoiseScale = 1.0f;

ofFloatColor Hair::baseColor = ofFloatColor::black;
ofFloatColor Hair::tipColor  = ofFloatColor::white;

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOpenP5NoiseSphere::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
    customGui->addIntSlider("HAIR COUNT", 1000, 20000, &count);
    customGui->addSlider("SPHERE SCALE", 0.0, 75, &sphereSize);
    customGui->addSlider("NOISE SPEED", 0.0, 10.0, &noiseSpeed);
    customGui->addRangeSlider("NOISE SCALE", 0.0, 4.0, &Hair::minNoiseScale, &Hair::maxNoiseScale);
    
    customGui->addSpacer();
    customGui->addSlider("SPHERE HUE", 0.0f, 1.0f, &sphereColor.r);
    customGui->addSlider("SPHERE SAT", 0.0f, 1.0f, &sphereColor.g);
    customGui->addSlider("SPHERE BRI", 0.0f, 1.0f, &sphereColor.b);
    customGui->addSlider("SPHERE ALPHA", 0.0f, 1.0f, &sphereColor.a);
    customGui->addSpacer();
    customGui->addRangeSlider("HAIR LENGTH", 0.0, 5.0, &minHairLength, &maxHairLength);
    customGui->addRangeSlider("HAIR LINE WIDTH", 0.1f, 10.0f, &minHairLineWidth, &maxHairLineWidth);
    
    customGui->addSpacer();
    customGui->addRangeSlider("BASE HUE", 0.0f, 1.0f, &minBaseColor.r, &maxBaseColor.r);
    customGui->addRangeSlider("BASE SAT", 0.0f, 1.0f, &minBaseColor.g, &maxBaseColor.g);
    customGui->addRangeSlider("BASE BRI", 0.0f, 1.0f, &minBaseColor.b, &maxBaseColor.b);
    customGui->addRangeSlider("BASE ALPHA", 0.0f, 1.0f, &minBaseColor.a, &maxBaseColor.a);
    customGui->addSpacer();
    customGui->addRangeSlider("TIP HUE", 0.0f, 1.0f, &minTipColor.r, &maxTipColor.r);
    customGui->addRangeSlider("TIP SAT", 0.0f, 1.0f, &minTipColor.g, &maxTipColor.g);
    customGui->addRangeSlider("TIP BRI", 0.0f, 1.0f, &minTipColor.b, &maxTipColor.b);
    customGui->addRangeSlider("TIP ALPHA", 0.0f, 1.0f, &minTipColor.a, &maxTipColor.a);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5NoiseSphere::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
    
	drawingGui = new ofxUISuperCanvas("POINTS_LINES", gui);
	drawingGui->copyCanvasStyle(gui);
	drawingGui->copyCanvasProperties(gui);
	drawingGui->setName("PointsLines");
	drawingGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	drawingGui->addToggle("DRAW POINTS", &drawPoints);
	drawingGui->addSlider("POINT ALPHA", 0, 1.0, &pointAlpha);
    drawingGui->addSlider("POINT SIZE", 1.0, 3.0, &pointSize);
	drawingGui->addSpacer();
	drawingGui->addToggle("DRAW LINES", &drawLines);
	drawingGui->addSlider("LINE ALPHA", 0, 1.0, &lineAlpha);

	ofAddListener(drawingGui->newGUIEvent, this, &CloudsVisualSystemOpenP5NoiseSphere::selfGuiEvent);
	guis.push_back(drawingGui);
	guimap[drawingGui->getName()] = drawingGui;

    selfSetupAudioGui();
}

void CloudsVisualSystemOpenP5NoiseSphere::selfGuiEvent(ofxUIEventArgs &e)
{

}

void CloudsVisualSystemOpenP5NoiseSphere::selfSetupAudioGui()
{
    audioGui = new ofxUISuperCanvas("AUDIO", gui);
	audioGui->copyCanvasStyle(gui);
	audioGui->copyCanvasProperties(gui);
	audioGui->setName("Audio");
	audioGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    audioGui->addSpacer();
    vector<string> soundNames;
    for (int i = 0; i < soundsDir.size(); i++) {
        soundNames.push_back(soundsDir.getName(i));
    }
    audioGui->addRadio("SOUNDS", soundNames);
    
    audioGui->addSpacer();
    audioGui->addSlider("LEVEL", 0.0f, 1.0f, 0.0f);
    audioGui->addSlider("LEVEL ADJUST", 0.1f, 10.0f, &levelAdjust);

    audioGui->addSpacer();
	audioGui->addSlider("SCROLL SPEED", 0.0f, 2.5f, &scrollSpeed);
    
    audioGui->addSpacer();
	audioGui->addSlider("LEVEL OFFSET", 0, 1, &levelOffset);
    audioGui->addSlider("LEVEL SCALE", 0, 10, &levelScale);
    audioGui->addSlider("LEVEL DECAY RATE", 0, 1, &levelDecayRate);
    audioGui->addToggle("INVERT LEVEL", &bInvertLevel);
    
    audioGui->addSpacer();
    audioGui->addToggle("LEVEL TO NOISE", &bLevelToNoise);
    audioGui->addSlider("LEVEL TO NOISE SCALE", 0, 100, &levelToNoiseScale);
    audioGui->addSlider("LEVEL TO NOISE RATIO", 0, 1, &levelToNoiseRatio);
    
	ofAddListener(audioGui->newGUIEvent, this, &CloudsVisualSystemOpenP5NoiseSphere::guiAudioEvent);
	guis.push_back(audioGui);
	guimap[audioGui->getName()] = audioGui;
}

void CloudsVisualSystemOpenP5NoiseSphere::guiAudioEvent(ofxUIEventArgs &e)
{
    // Let's look through the files dropdown for a match.
    string name = e.widget->getName();
    for (int i = 0; i < soundsDir.numFiles(); i++) {
        if (name == soundsDir.getName(i) && ((ofxUIToggle *)e.widget)->getValue()) {
            selectedSoundsIdx = i;
            reloadSound();
            break;
        }
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5NoiseSphere::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5NoiseSphere::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5NoiseSphere::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5NoiseSphere::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemOpenP5NoiseSphere::selfSetDefaults(){
	
	drawLines = true;
	drawPoints = false;
	pointSize = 2.0;
	lineAlpha = 1.0;
	pointAlpha = 1.0;
	
	count = 10000;
	
    sphereSize = 75.0f;
    minHairLength = maxHairLength = 1.0f;
    minHairLineWidth = maxHairLineWidth = 0.1f;
    
    currLevel = 0;
    levelAdjust = 1.0f;
    
    sphereColor = ofFloatColor::gray;
    minBaseColor = maxBaseColor = ofFloatColor::black;
    minTipColor  = maxTipColor  = ofFloatColor::white;
	
	noisePosition = 0;
	noiseSpeed = 0;
	noiseScale = 1;

	radius = 75;
	
    levelOffset = 1;
    levelScale = 2;
    levelDecayRate = 0.99f;
    bInvertLevel = false;
    
    bLevelToNoise = false;
    levelToNoiseScale = 50;
    levelToNoiseRatio = 0.5f;
	
    // set up hairball
	radius = 75;
	
    scrollY = -radius;
    scrollSpeed = 0.1f;

	soundPlayerReady = false;
	videoPlayerReady = false;

}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOpenP5NoiseSphere::selfSetup()
{

	leftBuffer  = NULL;
    rightBuffer = NULL;
    bAudioBuffered = false;
    
    soundsDir.listDir(getVisualSystemDataPath() + "sounds" );
    soundsDir.sort();
    selectedSoundsIdx = 0;
    reloadSound();
    
    Hair::levelScaleLookUp = new float[(int)radius * 2 + 1];
    for (int i = 0; i < radius * 2 + 1; i++) {
        Hair::levelScaleLookUp[i] = 0;
    }
    
	generateNoiseSphere();
	
    reloadShader();
}

void CloudsVisualSystemOpenP5NoiseSphere::generateNoiseSphere(){

	list.clear();
	for (int i=0; i<count; i++) {
		list.push_back( Hair(radius) );
	}
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5NoiseSphere::selfPresetLoaded(string presetPath){

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5NoiseSphere::selfBegin(){
	if(bModeVideo){
		videoPlayer.play();
		videoPlayer.setLoopState(OF_LOOP_NORMAL);
	}
	else {
		soundPlayer.play();
		soundPlayer.setLoop(true);
	}
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5NoiseSphere::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP5NoiseSphere::selfUpdate()
{
	
	if(list.size() != count){
		generateNoiseSphere();
	}
	
    if (bModeVideo) {
        videoPlayer.update();
        if (videoPlayer.isAudioLoaded()) {
            if (bAudioBuffered == false) {
                float * interleavedBuffer = videoPlayer.getAllAmplitudes();
                numAmplitudesPerChannel = videoPlayer.getNumAmplitudes() / 2;
                leftBuffer  = new float[numAmplitudesPerChannel];
                rightBuffer = new float[numAmplitudesPerChannel];
                
                for (int i = 0; i < numAmplitudesPerChannel; i++) {
                    leftBuffer[i]  = interleavedBuffer[i * 2 + 0];
                    rightBuffer[i] = interleavedBuffer[i * 2 + 1];
                }
                
                bAudioBuffered = true;
            }
            
            currLevel = ABS(videoPlayer.getAmplitude()) * levelAdjust;
        }
    }
    else {
        ofSoundUpdate();
        bAudioBuffered = true;

        currLevel = ofSoundGetSpectrum(1)[0] * levelAdjust;
    }
    
    if (bAudioBuffered) {
        ((ofxUISlider *)audioGui->getWidget("LEVEL"))->setValue(currLevel);
        
        if (bLevelToNoise) {
            float newScale = ofLerp(Hair::maxNoiseScale, currLevel * levelToNoiseScale, levelToNoiseRatio);
            Hair::maxNoiseScale = Hair::minNoiseScale = newScale;
        }
        
        for (int i = 0; i < radius * 2; i++) {
            float currY = i - radius;  // Range: [-radius, radius]
            float newLevelScale;
            if (bInvertLevel) {
                newLevelScale = ofMap(ABS(currY - scrollY), 0, radius, 1, levelOffset + currLevel * levelScale);
            }
            else {
                newLevelScale = ofMap(ABS(currY - scrollY), 0, radius, levelOffset + currLevel * levelScale, 1);
            }
            Hair::levelScaleLookUp[i] = MAX(Hair::levelScaleLookUp[i] * (0.9f + levelDecayRate * 0.1f), newLevelScale);
        }
        
        // adjust hair color
        ofFloatColor hsbConvertColor = minBaseColor.getLerped(maxBaseColor, currLevel);
        Hair::baseColor.setHsb(hsbConvertColor.r, hsbConvertColor.g, hsbConvertColor.b, hsbConvertColor.a);
        hsbConvertColor = minTipColor.getLerped(maxTipColor, currLevel);
        Hair::tipColor.setHsb(hsbConvertColor.r, hsbConvertColor.g, hsbConvertColor.b, hsbConvertColor.a);
        
        // scroll up and down
        scrollAng += scrollSpeed;
        scrollY = sin(scrollAng) * radius;
    }
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP5NoiseSphere::selfDraw()
{    
    ofPushMatrix();
	ofPushStyle();
//	glDisable(GL_LIGHTING);
//	glEnable(GL_DEPTH_TEST);
	ofEnableAlphaBlending();

    float rxp = ((GetCloudsInputX()-(ofGetWidth()/2))*0.3);
	float ryp = ((ofGetMouseY()-(ofGetHeight()/2))*0.3);
	rx = (rx*0.9)+(rxp*0.1);
	ry = (ry*0.9)+(ryp*0.1);
	ofRotateY(rx);
	ofRotateX(ry);
    
	ofFill();
    ofFloatColor hsbSphereColor;
    hsbSphereColor.setHsb(sphereColor.r, sphereColor.g, sphereColor.b, sphereColor.a);
    ofSetColor(hsbSphereColor);
	ofSphere(0, 0, sphereSize);
	
	ofMesh mesh;
	noisePosition += noiseSpeed;
	for (int i = 0; i < count; i++) {
		list[i].draw(mesh, noisePosition, minHairLength + ofSignedNoise(list[i].luckyNumber) * (maxHairLength - minHairLength), scrollY);
	}    
	
	if(drawLines){
		shader.begin();
		shader.setUniform1f("alphaDamp", lineAlpha);
		ofSetLineWidth(ofMap(currLevel, 0, 1, minHairLineWidth, maxHairLineWidth));
		mesh.setMode(OF_PRIMITIVE_LINES);
		mesh.draw();
		shader.end();

	}
	
	if(drawPoints){

		shader.begin();
		glPushAttrib(GL_POINT_BIT);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
		glEnable(GL_POINT_SMOOTH);
		
		shader.setUniform1f("alphaDamp", pointAlpha);
		shader.setUniform1f("pointSize", pointSize);
		mesh.setMode(OF_PRIMITIVE_POINTS);
		mesh.draw();
		shader.end();
		
		glPopAttrib();
	}

	ofPopStyle();
    ofPopMatrix();
}

// draw any debug stuff here
void CloudsVisualSystemOpenP5NoiseSphere::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOpenP5NoiseSphere::selfDrawBackground(){
    
	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5NoiseSphere::selfEnd(){

	if(bModeVideo){
		videoPlayer.stop();
	}
	else {
		soundPlayer.stop();
	}
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOpenP5NoiseSphere::selfExit()
{
    if (leftBuffer  != NULL) delete [] leftBuffer;
    if (rightBuffer != NULL) delete [] rightBuffer;
    leftBuffer = rightBuffer = NULL;
    
    delete [] Hair::levelScaleLookUp;
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5NoiseSphere::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		generateNoiseSphere();
		reloadShader();
	}
}
void CloudsVisualSystemOpenP5NoiseSphere::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5NoiseSphere::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5NoiseSphere::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5NoiseSphere::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5NoiseSphere::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5NoiseSphere::reloadSound()
{
    // close whatever sound was previously open
    soundPlayer.stop();
    soundPlayer.unloadSound();
    videoPlayer.stop();
    videoPlayer.close();
    
    ofFile file = soundsDir.getFile(selectedSoundsIdx);
    if (file.getExtension() == "mp4" || file.getExtension() == "mov") {
        bModeVideo = true;
        
        videoPlayer.loadMovie(file.getAbsolutePath());
		//JG don't play until begin()
//        videoPlayer.play();
//        videoPlayer.setLoopState(OF_LOOP_NORMAL);
		videoPlayerReady = true;
    }
    else {
        bModeVideo = false;
        soundPlayer.loadSound(file.getAbsolutePath());
		//JG dont play until begin()
//        soundPlayer.play();
//        soundPlayer.setLoop(true);
		
		soundPlayerReady = true;
    }
}

void CloudsVisualSystemOpenP5NoiseSphere::reloadShader(){
	shader.load(getVisualSystemDataPath() + "shaders/noisehair");
}
