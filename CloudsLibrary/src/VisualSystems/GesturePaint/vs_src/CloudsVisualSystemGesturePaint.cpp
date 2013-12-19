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
	
	cout << "loading hblur" << endl;
	hblurShader.load(getVisualSystemDataPath() + "shaders/hblur.vert",
					 getVisualSystemDataPath() + "shaders/blur.frag");
	cout << "loading vblur" << endl;
	vblurShader.load(getVisualSystemDataPath() + "shaders/vblur.vert",
					 getVisualSystemDataPath() + "shaders/blur.frag");

//	vblurShader.load(getVisualSystemDataPath() + "shaders/blur");
	brushImage.loadImage(getVisualSystemDataPath() + "images/brush.png");
	paperImage.loadImage(getVisualSystemDataPath() + "images/paper.jpg");
	noiseFlowTex.loadImage(getVisualSystemDataPath() + "images/noise.png");
	
	cout << "loading paper mix" << endl;
	paperMixShader.load(getVisualSystemDataPath() + "shaders/papermix");
	
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

void CloudsVisualSystemGesturePaint::reallocateFramebuffers(){
	canvassrc.allocate(getSharedRenderTarget().getWidth(),
					   getSharedRenderTarget().getHeight(),
					   GL_RGBA);
	canvasdst.allocate(canvassrc.getWidth(),
					   canvassrc.getHeight(),
					   GL_RGBA);
	
	waterdst.allocate(getSharedRenderTarget().getWidth()*.25,
					  getSharedRenderTarget().getHeight()*.25,
					  GL_RGB32F);
	watersrc.allocate(getSharedRenderTarget().getWidth()*.25,
					  getSharedRenderTarget().getHeight()*.25,
					  GL_RGB32F);
	
	
	waterdst.begin();
	ofClear(0,0,0,0);
	waterdst.end();
	
	watersrc.begin();
	ofClear(0,0,0,0);
	watersrc.end();
	
	canvasdst.begin();
	ofClear(0,0,0,0);
	canvasdst.end();
	
	canvassrc.begin();
	ofClear(0,0,0,0);
	canvassrc.end();
	
	meshFromFbo(canvasMesh, canvassrc);
	meshFromFbo(waterMesh, watersrc);
	
	paperRect = ofRectangle(0,0,paperImage.getWidth(),paperImage.getHeight());
	ofRectangle screenRect(0,0,canvassrc.getWidth(),canvassrc.getHeight());
	paperRect.scaleTo(screenRect, OF_ASPECT_RATIO_KEEP_BY_EXPANDING);
}

void CloudsVisualSystemGesturePaint::meshFromFbo(ofMesh& m, ofFbo& f){
	
	m.clear();
	
	m.addVertex(ofVec3f(0,0,0));
	m.addVertex(ofVec3f(f.getWidth(),0,0));
	m.addVertex(ofVec3f(0,f.getHeight(),0));
	m.addVertex(ofVec3f(f.getWidth(),f.getHeight(),0));
	
	m.addTexCoord(ofVec2f(0,0));
	m.addTexCoord(ofVec2f(f.getWidth(),0));
	m.addTexCoord(ofVec2f(0,f.getHeight()));
	m.addTexCoord(ofVec2f(f.getWidth(),f.getHeight()));
	
	m.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	
}

//normal update call
void CloudsVisualSystemGesturePaint::selfUpdate(){
	if(!canvassrc.isAllocated() ||
	   canvassrc.getWidth() != getSharedRenderTarget().getWidth() ||
	   canvassrc.getHeight() != getSharedRenderTarget().getHeight())
	{
		reallocateFramebuffers();
	}
	
	glDisable(GL_DEPTH_TEST);
	ofSetColor(255, 255);
	
	waterdst.begin();
	ofDisableAlphaBlending();
	ofClear(0, 0, 0, 0);
	
	hblurShader.begin();
	hblurShader.setUniformTexture("s_texture", watersrc.getTextureReference(), 1);
	hblurShader.setUniform2f("dimensions", waterdst.getWidth(), waterdst.getHeight());
	waterMesh.draw();
	hblurShader.end();
	waterdst.end();
	
	swap(watersrc,waterdst);
	
	waterdst.begin();
	ofDisableAlphaBlending();
	ofClear(0, 0, 0, 0);
	
	vblurShader.begin();
	vblurShader.setUniformTexture("s_texture", watersrc.getTextureReference(), 1);
	vblurShader.setUniform2f("dimensions", waterdst.getWidth(), waterdst.getHeight());
	waterMesh.draw();
	vblurShader.end();
	
	ofEnableAlphaBlending();
	
	for(int i = 0; i < depositPoints.size(); i++){
		brushImage.draw( depositPoints[i] * .25 );
	}
	
	waterdst.end();
	
	swap(watersrc,waterdst);

	canvasdst.begin();
//	ofEnableAlphaBlending();
	ofDisableAlphaBlending();
	ofClear(0, 0, 0, 0);
	paperMixShader.begin();
	paperMixShader.setUniformTexture("source_texture", canvassrc.getTextureReference(), 1);
	paperMixShader.setUniformTexture("water_texture", watersrc.getTextureReference(),  2);
	paperMixShader.setUniformTexture("flow_texture", noiseFlowTex.getTextureReference(),  3);
	paperMixShader.setUniform2f("dimensions", canvassrc.getWidth(), canvassrc.getHeight());

//	ofDisableAlphaBlending();
//	canvassrc.getTextureReference().bind();
	canvasMesh.draw();
//	canvassrc.getTextureReference().unbind();
	paperMixShader.end();
	
	ofPushStyle();
	ofEnableAlphaBlending();
	for(int i = 0; i < depositPoints.size(); i++){
		ofSetColor(ofColor::fromHsb(fmod(ofGetElapsedTimef()*20, 255.f), 255.0f, 255.0f));
		brushImage.draw( depositPoints[i] );
	}
	depositPoints.clear();
	ofPopStyle();
	
	canvasdst.end();
	
	swap(canvasdst,canvassrc);
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
	//ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	ofEnableAlphaBlending();
	paperImage.draw(paperRect);
	canvassrc.getTextureReference().draw(0,0);	
	watersrc.getTextureReference().draw(0,0);
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemGesturePaint::selfEnd(){
	
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