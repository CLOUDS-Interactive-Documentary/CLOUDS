#pragma once

#include "ofMain.h"

#include "CloudsClip.h"
#include "CloudsRGBDCombinedExporter.h"
#include "ofxRGBDCPURenderer.h"
#include "ofxRGBDPlayer.h"
#include "ofxDepthHoleFiller.h"

class CloudsClipExportManager : public ofThread {
  public:
	CloudsClipExportManager();
	~CloudsClipExportManager();
		
	void exportClip(CloudsClip clip);
	bool isDone();
	float percentComplete();
	bool setExportDirectory(string directory);
	string alternativeVideoFolder;
	int framesRemaining;
  protected:
	
	void medianFilter();
	
	CloudsClip currentClip;
	ofxRGBDCPURenderer renderer;
	ofxRGBDPlayer rgbdPlayer;
	CloudsRGBDCombinedExporter exporter;
	
    ofxDepthHoleFiller holeFiller;
    
	ofShortPixels medianPixels;
	
	string exportDirectory;
	
	bool done;

	int currentFrame;
	
	void threadedFunction();
	
};
