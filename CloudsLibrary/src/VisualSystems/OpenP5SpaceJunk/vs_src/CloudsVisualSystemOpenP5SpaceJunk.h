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


class Cube {
  public:
	int w;
	int h;
	int d;
	int shiftX;
    int shiftY;
    int shiftZ;
    int rotX;
    int rotY;
    int rotZ;

	
	Cube(ofVboMesh& mesh, ofMesh& baseBox,
		 ofVboMesh& stroke, ofMesh& baseStroke,
		 int _w, int _h, int _d,
		 int _shiftX, int _shiftY, int _shiftZ,
		 int _rotX, int _rotY, int _rotZ ){
		
        w = _w;
        h = _h;
        d = _d;
        shiftX = _shiftX;
        shiftY = _shiftY;
        shiftZ = _shiftZ;
        rotX = _rotX;
        rotY = _rotY;
        rotZ = _rotZ;

		ofMatrix4x4 transform;
		ofMatrix4x4 rotate;
		transform.scale(w,h,d);
		rotate.rotate(rotX,1,0,0);
		rotate.rotate(rotY,0,1,0);
		rotate.rotate(rotZ,0,0,1);
		transform *= rotate;
		transform.translate(shiftX, shiftY, shiftZ);

		ofIndexType initialIndex = mesh.getNumVertices();
		for(int i = 0; i < baseBox.getVertices().size(); i++){
			mesh.addVertex(baseBox.getVertices()[i] * transform );
			mesh.addNormal(baseBox.getNormals()[i] * rotate);
		}
		for(int i = 0; i < baseBox.getNumIndices(); i++){
			mesh.addIndex(initialIndex + baseBox.getIndices()[i] );
		}
		
		for(int i = 0; i < baseStroke.getNumVertices(); i++){
			stroke.addVertex(baseStroke.getVertex(i) * transform);
		}
	}
	
};

//TODO: rename this to your own visual system
class CloudsVisualSystemOpenP5SpaceJunk : public CloudsVisualSystem {
  public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "OpenP5SpaceJunk";
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

	void selfSetDefaults();
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
	

protected:
    
    //  Your Stuff
    //
	
	ofxUISuperCanvas* customGui;
	
	ofFloatColor color1HSB;
	ofFloatColor color2HSB;
	
	ofVboMesh mesh;
	ofVboMesh stroke;
	ofMesh baseBox;
	ofMesh baseStroke;
	
	int limit;
	int spread;
    float ang;
    float speed_ = 10; //a number that increments ang
    
	vector<Cube> list;
    
    float XrandMin;
    float XrandMax;
    float YrandMin;
    float YrandMax;
    float ZrandMin;
    float ZrandMax;
    
    float XrotMin;
    float XrotMax;
    float YrotMin;
    float YrotMax;
    float ZrotMin;
    float ZrotMax;
    
    bool shouldRegenerate; 
		

};
