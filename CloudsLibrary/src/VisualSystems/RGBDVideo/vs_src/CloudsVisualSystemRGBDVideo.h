#pragma once

#include "ofMain.h"
#include "CloudsVisualSystem.h"

//#include "ofxAVFVideoPlayer.h"
typedef struct{
	ofRange depthRange;
	ofVec2f depthFOV;
	ofVec2f depthPP;
} Intrinsics;

class CloudsVisualSystemRGBDVideo : public CloudsVisualSystem {
public:
	
	CloudsVisualSystemRGBDVideo();
	
	string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
    void selfSetDefaults();
    
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
	void selfPresetLoaded(string presetPath);
	
    void selfDraw();
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
	
	ofCamera& getCameraRef(){
		return cloudsCamera;
	}
	
  protected:
    bool bDrawVideoDebug;
	ofVideoPlayer player;
	Intrinsics videoIntrinsics;
    
    bool blendModeAdd; //vs screen
    
    ofxUISuperCanvas* occlusionGui;
	ofShader occlusionShader;
    ofVbo occlusionMesh;
    bool bEnableOcclusion;
    bool bEnableOcclusionDebug;
	float occlusionSimplifyX;
	float occlusionSimplifyY;
    bool refreshOcclusion;
    void generateOcclusion();
    int occlusionVertexCount;
    int occlusionIndexCount;
    void drawOcclusionLayer();

	ofxUISuperCanvas* meshLayer;
	ofShader meshShader;
    ofVbo meshVbo;
    bool bEnableMesh;
	float meshAlpha;
	float meshSimplifyX;
	float meshSimplifyY;
    bool refreshMesh;
    void generateMesh();
    int meshVertexCount;
    int meshIndexCount;
    void drawMeshLayer();

	ofxUISuperCanvas* linesGui;
    ofShader linesShader;
	ofVbo lines;
    bool bEnableLines;
	float lineGranularity;
	float lineSpacing;
	float lineThickness;
    bool refreshLines;
	void generateLines();

    ofxUISuperCanvas* pointsGui;
    ofShader pointsShader;
	ofVboMesh points;
    bool bEnablePoints;
    float pointAlpha;
	float pointsSimplifyX;
	float pointsSimplifyY;
	float pointSize;
    bool refreshPoints;
    void generatePoints();
	void drawPointLayer();

	void setupGeneralUniforms(ofShader& shader);
    
	float pointscale;
    ofVec3f pointShift;

    ofxUISuperCanvas* dustGui;
    ofVboMesh dustMeshBig,dustMeshSmall;
    bool bDrawDust;
    int dustParticleCount;
    float dustAlpha;
    float dustRadius;
    float dustSizeWeight, currentDustSizeWeight;
    float dustPointSizeSmall;
    float dustPointSizeBig;
    void generateDust();
    
	void reloadShader();
	
	bool playMovie(string path);
	string loadMoviePath;
	string movieBaseFileName;
    
#ifdef VHX_MEDIA
    CloudsMedia* waitingMedia;
    void vhxRequestComplete(CloudsVHXEventArgs& args);
#endif
    
    float yLift;
	bool movieLoaded;
	
	ofxUITextInput* videoPathField;
};