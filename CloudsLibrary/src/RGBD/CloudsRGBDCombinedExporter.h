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

#include "ofxPCL.h"

#include "ofxXmlSettings.h"

#include "ofxRGBDPlayer.h"
#include "ofxRGBDRenderer.h"
#include "ofxRGBDCPURenderer.h"

class CloudsRGBDCombinedExporter {
public:
	
	CloudsRGBDCombinedExporter();
	~CloudsRGBDCombinedExporter();
    
	void setRenderer(ofxRGBDCPURenderer* renderer);
	void setPlayer(ofxRGBDPlayer* player);
	
	void render(string outputPath, string clipName);
	
	ofIntRange inoutPoint;
	float minDepth;
	float maxDepth;
    
protected:
    
	ofColor getColorForZDepth(unsigned short z);
	void writeMetaFile(string outputDirectory);
	
	ofxRGBDCPURenderer* renderer;  // It has a mesh, call .getReducedMesh();
	ofxRGBDPlayer*      player;
	
	ofRectangle videoRectangle;
	ofPixels    outputImage;
};

#endif
