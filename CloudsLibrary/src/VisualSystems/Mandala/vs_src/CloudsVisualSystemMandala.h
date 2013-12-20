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
#include "CloudsGlobal.h"
#include "ofxSimpleSurface.h"
#include "ofxObjLoader.h"
#include "ofxTween.h"
#include "MandalaTicker.h"
#include "MandalaComponent.h"
#include "SurfacePoint.h"



class SPMesh :  public ofNode{
public:
	SPMesh(){
		sp = NULL;
		bDraw = true;
	};
	~SPMesh(){};
	
	void update()
	{
		for (int i=0; i<tickers.size(); i++)
		{
			tickers[i].update();
		}
		if(sp != NULL)
		{
			sp->update();
			
			resetTransform();
			
			setPosition( sp->position + sp->normal*offset );
			
			setScale( getScale() );
			
			lookAt( getPosition() + sp->normal * 100.);
		}
	}
	
	void draw()
	{	
		if(bDraw && m!=NULL)
		{
			transformGL();
			m->draw();
			restoreTransformGL();
		}
	}
	
	
	
	ofVboMesh* m;
	bool bDraw;
	SurfacePoint* sp;
	
	ofVec3f offset;
	
	vector< MandalaTicker<float> > tickers;
};

class SPMeshTail{
public:
	SPMeshTail()
	{
		m = NULL;
		smoothing = .8;
		bIsSetup = false;
	};
	~SPMeshTail()
	{
		line.clear();
		points.clear();
	};
	
	void setup( SPMesh* _m, ofVec3f _localOffset=ofVec3f(0,0,0))
	{
		if(_m)
		{
			bIsSetup = true;
			m = _m;
			localOffset = _localOffset;
			
			points.resize(15);
			uv.resize(points.size());
			colors.resize(points.size());
			worldPos = getRootPosoition();
			float step = 1./float(points.size()-1);
			for(int i=0; i<points.size(); i++)
			{
				points[i] = worldPos;
				uv[i].set(step * i);

				colors[i].set( 1, 1, 1, 1. - step*i );
			}
			
			line.setVertexData( &points[0], points.size(), GL_DYNAMIC_DRAW );
			line.setTexCoordData( &uv[0], uv.size(), GL_DYNAMIC_DRAW );
			line.setColorData( &colors[0], colors.size(), GL_DYNAMIC_DRAW );
		}
	}
	
	void update()
	{
		if(!bIsSetup)
		{
			setup(m);
		}
		if(m)
		{
			worldPos = getRootPosoition();
			
			points[0] = worldPos;
			
			float dynamicSmoothing = smoothing * min(1.f, lastHead.distance(worldPos) / 5.f );
			
			float msmoothing = 1. - dynamicSmoothing;
			for(int i=points.size()-1; i>0; i--)
			{
				points[i] = points[i-1] * dynamicSmoothing + points[i] * msmoothing;
			}
			
			line.updateVertexData( &points[0], points.size() );
			
			lastHead = worldPos;
		}
	}
	
	void draw()
	{
		ofSetColor(255, 255, 255);
		line.draw(GL_LINE_STRIP, 0, points.size());
	}
	
	ofVec3f getRootPosoition()
	{
		if(m == NULL){
			cout << "no mesh" << endl;
			return ofVec3f(0,0,0);
		}
		
		return localOffset * m->getGlobalTransformMatrix();
	}
	
	void operator=( const SPMeshTail t)
	{
		m = t.m;
		localOffset = t.localOffset;
		worldPos = t.worldPos;
		
		points = t.points;
		uv = t.uv;
		colors = t.colors;
		
		smoothing = t.smoothing;
	};
	
	SPMesh* m;
	ofVec3f localOffset;
	ofVec3f worldPos;
	
	float smoothing;
	
	ofVbo line;
	
	vector<ofVec3f> points;
	vector<ofVec2f> uv;
	vector<ofFloatColor> colors;
	
	ofVec3f lastHead;
	
	bool bIsSetup;
};

class TailLoft{
public:
	
	TailLoft(SPMeshTail* _t1 = NULL, SPMeshTail* _t2 = NULL)
	{
		setup(_t1, _t2);
	}
	
	~TailLoft()
	{
		m.clear();
	}
	void setup (SPMeshTail* _t1 = NULL, SPMeshTail* _t2 = NULL)
	{
		t1 = _t1;
		t2 = _t2;
		
		color.setHue( (.1 * int(ofRandom(0,10)) ) * 255 );
		color.setSaturation( 200 );
		color.setBrightness( 255 );
		
		if(t1 != NULL && t2 != NULL)
		{
			//create out mesh
			m.clear();
			
			indices.clear();
			vertices.clear();
			normals.clear();
			
			vertices.resize(t1->points.size()*2);
			
			for(int i=0; i<t1->points.size(); i++)
			{
				vertices[i*2] = t1->points[i];
				vertices[i*2+1] = t2->points[i];
			}
			
			normals.resize(vertices.size());
			
			for(int i=1; i<t1->points.size(); i++)
			{
				indices.push_back( (i-1)*2 + 1 );
				indices.push_back( (i-1)*2 );
				indices.push_back( i*2 );
				
				indices.push_back( (i-1)*2 + 1 );
				indices.push_back( i*2 );
				indices.push_back( i*2 + 1 );
			}
			indexCount = indices.size();

			updateNormals();
			
			m.setVertexData( &vertices[0], vertices.size(), GL_DYNAMIC_DRAW );
			m.setNormalData( &normals[0], normals.size(), GL_DYNAMIC_DRAW );
			m.setIndexData( &indices[0], indices.size(), GL_DYNAMIC_DRAW );
		}
			
	}
	
	void updateNormals()
	{
		for(int i=0; i<normals.size(); i++)
		{
			normals[i].set(0,0,0);
		}
		
		int i0, i1, i2;
		ofVec3f n;
		for(int i=0; i<indices.size(); i+=3)
		{
			i0 = indices[i];
			i1 = i0+1;
			i2 = i0+2;
			
			n = normalFrom3Points( vertices[i0], vertices[i1], vertices[i2]);
			normals[i0] += n;
			normals[i1] += n;
			normals[i2] += n;
		}
		
		for(int i=0; i<normals.size(); i++)
		{
			normals[i].normalize();
		}
		
		m.setNormalData( &normals[0], normals.size(), GL_DYNAMIC_DRAW );
	}
	
	void update()
	{
		if(t1 != NULL && t2 != NULL){
			for(int i=0; i<t1->points.size(); i++)
			{	
				vertices[i*2] = t1->points[i];
				vertices[i*2+1] = t2->points[i];
			}
			
			m.setVertexData( &vertices[0], vertices.size(), GL_DYNAMIC_DRAW);
		}
		
		updateNormals();
	}
	
	void draw()
	{
		ofSetColor( color );
		m.drawElements(GL_TRIANGLES, indexCount );
	}
	
	
	ofVec3f normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2)
	{
		return (p2 - p1).cross( p0 - p1).normalized();
	}
	
	SPMeshTail* t1;
	SPMeshTail* t2;
	
	vector<ofVec3f> vertices;
	vector<ofVec3f> normals;
	vector<ofIndexType> indices;
	
	ofColor color;
	
	ofVbo m;
	ofIndexType indexCount;
};
class CloudsVisualSystemMandala : public CloudsVisualSystem {
public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "Mandala";
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
	
	void loadShaders();
	void drawSurface(ofxSimpleSurface& surface );
	void drawShapesToFbo(float t=ofGetElapsedTimef());
	void setupNodules( ofVboMesh& m, int numW = 11, int numH = 5);
	
	//models
	vector<ofVboMesh*> meshes;
	
	//GUI
	ofxUISuperCanvas* customGui;
	ofImage colorMap;
	
	ofxUISuperCanvas* surfaceGui;
	
	ofxUISuperCanvas* offsetMeshGui;
	
	ofxUISuperCanvas* clockGui;
	vector<ofxUISuperCanvas*> componentSubGuis;
	
	
	//SHADERS
	ofShader normalShader;
	ofShader facingRatio;
	ofShader surfaceShader;
	ofShader ribbonShader;
	ofShader simpleShader;
	ofShader noiseSurfaceShader;
	float polarAlphaExpo, polarAlphaExpoScale;
	
	//surface
	void setupSphereSurface(float radius, int numU, int numV);
	vector< vector<ofVec3f> > sphereBaseCV;
	void drawSurfaceControlVertices(ofxSimpleSurface& s);
	
	float surfaceNoiseTime;
	MandalaTicker<float> noiseTimeTicker;
	
	float noiseTime, noiseTimeScale, noiseScale, noiseOffset;
	
	
	int numU, numV;
	float radius;
	ofxSimpleSurface sphereSurface;
	
	
	//components
	SurfacePoint* addSurfacePoint( ofxSimpleSurface& s, ofVec2f uv );
	
	void setClock( int numCogs=16, float scale=60, int octaves=10, float octaveScale=2., float startSpeed=.05 );
	void clearClock();
	bool bDrawClock;
	float clockNumCogs, clockScale, clockOctaves, cloackOctaveScale, clockMaxSpeed, clockTime, clockSpread, clockV;
	
	vector<SurfacePoint*> surfacePoints;
	
	map<string, ofVboMesh*> meshOptions;//names of the different meshes availlable to the components
	
	vector<SPMesh> surfaceMeshes;
	vector<SPMeshTail> tails;
	
	bool bDrawBoxClock;
	vector <MandalaComponent*> boxClockComponents;
	
	//meshes
	void loadHexMesh( string loc );
	void drawOffsetMesh();
	ofVboMesh cubeMesh;
	ofVbo hexMesh;
	int hexMeshVertexCount;
	ofNode hexMeshNode;
	
	vector<string> modelNames;
	ofFloatColor c0, c1;
	bool bDrawOffsetMesh;
	float time, faceScale, faceOffset, noiseOctaveScale, noiseExpo, noiseExpoScale, meshNoiseScale, colorMixScale;
	
	//ANIMATIIONS
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
	
	vector< MandalaTicker<float> > tickersf;
	vector< MandalaTicker<ofVec2f> > tickers2f;
	vector< MandalaTicker<ofVec3f> > tickers3f;
	vector< MandalaTicker<ofVec4f> > tickers4f;
	
	map<string, bool> whatsDrawn;
	
	
	bool bDrawSurface, bSmoothSurface, bDrawTails, bDrawSpokes;
	
	
	float lastTime, currentTime;
	
	vector<TailLoft> lofts;
	
	//temp
};
