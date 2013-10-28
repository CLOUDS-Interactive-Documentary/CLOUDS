//
//  CloudsVisualSystemExampleOpenSteer.cpp
//

#include "CloudsVisualSystemExampleOpenSteer.h"


//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemExampleOpenSteer::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

    
    float dim = 16;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 255-xInit;
	
    customGui->addWidgetDown(new ofxUILabel("INITIAL VALUES", OFX_UI_FONT_MEDIUM));
    customGui->addSlider("N BOIDS", 10, 300, &Boid::nBoids);
    customGui->addSlider("TRAIL VERTEX COUNT", 20, 200, &Boid::trailVertexCount);
    customGui->addSlider("TRAIL DURATION", 1, 10, &Boid::trailDuration);
    customGui->addSlider("INITIAL POSITION RADIUS", 10, 50, &Boid::fInitialPositionRadius);

    vector<string> names;
	names.push_back("FLOCK");
	names.push_back("PATH");
	names.push_back("OBSTACLE");
    names.push_back("PURSUIT");
    customGui->addWidgetDown(new ofxUILabel("SIMULATIONS", OFX_UI_FONT_MEDIUM));
    customGui->addLabel("CLICK AGAIN TO RESET", OFX_UI_FONT_SMALL);
	customGui->addRadio("SIMULATIONS", names, OFX_UI_ORIENTATION_HORIZONTAL);
    
    customGui->addSpacer(length-xInit, 2);
	customGui->addWidgetDown(new ofxUILabel("BOIDS", OFX_UI_FONT_MEDIUM));
    customGui->addToggle("DRAW TRAIL", &Boid::bDrawTrail);
    customGui->addToggle("DRAW ANNOTATIONS", &Boid::bDrawAnnotations);
    customGui->addSlider("RADIUS", 0, 10, &Boid::radius);
    customGui->addSlider("MAX FORCE", 10, 50, &Boid::fMaxForce);
    customGui->addSlider("MAX SPEED", 5, 40, &Boid::fMaxSpeed);
    

    
    length = (customGui->getGlobalCanvasWidth()-customGui->getWidgetSpacing()*5)/3.;
    dim = customGui->getGlobalSliderHeight();
    
    
    customGui->addWidgetDown(new ofxUILabel("BOID COLOR", OFX_UI_FONT_MEDIUM));
    customGui->addMinimalSlider("BODY_R", 0.0, 1.0, &bodyColor.r, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("BODY_G", 0.0, 1.0, &bodyColor.g, length, dim)->setShowValue(false);
    customGui->addMinimalSlider("BODY_B", 0.0, 1.0, &bodyColor.b, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    
    customGui->addWidgetDown(new ofxUILabel("TRAIL COLOR", OFX_UI_FONT_MEDIUM));
    customGui->addMinimalSlider("TRAIL_R", 0.0, 1.0, &trailColor.r, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("TRAIL_G", 0.0, 1.0, &trailColor.g, length, dim)->setShowValue(false);
    customGui->addMinimalSlider("TRAIL_B", 0.0, 1.0, &trailColor.b, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    

    customGui->addWidgetDown(new ofxUILabel("TICK COLOR", OFX_UI_FONT_MEDIUM));
    customGui->addMinimalSlider("TICK_R", 0.0, 1.0, &tickColor.r, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("TICK_G", 0.0, 1.0, &tickColor.g, length, dim)->setShowValue(false);
    customGui->addMinimalSlider("TICK_B", 0.0, 1.0, &tickColor.b, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    customGui->addSpacer();
    customGui->addLabel("FLOCKING", OFX_UI_FONT_SMALL);
    customGui->addSlider("COHESION WEIGHT", 5, 20, &Boid::cohesionWeight);
    customGui->addSlider("SEPARATION WEIGHT", 5, 20, &Boid::separationWeight);
    customGui->addSlider("ALIGNMENT WEIGHT", 5, 20, &Boid::alignmentWeight);
    
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemExampleOpenSteer::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemExampleOpenSteer::selfGuiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    if(name == "FLOCK"){
        setSimulation(0);
	} else if(name == "PATH"){
        setSimulation(1);
    } else if(name == "OBSTACLE"){
        setSimulation(2);
    } else if (name == "PURSUIT"){
        setSimulation(3);
    } else if(name=="DRAW TRAIL") {
        //ofxUIButton *button = (ofxUIButton *) e.widget;
        //Boid::bDrawTrail = button->getValue();
    } else if(name=="MAX FORCE" && currentSimulation) {
        for(int i=0; i<currentSimulation->getVehicles().size(); i++) {
            currentSimulation->getVehicles()[i]->setMaxForce(Boid::fMaxForce);
        }
    } else if(name=="MAX SPEED" && currentSimulation) {
        for(int i=0; i<currentSimulation->getVehicles().size(); i++) {
            currentSimulation->getVehicles()[i]->setMaxSpeed(Boid::fMaxSpeed);
        }
    } else if(name=="RADIUS" && currentSimulation) {
        for(int i=0; i<currentSimulation->getVehicles().size(); i++) {
            currentSimulation->getVehicles()[i]->setRadius(Boid::radius);
        }
    } else if(name=="BODY_R") {
        Boid::bodyColor.setR(bodyColor.r);
    } else if(name=="BODY_G") {
        Boid::bodyColor.setG(bodyColor.g);
    } else if(name=="BODY_B") {
        Boid::bodyColor.setB(bodyColor.b);
    } else if(name=="TRAIL_R") {
        Boid::trailColor.setR(trailColor.r);
    } else if(name=="TRAIL_G") {
        Boid::trailColor.setG(trailColor.g);
    } else if(name=="TRAIL_B") {
        Boid::trailColor.setB(trailColor.b);
    } else if(name=="TICK_R") {
        Boid::tickColor.setR(tickColor.r);
    } else if(name=="TICK_G") {
        Boid::tickColor.setG(tickColor.g);
    } else if(name=="TICK_B") {
        Boid::tickColor.setG(tickColor.b);
    } else if(name=="N BOIDS") {
        Boid::nBoids = floor( ((ofxUISlider*)e.widget)->getScaledValue() + .5 );
        ((ofxUISlider*)e.widget)->setValue( Boid::nBoids );
    } else if(name=="TRAIL VERTEX COUNT") {
        Boid::trailVertexCount = floor( ((ofxUISlider*)e.widget)->getScaledValue() + .5 );
        ((ofxUISlider*)e.widget)->setValue( Boid::trailVertexCount );

    } else if(name=="TRAIL DURATION") {


    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemExampleOpenSteer::selfSetupSystemGui(){
	
}

void CloudsVisualSystemExampleOpenSteer::guiSystemEvent(ofxUIEventArgs &e){

}

//use render gui for display settings, like changing colors
void CloudsVisualSystemExampleOpenSteer::selfSetupRenderGui(){

}

void CloudsVisualSystemExampleOpenSteer::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemExampleOpenSteer::selfSetup(){
    Boid::bodyColor.set(1, 1, 1);
    Boid::trailColor.set(0.5f, 0.5f, 0.5f);
    Boid::tickColor.set(1.0, 1.0, 1.0);
    
    // add the simulations to the vector and initialize the first one
	simulations.push_back(&flocking);
	simulations.push_back(&pathFollowing);
	simulations.push_back(&obstacleAvoidance);
    simulations.push_back(&pursuitAndEvade);
		
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemExampleOpenSteer::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemExampleOpenSteer::selfBegin(){
    ofEnableAlphaBlending();
	
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
	glEnable(GL_DEPTH_TEST);
	
	// the sample simulations are quite small in scale, so
	// we wanna make sure our camera is close enough
	//getCameraRef()->setDistance(50);
    
    currentSimulation = NULL;
	setSimulation(0);
}

void CloudsVisualSystemExampleOpenSteer::setSimulation( int simulationIndex ){
	// just make sure we are inside out vector
	this->simulationIndex = ofClamp(simulationIndex, 0, simulations.size() - 1);
	
	// exit the current simulation
	if(currentSimulation) currentSimulation->exit();
	
	// define the current simulation
	currentSimulation = simulations[simulationIndex];
	
	// initialize the current simulation
	currentSimulation->setup();
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemExampleOpenSteer::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemExampleOpenSteer::selfUpdate(){
    currentSimulation->update();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemExampleOpenSteer::selfDraw(){
    cam.begin();
    currentSimulation->draw();
	cam.end();
}

// draw any debug stuff here
void CloudsVisualSystemExampleOpenSteer::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemExampleOpenSteer::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemExampleOpenSteer::selfEnd(){

}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemExampleOpenSteer::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemExampleOpenSteer::selfKeyPressed(ofKeyEventArgs & args){
    /*
    if( args.key == OF_KEY_LEFT ){
		simulationIndex--;
		if(simulationIndex < 0) simulationIndex = simulations.size() - 1;
		setSimulation(simulationIndex);
	}
	else if( args.key == OF_KEY_RIGHT ){
		simulationIndex++;
		if(simulationIndex > simulations.size() - 1) simulationIndex = 0;
		setSimulation(simulationIndex);
	}
     */
}

void CloudsVisualSystemExampleOpenSteer::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemExampleOpenSteer::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleOpenSteer::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleOpenSteer::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemExampleOpenSteer::selfMouseReleased(ofMouseEventArgs& data){
	
}