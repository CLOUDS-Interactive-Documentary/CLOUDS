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

class FireworkRocket;
class FireworkRocket{
public:
	FireworkRocket(){
		vertices = NULL;
		bStarted = bEnded = false;
	}
	~FireworkRocket(){};
	
	void setup( float _startTime, float _span, ofVec3f _startPos, ofVec3f _endPos, ofVbo* v )
	{
		startTime = _startTime;
		startPos = _startPos;
		span = _span;
		endPos = _endPos;
		endTime = startTime + span;
		
		bStarted = bEnded = false;
		
		vertices = v;
	}
	
	void update( float elapsedTime = ofGetElapsedTimef() )
	{
		if(!bStarted && elapsedTime >= startTime)
		{
			bStarted = true;
		}
		else if(!bEnded && elapsedTime >= endTime )
		{
			bEnded = true;
		}
		
		age = ofMap( elapsedTime, startTime, endTime, 0, 1, true );
		

	}
	
	void draw()
	{
		ofSetLineWidth( 3  );
		float a = ofClamp( ofMap( age, 0, 1., 0, 2), 0, 1 );
		float b = ofClamp( ofMap( age, 0, 1, -1, 1 ), 0, 1);
		

		ofLine(startPos * (1.-b) + endPos * b, endPos * a + startPos * (1.-a) );
	}
	
	
	void 	  operator=( const FireworkRocket r)
	{
		startTime = r.startTime;
		endTime = r.endTime;
		age = r.age;
		span = r.span;
		
		startPos = r.startPos;
		cv = r.cv;
		endPos = r.endPos;
	};
	
	ofVec3f startPos, endPos, cv;
	float startTime, endTime, span, age;
	bool bStarted, bEnded;
	
	ofVbo* vertices;
};

class FireworkEmitter;
class FireworkEmitter{
public:
	FireworkEmitter(){
		span = 1;
		startTime = ofGetElapsedTimef();
		endTime = startTime + span;
		bClamp = true;
		bEnded = false;
	};
	
	FireworkEmitter(float _span)
	{
		span = _span;
		startTime = ofGetElapsedTimef();
		endTime = startTime + span;
		bClamp = true;
		bEnded = false;
	};
	~FireworkEmitter(){};
	
	void setup( float _startTime, float _span, ofVec3f _startPos, ofVec3f _endPos )
	{
		startTime = _startTime;
		span = _span;
		endTime = startTime + span;
		bStarted = bEnded = false;
		
		startPos = _startPos;
		endPos = _endPos;
	}
	
	void update(float t = ofGetElapsedTimef())
	{
		age = ofxTween::map( t, startTime, endTime, 0, 1, true, ease, ofxTween::easeOut );
		lastPos = pos;
		pos = startPos * (1.-age) + endPos*age;
		
		if(lastPos != pos){
			vel = lastPos - pos;
		}
		
		if(!bStarted && t >= startTime)
		{
			bStarted = true;
		}
		
		else if( t>= endTime)
		{
			bEnded = true;
		}
	}
	
	void 	  operator=( const FireworkEmitter e)
	{
		startTime = e.startTime;
		endTime = e.endTime;
		age = e.age;
		span = e.span;
		
		startPos = e.startPos;
		endPos = e.endPos;
		lastPos = e.lastPos;
		pos = e.pos;
		vel = e.vel;
		origin = e.origin;
		q = e.q;
	};
	
	float startTime, endTime, age, span;
	bool bClamp;
	ofVec3f startPos, endPos, lastPos, pos, vel, origin;
	ofQuaternion q;
	
	bool bEnded, bStarted;
	ofxEasingSine ease;
};

//TODO: rename this to your own visual system
class CloudsVisualSystemFireworks : public CloudsVisualSystem {
public:
    
	
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "Fireworks";
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
	
	// use this to draw the point cloud
	void selfDrawRGBD();
	
    // draw any debug stuff here
	void selfDrawDebug();
	
	//draws through RGBD camera;
	//	void selfDrawRGBD();
	
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
	
	void loadFileToGeometry( string loc, vector<ofVec3f>& points );

	
//	ofCamera& getCameraRef(){
//		//		return cloudsCamera;
//		return camera;
//	}
	
	
	void updateVbo();
	void explodeFireWork( ofVec3f origin=ofVec3f(), ofVec3f vel=ofVec3f() );
	void emitFromPoint( ofVec3f point, ofVec3f dir, float lifespan=ofRandom(1.,3.), float t=ofGetElapsedTimef() );
	
	void trailPoint( ofVec3f point, ofVec3f vel = ofVec3f(), int count = 10 );
	
	//camera
//	ofEasyCam camera;
	ofCamera camera;
	ofVec3f camTargetDelta;
	
	vector<ofVec3f>	dodecagedronPoints;
	vector<ofVec3f>	icosahedronPoints;
	vector<ofVec3f>	octahedronPoints;
	vector<ofVec3f>	tetrahedronPoints;
	
	ofShader shader;
	ofVbo vbo;
	ofVec3f camPos;
	ofVec3f camTarget;
	ofVec3f camAim, camVel;
	
	//particles
	ofVec3f* positions;
	ofVec3f* velocities;
	ofVec3f* baseVelocities;
	ofFloatColor* lifeData;
	ofIndexType* indices;
	int indexCount, nextIndex;
	float minExplosionTime, maxExplosionTime;
	
	bool bUpdateVbo;
	ofVec3f fireworkGravity, particleGravity;
	float minVel, maxVel;
	float maxFWVel;
	
	//firework explosion stuff
	vector<ofVec3f> spawnPos;
	vector<ofVec3f> spawnVel;
	vector<float> spawnTime;
	vector <float> startTimes;
	float fireWorkExplosionTime;
	
	//render attributes
	ofVec4f startColor, endColor;
	
	ofxUISuperCanvas* fireworksRenderGui;
	ofxUISuperCanvas* fireworksBehaviorGui;
	
	float camSpeed;
	
	ofImage spriteImage;
	
	float nextFireworkExplosionTime;
	void explodeFireWorkAtRandom();
	void explodeFireWorkAtPoint(ofVec3f point, float t=ofGetElapsedTimef() );
	
	void explodeGeometry( vector<ofVec3f>& vertices, ofVec3f offset, ofVec3f rocketStart );
	void dodecahedronExplostion( ofVec3f offset );
	
	ofVec3f nextExplosion;
	
	int numSprites;
	
	vector< FireworkEmitter > emitters;
	int emitterCount;
	ofVec3f emitterCentroid;
	
	ofFbo glowFbo0;
	ofFbo glowFbo1;
	ofFbo glowFbo2;
	ofFbo glowFbo3;
	ofFbo glowFbo4;
	ofFbo glowFbo5;
	ofFbo glowFbo6;
	ofShader glowShader;
	
	vector<FireworkRocket> rockets;
	int rocketCount;
	
	ofVbo curvePoints;
	
protected:
	
	
	ofxUISuperCanvas* camGui;
	
	ofxUISuperCanvas* customGui;
	bool customToggle;
	float customFloat1;
	float customFloat2;
	
	float farClip, nearClip;
	
	ofImage someImage;
	ofShader pointcloudShader;
	ofVboMesh simplePointcloud;
	
	int FIREWORKS_NUM_PARTICLES;
	
//	map<string, ofImage> sprites;
	ofImage triangleImage;
	ofImage squareImage;
	ofImage circleImage;
	
	float minLifeSpan, maxLifeSpan;
	ofImage colorSampleImage;
	ofxUIImageSampler* startColorSampler;
	ofxUIImageSampler* endColorSampler;
	
	vector <ofVec3f> debugSpheres;
	
	bool bAnimateCamera;
};
