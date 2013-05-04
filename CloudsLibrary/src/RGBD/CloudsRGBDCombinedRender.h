//
//  CloudsRGBDCombinedRender.h
//  CloudsRGBDCombinedRender
//
//  Created by Patricio Gonzalez Vivo on 4/14/13.
//
//

#ifndef CLOUDS_RGBD_COMBINED_RENDER
#define CLOUDS_RGBD_COMBINED_RENDER

#include "ofMain.h"
#include "ofxXmlSettings.h"

class CloudsRGBDCombinedRender {
public:
    
    CloudsRGBDCombinedRender();
    ~CloudsRGBDCombinedRender();
	
    //  SET
    //
	bool setup(string videoPath);
    void setTexture(ofBaseHasTexture& _tex);
	void setShaderPath(string _shaderPath);
    
    //  Use these to project and draw textured custom geometry
    //
	bool bindRenderer();
	void unbindRenderer();
    
    //  UPDATE
    //
    void update();
    
    //  DRAW
    //
    void drawMesh();
	void drawPointCloud();
	void drawWireFrame();
	void draw(ofPolyRenderMode drawMode);
    
    void reloadShader();
    
    ofVec3f worldPosition;
	ofVec3f worldRotation;
    
    ofVec2f shift;
	ofVec2f scale;
    
    //  Geometry
    //
    ofVec2f simplify;
    
    float edgeClip;
	float farClip;
	float nearClip;
    
    bool bFlipTexture;
    bool bMirror;
    
protected:
    
    void setupProjectionUniforms();
    void setSimplification(ofVec2f _simplification);
    void setTextureScaleForImage(ofBaseHasTexture& _texture);
    
    ofBaseHasTexture *tex;
	ofShader shader;
    string shaderPath;
    
    ofMesh mesh;
    
    //  RGB
    //
	ofRectangle colorRect;
    ofVec2f     colorScale;
	ofVec2f     colorPrincipalPoint;
	ofVec2f     colorFOV;
    
    float       depthToRGBRotation[9];
	ofVec3f     depthToRGBTranslation;
	ofVec3f     distortionK;
	ofVec2f     distortionP;
    
    //  Depth
    //
    ofRectangle depthRect;
    ofVec2f     depthPrincipalPoint;
	ofVec2f     depthFOV;
    
    //  Normals
    //
    ofRectangle normalRect;
    
    bool bRendererBound;
    bool bMeshGenerated;
    
};

#endif
