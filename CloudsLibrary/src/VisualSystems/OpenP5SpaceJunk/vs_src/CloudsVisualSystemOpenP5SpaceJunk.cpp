//
//  CloudsVisualSystemOpenP5SpaceJunk.cpp
//

#include "CloudsVisualSystemOpenP5SpaceJunk.h"
#include "ofxObjLoader.h"


//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemOpenP5SpaceJunk::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    customGui->addButton("REGENERATE", &shouldRegenerate);
	
    customGui->addSlider("Rotation Speed", 0, 25, &speed_);
	customGui->addRangeSlider("X Scale", 0, 50, &XrandMin, &XrandMax);
    customGui->addRangeSlider("Y Scale", 0, 50, &YrandMin, &YrandMax);
    customGui->addRangeSlider("Z Scale", 0, 50, &ZrandMin, &ZrandMax);
    customGui->addRangeSlider("X Rotate", 0, 90, &XrotMin, &XrotMax);
    customGui->addRangeSlider("Y Rotate", 0, 90, &YrotMin, &YrotMax);
    customGui->addRangeSlider("Z Rotate", 0, 90, &ZrotMin, &ZrotMax);

	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemOpenP5SpaceJunk::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemOpenP5SpaceJunk::selfGuiEvent(ofxUIEventArgs &e){
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
	
    if(e.widget->getName() == "REGENERATE" && ((ofxUIButton*)e.widget)->getValue() ){
		shouldRegenerate = true;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemOpenP5SpaceJunk::selfSetupSystemGui(){
	
}

void CloudsVisualSystemOpenP5SpaceJunk::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemOpenP5SpaceJunk::selfSetupRenderGui(){

}

void CloudsVisualSystemOpenP5SpaceJunk::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemOpenP5SpaceJunk::selfSetup(){
    ang = 0;
    shouldRegenerate = true;
	ofMesh combinedBox;

	ofxObjLoader::load(getVisualSystemDataPath() + "geometry/box.obj", combinedBox);
	
	for(int i = 0; i < combinedBox.getNumIndices(); i+=3){
		ofIndexType indxA = combinedBox.getIndices()[i+0];
		ofIndexType indxB = combinedBox.getIndices()[i+1];
		ofIndexType indxC = combinedBox.getIndices()[i+2];
		
		ofVec3f& vA = combinedBox.getVertices()[indxA];
		ofVec3f& vB = combinedBox.getVertices()[indxB];
		ofVec3f& vC = combinedBox.getVertices()[indxC];
		baseBox.addVertex(vA);
		baseBox.addVertex(vB);
		baseBox.addVertex(vC);
		
		ofVec3f normal = (vB - vA).getCrossed(vC - vA).normalized();
		baseBox.addNormal(normal);
		baseBox.addNormal(normal);
		baseBox.addNormal(normal);
		
		baseBox.addIndex(i+0);
		baseBox.addIndex(i+1);
		baseBox.addIndex(i+2);
		
	}
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemOpenP5SpaceJunk::selfPresetLoaded(string presetPath){
	shouldRegenerate = true;
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemOpenP5SpaceJunk::selfBegin(){
	shouldRegenerate = true;
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemOpenP5SpaceJunk::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemOpenP5SpaceJunk::selfUpdate(){
	if(shouldRegenerate){
		shouldRegenerate = false;
		list.clear();
		mesh.clear();
		
		cout << "REGENERATING" << endl;
		
		for (int i = 0; i < limit; i++) {

			if(ofRandomuf() > .8){
				list.push_back( Cube(mesh,baseBox,
									 ofRandom(XrandMin*1.5, XrandMax*1.5), //scale
									 ofRandom(YrandMin*1.5, YrandMax*1.5),
									 ofRandom(ZrandMin*1.5, ZrandMax*1.5),  // 4,20 4,20, 2,20
									 
									 ofRandom(-140, 140), //pose
									 ofRandom(-140, 140),
									 ofRandom(-140, 140),
									 
									 ofRandom(XrotMin*1.5, XrotMax*1.5), //rot
									 ofRandom(YrotMin*1.5, YrotMax*1.5),
									 ofRandom(ZrotMin*1.5, ZrotMax*1.5) ) );
				}
				else {
					list.push_back( Cube(mesh,baseBox,
										 ofRandom(XrandMin, XrandMax), //scale
										 ofRandom(YrandMin, YrandMax),
										 ofRandom(ZrandMin, ZrandMax),  // 4,20 4,20, 2,20
										 
										 ofRandom(-140, 140), //pose
										 ofRandom(-140, 140),
										 ofRandom(-140, 140),
										 
										 ofRandom(XrotMin, XrotMax), //rot
										 ofRandom(YrotMin, YrotMax),
										 ofRandom(ZrotMin, ZrotMax) ) );
			}
			
//			list.push_back( Cube(ofRandom(XrandMin, XrandMax), ofRandom(YrandMin, YrandMax), ofRandom(ZrandMin, ZrandMax),  // 4,20 4,20, 2,20
//								 ofRandom(-140, 140), ofRandom(-140, 140), ofRandom(-140, 140),
//								 ofRandom(XrotMin, XrotMax), ofRandom(YrotMin, YrotMax), ofRandom(ZrotMin, ZrotMax) ) );
		}
	}
	
	ang += (speed_ / 10000); //angular rotation speed

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemOpenP5SpaceJunk::selfDraw(){
    

    ofPushStyle();
    glEnable(GL_DEPTH_TEST);
    
	ofPushMatrix();
	ofEnableAlphaBlending();
	
	//Rotate breaks oculus overlay
    ofRotateX(ofRadToDeg(ang));     //X rotation - converts radians to degrees
    ofRotateY(ofRadToDeg(ang));     //Y rotation
	
    mat->begin();

	mesh.draw();
    mat->end(); //enables material
	
    ofPopStyle();
    ofPopMatrix();
	
    
}

// draw any debug stuff here
void CloudsVisualSystemOpenP5SpaceJunk::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemOpenP5SpaceJunk::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemOpenP5SpaceJunk::selfEnd(){

	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemOpenP5SpaceJunk::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemOpenP5SpaceJunk::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemOpenP5SpaceJunk::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemOpenP5SpaceJunk::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SpaceJunk::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SpaceJunk::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemOpenP5SpaceJunk::selfMouseReleased(ofMouseEventArgs& data){
	
}