//
//  CloudsClipExportManager.cpp
//  CloudsRGBDExporter
//
//  Created by James George on 5/12/13.
//
//

#include "CloudsClipExportManager.h"

//--------------------------------------------------------------
unsigned short getMedianValueOf5(unsigned short* a){
	unsigned short a0 = a[0];
	unsigned short a1 = a[1];
	unsigned short a2 = a[2];
	unsigned short a3 = a[3];
	unsigned short a4 = a[4];
	
	if (a1 < a0) 	swap(a0, a1);
	if (a2 < a0)	swap(a0, a2);
	if (a3 < a0)	swap(a0, a3);
	if (a4 < a0)	swap(a0, a4);
	if (a2 < a1)	swap(a1, a2);
	if (a3 < a1) 	swap(a1, a3);
	if (a4 < a1)	swap(a1, a4);
	if (a3 < a2)	swap(a2, a3);
	if (a4 < a2)	swap(a2, a4);
	
	return a2;
}

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

	holeFiller.setKernelSize(3);
    holeFiller.setIterations(3);
    
    
	done = false;
	currentFrame = clip.startFrame - 24; //24 frame handle
	
	rgbdPlayer.setUseTexture(false);
	
	cout << "BEGINNING EXPORT OF CLIP ON THREAD " << currentClip.getLinkName() << " ALT FOLDER " << alternativeVideoFolder << endl;
	rgbdPlayer.setAlternativeVideoFolder( alternativeVideoFolder, true );
	if(!rgbdPlayer.setup( currentClip.getSceneFolder() )){
		ofLogError() << "Scene at path " << currentClip.getSceneFolder() << " Failed to load scene";
		done = true;
		return;
	}
	if(!rgbdPlayer.alternativeVideoIsConfirmed()){
		ofLogError() << "Movie file in " << alternativeVideoFolder << " does not have the same number of frames as " << currentClip.sourceVideoFilePath << endl;
		done = true;
		return;
		
	}
	renderer.cacheValidVertices = true;
	renderer.setup(rgbdPlayer.getScene().calibrationFolder);
	renderer.setRGBTexture(*rgbdPlayer.getVideoPlayer());
	
	medianPixels.allocate(rgbdPlayer.getDepthPixels().getWidth(),
						  rgbdPlayer.getDepthPixels().getHeight(), OF_IMAGE_GRAYSCALE);
	renderer.setDepthImage(medianPixels);

	currentClip.loadAdjustmentFromXML(true);
	renderer.nearClip = currentClip.minDepth;
	renderer.farClip = currentClip.maxDepth;
	renderer.colorMatrixTranslate = currentClip.adjustTranslate;
	renderer.colorMatrixRotate = currentClip.adjustRotate;
	renderer.scale = currentClip.adjustScale;
	
	rgbdPlayer.getVideoPlayer()->setFrame(currentFrame);
	rgbdPlayer.getVideoPlayer()->update();
	renderer.update();
	
	exporter.minBlobSize = currentClip.contourMinBlobSize;
	exporter.targetColor = currentClip.contourTargetColor;
	exporter.contourThreshold = currentClip.contourTargetThreshold;
	exporter.facePosition = renderer.getWorldPoint(currentClip.faceCoord, rgbdPlayer.getDepthSequence()->getPixels());
	
	exporter.prepare();
	
	cout << "STARTING CLIP EXPORT face position " << exporter.facePosition << " coord (" << currentClip.faceCoord << ")" << endl;
	
	startThread(true, false);
}

bool CloudsClipExportManager::isDone(){
	return done;
}

float CloudsClipExportManager::percentComplete(){
	if(done) return 0;
	return 1.0 * (currentFrame - currentClip.startFrame) / (currentClip.endFrame - currentClip.startFrame);
}

void CloudsClipExportManager::medianFilter(){
	
	int currentDepthFrame = rgbdPlayer.getDepthSequence()->getCurrentFrame();
	ofShortPixels pixels[5];
	unsigned short medianBuffer[5];
	
	int maxDepthFrame = rgbdPlayer.getDepthSequence()->getTotalNumFrames()-1;
	rgbdPlayer.getDepthSequence()->getPixelsAtFrame(ofClamp(currentDepthFrame-2,0,maxDepthFrame), pixels[0]);
	rgbdPlayer.getDepthSequence()->getPixelsAtFrame(ofClamp(currentDepthFrame-1,0,maxDepthFrame), pixels[1]);
	rgbdPlayer.getDepthSequence()->getPixelsAtFrame(ofClamp(currentDepthFrame+0,0,maxDepthFrame), pixels[2]);
	rgbdPlayer.getDepthSequence()->getPixelsAtFrame(ofClamp(currentDepthFrame+1,0,maxDepthFrame), pixels[3]);
	rgbdPlayer.getDepthSequence()->getPixelsAtFrame(ofClamp(currentDepthFrame+2,0,maxDepthFrame), pixels[4]);
	
	for(int i = 0; i < pixels[0].getWidth()*pixels[0].getHeight(); i++){
		for(int p = 0; p < 5; p++){
			medianBuffer[p] = pixels[p].getPixels()[ i ];
		}
		medianPixels.getPixels()[i] = getMedianValueOf5( medianBuffer );
	}
    
    holeFiller.close(medianPixels);

}

void CloudsClipExportManager::threadedFunction(){
		
	string outputDirectory = exportDirectory + currentClip.getCombinedPNGExportFolder();
	ofDirectory dir(outputDirectory);
	if(!dir.exists()){
		dir.create();
	}
	
	//TEMP
	//exporter.writeMetaFile(outputDirectory, &renderer);
	
	exporter.log = "writing " + currentClip.getID() + " from " + rgbdPlayer.getScene().name + "\n";
	int lastFrame = currentFrame;
	bool completedClip = false;
	while( isThreadRunning() && !completedClip ){ //24 frame handle
		
		//cout << "Exporting  " << currentClip.getLinkName() << " : " << currentFrame << endl;
		
		medianFilter();
		
		rgbdPlayer.update();
		renderer.update();
		
		exporter.renderFrame(outputDirectory, currentClip.getID(), &renderer, rgbdPlayer.getVideoPlayer()->getPixelsRef(), currentFrame);

		rgbdPlayer.getVideoPlayer()->nextFrame();
		currentFrame = rgbdPlayer.getVideoPlayer()->getCurrentFrame();

        if(lastFrame == currentFrame){
            exporter.log += "Frame stuck on " + ofToString(currentFrame) + "\n";
        	completedClip = true;
        }
        
		if(currentFrame > currentClip.endFrame + 24){
            exporter.log += "Export completed successfully \n";
			completedClip = true;
		}
        
        lastFrame = currentFrame;
		ofSleepMillis(10);
	}
	
	if(completedClip){
		exporter.writeMetaFile(outputDirectory, &renderer);
		
	}
	ofBuffer logFileBuf(exporter.log);
    string logFilePath = outputDirectory + "/" + currentClip.getID() + "_log.txt";
    cout << "Saving log to " << logFilePath << endl;
    
	ofBufferToFile(logFilePath, logFileBuf);
    
	done = true;
}
