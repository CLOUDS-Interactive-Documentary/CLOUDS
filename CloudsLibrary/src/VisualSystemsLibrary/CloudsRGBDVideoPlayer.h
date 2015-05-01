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
#include "ofxSubtitles.h"

// Secondary threads are only used to preload videos on Windows.
// (This is done automatically on Mac with the AVFVideoPlayer)
class CloudsRGBDVideoPlayer {
public:
    
    CloudsRGBDVideoPlayer();
    ~CloudsRGBDVideoPlayer();
	
	bool setup(string videoPath, string calibrationXMLPath, string subtitlesPath = "", float offsetTime = 0, float clipVolume = 1);
	bool setupVO(string audioPath, string subtitlePath = "");
	void swapAndPlay();

    string nextVideoPath;
    string nextCalibrationXMLPath;
    string nextSubtitlesPath;
    float nextOffsetTime;
    float nextClipVolume;
    bool bLoadResult;
	bool bPlayWhenReady;

	void begin(ofShader& shader);
	void end();
    
	//  CYCLE
	//
	void stop();
    
	bool isPlaying();
	bool isDone();
    bool clipJustFinished();
    
    void pause();
    void unpause();
	bool forceStop; //default to true when playin in clouds
    
	float getFadeIn();
	float getFadeOut();
    
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
	bool playingVideo;

	float maxVolume;
    
    float currentClipVolumeAdjustment;
    float nextClipVolumeAdjustment;
    float currentMaxVolume;
	
	bool hasSubtitles();
	ofPtr<ofxSubtitles> getSubtitles();

    void drawSubtitles();
	void drawSubtitles(int x, int y, float fade = 1.0);

	//subtitle 2D params
	float subtitle2DOffsetLowerThirds;
	float subtitle2DOffsetVisualSystem;
	bool showingLowerThirds;
	int subtitleFontSize;
	//type
	float englishSubtitleKerning;
	float japaneseSubtitleKerning;
	int fontLoadWidth;
    
  protected:

	//  UPDATE
    //
	bool bEventRegistered;
    void update(ofEventArgs& args);
	
	void startPlayer();

	ofPtr<ofVideoPlayer> currentPlayer;
	ofPtr<ofVideoPlayer> nextPlayer;
    
	bool nextClipIsVO;
    bool clipPrerolled;
	bool playerPaused;
    bool bClipJustFinished;
    
	float fadeInValue;
	float fadeOutValue;
    //float currentAudioVolume;
    bool wasPlayingLastFrame;
    /* Subtitles */
    bool loadSubtitles(string path);
    bool currentClipHasSubtitles;
    bool nextClipHasSubtitles;
    
    ofPtr<ofxSubtitles> currentSubtitles;
    ofPtr<ofxSubtitles> nextSubtitles;

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

	
 
};
