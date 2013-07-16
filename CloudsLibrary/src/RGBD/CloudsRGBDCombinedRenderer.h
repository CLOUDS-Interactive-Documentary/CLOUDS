//
//  CloudsRGBDCombinedRender.h
//  CloudsRGBDCombinedRender
//
//  Created by Patricio Gonzalez Vivo on 4/14/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#ifdef AVF_PLAYER
#include "ofxAVFVideoPlayer.h"
#endif

class CloudsRGBDCombinedRenderer {
public:
    
    CloudsRGBDCombinedRenderer();
    ~CloudsRGBDCombinedRenderer();
	
    //  SET
    //
	bool setup(string videoPath, string calibrationXMLPath);

	void setShaderPath(string _shaderPath);
	void reloadShader();
    
	void setSimplification(ofVec2f _simplification);
	
    //  Use these to project and draw textured custom geometry
    //
	bool bindRenderer();
	void unbindRenderer();
    
    //  UPDATE
    //
    void update();
    
	//  CYCLE
	//
	
	bool isPlaying();
	bool isDone();
	
    //  DRAW
    //
    void drawMesh();
	void drawPointCloud();
	void drawWireFrame();
	void draw(ofPolyRenderMode drawMode);

	ofPtr<ofVideoPlayer> getSharedPlayerPtr();
#ifdef AVF_PLAYER
	ofxAVFVideoPlayer& getPlayer();
#else
	ofVideoPlayer& getPlayer();
#endif
	
	ofShader& getShader();
	
	// Move in 3D Space
    ofVec3f worldPosition;
	ofVec3f worldRotation;
	
	// Fix extrinsics
	ofVec3f adjustTranslate;
	ofVec3f adjustRotate;
	ofVec2f adjustScale;
    
	ofVec3f headPosition;
	
	float minDepth;
	float maxDepth;
	
	float flowPosition;
    float edgeClip;
	float farClip;
	float nearClip;

    bool bFlipTexture;
    bool bMirror;
    
  protected:
    void setupProjectionUniforms();
    void setTextureScaleForImage(ofBaseHasTexture& _texture);

#ifdef AVF_PLAYER
	ofxAVFVideoPlayer avPlayer;
#else
	ofPtr<ofVideoPlayer> player;
#endif
	
	ofShader shader;
    string shaderPath;
    
	// GEOMETRY
	//
    ofVboMesh mesh;
    ofVec2f simplify;
    
    //  RGB
    //
	ofRectangle colorRect;
    ofVec2f     colorScale;
	ofVec2f     colorPrincipalPoint;
	ofVec2f     colorFOV;
    
	ofMatrix4x4 extrinsics;
	ofVec3f     distortionK;
	ofVec2f     distortionP;
    
    //  Depth
    //
    ofRectangle depthRect;
    ofVec2f     depthPrincipalPoint;
	ofVec2f     depthFOV;
    
    //  Normals, Face Features, and Delta Movement
    //
	//surface normal
    ofRectangle normalRect;
	//describes facial features
	ofRectangle faceFeatureRect;
	//this describes the change each frame
	ofRectangle deltaChangeRect;
	
	bool useFaces;
	
    bool bRendererBound;
    bool bMeshGenerated;
    
};
