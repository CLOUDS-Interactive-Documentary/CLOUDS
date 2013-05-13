//
//  CloudsClipExportManager.cpp
//  CloudsRGBDExporter
//
//  Created by James George on 5/12/13.
//
//

#include "CloudsClipExportManager.h"

CloudsClipExportManager::CloudsClipExportManager(){
	done = true;
	currentFrame = 0;
}

CloudsClipExportManager::~CloudsClipExportManager(){
	
}

bool CloudsClipExportManager::setExportDirectory(string directory){
	exportDirectory = ofFilePath::addTrailingSlash(directory);
}

void CloudsClipExportManager::exportClip(CloudsClip clip){

	currentClip = clip;
	
	done = false;
	currentFrame = clip.startFrame;
	
	rgbdPlayer.setUseTexture(false);
	
	cout << "BEGINNING EXPORT OF CLIP ON THREAD " << currentClip.getLinkName() << endl;
	
	if(!rgbdPlayer.setup( currentClip.getSceneFolder() )){
		ofLogError() << "Scene at path " << currentClip.getSceneFolder() << " Failed to load movie";
	}
	renderer.cacheValidVertices = true;
	renderer.setup(rgbdPlayer.getScene().calibrationFolder);
	renderer.setRGBTexture(*rgbdPlayer.getVideoPlayer());
	renderer.setDepthImage(rgbdPlayer.getDepthPixels());
	
	rgbdPlayer.getVideoPlayer()->setFrame(currentFrame);
	
	exporter.minDepth = currentClip.minDepth;
	exporter.maxDepth = currentClip.maxDepth;
	
	startThread(true, false);
}

bool CloudsClipExportManager::isDone(){
	return done;
}

float CloudsClipExportManager::percentComplete(){
	if(done) return 0;
	return 1.0 * currentFrame / (currentClip.endFrame - currentClip.startFrame);
}

void CloudsClipExportManager::threadedFunction(){
	
	
	string outputDirectory = exportDirectory + currentClip.getCombinedPNGExportFolder();
	ofDirectory dir(outputDirectory);
	if(!dir.exists()){
		dir.create();
	}
	
	while( isThreadRunning() && currentFrame <= currentClip.endFrame ){
		
		rgbdPlayer.update();
		renderer.update();
		
		exporter.renderFrame(outputDirectory, currentClip.getLinkName(), &renderer, rgbdPlayer.getVideoPlayer()->getPixelsRef(), currentFrame);
		cout << "Saving frame to " << outputDirectory << endl;
		rgbdPlayer.getVideoPlayer()->nextFrame();
		currentFrame = rgbdPlayer.getVideoPlayer()->getCurrentFrame();
		
		ofSleepMillis(10);
	}
	
	done = true;
}
