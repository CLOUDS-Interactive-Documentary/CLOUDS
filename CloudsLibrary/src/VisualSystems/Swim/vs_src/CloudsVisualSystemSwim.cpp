//
//  CloudsVisualSystemSwim.cpp
//

#include "CloudsVisualSystemSwim.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemSwim::selfSetup()
{
    bubbles.init(getVisualSystemDataPath());
    creatures.init(getVisualSystemDataPath());
    
 //   post.init(ofGetWidth(), ofGetHeight(), true);
 //   post.createPass<FxaaPass>();
 //   post.createPass<BloomPass>();
    
    /*
	videoLoaded = false;
	
	if(ofFile::doesFileExist(getVisualSystemDataPath() + "TestVideo/Jer_TestVideo.mov")){
		getRGBDVideoPlayer().setup(getVisualSystemDataPath() + "TestVideo/Jer_TestVideo.mov",
								   getVisualSystemDataPath() + "TestVideo/Jer_TestVideo.xml" );
		
		getRGBDVideoPlayer().swapAndPlay();
		
		for(int i = 0; i < 640; i += 2){
			for(int j = 0; j < 480; j+=2){
				simplePointcloud.addVertex(ofVec3f(i,j,0));
			}
		}
		
		pointcloudShader.load(getVisualSystemDataPath() + "shaders/rgbdcombined");
		videoLoaded = true;
	}
	
	
    //	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	*/
}

//normal update call
void CloudsVisualSystemSwim::selfUpdate()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
    bubbles.update();
    creatures.update();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemSwim::selfDraw()
{
    creatures.draw();
    bubbles.draw();
    /*
	if(videoLoaded){
		ofPushMatrix();
		setupRGBDTransforms();
		pointcloudShader.begin();
		getRGBDVideoPlayer().setupProjectionUniforms(pointcloudShader);
		simplePointcloud.drawVertices();
		pointcloudShader.end();
		ofPopMatrix();
	}
	*/
}

void CloudsVisualSystemSwim::selfPostDraw()
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
 //   post.process(CloudsVisualSystem::getSharedRenderTarget(), false);
 //   post.getProcessedTextureReference().draw(0, ofGetHeight(), ofGetWidth(), -ofGetHeight());
    glPopAttrib();
}

//use render gui for display settings, like changing colors
void CloudsVisualSystemSwim::selfSetupRenderGui()
{
    rdrGui->addLabel("Flocking");
    rdrGui->addSpacer();
    rdrGui->addSlider("zoneRadius", 50.f, 2000.f, &creatures.zoneRadius);
    rdrGui->addSlider("alignmentLower", 0.f, 1.f, &creatures.alignmentLower);
    rdrGui->addSlider("alignmentUpper", 0.f, 1.f, &creatures.alignmentUpper);
    rdrGui->addSlider("repelStrength", 0.f, 1.f, &creatures.repelStrength);
    rdrGui->addSlider("alignStrength", 0.f, 1.f, &creatures.alignStrength);
    rdrGui->addSlider("attractStrength", 0.f, 1.f, &creatures.attractStrength);
    rdrGui->addSlider("maxDistFromCentre", 500.f, 4000.f, &creatures.maxDistFromCentre);
    
    rdrGui->addLabel("Creatures");
    rdrGui->addSpacer();
    rdrGui->addSlider("numJellyOne", 20, 300, &creatures.numJellyOne);
    rdrGui->addSlider("numJellyTwo", 20, 300, &creatures.numJellyTwo);
    rdrGui->addSlider("numGreyFish", 20, 300, &creatures.numGreyFish);
    rdrGui->addSlider("numYellowFish", 20, 300, &creatures.numYellowFish);
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemSwim::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
	customGui->addButton("Custom Button", false);
	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemSwim::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemSwim::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemSwim::selfSetupSystemGui(){
	
}

void CloudsVisualSystemSwim::guiSystemEvent(ofxUIEventArgs &e){
	
}


void CloudsVisualSystemSwim::guiRenderEvent(ofxUIEventArgs &e){
	
}



// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemSwim::selfPresetLoaded(string presetPath) {
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemSwim::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemSwim::selfSceneTransformation(){
	
}





// draw any debug stuff here
void CloudsVisualSystemSwim::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemSwim::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemSwim::selfEnd(){
	
	simplePointcloud.clear();
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemSwim::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemSwim::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemSwim::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemSwim::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMouseReleased(ofMouseEventArgs& data){
	
}