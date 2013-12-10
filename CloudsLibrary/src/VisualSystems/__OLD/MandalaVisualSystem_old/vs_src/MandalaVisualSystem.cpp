//
//  MandalaVisualSystem.cpp
//

#include "MandalaVisualSystem.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void MandalaVisualSystem::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidth( customGui->getRect()->getWidth() * 2 );
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("globalScale.x", -5, 5, &mandalaGlobalScale.x )->setIncrement(.01);
	customGui->addSlider("globalScale.y", -5, 5, &mandalaGlobalScale.y )->setIncrement(.01);
	customGui->addSlider("globalScale.z", -5, 5, &mandalaGlobalScale.z )->setIncrement(.01);
	
	customGui->addSlider("globalRotation.x", -180, 180, &globalRotation.x )->setIncrement(.01);
	customGui->addSlider("globalRotation.y", -180, 180, &globalRotation.y )->setIncrement(.01);
	customGui->addSlider("globalRotation.z", -180, 180, &globalRotation.z )->setIncrement(.01);
	
	customGui->addSlider("globalRotationVel.x", -180, 180, &globalRotationVelocity.x )->setIncrement(.01);
	customGui->addSlider("globalRotationVel.y", -180, 180, &globalRotationVelocity.y )->setIncrement(.01);
	customGui->addSlider("globalRotationVel.z", -180, 180, &globalRotationVelocity.z )->setIncrement(.01);
	
	ofAddListener(customGui->newGUIEvent, this, &MandalaVisualSystem::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
	
	
	subsystemGui = new ofxUISuperCanvas("sub systems", gui);
	subsystemGui->copyCanvasStyle(gui);
	subsystemGui->copyCanvasProperties(gui);
	subsystemGui->setName("sub systems");
	subsystemGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	subsystemGui->addFPS();
	subsystemGui->addSpacer();
	
	vector <string> subsystemNames;
	for (map<string, int>::iterator it=subSystems.begin(); it!=subSystems.end(); it++) {
		subsystemNames.push_back( it->first );
	}
	subsystemGui->addRadio("subsystem radio", subsystemNames );
	
	
	ofAddListener(subsystemGui->newGUIEvent, this, &MandalaVisualSystem::selfGuiEvent);
	guis.push_back(subsystemGui);
	guimap[customGui->getName()] = subsystemGui;

	
}

void MandalaVisualSystem::selfGuiEvent(ofxUIEventArgs &e){
	
	string name = e.getName();
	int kind = e.widget->getKind();
	
	if( kind == OFX_UI_WIDGET_TOGGLE && e.getToggle()->getValue() == true )
	{
		if(e.widget->getParent()->getName() == "subsystem radio")
		{
			if(subSystems.find(name) != subSystems.end())
			{
				nextSubsystem = subSystems[name];
			}
		}
	}
}

//Use system gui for global or logical settings, for exmpl
void MandalaVisualSystem::selfSetupSystemGui(){
	
}

void MandalaVisualSystem::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void MandalaVisualSystem::selfSetupRenderGui(){

}

void MandalaVisualSystem::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void MandalaVisualSystem::selfSetup(){
	
	videoLoaded = false;

	//getVisualSystemDataPath() + "images/someImage.png";
	
	//define our subSystems
	subSystems["noise field"] = 0;
	subSystems["default"] = 1;
	subSystems["muybridge"] = 2;
//	subSystems["emanation"] = 3;
	
	currentSubsystem = nextSubsystem = -1;

	
	
	//defaults
	mandalaGlobalScale.set(1,1,1);
	
	
	//shader setup
	loadShaders();
	
		

	
	//TODO: work out a better system for only loading images when the subsystem is built. and clean up on teardown
	images.resize(4);
	
	images[0].loadImage( getVisualSystemDataPath() + "images/elsaHat.png" );
	images[1].loadImage( getVisualSystemDataPath() + "images/germanBirdRamp.jpg" );
	images[2].loadImage( getVisualSystemDataPath() + "images/greenFatBird.jpg" );
	images[3].loadImage( getVisualSystemDataPath() + "images/laser-eyes1.jpg" );
	
	for (int i=0; i<images.size(); i++) {
		textures.push_back( &images[i].getTextureReference() );
	}
	
	vid.loadMovie(getVisualSystemDataPath() + "images/aliens.gif" );
	vid.play();
	textures.push_back( &vid.getTextureReference() );
	
	muybridgeRide.resize(15);
	for(int i=0; i<muybridgeRide.size(); i++)
	{
		muybridgeRide[i].loadImage( getVisualSystemDataPath() + "images/Muybridge_race_horse_animated_" + ofToString(i+1) +".png" );
	}
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void MandalaVisualSystem::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void MandalaVisualSystem::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void MandalaVisualSystem::selfSceneTransformation(){
	
}

//normal update call
void MandalaVisualSystem::selfUpdate()
{
	vid.update();
	
	if (currentSubsystem != nextSubsystem )
	{
		currentSubsystem = nextSubsystem;
		
		//remove the old subsystem
		tearDownSubsystem();
		
		//build our sub system
		switch (nextSubsystem) {
				
			//NOISE FIELD
			case 0:
				buildNoiseFieldSubsystem();
				break;
				
			case 1:
				buildDefaultSubsystem();
				break;
				
			case 2:
				buildMuybridgeSubsystem();
				break;
				
			default:
				buildDefaultSubsystem();
				break;
		}
		
	}
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void MandalaVisualSystem::selfDraw()
{
	float t = ofGetElapsedTimef();
	ofPushMatrix();
	
	ofScale( mandalaGlobalScale.x, mandalaGlobalScale.y, mandalaGlobalScale.z );
	
	ofRotateX( globalRotation.x + globalRotationVelocity.x * t );
	ofRotateY( globalRotation.y + globalRotationVelocity.y * t );
	ofRotateZ( globalRotation.z + globalRotationVelocity.z * t );
	
	ofEnableAlphaBlending();
	
	glLineWidth( 2 );
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	

	glEnable(GL_DEPTH_TEST);
//	glDisable(GL_DEPTH_TEST);
//	glDepthFunc(GL_GREATER);
	
	
	glClearDepth(1.);
	glDepthFunc(GL_LESS);
	
	drawMandala();
	
	
	glClearDepth(1.);
	glDepthFunc(GL_LESS);
	
	glCullFace(GL_BACK);
	drawMandala();
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	
	ofPopMatrix();
}

void MandalaVisualSystem::drawRandomTextures()
{
	for (int i=0; i<cogs.size(); i++) {
		float rotY = ofGetElapsedTimef() * cogs[i]->radius;
		ofPushMatrix();
		ofRotate( rotY, 0, 1, 0);
		
		if(i%2){
			cogs[i]->frontTexture = textures[i%textures.size()];
			cogs[i]->sideTexture = textures[(i+1)%textures.size()];
			cogs[i]->draw( &deformedAndTextured, currentSubsystem );
		}
		else{
			cogs[i]->drawBorders( &deformerShader );
		}
		
		ofPopMatrix();
	}
}

void MandalaVisualSystem::drawMandala()
{

	
	switch (currentSubsystem) {
		case 0:
			//Noise field
//			drawAllTheCogs( &deformerShader );
			for (int i=0; i<cogs.size(); i++) {
//				cogs[i]->bDrawWireframe = true;
				cogs[i]->draw( &deformedAndTextured );
			}
			break;
		case 1:
			drawRandomTextures();
			break;
			
		case 2:
			drawAllTheCogs( &deformedAndTextured, 2 );
			break;
			
		default:
			drawRandomTextures();
			break;
	}
}

void MandalaVisualSystem::drawAllTheCogs( ofShader* shader, int renderMode )
{
	for (int i=0; i<cogs.size(); i++) {
		cogs[i]->draw( shader, renderMode );
	}
}

// draw any debug stuff here
void MandalaVisualSystem::selfDrawDebug(){
	
	glLineWidth( 4 );
	ofSetColor(255,0,0);
	ofLine(-100, 0,0, 100, 0,0);
	ofSetColor(0,255,0);
	ofLine(0,-100, 0, 0,100,0);
	ofSetColor(0,0,255);
	ofLine(0,0,-100, 0,0,100);
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void MandalaVisualSystem::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void MandalaVisualSystem::selfEnd(){
	
	simplePointcloud.clear();
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void MandalaVisualSystem::selfExit()
{
	
	tearDownSubsystem();
	
	for (int i=0; i<images.size(); i++) {
		images[i].clear();
	}
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void MandalaVisualSystem::selfKeyPressed(ofKeyEventArgs & args)
{
	if(args.key == 'l' || args.key == 'L' )
	{
		loadShaders();
	}
}
void MandalaVisualSystem::selfKeyReleased(ofKeyEventArgs & args){
	
}

void MandalaVisualSystem::selfMouseDragged(ofMouseEventArgs& data){
	
}

void MandalaVisualSystem::selfMouseMoved(ofMouseEventArgs& data){
	
}

void MandalaVisualSystem::selfMousePressed(ofMouseEventArgs& data){
	
}

void MandalaVisualSystem::selfMouseReleased(ofMouseEventArgs& data){
	
}

void MandalaVisualSystem::loadShaders()
{
	normalShader.load( getVisualSystemDataPath() + "shaders/normalShader");
	deformerShader.load( getVisualSystemDataPath() + "shaders/deformer");
	
	deformedAndTextured.load( getVisualSystemDataPath() + "shaders/deformer.vert", getVisualSystemDataPath() + "shaders/CogTextured.frag");
}

void MandalaVisualSystem::tearDownSubsystem()
{
	for (vector<Cog*>::iterator it = cogs.begin(); it != cogs.end(); it++) {
		(*it)->clear();
	}
	
	cogs.clear();
}

Cog* MandalaVisualSystem::addCog( float _radius, float _thickness, float _startU, float _sweepU, float _startV, float _sweepV)
{
	cogs.push_back( new Cog( _radius, _thickness, _startU, _sweepU, _startV, _sweepV) );
	return cogs.back();
}

Cog* MandalaVisualSystem::addCog( float _radius, float _thickness, float _startU, float _sweepU, float _startV, float _sweepV, int _subdU, int _subdV)
{
	cogs.push_back( new Cog( _radius, _thickness, _startU, _sweepU, _startV, _sweepV, _subdU, _subdV) );
	return cogs.back();
}

void MandalaVisualSystem::buildDefaultSubsystem()
{
	//create some cogs
	for (int i=0; i<10; i++) {
		cogs.push_back( new Cog(1, 1, .1 * i, .09, .2, .6) );
		if(i%2)	cogs.push_back( new Cog(3, 2, .1 * i, .09, .2, .6) );
		if(i%4) cogs.push_back( new Cog(8, 2, .1 * i, .09, .2, .6) );

		cogs.push_back( new Cog(12, 1, .1 * i, .09, .2, .6) );
		if(i%2)	cogs.push_back( new Cog(14, 1, .1 * i, .09, .2, .6) );
		if(i%4) cogs.push_back( new Cog(16, 1, .1 * i, .09, .2, .2) );

		if(i%2) cogs.push_back( new Cog(18, 3, .1 * i, .09, .2, .7) );
		cogs.push_back( new Cog(22, 10, .1 * i, .09, .2, .3) );
		if(i%3)cogs.push_back( new Cog( 33, 30, .1 * i, .09, .4, .1) );
	}
}

void MandalaVisualSystem::buildNoiseFieldSubsystem()
{
	int numRings = 8;
	float thickness = 20;
	float roughSize = 50;
	float minRadius = 5;
	
	float radius, circumfrence, minCol = 0.25, maxCol = 1.5;
	int subd;
	for (int i=0; i<numRings; i++) {
		radius = thickness * i + thickness*2;
		
		circumfrence = TWO_PI * radius;
		subd = int(circumfrence / roughSize);
		
		float step = .5/subd;
		for(int j=0; j<subd; j++){
			float sweep = .9 * step;
			Cog* c = addCog(radius + ofRandom(thickness) + minRadius, thickness, float(j) * step, sweep, ofRandom(2.), .025 + ofRandom(.3), Cog::radiansToSdubd(sweep), 6 );
			
			c->sideColor.set( ofRandom(minCol, maxCol), ofRandom(minCol, maxCol), ofRandom(minCol, maxCol), 1. );
			c->frontColor.set( c->sideColor.getInverted() * 2.);
			
			c->bDrawBorders = true;
			c->edgeLinewidth = 3;
		}
	}
}

void MandalaVisualSystem::buildMuybridgeSubsystem()
{
	//15 images
	float step = 1. /float(muybridgeRide.size());
	float radius = 30;
	float thickness = 30;
	float sweep = step * .9;
	
	for (int i=0; i<muybridgeRide.size(); i++)
	{
		Cog* c = addCog(radius, thickness, float(i)*step, sweep, .435, .13 );
		
		c->sideColor.set( 1,1,1,1 );
		c->frontColor.set( 1,1,1,1 );
		
		c->frontTexture = &muybridgeRide[i].getTextureReference();
	}
		
}

