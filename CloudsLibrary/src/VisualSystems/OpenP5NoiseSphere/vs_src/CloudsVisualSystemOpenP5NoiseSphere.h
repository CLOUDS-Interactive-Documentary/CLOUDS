//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
// Welcome to the EMPTY CloudsVisualSystem
//
//
//

#pragma once

#include "CloudsVisualSystem.h"

class Hair {
  public:
	float radius;
	float z;
	float phi;
	float largo;
	float theta;
	
	Hair(float radius) : radius(radius){
		z = ofRandom(-radius, radius);
		phi = ofRandom(TWO_PI);
		largo = ofRandom(1.05, 1.1);
		theta = asin(z/radius);
	}
	
	void draw(ofMesh& mesh, float noisePosition, float noiseScale, float solidSphereAlpha) {
		float off = (ofNoise(noisePosition * 0.0005, sin(phi)) - 0.5) * 0.3 * noiseScale;
		float offb = (ofNoise(noisePosition * 0.0007, sin(z) * 0.01)-0.5) * 0.3 * noiseScale;
		
		float thetaff = theta+off;
		float phff = phi+offb;
		float x = radius * cos(theta) * cos(phi);
		float y = radius * cos(theta) * sin(phi);
		float z = radius * sin(theta);
		//mouse x & y
//		float msx= screenX(x, y, z);
//		float msy= screenY(x, y, z);
		
		float xo = radius * cos(thetaff) * cos(phff);
		float yo = radius * cos(thetaff) * sin(phff);
		float zo = radius * sin(thetaff);
		
		float xb = xo * largo;
		float yb = yo * largo;
		float zb = zo * largo;

		mesh.addColor(ofFloatColor::white*solidSphereAlpha);
		mesh.addVertex( ofVec3f(x,y,z) );
		mesh.addColor(ofFloatColor::white);
		mesh.addVertex( ofVec3f(xb,yb,zb) );
		
//		beginShape(LINES);
//		stroke(0);
//		vertex(x, y, z);
//		stroke(200, 150);
//		vertex(xb, yb, zb);
//		endShape();
	}
};

//TODO: rename this to your own visual system
class CloudsVisualSystemOpenP5NoiseSphere : public CloudsVisualSystem {
  public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "OpenP5NoiseSphere";
	}

	//These methods let us add custom GUI parameters and respond to their events
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
	//Use system gui for global or logical settings, for exmpl
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
	//use render gui for display settings, like changing colors
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);

	// selfSetup is called when the visual system is first instantiated
	// This will be called during a "loading" screen, so any big images or
	// geometry should be loaded here
    void selfSetup();

	// selfBegin is called when the system is ready to be shown
	// this is a good time to prepare for transitions
	// but try to keep it light weight as to not cause stuttering
    void selfBegin();

	// selfPresetLoaded is called whenever a new preset is triggered
	// it'll be called right before selfBegin() and you may wish to
	// refresh anything that a preset may offset, such as stored colors or particles
	void selfPresetLoaded(string presetPath);
    
	//do things like ofRotate/ofTranslate here
	//any type of transformation that doesn't have to do with the camera
    void selfSceneTransformation();
	
	//normal update call
	void selfUpdate();

	// selfDraw draws in 3D using the default ofEasyCamera
	// you can change the camera by returning getCameraRef()
    void selfDraw();
		
    // draw any debug stuff here
	void selfDrawDebug();

	// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
	void selfDrawBackground();

	// this is called when your system is no longer drawing.
	// Right after this selfUpdate() and selfDraw() won't be called any more
	void selfEnd();

	// this is called when you should clear all the memory and delet anything you made in setup
    void selfExit();

	//events are called when the system is active
	//Feel free to make things interactive for you, and for the user!
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
	
	

    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
//	ofCamera& getCameraRef(){
//		return myCustomCamera;
//	}


protected:
    
    //  Your Stuff
    //
	
	ofxUISuperCanvas* customGui;
	
	ofFloatColor color1HSB;
	ofFloatColor color2HSB;
	
	int count = 4000;
	vector<Hair> list;
	float radius;
	float rx = 0;
	float ry = 0;

    float noisePosition;
	float noiseSpeed;
	float noiseScale;
	
    //inner sphere
    
    void        loadVbo(ofVboMesh &_vbo, string _file);
    
    float       wireSphereScale, solidSphereScale, haloSphereScale;
    float       wireSphereAlpha, solidSphereAlpha, haloSphereAlpha;

	

};
