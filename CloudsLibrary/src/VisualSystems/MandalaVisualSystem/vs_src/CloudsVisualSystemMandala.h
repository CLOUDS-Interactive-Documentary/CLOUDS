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
#include "ofxSimpleSurface.h"
#include "ofxObjLoader.h"
#include "ofxTween.h"
#include "MandalaTicker.h"

class MandalaSurfaceShape : public ofNode{
public:
	MandalaSurfaceShape( ofxSimpleSurface* _surface = NULL, ofVboMesh* _m=NULL)
	{
		setSurface( _surface );
		setMesh( _m );
		
		offset = 0.;
		
		scl.set(1,1,1);
	}
	~MandalaSurfaceShape(){}
	
	void setSurface( ofxSimpleSurface* _surface = NULL )
	{
		surface = _surface;
	}
	void setMesh( ofVboMesh* _m=NULL )
	{
		m = _m;
		
		color.set( ofRandom(2.), ofRandom(2.), ofRandom(2.) );
		scl.set(ofRandom(30,300), ofRandom(30,300),ofRandom(30,300));
	}
	void updateTransform()
	{
		if(surface != NULL)
		{
			//get surface info
//			surface->getMeshPositionAndNormal( pOnS, nOnS, uv.x, uv.y );
			surface->getSurfacePositionAndNormal( pOnS, nOnS, uv.x, uv.y );
			
			//set up our transform
			resetTransform();
			
			//position & rotate to surface
			setPosition(pOnS + nOnS * offset );
			lookAt( getPosition() + nOnS * 10. );
			
			//TODO: local rotation
//			q.makeRotate( getUpDir(), nOnS);
//			setOrientation(q);
			
			setScale( getScale() );
		}
	}
	
	void draw()
	{
		ofSetColor( color );
		updateTransform();
		
		transformGL();
		if(m!=NULL)	m->draw();
		restoreTransformGL();
	}
	
	//private:
	ofxSimpleSurface* surface;
	ofVboMesh* m;
	ofVec2f uv;
	
	ofQuaternion q;
	ofVec3f pOnS, nOnS, scl;
	float offset;
	
	ofFloatColor color;
};


class MandalaNodule : public ofNode{
public:
	MandalaNodule(){
		//helo nodual
		mesh = NULL;
	}
	~MandalaNodule(){
		//bye nodual
		m.clear();
	}
	
	void setLoopBoundry(int x=0, int y=0, int w=ofGetWidth(), int h=ofGetHeight())
	{
		loopBoundry.set(x, y, w, h);
	}
		
	//append another mesh to our mesh
	void addMesh( ofVboMesh& inMesh, ofFloatColor color=ofFloatColor(1,1,1), ofVec3f offset=ofVec3f(), ofVec3f scale=ofVec3f(1,1,1))
	{
		vector<ofVec3f> v = inMesh.getVertices();
		vector<ofVec3f>& n = inMesh.getNormals();
		vector<ofVec2f>& tc = inMesh.getTexCoords();
		vector<ofIndexType> indices = inMesh.getIndices();
		vector<ofFloatColor> c(v.size());
		
		//add our vertices
		int startIndex = m.getNumVertices();
		for (int i=0; i<v.size(); i++)
		{
			//we are hard positioning and scaling here... can't be undone.
			//so, for animation it's best to use the ofNode methods for transforming as it's more flexible
			v[i] *= scale;
			v[i] += offset;
			c[i] = color;
		}
		
		for (int i=0; i<indices.size(); i++)
		{
			indices[i] += startIndex;
		}
		
		m.addVertices( v );
		if(n.size())	m.addNormals( n );
		if(tc.size())	m.addTexCoords( tc );
		m.addColors( c );
		m.addIndices( indices );
		
		mesh = &m;
	}
	
	void setMesh( ofVboMesh& _m)
	{
		mesh = &_m;
	}
	
	
	
	void updatePosition()
	{
		ofVec3f pos = getPosition();
		
		if( loopBoundry.inside( pos ))
		{
//			cout<< "out of bounds" << endl;
			if(pos.x < loopBoundry.getLeft())
			{
				setPosition( pos.x + loopBoundry.getWidth(), pos.y, pos.z);
			}
			else
			{
				setPosition( pos.x - loopBoundry.getWidth(), pos.y, pos.z);
			}
		}
	}
	
	void drawMesh( bool bFill = true, bool bEdges=false, bool bPoints=false )
	{
		if(mesh != NULL)
		{
			if(bFill)	mesh->draw();
			if(bEdges)	mesh->drawWireframe();
			if(bPoints)	mesh->drawVertices();
		}
	}
	
	void draw( bool bFill = true, bool bEdges=false, bool bPoints=false )
	{
		ofSetColor( offsetColor );
		ofPushMatrix();
		ofMultMatrix(getGlobalTransformMatrix());
		
		drawMesh(bFill,bEdges,bPoints);
		
		ofPopMatrix();
		
		
		float w = loopBoundry.getWidth();
		
		ofVec3f pos = getPosition();
		ofVec3f scl = getScale();
		if(pos.x - scl.x <= loopBoundry.getLeft())
		{
			ofPushMatrix();
			ofTranslate(loopBoundry.getWidth(), 0,0);
			ofMultMatrix(getGlobalTransformMatrix());
			
			drawMesh(bFill,bEdges,bPoints);
			
			ofPopMatrix();
			
//			if(pos.x < loopBoundry.getLeft())
//			{
//				setPosition(pos.x + w, pos.y, pos.z);
//			}
		}

		else if( pos.x + scl.x >= loopBoundry.getRight())
		{
			if(pos.x < loopBoundry.getLeft())
			{
				setPosition(pos.x + w, pos.y, pos.z);
			}
			ofPushMatrix();
			ofTranslate(-loopBoundry.getWidth(), 0,0);
			ofMultMatrix(getGlobalTransformMatrix());
			
			drawMesh(bFill,bEdges,bPoints);
			
			ofPopMatrix();
			
//			if(pos.x > loopBoundry.getRight())
//			{
//				setPosition(pos.x - w, pos.y, pos.z);
//			}
		}
//		ofSetColor( 255 );
	}
	
	ofVboMesh& getMesh()
	{
		return m;
	}
	
	ofVboMesh m;
	ofVboMesh* mesh;
	
	ofVec3f animationStartTime, animationEndTime;
	ofVec3f targetPos, targetScl, targetRot;
	
	bool bMoving, bScaling, bRotating;
	
	ofFloatColor offsetColor;
	
	ofRectangle loopBoundry;
	
};

//TODO: rename this to your own visual system
class CloudsVisualSystemMandala : public CloudsVisualSystem {
public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "MandalaVisualSystem";
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
	
	
    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
//	ofCamera& getCameraRef(){
//		return camera;
//	}
	
	//
	//	ofCamera& getCameraRef(){
	//		return cloudsCamera;
	//	}
	
	
protected:
	
	
	void drawSurface(ofxSimpleSurface& surface );
	void drawShapesToFbo(float t=ofGetElapsedTimef());
	void setupNodules( ofVboMesh& m, int numW = 11, int numH = 5);
	
	template <class T>
	void addTicker( MandalaTicker<T> ticker ){
		//we need to specialize this per data type
		cout << "we don't handle this data type yet" << endl;
	}
	void addTicker( MandalaTicker<float> ticker );
	void addTicker( MandalaTicker<ofVec2f> ticker );
	void addTicker( MandalaTicker<ofVec3f> ticker );
	void addTicker( MandalaTicker<ofVec4f> ticker );
	void updateTickers( float t=ofGetElapsedTimef());
	
	MandalaSurfaceShape* addSurfaceShape( ofxSimpleSurface* s, ofVboMesh* m );
	
	vector<ofVboMesh*> meshes;
	
	ofxUISuperCanvas* customGui;
	ofxUISuperCanvas* shapesGui;
	ofxUISuperCanvas* surfaceGui;
	
	ofImage colorMap;
	
	float controlSplinesLineWidth;
	vector < vector< ofVec3f > > cv;
	ofxSimpleSurface surface;
	
	ofShader normalShader;
	ofShader facingRatio;
	ofShader surfaceShader;
	float polarAlphaExpo, polarAlphaExpoScale;
	
	void loadShaders();
	
//	ofEasyCam camera;
	
	
	ofNode sweeper;
	vector<ofVec3f> profileVertices;
	int numSpans;
	
	ofxSimpleSpline profileCurve;
	ofxSimpleSpline waveSpline;
	
	ofImage debugImage;
	
	ofFbo animatedMap;
	
	ofVboMesh square, circle, triangle, dodecahedron, box;

	
	ofxSimpleSpline* blendCurve0;
	ofxSimpleSpline* blendCurve1;
	
	
	
	vector <MandalaNodule> nodules;
	bool bDrawFill, bDrawEdges, bDrawPoints;
	float noduleLineWidth;
	
	bool bDrawSurfaceWirframe, bDrawSurfaceSplines;
	
	bool bMirrorSurface, bSmoothSurface;
	
	ofShader noduleShader;
	float cutoff, shapeWidth;
	
	ofBlendMode noduleBlendType, surfaceBlendMode;
	
	float controlSplineOffset;
	
	float alphaSmoothScale, alphaSmoothExpo;
	float waveScale;
		
	float noiseTimeScale, noiseScale, noiseOffset, noiseTime;
	float blinkyness, blinkynessScale, blinkynessTimeScale;
	
//	ofEasyCam orthoCamera;
	
	float noduleRotation;
	float numW, numH;
	float shapesScale;
	ofVec2f noduleSize;
	ofVboMesh* currentShape;
	
	ofTexture* currentTexture;
	bool bUseRandomColors;
	
	string profileType;
	vector<string> profileTypeNames;
	
	ofVec3f tickDebug;
	MandalaTicker<ofVec3f> posTicker;
	
	ofNode surfaceBox;
	ofVec2f surfaceBoxUV;
	
	MandalaTicker<ofVec2f> uvTicker;
	
//	vector< vector< MandalaTicker<ofVec2f> > > uvTickers;
	
	vector<MandalaSurfaceShape*> surfaceShapes;
	vector< MandalaTicker<ofVec2f> > uvTickers;
	
	vector< MandalaTicker<float> > tickersf;
	vector< MandalaTicker<ofVec2f> > tickers2f;
	vector< MandalaTicker<ofVec3f> > tickers3f;
	vector< MandalaTicker<ofVec4f> > tickers4f;
	
	bool bDrawSurface;
};
