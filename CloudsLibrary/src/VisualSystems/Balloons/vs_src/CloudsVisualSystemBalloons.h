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
#include "ofxObjLoader.h"

class Credit
{
public:
	Credit(string _title, string _name, ofVec3f _pos) :
	name( _name ),
	title( _title ),
	pos(_pos)
	{
		setup();
	}
	
	~Credit()
	{
		
	}
	
	void setup()
	{
		getTextWidth();
	}
	
	void getTextWidth()
	{
		//figure out the text width here
		
		width = 75;//temp
	}
	
	ofVec3f getLeft()
	{
		return ofVec3f(pos.x - width*.5, pos.y, pos.z);
	}
	
	void draw()
	{
		//draw text at the position
		glDisable(GL_CULL_FACE);
		ofSetColor(255,255,255);
//		ofDrawBitmapString("YO DAWG, I PUT SOME WORDS IN YOUR BALLOONS", pos.x - width*.5, pos.y + );
//		ofDrawBitmapString("Dr. Van Nostron", pos.x - width*.5, pos.y - 6);
		
		ofSetColor(255, 255, 0);
		glLineWidth(2);
		ofLine(pos.x - width*.5, pos.y, pos.z, pos.x + width*.5, pos.y, pos.z);
	}
	
	string title;
	string name;
	
	ofVec3f pos;
	float width;
};

//TODO: rename this to your own visual system
class CloudsVisualSystemBalloons : public CloudsVisualSystem {
  public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName()
	{
		return "Balloons";
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

	//This is called whenever a new preset is loaded, before selfSetup()
	//use it to ensure all your simple variables are initialized to an
	//acceptable default state
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
	
	ofVec3f randomPointInSphere(float rad, ofVec3f center = ofVec3f(0,0,0))
	{
		ofVec3f p;
		p.x = ofRandom(-rad, rad);
		p.y = ofRandom(-rad, rad);
		p.z = ofRandom(-rad, rad);
		
		while(p.lengthSquared() > rad*rad)
		{
			p.x = ofRandom(-rad, rad);
			p.y = ofRandom(-rad, rad);
			p.z = ofRandom(-rad, rad);
		}
		
		return p + center;
	}
	
	void setBalloonPositions();
	void setBalloonColors();
	
	template<class T>
	T mix( T x, T y, float u)
	{
		return x * (1.f - u) + y * u;
	}
	
	
	float DistancePointLine( ofVec3f& p3, ofVec3f& p1, ofVec3f& p2 )
    {
        ofVec3f intersection = IntersectionPointLine( p3, p1, p2 );

		return p3.distance(intersection);
    }
	
    ofVec3f IntersectionPointLine( ofVec3f& p3, ofVec3f& p1, ofVec3f& p2 )
    {
		ofVec3f intersection;
        ofVec3f diff = p3 - p1;
        ofVec3f dir = p2 - p1;
		
        float u = diff.dot( dir ) / dir.dot( dir );
		
		if ( u < 0.0f )	return p1;
		else if( u > 1.0f )	return p2;
		
		intersection = p1 + dir * u;
		return intersection;
    }
	
	
	ofCamera& getCameraRef(){
		return cloudsCamera;
	}
	
protected:
    
    //  Your Stuff
    //
	
	ofxUISuperCanvas* customGui;
	ofxUISuperCanvas* colorGui;
	ofxUISuperCanvas* balloonReleaseGui;
	ofxUISuperCanvas* textGui;
	
	int releaseType;

	
	ofVbo vbo;
	int total;
	float dim;
	float netHeight;
	
	ofShader shader;
	ofShader posShader;
	ofShader velShader;
	ofShader quatShader;
	float noiseScl, offset, noiseSampleScale, velAtten, radius, accScl, gravity, attractionToCenter, cameraBounce,cameraAttractionToCenter, cameraOffset;
	float spawnRad, cameraTargetDist;
	float highSpeedScale, speedLow, speedHi, highSpeedPercent;
	
	float balloonFrameVal;
	
	bool bReleased;
	
	int dimY, dimX;
	ofFbo posFbo0;
	ofFbo posFbo1;
	ofFbo velFbo0;
	ofFbo velFbo1;
	ofFbo colFbo;
	ofFbo quatFbo;
	
	ofColor c0, c1, c2, c3;
	float w0, w1, w2, w3;
	
	int c0Hue, c0Sat, c0Bri;
	int c1Hue, c1Sat, c1Bri;
	int c2Hue, c2Sat, c2Bri;
	int c3Hue, c3Sat, c3Bri;
	
	ofFbo* p0;
	ofFbo* p1;
	ofFbo* v0;
	ofFbo* v1;
	
	ofVec3f l0;
	
	ofVec3f camerVel;
	
	ofVec3f balloon00Pos;
	ofFloatPixels pospix;
	
	ofImage sphericalMap;
	
	ofEasyCam c;
	
	ofVec3f line0, line1;

	float textSpeed, textRadius, creditLightDist;
	vector<Credit> credits;
	
	int shininess;
	float lightScale, facingRatioScale, creditLightScale;
};
