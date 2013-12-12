//
//  CloudsVisualSystemGesturePaint.cpp
//

#include "CloudsVisualSystemGesturePaint.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemGesturePaint::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
//	customGui->addSlider("Custom Float 1", 1, 1000, &customFloat1);
//	customGui->addSlider("Custom Float 2", 1, 1000, &customFloat2);
//	customGui->addButton("Custom Button", false);
//	customGui->addToggle("Custom Toggle", &customToggle);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemGesturePaint::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemGesturePaint::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}


//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemGesturePaint::selfSetupSystemGui(){
	
}

void CloudsVisualSystemGesturePaint::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemGesturePaint::selfSetupRenderGui(){

}

void CloudsVisualSystemGesturePaint::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemGesturePaint::selfSetup(){
	reloadShader();
}

void CloudsVisualSystemGesturePaint::reloadShader(){
	hblurShader.load(getVisualSystemDataPath() + "shaders/hblur.vert",
					 getVisualSystemDataPath() + "shaders/blur.frag");
	vblurShader.load(getVisualSystemDataPath() + "shaders/vblur.vert",
					 getVisualSystemDataPath() + "shaders/blur.frag");
	
//	vblurShader.load(getVisualSystemDataPath() + "shaders/blur");
	brushImage.loadImage(getVisualSystemDataPath() + "images/brush.png");
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemGesturePaint::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemGesturePaint::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemGesturePaint::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemGesturePaint::selfUpdate(){
	if(!src.isAllocated() ||
	   src.getWidth() != getSharedRenderTarget().getWidth() ||
	   src.getHeight() != getSharedRenderTarget().getHeight())
	{
		src.allocate(getSharedRenderTarget().getWidth(),
					 getSharedRenderTarget().getHeight(),
					 GL_RGB32F);
		dst.allocate(getSharedRenderTarget().getWidth(),
					 getSharedRenderTarget().getHeight(),
					 GL_RGB32F);
		
		src.begin();
		ofClear(0,0,0,0);
		src.end();
		dst.begin();
		ofClear(0,0,0,0);
		dst.end();
		
		srcMesh.clear();
		srcMesh.addVertex(ofVec3f(0,0,0));
		srcMesh.addVertex(ofVec3f(dst.getWidth(),0,0));
		srcMesh.addVertex(ofVec3f(0,dst.getHeight(),0));
		srcMesh.addVertex(ofVec3f(dst.getWidth(),dst.getHeight(),0));
		
		srcMesh.addTexCoord(ofVec2f(0,0));
		srcMesh.addTexCoord(ofVec2f(dst.getWidth(),0));
		srcMesh.addTexCoord(ofVec2f(0,dst.getHeight()));
		srcMesh.addTexCoord(ofVec2f(dst.getWidth(),dst.getHeight()));
		srcMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	}
	
	glDisable(GL_DEPTH_TEST);
	ofSetColor(255, 255);
	
	hblurShader.begin();
	hblurShader.setUniformTexture("s_texture", src.getTextureReference(), 1);
	hblurShader.setUniform2f("dimensions", dst.getWidth(), dst.getHeight());
	dst.begin();
	ofEnableAlphaBlending();
	ofClear(0, 0, 0, 0);
	srcMesh.draw();
	dst.end();
	hblurShader.end();
	
	swap(src,dst);
	
	vblurShader.begin();
	vblurShader.setUniformTexture("s_texture", src.getTextureReference(), 1);
	vblurShader.setUniform2f("dimensions", dst.getWidth(), dst.getHeight());
	dst.begin();
	ofEnableAlphaBlending();
	ofClear(0, 0, 0, 0);
	srcMesh.draw();
	vblurShader.end();
	
	for(int i = 0; i < depositPoints.size(); i++){
		//TODO: set color
		brushImage.draw( depositPoints[i] );
	}
	dst.end();
	
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemGesturePaint::selfDraw(){
}

// draw any debug stuff here
void CloudsVisualSystemGesturePaint::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemGesturePaint::selfDrawBackground(){

	//ofEnableAlphaBlending();
	glDisable(GL_DEPTH_TEST);
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	dst.getTextureReference().draw(0,0);
	swap(src,dst);
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemGesturePaint::selfEnd(){
	
	simplePointcloud.clear();
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemGesturePaint::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemGesturePaint::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		reloadShader();
	}
}
void CloudsVisualSystemGesturePaint::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemGesturePaint::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemGesturePaint::selfMouseMoved(ofMouseEventArgs& data){
	depositPoints.push_back(ofVec2f(data.x,data.y));
}

void CloudsVisualSystemGesturePaint::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemGesturePaint::selfMouseReleased(ofMouseEventArgs& data){
	
}