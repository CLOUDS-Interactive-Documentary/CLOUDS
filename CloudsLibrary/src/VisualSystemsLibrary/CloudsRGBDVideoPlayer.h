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
#include "ofRange.h"
#include "CloudsGlobal.h"


#ifdef SHOW_SUBTITLES
  #include "ofxSubtitles.h"
  #include "CloudsVisualSystem.h"
#endif

class CloudsRGBDVideoPlayer {
public:
    
    CloudsRGBDVideoPlayer();
    ~CloudsRGBDVideoPlayer();
	
    //  SET
    //
	bool setup(string videoPath, string calibrationXMLPath, string subtitlesPath = "", float offsetTime = 0, float clipVolume =1);
	bool setupVO(string audioPath);
	void swapAndPlay();
	
	void setupProjectionUniforms(ofShader& shader);
    
	//  CYCLE
	//
	
	bool isPlaying();
	bool isDone();
	bool forceStop; //default to true when playin in clouds
    
	float getFadeIn(){
		return fadeInValue;
	};
	float getFadeOut(){
		return fadeOutValue;
	};

#ifdef AVF_PLAYER
	ofxAVFVideoPlayer& getPlayer();
#else
	ofVideoPlayer& getPlayer();
#endif
	
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
  protected:

	//  UPDATE
    //
	bool bEventRegistered;
    void update(ofEventArgs& args);
	
#ifdef AVF_PLAYER
	ofPtr<ofxAVFVideoPlayer> currentPlayer;
	ofPtr<ofxAVFVideoPlayer> nextPlayer;
#else
	ofPtr<ofVideoPlayer> currentPlayer;
	ofPtr<ofVideoPlayer> nextPlayer;
#endif
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
public:
    bool haveSubtitles();
    void drawSubtitles(float x, float y);
private:
    bool loadSubtitles(string path);
    bool currentHaveSubtitles;
    bool nextHaveSubtitles;
    
#ifdef SHOW_SUBTITLES
    ofxSubtitles currentSubtitles;
    ofxSubtitles nextSubtitles;
#endif
 
};
