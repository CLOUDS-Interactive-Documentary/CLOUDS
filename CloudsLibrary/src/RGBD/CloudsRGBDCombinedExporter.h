//
//  CloudsRGBDCombinedExporter.h
//  RGBDVisualize
//
//  Created by Patricio Gonzalez Vivo on 4/14/13.
//
//

#ifndef CLOUDS_RGBD_COMBINED_EXPORTER
#define CLOUDS_RGBD_COMBINED_EXPORTER

#include "ofMain.h"

#include "ofxXmlSettings.h"

#include "ofxRGBDPlayer.h"
#include "ofxRGBDRenderer.h"
#include "ofxRGBDCPURenderer.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"

class CloudsRGBDCombinedExporter {
  public:
	
	CloudsRGBDCombinedExporter();
	~CloudsRGBDCombinedExporter();
    
	void writeMetaFile(string outputDirectory, ofxRGBDCPURenderer* renderer);
	
	void setRenderer(ofxRGBDCPURenderer* renderer);
	void setPlayer(ofxRGBDPlayer* player);
	
	void prepare();
	
	void render(string outputPath, string clipName);
	void renderFrame(string outputPath, string clipName, ofxRGBDCPURenderer* renderer, ofPixelsRef videoPixels, int frameNum);

	ofxFaceTracker tracker;
	ofxCv::FlowPyrLK pyrLk;
	
	ofIntRange inoutPoint;
    
protected:
    
	ofColor getColorForZDepth(unsigned short z, float minDepth, float maxDepth);
	void interpolatePolyLine(ofPolyline& a, ofPolyline& b, ofPolyline& out, float delta);
	void addFaceToPixels(ofPixelsRef& targetPixels, ofPixelsRef& tempPixels, ofRectangle target,
						 ofPolyline& leftEye, ofPolyline& rightEye, ofPolyline& faceOutline);
	
	ofxRGBDCPURenderer* renderer;  // It has a mesh, call .getReducedMesh();
	ofxRGBDPlayer* player;
	float minDepth;
	float maxDepth;
	ofRectangle videoRectangle;
	ofPixels    outputImage;
	
	//face things
	ofPolyline lastRightEye;
    ofPolyline lastLeftEye;
    ofPolyline lastFace;
	
	//for offscreen
	ofCairoRenderer cairoRenderer;

//	ofFbo faceFBO; //draw target for face frame
	ofPixels faceFrame; //copy to destination
	bool inFace;
	bool foundFirstFace;
	int lastFaceFrameFound;
};

#endif
