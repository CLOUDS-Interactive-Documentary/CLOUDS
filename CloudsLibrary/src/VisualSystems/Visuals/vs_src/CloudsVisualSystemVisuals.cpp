//
//  CloudsVisualSystemVisuals.cpp
//

#include "CloudsVisualSystemVisuals.h"

//This is called whenever a new preset is loaded, before selfSetup()
//use it to ensure all your simple variables are initialized to an
//acceptable default state
void CloudsVisualSystemVisuals::selfSetDefaults(){
    cylinderCellsWide = 5;
    cylinderCellsTall = 5;
    cylRadius = 0;
    rowHeight = 0;
    imageScale = 1.0;
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemVisuals::selfSetupGui(){

	cylinderGui = new ofxUISuperCanvas("CYLINDER", gui);
	cylinderGui->copyCanvasStyle(gui);
	cylinderGui->copyCanvasProperties(gui);
	cylinderGui->setName("Custom");
	cylinderGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

	cylinderGui->addIntSlider("CELLS WIDE", 1, 15, &cylinderCellsWide);
	cylinderGui->addIntSlider("CELLS TALL", 1, 15, &cylinderCellsTall);
	cylinderGui->addSlider("RADIUS", 0, 500, &cylRadius);
	cylinderGui->addSlider("ROW HEIGHT", 0, 500, &rowHeight);
	cylinderGui->addSlider("IMAGE SCALE", 0, 1.0, &imageScale);

	ofAddListener(cylinderGui->newGUIEvent, this, &CloudsVisualSystemVisuals::selfGuiEvent);
	guis.push_back(cylinderGui);
	guimap[cylinderGui->getName()] = cylinderGui;
}

void CloudsVisualSystemVisuals::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
    layoutThumbnails();
//	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemVisuals::selfSetupSystemGui(){
	
}

void CloudsVisualSystemVisuals::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemVisuals::selfSetupRenderGui(){

}

void CloudsVisualSystemVisuals::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemVisuals::selfSetup(){
    ofBuffer systemsFiles = ofBufferFromFile(getVisualSystemDataPath() + "visualslist.txt");
    while(!systemsFiles.isLastLine()){
        string systemName = systemsFiles.getNextLine();
        string imageFileName = getVisualSystemDataPath() + "thumbs/" + systemName + ".png";
        if(ofFile(imageFileName).exists()){
            thumbs[systemName].image.loadImage(imageFileName);
        }
        else{
            ofLogError("CloudsVisualSystemVisuals::selfSetup") << "Missing thumbnail for " << imageFileName;
        }
    }
    
    layoutThumbnails();
}

void CloudsVisualSystemVisuals::layoutThumbnails(){
    int curInt = 0;
    map<string, VisualThumb>::iterator it;
    float totalheight = (thumbs.size() / cylinderCellsTall) * rowHeight;
    for(it = thumbs.begin(); it != thumbs.end(); it++){
        int x = curInt % cylinderCellsWide;
        int y = curInt / cylinderCellsWide;
        float arcPoint = ofMap(x, 0, cylinderCellsWide, 0, 360);
        
        ofVec3f pos(0,y*rowHeight - totalheight/2, cylRadius);
        pos.rotate(arcPoint, ofVec3f(0,1,0));
        ofVec3f centerPos = pos;
        centerPos.z = centerPos.x = 0;
        ofNode n;
        
        n.lookAt(centerPos);
        it->second.mesh.clear();
        it->second.pos = pos;
        it->second.rotation = arcPoint;
        
        curInt++;
    }
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemVisuals::selfPresetLoaded(string presetPath){
	layoutThumbnails();
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemVisuals::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemVisuals::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemVisuals::selfUpdate(){

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemVisuals::selfDraw(){
    map<string, VisualThumb>::iterator it;
    for(it = thumbs.begin(); it != thumbs.end(); it++){
//        ofPushMatrix();
//        ofRotate(it->second.rotation, 0, 1, 0);
//        ofTranslate(it->second.pos);
//        it->second.image.draw(-1920*imageScale*.5,
//                              -1080*imageScale*.5,
//                              1920*imageScale,
//                              1080*imageScale);
//        ofSetColor(255);
//        ofDrawSphere( 0, 0, 0, 20);
//        ofPopMatrix();
    }
}

// draw any debug stuff here
void CloudsVisualSystemVisuals::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemVisuals::selfDrawBackground(){

	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemVisuals::selfEnd(){	
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemVisuals::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemVisuals::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemVisuals::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemVisuals::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemVisuals::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemVisuals::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemVisuals::selfMouseReleased(ofMouseEventArgs& data){
	
}