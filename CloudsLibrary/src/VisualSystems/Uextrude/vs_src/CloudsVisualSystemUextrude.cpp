//
//  CloudsVisualSystemUextrude.cpp
//

#include "CloudsVisualSystemUextrude.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemUextrude::selfSetupGui(){


}

void CloudsVisualSystemUextrude::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemUextrude::selfSetupSystemGui(){
	
}

void CloudsVisualSystemUextrude::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemUextrude::selfSetupRenderGui(){

}

void CloudsVisualSystemUextrude::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemUextrude::selfSetup(){
			
    
    //load(getVisualSystemDataPath()+"shaders/city");
	
    //someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
    
    glEnable(GL_DEPTH_TEST);
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	//initialize the video grabber
	vidGrabber.initGrabber(640,480);
	
	//this determines how much we push the meshes out
	extrusionAmount = 500.0;
    
	//store the width and height for convenience
	int width = vidGrabber.getWidth();
	int height = vidGrabber.getHeight();
	
	//taken right from the MeshDistort example
	//except we add texture coords now
	for (int y = 0; y < height; y++){
		for (int x = 0; x<width; x++){
			mainMesh.addVertex(ofPoint(x,y,0));	// mesh index = x + y*width
			mainMesh.addTexCoord(ofVec2f(x,y)); // lock each vertex to the right texture coordinate
		}
	}
	
	for (int y = 0; y<height-1; y++){
		for (int x=0; x<width-1; x++){
			mainMesh.addIndex(x+y*width);				// 0
			mainMesh.addIndex((x+1)+y*width);			// 1
			mainMesh.addIndex(x+(y+1)*width);			// 10
			
			mainMesh.addIndex((x+1)+y*width);			// 1
			mainMesh.addIndex((x+1)+(y+1)*width);		// 11
			mainMesh.addIndex(x+(y+1)*width);			// 10
		}
	}
	
	extrudeShader.load(getVisualSystemDataPath() + "shaders/meshExtrude");
    
	
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemUextrude::selfPresetLoaded(string presetPath){
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemUextrude::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemUextrude::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemUextrude::selfUpdate(){
    
    vidGrabber.update();

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemUextrude::selfDraw(){
	
    extrudeShader.begin();
	extrudeShader.setUniformTexture("image", vidGrabber, 0);
	extrudeShader.setUniform1f("extrusionAmount", extrusionAmount);
	
	ofPushMatrix();
	ofTranslate(ofGetWidth()/2- vidGrabber.getWidth()/2,
				ofGetHeight()/2-vidGrabber.getHeight()/2);
	
	mainMesh.drawFaces();
    
	ofPopMatrix();
	
	extrudeShader.end();
	
	
}

// draw any debug stuff here
void CloudsVisualSystemUextrude::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemUextrude::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemUextrude::selfEnd(){
	
    
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemUextrude::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemUextrude::selfKeyPressed(int key){
    
    if(key == 'R'){
		extrudeShader.load("meshExtrude");
	}
	
}
void CloudsVisualSystemUextrude::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemUextrude::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemUextrude::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemUextrude::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemUextrude::selfMouseReleased(ofMouseEventArgs& data){
	
}