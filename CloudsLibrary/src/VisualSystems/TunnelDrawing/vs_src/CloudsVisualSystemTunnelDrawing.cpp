//
//  CloudsVisualSystemTunnelDrawing.cpp
//

#include "CloudsVisualSystemTunnelDrawing.h"


//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemTunnelDrawing::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	customGui->addSlider("projection dist", -100, 100, &screenSpaceProjectDistance);
	customGui->addSlider("cam speed", -4, 0, &fallOffSpeed);

	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemTunnelDrawing::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemTunnelDrawing::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
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
	
    
	//initialize the variable so it's off at the beginning
//    usecamer = true;
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
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
    
	ofVec3f sumOfAllPoints(0,0,0);
	for(int i = 0; i < points.size(); i++){
		points[i].z -= fallOffSpeed;
		sumOfAllPoints += points[i];
	}
	center = sumOfAllPoints / points.size();

	
//	camera.setPosition( ofVec3f(getSharedRenderTarget().getWidth()/2,
//								getSharedRenderTarget().getHeight()/2, 0));
	
	//camera.lookAt(center);
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemTunnelDrawing::selfDraw(){
	
//	if(usecamera){
//        camera.begin();
//    }
	ofSetColor(0);
	//do the same thing from the first example...
    ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	for(int i = 1; i < points.size(); i++){
        
		//find this point and the next point
		ofVec3f thisPoint = points[i-1];
		ofVec3f nextPoint = points[i];
        
		//get the direction from one to the next.
		//the ribbon should fan out from this direction
		ofVec3f direction = (nextPoint - thisPoint);
        
		//get the distance from one point to the next
		float distance = direction.length();
        
		//get the normalized direction. normalized vectors always have a length of one
		//and are really useful for representing directions as opposed to something with length
		ofVec3f unitDirection = direction.normalized();
        
		//find both directions to the left and to the right
		ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,0,1));
		ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));
        
		//use the map function to determine the distance.
		//the longer the distance, the narrower the line.
		//this makes it look a bit like brush strokes
		float thickness = ofMap(distance, 0, 60, 20, 2, true);
        
		//calculate the points to the left and to the right
		//by extending the current point in the direction of left/right by the length
		ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
		ofVec3f rightPoint = thisPoint+toTheRight*thickness;
        
		//add these points to the triangle strip
		mesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
		mesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
	}
    
	//end the shape
	mesh.draw();
    
	
    
	//if we're using the camera, take it away
//    if(usecamera){
//    	camera.end();
//    }
    
	
}

// draw any debug stuff here
void CloudsVisualSystemTunnelDrawing::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemTunnelDrawing::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
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

void CloudsVisualSystemTunnelDrawing::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemTunnelDrawing::selfMouseMoved(ofMouseEventArgs& data){
    
    
//    if(usecamera){
//        float rotateAmount = ofMap(data.x, 0, data.y, 0, 360);
//        ofVec3f furthestPoint;
//        if (points.size() > 0) {
//            furthestPoint = points[0];
//        }
//        else
//        {
//            furthestPoint = ofVec3f(data.x, data.y, 0);
//        }
//        
//        ofVec3f directionToFurthestPoint = (furthestPoint - center);
//        ofVec3f directionToFurthestPointRotated = directionToFurthestPoint.rotated(rotateAmount, ofVec3f(0,1,0));
//        camera.setPosition(center + directionToFurthestPointRotated);
//        camera.lookAt(center);
//    }
	//otherwise add points like before
//    else{
	
	ofVec3f mousePoint = camera.screenToWorld( ofVec3f(GetCloudsInputX(),
													   GetCloudsInputY(),
													   screenSpaceProjectDistance) );
	points.push_back(mousePoint);
//    }
    	
}

void CloudsVisualSystemTunnelDrawing::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemTunnelDrawing::selfMouseReleased(ofMouseEventArgs& data){
	
}