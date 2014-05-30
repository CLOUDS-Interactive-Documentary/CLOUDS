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

#include "ofRange.h"
#include "CloudsGlobal.h"


#ifdef SHOW_SUBTITLES
    #include "ofxSubtitles.h"
#endif


class CloudsRGBDVideoPlayer
#ifdef TARGET_WIN32
    : public ofThread
#endif
{
public:
    
    CloudsRGBDVideoPlayer();
    ~CloudsRGBDVideoPlayer();
	
    //  SET
    //
	bool setup(string videoPath, string calibrationXMLPath, string subtitlesPath = "", float offsetTime = 0, float clipVolume =1);
	bool setupVO(string audioPath);
	void swapAndPlay();

#ifdef TARGET_WIN32
    void threadedFunction();

    string videoPath;
    string calibrationXMLPath;
    string subtitlesPath;
    float offsetTime;
    float clipVolume;
#endif
	
	void setupProjectionUniforms(ofShader& shader);
    
	//  CYCLE
	//
	void stop();
    
	bool isPlaying();
	bool isDone();
	bool forceStop; //default to true when playin in clouds
    
	float getFadeIn(){
		return fadeInValue;
	};
	float getFadeOut(){
		return fadeOutValue;
	};

//#ifdef TARGET_OSX
//	ofxAVFVideoPlayer& getPlayer();
//#else
//	ofDirectShowPlayer& getPlayer();
//#endif
	ofVideoPlayer& getPlayer();
	ofTexture& getTextureReference();

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
	
	bool playingVO;

	float maxVolume;
    
    float currentClipVolumeAdjustment;
    float nextClipVolumeAdjustment;
    float currentMaxVolume;
	
	bool hasSubtitles();
    void drawSubtitles();
    //deleteme
    void drawSubtitles(int x,int y);

  protected:

	//  UPDATE
    //
	bool bEventRegistered;
    void update(ofEventArgs& args);
	
//#ifdef TARGET_OSX
//	ofPtr<ofxAVFVideoPlayer> currentPlayer;
//	ofPtr<ofxAVFVideoPlayer> nextPlayer;
//#else
//	ofPtr<ofDirectShowPlayer> currentPlayer;
//	ofPtr<ofDirectShowPlayer> nextPlayer;
//	ofTexture videoTexture;
//#endif
	ofPtr<ofVideoPlayer> currentPlayer;
	ofPtr<ofVideoPlayer> nextPlayer;

	ofPtr<ofSoundPlayer> currentVoiceoverPlayer;
	ofPtr<ofSoundPlayer> nextVoiceoverPlayer;
	bool nextClipIsVO;


	
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
	
	//Skin variables
	ofFloatColor skinSampleColor;
	ofVec3f skinWeights;
	ofRange skinThreshold;
	
	string nextCalibrationXML;
	bool hasSkinSettings;
	bool useFaces;
	
	bool clipPrerolled;
	
//    bool bRendererBound;
//    bool bMeshGenerated;
	
	
	float fadeInValue;
	float fadeOutValue;
    
    /* Subtitles */
    bool loadSubtitles(string path);
    bool currentClipHasSubtitles;
    bool nextClipHasSubtitles;
    
#ifdef SHOW_SUBTITLES
    ofxSubtitles currentSubtitles;
    ofxSubtitles nextSubtitles;
#endif
 
};
