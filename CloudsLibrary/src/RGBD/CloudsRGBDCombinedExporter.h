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

class CloudsRGBDCombinedExporter {
public:
	
	CloudsRGBDCombinedExporter();
	~CloudsRGBDCombinedExporter();
    
	void writeMetaFile(string outputDirectory, ofxRGBDCPURenderer* renderer);
	
	void setRenderer(ofxRGBDCPURenderer* renderer);
	void setPlayer(ofxRGBDPlayer* player);
	
	void render(string outputPath, string clipName);
	void renderFrame(string outputPath, string clipName, ofxRGBDCPURenderer* renderer, ofPixelsRef videoPixels, int frameNum);
	
	ofIntRange inoutPoint;
    
protected:
    
	ofColor getColorForZDepth(unsigned short z, float minDepth, float maxDepth);
	
	ofxRGBDCPURenderer* renderer;  // It has a mesh, call .getReducedMesh();
	ofxRGBDPlayer*      player;
	float minDepth;
	float maxDepth;
	ofRectangle videoRectangle;
	ofPixels    outputImage;
};

#endif
