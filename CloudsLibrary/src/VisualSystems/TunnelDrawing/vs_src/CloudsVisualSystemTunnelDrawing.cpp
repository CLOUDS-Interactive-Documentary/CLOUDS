//
//  CloudsVisualSystemTunnelDrawing.cpp
//

#include "CloudsVisualSystemTunnelDrawing.h"


void CloudsVisualSystemTunnelDrawing::selfSetDefaults(){
	screenSpaceProjectDistance = .95;
	fallOffSpeed = -2;
	drawWireframe = false;
	drawFill = true;

#ifdef MOUSE_INPUT
	bPromptForInteraction = true;
#endif
    primaryCursorMode = CURSOR_MODE_DRAW;
    secondaryCursorMode = CURSOR_MODE_DRAW;

	#ifdef OCULUS_RIFT
	camera.begin( ofRectangle(0,0,getCanvasWidth(),getCanvasHeight()) );
	camera.end();
	#endif

}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemTunnelDrawing::selfSetupGui(){

	customGui = new ofxUISuperCanvas("TUNNEL", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Tunnel");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	customGui->addSlider("projection dist", .9,1.0, &screenSpaceProjectDistance);
	customGui->addSlider("cam speed", -4, 0, &fallOffSpeed);
	customGui->addRangeSlider("thickness range", 0, 20, &thicknessRange.min, &thicknessRange.max);
	customGui->addRangeSlider("distance range",  0, 10, &distanceRange.min,  &distanceRange.max);

	customGui->addToggle("DRAW FILL", &drawFill);
    float length = (customGui->getGlobalCanvasWidth() - customGui->getWidgetSpacing()*5)/3.;
    float dim = customGui->getGlobalSliderHeight();
    customGui->addLabel("RIBBON COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("RH", 0.0, 1.0, &ribbonColorHSV.r, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("RS", 0.0, 1.0, &ribbonColorHSV.g, length, dim)->setShowValue(false);
    customGui->addMinimalSlider("RV", 0.0, 1.0, &ribbonColorHSV.b, length, dim)->setShowValue(false);
    customGui->addMinimalSlider("RA", 0.0, 1.0, &ribbonColorHSV.a, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    customGui->autoSizeToFitWidgets();

	customGui->addToggle("DRAW WIREFRAME", &drawWireframe);
	customGui->addLabel("WIREFRAME COLOR", OFX_UI_FONT_SMALL);
    customGui->addMinimalSlider("WH", 0.0, 1.0, &wireframeColorHSV.r, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("WS", 0.0, 1.0, &wireframeColorHSV.g, length, dim)->setShowValue(false);
    customGui->addMinimalSlider("WV", 0.0, 1.0, &wireframeColorHSV.b, length, dim)->setShowValue(false);
    customGui->addMinimalSlider("WA", 0.0, 1.0, &wireframeColorHSV.a, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    customGui->autoSizeToFitWidgets();

	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemTunnelDrawing::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemTunnelDrawing::selfGuiEvent(ofxUIEventArgs &e){
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemTunnelDrawing::selfSetupSystemGui(){
	
}

void CloudsVisualSystemTunnelDrawing::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemTunnelDrawing::selfSetupRenderGui(){

}

void CloudsVisualSystemTunnelDrawing::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemTunnelDrawing::selfSetup(){
	colorShader.load(getVisualSystemDataPath() + "shaders/ribbon");
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemTunnelDrawing::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemTunnelDrawing::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemTunnelDrawing::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemTunnelDrawing::selfUpdate(){

#ifndef MOUSE_INPUT
	map<int, CloudsInteractionEventArgs>::iterator init;
	for(init = GetCloudsInputPoints().begin(); init != GetCloudsInputPoints().end(); init++){
		//cout << "adding interaction point " << init->second.position << endl;
		addInteractionPoint(init->second);
	}
#endif

	map<int, RibbonTrail>::iterator it;
	for(it = currentTrails.begin(); it != currentTrails.end(); it++){
		for(int i = 0; i < it->second.points.size(); i++){
			it->second.points[i].z -= fallOffSpeed;
		}
	}
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemTunnelDrawing::selfDraw(){
	
	ofPushStyle();
	ofSetColor(255);
	ofEnableAlphaBlending();
	ofDisableDepthTest();

	//do the same thing from the first example...
    ofMesh mesh;
	map<int, RibbonTrail>::iterator it;
	for(it = currentTrails.begin(); it != currentTrails.end(); it++){
		drawRibbonTrail(it->second, mesh);
	}
	
	ofFloatColor ribbonColor = ofFloatColor::fromHsb(ribbonColorHSV.r,
													 ribbonColorHSV.g,
													 ribbonColorHSV.b,
													 ribbonColorHSV.a);
	ofFloatColor wireframeColor = ofFloatColor::fromHsb(wireframeColorHSV.r,
														wireframeColorHSV.g,
														wireframeColorHSV.b,
														wireframeColorHSV.a);
	//end the shape
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	colorShader.begin();
	if(drawFill){
		colorShader.setUniform4f("color",ribbonColor.r,
									ribbonColor.g,
									ribbonColor.b,
									ribbonColor.a);
		mesh.draw();
	}
	if(drawWireframe){
		colorShader.setUniform4f("color",wireframeColor.r,
									wireframeColor.g,
									wireframeColor.b,
									wireframeColor.a);
		mesh.drawWireframe();
	}
	colorShader.end();
	
	ofEnableDepthTest();
	ofEnableAlphaBlending();
	ofPopStyle();
}

void CloudsVisualSystemTunnelDrawing::drawRibbonTrail(const RibbonTrail& trail, ofMesh& geo){
	
	for(int i = 1; i < trail.points.size(); i++){
        
		//find this point and the next point
		ofVec3f thisPoint = trail.points[i-1];
		ofVec3f nextPoint = trail.points[i];
        
		//get the direction from one to the next.
		//the ribbon should fan out from this direction
		ofVec3f direction = (nextPoint - thisPoint);
        //direction.z = 0;
		
		//get the distance from one point to the next
		float distance = direction.length();
        
		//get the normalized direction. normalized vectors always have a length of one
		//and are really useful for representing directions as opposed to something with length
		ofVec3f unitDirection = direction.normalized();
        
		//find both directions to the left and to the right
		ofVec3f toTheLeft  = unitDirection.getRotated(-90, ofVec3f(0,0,1));
		ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));
        
		//use the map function to determine the distance.
		//the longer the distance, the narrower the line.
		//this makes it look a bit like brush strokes
		float thickness = ofMap(distance,
								distanceRange.min, distanceRange.max,
								thicknessRange.max, thicknessRange.min, true);
		
		//calculate the points to the left and to the right
		//by extending the current point in the direction of left/right by the length
		ofVec3f leftPoint  = thisPoint + toTheLeft  * thickness;
		ofVec3f rightPoint = thisPoint + toTheRight * thickness;
        
		if(i == 1){
			geo.addColor(ofFloatColor(0,0,0,0));
			geo.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
			geo.addColor(ofFloatColor(0,0,0,0));
			geo.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
		}
		
		//add these points to the triangle strip
		geo.addColor(ofFloatColor::white);
		geo.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
		geo.addColor(ofFloatColor::white);
		geo.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
		
		if( i == trail.points.size()-1 ){
			geo.addColor(ofFloatColor(0,0,0,0));
			geo.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
			geo.addColor(ofFloatColor(0,0,0,0));
			geo.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
		}
	}
}

// draw any debug stuff here
void CloudsVisualSystemTunnelDrawing::selfDrawDebug(){
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemTunnelDrawing::selfDrawBackground(){
	
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemTunnelDrawing::selfEnd(){
	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemTunnelDrawing::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemTunnelDrawing::selfKeyPressed(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemTunnelDrawing::selfKeyReleased(ofKeyEventArgs & args){

}

void CloudsVisualSystemTunnelDrawing::selfInteractionDragged(CloudsInteractionEventArgs& data){
#ifdef MOUSE_INPUT
   addInteractionPoint(data);
#endif
}

void CloudsVisualSystemTunnelDrawing::selfInteractionMoved(CloudsInteractionEventArgs& data){
#ifdef MOUSE_INPUT
    addInteractionPoint(data);
#endif
}

void CloudsVisualSystemTunnelDrawing::addInteractionPoint(CloudsInteractionEventArgs& args){
	ofVec3f mousePoint = camera.screenToWorld( ofVec3f(args.position.x,
													   args.position.y,
													   screenSpaceProjectDistance),
													   ofRectangle(0,0,getCanvasWidth(),getCanvasHeight()));
    if (currentTrails[args.playerId].points.empty() ||
        currentTrails[args.playerId].points.back().distance(mousePoint) > 10.0) {
        currentTrails[args.playerId].points.push_back(mousePoint);
    }
	
}

void CloudsVisualSystemTunnelDrawing::selfInteractionStarted(CloudsInteractionEventArgs& data){
	
}

void CloudsVisualSystemTunnelDrawing::selfInteractionEnded(CloudsInteractionEventArgs& data){
	
}