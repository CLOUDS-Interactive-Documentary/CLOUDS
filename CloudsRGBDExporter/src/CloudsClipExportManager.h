#pragma once

#include "ofMain.h"

#include "CloudsClip.h"
#include "CloudsRGBDCombinedExporter.h"
#include "ofxRGBDCPURenderer.h"
#include "ofxRGBDPlayer.h"

class CloudsClipExportManager : public ofThread {
  public:
	CloudsClipExportManager();
	~CloudsClipExportManager();
		
	void exportClip(CloudsClip clip);
	bool isDone();
	float percentComplete();
	bool setExportDirectory(string directory);

  protected:


	CloudsClip currentClip;
	ofxRGBDCPURenderer renderer;
	ofxRGBDPlayer rgbdPlayer;
	CloudsRGBDCombinedExporter exporter;
	
	string exportDirectory;
	
	bool done;
	int currentFrame;
	
	void threadedFunction();
	
};
