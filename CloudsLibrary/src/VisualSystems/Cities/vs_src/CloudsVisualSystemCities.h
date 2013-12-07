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
	
	
	ofVec3f normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2);
	void makeBeveledCubeMeshWithTexCoords(ofVboMesh& m, float w=1., float h=1., float d=1.);
	
	void makeCubeMeshWithTexCoords(ofVboMesh& m, float w=1., float h=1., float d=1.);
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
	ofImage* colorMap;
    vector<string> colorMapNames;
	map<string, ofImage> colorMapImageMap;
	
	ofImage colorPalette;
    
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
	
	/////////////
	ofxUISuperCanvas* laserGui;
	bool bDrawPoints, bDrawEdges, bDrawMesh;
	
	ofVec3f projectorPosition;
	float projectorHeight;
	
	
	float superFakeAOAmount;
	float superFakeAOExpo;
	float shininess;
	float radiusAlphaScl;
	float sampleColorWeight;
	
	
	ofxUISuperCanvas* passOneGui;
	ofxUISuperCanvas* passTwoGui;
	bool bPassOne;
	bool bPassTwo;
	bool bPassOneDepthTest;
	bool bPassTwoDepthTest;
	ofFloatColor passOneColor, passTwoColor;
	float passOneAlpha, passTwoAlpha, passOneSampleColorWeight, passTwoSampleColorWeight;
	ofBlendMode passOneBlendMode;
	ofBlendMode passTwoBlendMode;
	map<string, ofBlendMode> blendModes;
	
	ofxUISuperCanvas* edgeGui;
	ofColor edgeColor;
	float edgeOffset, edgeAlpha;
	ofBlendMode edgeBlendMode;
	bool bUseEdgeMapProjection;
};