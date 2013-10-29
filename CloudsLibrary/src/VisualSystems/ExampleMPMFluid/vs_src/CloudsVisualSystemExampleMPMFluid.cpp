//
//  CloudsVisualSystemEmpty.cpp
//

#include "CloudsVisualSystemExampleMPMFluid.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events

void CloudsVisualSystemExampleMPMFluid::selfSetupGui(){
    customGui = new ofxUISuperCanvas("EXAMPLE MPM FLUID", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
	customGui->setName("EXAMPLE MPM FLUID");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    customGui->addLabel("PHYSICS");
    customGui->addSlider("PARTICLES", 0, 50000, &nParticles);
    customGui->addSlider("DENSITY", 0, 30, &density);
    customGui->addSlider("STIFFNESS", 0, 2, &stiffness);
    customGui->addSlider("VISCOSITY", 0, 4, &viscosity);
    customGui->addSlider("BULK VISCOSITY", 0, 10, &bulkViscosity);
    customGui->addSlider("ELASTICITY", 0, 4, &elasticity);
    customGui->addSlider("GRAVITY", 0, 0.2f, &gravity);
    
    
    float length = (customGui->getGlobalCanvasWidth()-customGui->getWidgetSpacing()*5)/3.;
    float dim = customGui->getGlobalSliderHeight();

    /* Particle color */
    //customGui->addSpacer();
    customGui->addSpacer();
    customGui->addLabel("LOOK");
    customGui->addLabel("PARTICLE COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("H", 0.0, 255, &pColor.r, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("S", 0.0, 255, &pColor.g, length, dim)->setShowValue(true);
    customGui->addMinimalSlider("B", 0.0, 255, &pColor.b, length, dim)->setShowValue(true);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    customGui->addSlider("PARTICLE LINE", 0, 5, &lineWidth);

    customGui->addSpacer();
    customGui->addLabel("INTERACTIVITY");
    vector<string> modes;
    modes.push_back("Mouse as force");
    modes.push_back("Mouse as obstacle");
    interModeRadio = customGui->addRadio("INTERACTIVE MODE", modes);
    customGui->addSlider("OBSTACLE SIZE", 0, 30, &obstacleSize);
    customGui->addSlider("MOUSE FORCE", 0, 20, &mouseForce);
    
    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemExampleMPMFluid::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemExampleMPMFluid::selfGuiEvent(ofxUIEventArgs &e)
{

}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemExampleMPMFluid::selfSetupSystemGui(){
	
}

void CloudsVisualSystemExampleMPMFluid::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemExampleMPMFluid::selfSetupRenderGui(){
    
}

void CloudsVisualSystemExampleMPMFluid::guiRenderEvent(ofxUIEventArgs &e)
{
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemExampleMPMFluid::selfSetup()
{
    fluid.setup(50000, 64, 40);

    nParticles = 20000;
    density = 5;
    stiffness = 0.5;
    bulkViscosity = 3.0;
    elasticity = 1;
    viscosity = 1;
    gravity = 0.002;
    
    obstacleSize = 4;
    
    pColor = ofColor::fromHsb(0, 0, 255);
    lineWidth = 1;
    
    mouseForce = 5;
    
    // add obstacle
    obstacle = new ofxMPMObstacle(0, 0, obstacleSize);
    fluid.addObstacle(obstacle);
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemExampleMPMFluid::selfPresetLoaded(string presetPath)
{

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemExampleMPMFluid::selfBegin()
{

}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemExampleMPMFluid::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemExampleMPMFluid::selfUpdate()
{
    
    fluid.numParticles = (int)nParticles;
    fluid.densitySetting = density;
    fluid.stiffness = stiffness;
    fluid.bulkViscosity = bulkViscosity;
    fluid.viscosity = viscosity;
    fluid.elasticity = elasticity;
    fluid.gravity = gravity/10;
    
    fluid.scaleFactor = scaleFactor = (float)ofGetWidth() / fluid.getGridSizeX();
    
    // handle interactivity
    ofxUIToggle* toggle = (ofxUIToggle*)interModeRadio->getToggles()[0];
    if (toggle->getValue()) {
        fluid.bDoMouse = true;
        fluid.bDoObstacles = false;
        fluid.mouseForce = mouseForce;
    }
    else {
        fluid.bDoMouse = false;
        fluid.bDoObstacles = true;
        obstacle->cx = (float)ofGetMouseX()/scaleFactor;
        obstacle->cy = (float)ofGetMouseY()/scaleFactor;
        obstacle->radius = obstacleSize;
        obstacle->radius2 = obstacleSize * obstacleSize;
    }
    
    fluid.update();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemExampleMPMFluid::selfDraw()
{
    
}

// draw any debug stuff here
void CloudsVisualSystemExampleMPMFluid::selfDrawDebug()
{
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemExampleMPMFluid::selfDrawBackground()
{
	// These improve the appearance of small lines and/or points.
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable (GL_LINE_SMOOTH);
	glEnable (GL_POINT_SMOOTH); // in case you want it
	glEnable (GL_MULTISAMPLE);
	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ofColor c = ofColor::fromHsb(pColor.r, pColor.g, pColor.b);
    c.a = 200;
	ofSetColor(c);
	glLineWidth(lineWidth); // or thicker, if you prefer
	
	ofPushMatrix();
    ofTranslate((ofGetWidth() - (float)fluid.getGridSizeX()*scaleFactor)/2, 0);
	ofScale(scaleFactor, scaleFactor, 1.0);
	
	// Draw the active particles as a short line,
	// using their velocity for their length.
	vector<ofVec2f> verts;
    
	for (int ip=0; ip<fluid.numParticles; ip++) {
		ofxMPMParticle* p = fluid.getParticles()[ip];
		verts.push_back(ofVec2f(p->x, p->y));
		verts.push_back(ofVec2f(p->x - p->u, p->y - p->v));
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &(verts[0].x));
	glDrawArrays(GL_LINES, 0, verts.size());
	glDisableClientState(GL_VERTEX_ARRAY);
	ofPopMatrix();
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemExampleMPMFluid::selfEnd()
{
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemExampleMPMFluid::selfExit()
{
    delete obstacle;
    obstacle = NULL;
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemExampleMPMFluid::selfKeyPressed(ofKeyEventArgs & args)
{
	
}
void CloudsVisualSystemExampleMPMFluid::selfKeyReleased(ofKeyEventArgs & args)
{
	
}

void CloudsVisualSystemExampleMPMFluid::selfMouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemExampleMPMFluid::selfMouseMoved(ofMouseEventArgs& data)
{
}

void CloudsVisualSystemExampleMPMFluid::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleMPMFluid::selfMouseReleased(ofMouseEventArgs& data){
	
}
