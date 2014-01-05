//
//  emptyVisualSystem.h
//
//  Created by Patricio Gonzalez Vivo on 7/23/13.
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "ofxCv.h"
#include "MyTracker.h"
#include "ofxAVFVideoPlayer.h"



class CloudsVisualSystemVision : public CloudsVisualSystem {
public:
    
    string getSystemName();
    
    void selfSetup();
    void selfSetupGuis();
    
	void selfPresetLoaded(string presetPath);
	void selfSetDefaults();
    void selfAutoMode();
    void selfUpdate();
    void selfDrawBackground();
    void selfDrawDebug();
    void selfSceneTransformation();
    void selfDraw();
    void selfExit();
    void selfBegin();
	void selfEnd();
    
	ofRectangle flowWindow;
	ofRectangle videoRect;
	
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    void resetFlowField();
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    void updateImagesForNewVideo();
    void updateOpticalFlowParameters();
    void updateContourTracking();
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);

    ofxUISuperCanvas *opticalFlowGui;
    ofxUISuperCanvas *contourTrackingGui;
    ofxUISuperCanvas *thresholdGui;
	
protected:

    ofPixels opticalFlowPixels;
    int skipFrames;
	ofPtr<ofxAVFVideoPlayer> player;
	ofPixels lastPixels;

    int playerIndex;
    int movieIndex;
    bool frameIsNew;
    bool bNewVideoLoaded;
    vector<string> movieStrings;
    int opticalFlowScale;
    vector<ofVec2f> flowMotion;

    float colorRange;
    float contourLifetimeColorRange;
    ofImage accumulation;
    ofImage diff;

    ofImage previousHeatMap;
    ofImage flowImage;
    ofImage previousFlowImage;

    bool bDrawHeatMap;
    bool bDrawFlowWindow;
    bool bContourTracking;
    bool bOpticalFlow;
    bool bDrawBoxes;
    bool bDrawLines;
    bool bLifeTime;
    bool bNumbers;
    bool bColor;
    bool bContours;
    int flowDensity;
	int currentFlowDensity;
	float hueShift;
	float flowLineAlpha;
	
    //Contour tracking stuff
    ofxCv::ContourFinder contourFinder;
	ofxCv::RectTrackerFollower<MyTracker> tracker;
    vector<ofPolyline> contours;
    void updateCVParameters();
    ofImage thresholded;
    ofxCv::RunningBackground background;
    cv::Rect accumRegion;

    //Optical flow types
    ofxCv::FlowFarneback farneback;
	ofxCv::FlowPyrLK pyrLk;
	ofxCv::Flow* curFlow;
    void updateOpticalFlow();
//    void clearAccumulation();
    void drawFlowHeatMap(int x, int y);

    ofDirectory videosDir;
    ofVboMesh flowMesh;
    float windowWidth;
    float windowHeight;
	float flowDamp;
	
    bool drawPlayer;
    bool drawThresholded;
    bool drawDiff;
    bool flowFirstFrame;
    float videoAlpha;
    float thresholdAlpha;
    float diffAlpha;

    ofVec2f averageFlow;
	
    void loadCurrentMovie();
    void loadMovieAtIndex(int movieIndex);
	void loadMovieWithName(string name);
    void updateSettingsForNewVideo();
    
//    void setMode(CVMode mode);
    int accumulationCount;
    cv::Scalar diffMean;
    
    ofTexture prev;
    //OPTICAL FLOW PARAMETERS
    float pyrScale;
    float levels;
    float winsize;
    float iterations;
    float polyN;
    float polySigma;
    bool useFarneback;
    float winSize;
    float maxLevel;
    float maxFeatures;
    float qualityLevel;
    float minDistance;
    float flowLineMultiplier;
    float windowAlpha;
    
    //CONTOUR PARAMETERS
    float cvPersistance;
    float cvMaxDistance;
    float cvMinAreaRadius;
    float cvMaxAreaRadius;
    float cvThresholdValue;
    bool cvUpdateParameters;
    float lineWidth;
    float flowColorMapRange;
    float flowLineWidth;
    float learningTime;
    float thresholdValue;
    //CONTOUR VIS PARAMS
    float boxHue;
    float boxSat;
    float boxBright;
    ofFloatColor boxColor;
    
    //SHADER
    ofShader shader;
    
    
};
