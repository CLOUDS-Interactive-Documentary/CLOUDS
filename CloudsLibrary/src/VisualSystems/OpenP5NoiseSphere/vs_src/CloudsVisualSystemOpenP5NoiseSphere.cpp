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
	
    customGui->addLabel("Solid Sphere");
    customGui->addSlider("Solid_Sphere_Scale", 0.0, .25, &solidSphereScale);
    customGui->addSlider("Solid_Sphere_Alpha", 0.0, 1.0, &solidSphereAlpha);
    
    customGui->addSlider("Noise Speed", 0.0, 10.0, &noiseSpeed);
    customGui->addRangeSlider("Noise Scale", 0.0, 4.0, &Hair::minNoiseScale, &Hair::maxNoiseScale);
    customGui->addSlider("Fur Length", 0.0, 4., &furLength);

    customGui->addSpacer();
    customGui->addRangeSlider("BASE HUE", 0.0f, 1.0f, &minBaseColor.r, &maxBaseColor.r);
    customGui->addRangeSlider("BASE SAT", 0.0f, 1.0f, &minBaseColor.g, &maxBaseColor.g);
    customGui->addRangeSlider("BASE BRI", 0.0f, 1.0f, &minBaseColor.b, &maxBaseColor.b);
    customGui->addSpacer();
    customGui->addRangeSlider("TIP HUE", 0.0f, 1.0f, &minTipColor.r, &maxTipColor.r);
    customGui->addRangeSlider("TIP SAT", 0.0f, 1.0f, &minTipColor.g, &maxTipColor.g);
    customGui->addRangeSlider("TIP BRI", 0.0f, 1.0f, &minTipColor.b, &maxTipColor.b);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5NoiseSphere::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
    
    selfSetupAudioGui();
}

void CloudsVisualSystemOpenP5NoiseSphere::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
}

void CloudsVisualSystemOpenP5NoiseSphere::selfSetupAudioGui()
{
    audioGui = new ofxUISuperCanvas("AUDIO", gui);
	audioGui->copyCanvasStyle(gui);
	audioGui->copyCanvasProperties(gui);
	audioGui->setName("Audio");
	audioGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    audioGui->addIntSlider("PEAK HOLD TIME", 0, 30, &fftAnalyzer[0].peakHoldTime);
    audioGui->addSlider("PEAK DECAY RATE", 0, 1, &fftAnalyzer[0].peakDecayRate);
    for (int i = 0; i < fftAnalyzer[0].nAverages; i++) {
        audioGui->addSlider("S" + ofToString(i), 0.0f, 30.0f, fftAnalyzer[0].peaks[i], 17.0f, 160.0f);
        audioGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    }
    audioGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    for (int i = 0; i < fftAnalyzer[0].nAverages; i++) {
        audioGui->addToggle("T" + ofToString(i), &peakToggles[i], 17.0f, 17.0f)->setLabelVisible(false);
        audioGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    }
    audioGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    audioGui->addSlider("COMBINED PEAK", 0.0f, 30.0f, &combinedPeak);
    audioGui->addSlider("FUR PEAK SCALAR", 0.0f, 1.0f, &furPeakScalar);
    audioGui->addSlider("LEVEL", 0.0f, 1.0f, 0.0f);

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

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOpenP5NoiseSphere::selfSetup()
{
    leftBuffer  = NULL;
    rightBuffer = NULL;
    peakToggles = NULL;
    bAudioBuffered = false;
    
    minBaseColor = maxBaseColor = ofFloatColor::black;
    minTipColor  = maxTipColor  = ofFloatColor::white;
    
//    string filePath = "TestVideo/Casey_Software_is_what_i_love_the_most";
//    string filePath = "TestVideo/Fernanda_social_network_hairballs";
//    string filePath = "TestVideo/Jer_TestVideo";
//    if (ofFile::doesFileExist(getVisualSystemDataPath() + filePath + ".mov")){
//		getRGBDVideoPlayer().setup(getVisualSystemDataPath() + filePath + ".mov",
//								   getVisualSystemDataPath() + filePath + ".xml" );
//		
//		getRGBDVideoPlayer().swapAndPlay();
//	}

    videoPlayer.loadMovie(getVisualSystemDataPath() + "TestVideo/RedNoise.mov");
    videoPlayer.play();
    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    
    // set up fft analyzer
    for (int i = 0; i < 2; i++) {
        fftAnalyzer[i].setup(44100, BUFFER_SIZE/2, 1);
        fftAnalyzer[i].peakHoldTime = 15;         // hold longer
        fftAnalyzer[i].peakDecayRate = 0.95f;     // decay slower
        fftAnalyzer[i].linearEQIntercept = 0.9f;  // reduced gain at lowest frequency
        fftAnalyzer[i].linearEQSlope = 0.01f;     // increasing gain at higher frequencies
    }
    
    peakToggles = new bool[fftAnalyzer[0].nAverages];
    for (int i = 0; i < fftAnalyzer[0].nAverages; i++) {
        peakToggles[i] = false;
    }

    // set up hairball
	radius = 75;
    
    Hair::levelScaleLookUp = new float[(int)radius * 2 + 1];
    for (int i = 0; i < radius * 2 + 1; i++) {
        Hair::levelScaleLookUp[i] = 0;
    }
    
	for (int i=0; i<count; i++) {
		list.push_back( Hair(radius) );
	}
    
	noisePosition = 0;
	noiseSpeed = 0;
	noiseScale = 1;
    
    scrollY = -radius;
    scrollSpeed = 0.1f;
    
    levelOffset = 1;
    levelScale = 2;
    levelDecayRate = 0.99f;
    bInvertLevel = false;
    
    bLevelToNoise = false;
    levelToNoiseScale = 50;
    levelToNoiseRatio = 0.5f;
	
    wireSphereScale = 0.9333;
    solidSphereScale = 0.8666;
    wireSphereAlpha = 0.0784;
    solidSphereAlpha = 1.0;
    
    ofEnableSmoothing();
    ofSetLineWidth(.1);

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
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5NoiseSphere::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP5NoiseSphere::selfUpdate()
{
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
        
        // calculate fft
        float avgPower = 0.0f;
        
        int idx = (int)(videoPlayer.getPosition() * (numAmplitudesPerChannel - 1));
        fft[0].powerSpectrum(idx, BUFFER_SIZE/2, leftBuffer,  BUFFER_SIZE, &magnitude[0][0], &phase[0][0], &power[0][0], &avgPower);
        fft[1].powerSpectrum(idx, BUFFER_SIZE/2, rightBuffer, BUFFER_SIZE, &magnitude[1][0], &phase[1][0], &power[1][0], &avgPower);
        for (int i = 0; i < BUFFER_SIZE/2; i++) {
            freq[0][i] = magnitude[0][i];
            freq[1][i] = magnitude[1][i];
        }
        
        fftAnalyzer[0].calculate(freq[0]);
        fftAnalyzer[1].calculate(freq[1]);
        
        // update gui sliders
        int combinedCount = 0;
        float newCombinedPeak = 0.0f;
        for (int i = 0; i < fftAnalyzer[0].nAverages; i++) {
            float monoPeak = ((fftAnalyzer[0].peaks[i] + fftAnalyzer[1].peaks[i]) / 2.0f);
            ((ofxUISlider *)audioGui->getWidget("S" + ofToString(i)))->setValue(monoPeak);
//            cout << i << " " << fftAnalyzer[0].peaks[i] << " " << fftAnalyzer[1].peaks[i] << " " << scaledAvgPeak << endl;
            
            if (peakToggles[i]) {
                newCombinedPeak += monoPeak;
                ++combinedCount;
            }
        }

        newCombinedPeak /= combinedCount;
        float peakLerpRatio = 0.5f;
        combinedPeak = combinedPeak * (1.0f - peakLerpRatio) + newCombinedPeak * peakLerpRatio;
        
        //    float combinedFurLength = furLength * (1.0f - furPeakScalar) + combinedPeak * furPeakScalar;
        
        // calculate hairball level scales based on amplitude and scrolling y-value
        float currLevel = ABS(videoPlayer.getAmplitude());
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
        Hair::baseColor.setHsb(hsbConvertColor.r, hsbConvertColor.g, hsbConvertColor.b);
        hsbConvertColor = minTipColor.getLerped(maxTipColor, currLevel);
        Hair::tipColor.setHsb(hsbConvertColor.r, hsbConvertColor.g, hsbConvertColor.b);
        
        // scroll up and down
        scrollAng += scrollSpeed;
        scrollY = sin(scrollAng) * radius;
    }
    
//    cout << videoPlayer.getAmplitude() << endl;
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP5NoiseSphere::selfDraw()
{    
	ofPushStyle();
	
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	ofDisableAlphaBlending();
	ofFill();
    ofSetColor(bgBri);
	ofSphere(0, 0, solidSphereScale*300 );

	
	float rxp = ((ofGetMouseX()-(ofGetWidth()/2))*0.3);
	float ryp = ((ofGetMouseY()-(ofGetHeight()/2))*0.3);
	rx = (rx*0.9)+(rxp*0.1);
	ry = (ry*0.9)+(ryp*0.1);
	ofRotateY(rx);
	ofRotateX(ry);
	//sphere(radio);
	
	ofMesh mesh;
	noisePosition += noiseSpeed;
	for (int i = 0;i < count; i++) {
		list[i].draw(mesh, noisePosition, furLength, scrollY);
	}
	mesh.setMode(OF_PRIMITIVE_LINES);
	mesh.draw();
    
	ofPopStyle();
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

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOpenP5NoiseSphere::selfExit()
{
    if (leftBuffer  != NULL) delete [] leftBuffer;
    if (rightBuffer != NULL) delete [] rightBuffer;
    if (peakToggles != NULL) delete [] peakToggles;
    
    delete [] Hair::levelScaleLookUp;
    
    leftBuffer = rightBuffer = NULL;
    peakToggles = NULL;
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5NoiseSphere::selfKeyPressed(ofKeyEventArgs & args){
	
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