//
//  CloudsVisualSystemNbody.cpp
//

#include "CloudsVisualSystemNbody.h"
#include "CloudsRGBDVideoPlayer.h"


//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemNbody::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addIntSlider("numParticles", 512, 512*512, &numParticles);
	customGui->addIntSlider("Trail Length", 1, 20, &trailLength);
	customGui->addBaseDraws("force tex", &force);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemNbody::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemNbody::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemNbody::selfSetupSystemGui(){
	
}

void CloudsVisualSystemNbody::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemNbody::selfSetupRenderGui(){

}

void CloudsVisualSystemNbody::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemNbody::loadShaders(){
	
	cout << "Loading render" << endl;
	renderParticles.load(getVisualSystemDataPath() + "shaders/render");
	cout << "Loading position" << endl;
	updatePosition.load(getVisualSystemDataPath() + "shaders/position");
	cout << "Loading velocity" << endl;
	updateVelocities.load(getVisualSystemDataPath() + "shaders/velocity");;
	cout << "Loading noise" << endl;			
	noiseForce.load(getVisualSystemDataPath() + "shaders/noise");

}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemNbody::selfSetup(){
	loadShaders();
	regenerate();
}

// draw any debug stuff here
void CloudsVisualSystemNbody::regenerate(){
	
	particles.clear();
	particleHeads.clear();
	
	for(int y = 0; y < numParticles / 512; y++){
		for(int x = 0; x < 512; x++){
			//placeholder
//			particleHeads.addVertex(ofVec3f(x,y,0));
			particles.addColor(ofFloatColor(0,0));
			particles.addVertex( ofVec3f(x,y*trailLength,0) );
			for(int t = 0; t < trailLength; t++){
				particles.addColor(ofFloatColor::white);
				particles.addVertex( ofVec3f(x,y*trailLength+t,0) );
			}
			particles.addColor(ofFloatColor(0,0));
			particles.addVertex( ofVec3f(x,y*(trailLength)-1,0) );
		}
	}
	
	particles.setMode(OF_PRIMITIVE_LINE_STRIP);

	
	//positions are for all the points, including the trails
	positionFront.allocate(512,numParticles/512*trailLength, GL_RGB32F );
	positionBack.allocate(512,numParticles/512*trailLength, GL_RGB32F );
	//but force and velocity is just for the heads!
	velocityFront.allocate(512,numParticles/512, GL_RGB32F );
	velocityBack.allocate(512,numParticles/512, GL_RGB32F );
	force.allocate(512,numParticles/512, GL_RGB32F );

	meshFromFbo(force, drawMesh);
	meshFromFbo(positionFront, drawMeshTrails);
	
	cout << "Generated " << particles.getNumVertices() << endl;

}

void CloudsVisualSystemNbody::meshFromFbo(ofFbo& fbo, ofMesh& mesh){
	
	mesh.addTexCoord(ofVec2f(0,0));
	mesh.addVertex(ofVec3f(0,0,0));
	
	mesh.addTexCoord(ofVec2f(fbo.getWidth(),0));
	mesh.addVertex(ofVec3f(fbo.getWidth(),0,0));
	
	mesh.addTexCoord(ofVec2f(0,fbo.getHeight()));
	mesh.addVertex(ofVec3f(0,fbo.getHeight(),0));
	
	mesh.addTexCoord(ofVec2f(fbo.getWidth(),fbo.getHeight()));
	mesh.addVertex(ofVec3f(fbo.getWidth(),fbo.getHeight(),0));
	
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemNbody::selfPresetLoaded(string presetPath){
	regenerate();
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemNbody::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemNbody::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemNbody::selfUpdate(){

	float dT = ofGetElapsedTimef() - ofGetLastFrameTime();
	
	force.begin();
	ofClear(0);
	noiseForce.begin();
	noiseForce.setUniform1f("trailLength", trailLength);
	drawMesh.draw();
	noiseForce.end();
	force.end();
	
	velocityFront.begin();
	updateVelocities.begin();
	updateVelocities.setUniform1f("dT", dT);
	updateVelocities.setUniformTexture("velocity", velocityBack.getTextureReference(), 0);
	updateVelocities.setUniformTexture("force", force.getTextureReference(), 1);
	//particleHeads.draw();
	drawMesh.draw();
	updateVelocities.end();
	velocityFront.end();
	
	positionFront.begin();
	updatePosition.begin();
	updatePosition.setUniform1f("dT", dT);
	updatePosition.setUniformTexture("velocity", velocityBack.getTextureReference(), 0);
	updatePosition.setUniformTexture("position", positionBack.getTextureReference(), 1);
	drawMeshTrails.draw();
	updatePosition.end();
	positionFront.end();
	
	swap(velocityFront,velocityBack);
	swap(positionFront,positionBack);

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemNbody::selfDraw(){
	
	renderParticles.begin();
	renderParticles.setUniformTexture("position", positionBack.getTextureReference(), 0);
	particles.draw();
	renderParticles.end();
	
}


// draw any debug stuff here
void CloudsVisualSystemNbody::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemNbody::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemNbody::selfEnd(){

}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemNbody::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemNbody::selfKeyPressed(ofKeyEventArgs & args){
	if(args.key == 'R'){
		regenerate();
	}
	if(args.key == 'S'){
		loadShaders();
	}
}

void CloudsVisualSystemNbody::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemNbody::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemNbody::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemNbody::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemNbody::selfMouseReleased(ofMouseEventArgs& data){
	
}