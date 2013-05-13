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
    
	void writeMetaFile(string outputDirectory);
	
	void setRenderer(ofxRGBDCPURenderer* renderer);
	void setPlayer(ofxRGBDPlayer* player);
	
	void render(string outputPath, string clipName);
	void renderFrame(string outputPath, string clipName, ofxRGBDCPURenderer* renderer, ofPixelsRef videoPixels, int frameNum);
	
	ofIntRange inoutPoint;
	float minDepth;
	float maxDepth;
    
protected:
    
	ofColor getColorForZDepth(unsigned short z);
	
	ofxRGBDCPURenderer* renderer;  // It has a mesh, call .getReducedMesh();
	ofxRGBDPlayer*      player;
	
	ofRectangle videoRectangle;
	ofPixels    outputImage;
};

#endif
