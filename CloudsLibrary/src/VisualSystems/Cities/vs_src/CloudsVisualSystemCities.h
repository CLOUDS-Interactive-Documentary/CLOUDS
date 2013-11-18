//
//  CloudsVisualSystemCities.h
//
//  Created by Patricio Gonzalez Vivo on 5/26/13.
//	with additions from lars berg
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "ofxObjLoader.h"


class CloudsVisualSystemCities : public CloudsVisualSystem {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfAutoMode();
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    void selfDraw();
    void selfPostDraw();
    void selfExit();
    void selfBegin();
	void selfEnd();
    
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
	
//    void guiLightingEvent(ofxUIEventArgs &e);
    
    void billBoard();
	
	
	ofVec3f normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2)
	{
		return (p2 - p1).cross( p0 - p1).normalized();
	}
	
	void makeBeveledCubeMeshWithTexCoords(ofVboMesh& m, float w=1., float h=1., float d=1.)
	{
		//load a simplecube using the obj loader and scale the vertices
		m.clear();
		ofxObjLoader::load( getVisualSystemDataPath() + "models/beveledCube.obj", m, true );
		
		vector<ofVec3f>& v = m.getVertices();
		for(int i=0; i < v.size(); i++){
			v[i].x *= w;
			v[i].y *= h;
			v[i].z *= d;
		}
	}
	
	void makeCubeMeshWithTexCoords(ofVboMesh& m, float w=1., float h=1., float d=1.)
	{
		//load a simplecube using the obj loader and scale the vertices
		m.clear();
		ofxObjLoader::load( getVisualSystemDataPath() + "models/cube.obj", m, false );
		
		vector<ofVec3f>& v = m.getVertices();
		for(int i=0; i < v.size(); i++){
			v[i].x *= w;
			v[i].y *= h;
			v[i].z *= d;
		}
	}
	
	void makeCubeMesh(ofVboMesh& m, float w=1., float h=1., float d=1.)
	{
		vector<ofVec3f> corners(8);
		vector<ofVec3f> vertices;
		vector<ofVec3f> normals;
		vector<ofVec2f> texCoords;
		vector<ofIndexType> indices;
		
		w *= .5;
//		h *= .5;
		d *= .5;
		
		corners[0].set( -w, 0, d );
		corners[1].set( w, 0, d );
		corners[2].set( w, h, d );
		corners[3].set( -w, h, d );
		
		corners[4].set( -w, 0, -d );
		corners[5].set( w, 0, -d );
		corners[6].set( w, h, -d );
		corners[7].set( -w, h, -d );
		
		ofVec3f left(-1,0,0);
		ofVec3f right(1,0,0);
		ofVec3f top(0,1,0);
		ofVec3f bottom(0,-1,0);
		ofVec3f front(0,0,1);
		ofVec3f back(0,0,-1);
		
		//front and back
		vertices.push_back( corners[0] );
		vertices.push_back( corners[1] );
		vertices.push_back( corners[2] );
		vertices.push_back( corners[3] );
		vertices.push_back( corners[4] );
		vertices.push_back( corners[5] );
		vertices.push_back( corners[6] );
		vertices.push_back( corners[7] );
		
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(3);
		
		indices.push_back(4);
		indices.push_back(7);
		indices.push_back(6);
		
		indices.push_back(4);
		indices.push_back(6);
		indices.push_back(5);
		
		//left and right
		vertices.push_back( corners[0] );
		vertices.push_back( corners[1] );
		vertices.push_back( corners[2] );
		vertices.push_back( corners[3] );
		vertices.push_back( corners[4] );
		vertices.push_back( corners[5] );
		vertices.push_back( corners[6] );
		vertices.push_back( corners[7] );
		
		indices.push_back(8);
		indices.push_back(11);
		indices.push_back(15);
		
		indices.push_back(8);
		indices.push_back(15);
		indices.push_back(12);
		
		indices.push_back(9);
		indices.push_back(13);
		indices.push_back(14);
		
		indices.push_back(9);
		indices.push_back(14);
		indices.push_back(10);
		
		//top and bottom
		vertices.push_back( corners[0] );
		vertices.push_back( corners[1] );
		vertices.push_back( corners[2] );
		vertices.push_back( corners[3] );
		
		vertices.push_back( corners[4] );
		vertices.push_back( corners[5] );
		vertices.push_back( corners[6] );
		vertices.push_back( corners[7] );
		
		indices.push_back(16);
		indices.push_back(20);
		indices.push_back(21);
		
		indices.push_back(16);
		indices.push_back(21);
		indices.push_back(17);
		
		indices.push_back(18);
		indices.push_back(22);
		indices.push_back(23);
		
		indices.push_back(18);
		indices.push_back(23);
		indices.push_back(19);
		
		//calculate normals
		normals.resize( vertices.size() );
		ofVec3f n;
		for (int i=0; i<indices.size(); i+=3) {
			n = normalFrom3Points(vertices[indices[i]], vertices[indices[i+1]], vertices[indices[i+2]]);
			normals[indices[i]] = n;
			normals[indices[i+1]] = n;
			normals[indices[i+2]] = n;
		}
		
		m.clear();
		m.addVertices( vertices );
		m.addNormals( normals );
		m.addTexCoords( texCoords );
		m.addIndices( indices );
	}
	
	void makeBigCubesVbo( int _size, int _resolution );
    
protected:
    void    makeGrid(float _size, int _resolution);
    void    generateCube(float _sizeX, float _sizeY, float _sizeZ);
    float   size, resolution, height;
    
    //  Noise
    //
    ofShader noiseShader;
    ofFbo    noiseFbo;
    float    noiseSpeed,noiseZoom;
    
    //  GrayScot
    //
    ofShader grayscottShader;
    ofFbo    grayscottFbo[2];
    float    diffU,diffV,k,f;
    float    grayscottLoops;
    float    grayscottFade;
    int      nPingPong;
    bool     bCleanGrayscott;
    bool     bGrayscott;
    
    //  Mask
    //
    ofShader maskShader;
    ofFbo    maskFbo;
    float   maskSize,maskCurve;
    
    //  Blocks
    //
    float   blocksMinSize,blocksMinDist,blocksAlpha;
    float   blockSize;
    GLuint  cube;
    
    //  Post
    //
    ofShader    postShader;
    ofTexture   postTexture;
    float       postChromaDist;
    float       postGrainDist;
	
	//LB
	ofImage colorMap;
	ofVboMesh cubeMesh;
	ofShader cubesShader;
	ofImage* overlayMap;
	bool bUseOverlay;
	
	ofVbo edgeVbo;
	bool bEdgeSetup;
	int edgeVboVertexCount, edgeVboIndexCount;
	float edgeLineWidth;
	
	ofCamera projector;
	
	vector<string> overlayNames;
	map<string, ofImage> overlayImageMap;
	ofImage facadeTexture;
	float overScale;
};