
#import "testView.h"
#include "ofxDepthImageCompressor.h"
#include "ofxTimecode.h"
#include "CloudsGlobal.h"

@implementation testView
@synthesize clipTable;

- (void)setup
{
	
	ofBackground(22);
	minSequenceDepth = 100;
	maxSequenceDepth = 200;
	contourThreshold = 100;
	minBlobSize = 0;
	selectColor = false;

	parser.loadFromFiles();

	exportFolder = ofBufferFromFile("SavedExportFolder.txt").getText();
	colorReplacementFolder = ofBufferFromFile("ColorReplacementFolder.txt").getText();
	
	if(exportFolder == ""){
		exportFolder = relinkFilePath("/Volumes/Nebula/MediaPackages/_exports/");
	}
	
	[exportFolderField setStringValue: [NSString stringWithUTF8String:exportFolder.c_str()] ];
	[colorReplacementField setStringValue: [NSString stringWithUTF8String:colorReplacementFolder.c_str()] ];

    
	[clipTable reloadData];
	
    cout << "Relinked Export Folder "<< exportFolder << endl;
	for(int i = 0; i < 4; i++){
		exportManagers.push_back(new CloudsClipExportManager());
		exportManagers[i]->setExportDirectory( exportFolder );
	}
	
	progressBars[0] = clipProgress1;
	progressBars[1] = clipProgress2;
	progressBars[2] = clipProgress3;
	progressBars[3] = clipProgress4;
	progressBars[4] = clipProgress5;
	progressBars[5] = clipProgress6;
	progressBars[6] = clipProgress7;
	progressBars[7] = clipProgress8;


	//renderer.setShaderPath( getDataPath() + "shaders/unproject");
	renderer.setupDefaultShader();
	
	gui = new ofxUICanvas(0,0,200,ofGetHeight());
	
	gui->addButton("Reset Camera", &resetCamera);
	gui->addButton("Save Alignment", &saveAlignment);
    
	gui->addSlider("Clip Position", 0, 1.0, &clipPosition);
	gui->addSlider("min depth", 300, 1000, &minDepth);
	gui->addSlider("max depth", 900, 1500, &maxDepth);
//	gui->addSlider("x texture translate", -20, 20, &translate.x);
//	gui->addSlider("y texture translate", -20, 20, &translate.y);
	gui->addSlider("x texture rotate", -5, 5, &rotate.x);
	gui->addSlider("y texture rotate", -5, 5, &rotate.y);
	gui->addSlider("x texture scale", .8, 1.2, &scale.x);
	gui->addSlider("y texture scale", .8, 1.2, &scale.y);
//
	gui->addToggle("select color", &selectColor);
	gui->addToggle("select face", &selectFace);
    //SM ADDED
    gui->addSlider("THRESHOLD_LOWER", 0.0, 1.0, &skinThresholdLower);
    gui->addSlider("THRESHOLD_UPPER", 0.0, 1.0, &skinThresholdUpper);
    gui->addSlider("HUE WEIGHT", 0.0, 1.0, &skinHueWeight);
    gui->addSlider("SATURATION WEIGHT", 0.0, 1.0, &skinSatWeight);
    gui->addSlider("BRIGTHNESS WEIGHT", 0.0, 1.0, &skinBrightWeight);
    gui->addSpacer();

//
//	gui->addSlider("contour threshold", 0, 200, &contourThreshold);
//	gui->addSlider("min blob size", 10*10, 300*300, &minBlobSize);

	
	gui->addToggle("pause", &pause);
	
	gui->addToggle("Show Histogram", &showHistogram);
	gui->addToggle("Show Log Histogram", &useLog);
    gui->autoSizeToFitWidgets();	
	[clipTable setDoubleAction:@selector(loadClipForAlignment:)];

	cam.setup();
	cam.autosavePosition = true;
	cam.loadCameraPosition();
	
	framebuffer.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 4);

	shaderSkinDetection.load(getDataPath() + "shaders/skinDetector");
    
    filler.setKernelSize(3);
    filler.setIterations(3);

	cout << "Finished setup " << ofxTimecode::timecodeForSeconds(parser.getAllClipDuration()) << " seconds" << endl;
}

- (void)update
{	
	if(startExport){
		[self cancelExport:self];
		
		colorReplacementFolder = string([[colorReplacementField stringValue] UTF8String]);
		exportFolder = [[exportFolderField stringValue] UTF8String] ;
		
		ofBuffer savedExportBuf;
		savedExportBuf.append( exportFolder );
		ofBuffer savedColorBuf;
		savedColorBuf.append( colorReplacementFolder );

		ofBufferToFile("SavedExportFolder.txt", savedExportBuf);
		ofBufferToFile("ColorReplacementFolder.txt", savedColorBuf);
		for(int i = 0; i < exportManagers.size(); i++){
			exportManagers[i]->alternativeVideoFolder = colorReplacementFolder;
            exportManagers[i]->setExportDirectory( exportFolder );
		}
		
		NSUInteger idx = [clipTable.selectedRowIndexes firstIndex];
		while (idx != NSNotFound) {
			// do work with "idx"
			selectedClips.push_back(parser.getAllClips()[idx]);
			
			// get the next index in the set
			idx = [clipTable.selectedRowIndexes indexGreaterThanIndex:idx];
		}
		
		exporting = selectedClips.size() > 0;
		[totalProgress setMinValue: 0.0];
		[totalProgress setMaxValue: 1.0*selectedClips.size()];
		
		cout << "exporting " << selectedClips.size() << endl;
		
        ofBuffer encodingScript;
        encodingScript.append("#!/bin/bash\n");
		for(int i = 0; i < selectedClips.size(); i++){
			encodingScript.append("#" + selectedClips[i].getID() + "\n" );
		}
        for(int i = 0; i < selectedClips.size(); i++){
            encodingScript.append( selectedClips[i].getFFMpegLine(colorReplacementFolder, exportFolder) );
        }
		
		char filename[512];
		sprintf(filename, "%s/ffmpeg_encode_M.%02d_D.%02d_H%02d_M.%02d.sh",exportFolder.c_str(),ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes() );
		
        ofBufferToFile(filename, encodingScript);
		
//        ofBufferToFile("~/Desktop/"+ofGetTimestampString()+".sh", encodingScript);
//        selectedClips.clear(); //STOP the export
		startExport = false;
	}
	
	if(exporting){
		for(int i = 0; i < exportManagers.size(); i++){
			if(currentClipIndex < selectedClips.size()  && exportManagers[i]->isDone()){
				cout << "EXPORTING CLIP " << selectedClips[currentClipIndex].getLinkName() << endl;;
				exportManagers[i]->exportClip(selectedClips[currentClipIndex]);
				currentClipIndex++;
				return;
			}
			
			[progressBars[i] setDoubleValue:exportManagers[i]->percentComplete() ];
		}
		[totalProgress setDoubleValue: currentClipIndex];	
	}
	
	cam.applyTranslation = cam.applyRotation = camRect.inside(mouseX,mouseY);
	
	if(resetCamera){
		cam.reset();
	}

	player.update();
	if(player.isLoaded()){
		
		if(player.getVideoPlayer()->isPlaying() && pause){
			player.getVideoPlayer()->stop();
		}
		else if(!player.getVideoPlayer()->isPlaying() && !pause){
			player.getVideoPlayer()->play();
		}
		
		if(pause){
			if(abs(clipPosition - player.getVideoPlayer()->getPosition()) > .0001){
				player.getVideoPlayer()->setPosition(clipPosition);
			}
		}
		else{
			clipPosition = player.getVideoPlayer()->getPosition();
		}
		
//		cout << "Finding contour on image size " << player.getVideoPlayer()->getWidth() << " " << player.getVideoPlayer()->getHeight() << " target color " << targetColor <<  " thresh " << contourThreshold << " blob size " << minBlobSize << endl;
//		contours.setMinArea(minBlobSize);
//		contours.setThreshold(contourThreshold);
//		contours.setTargetColor(targetColor);
//		contours.findContours(*player.getVideoPlayer());
        
        //SM REMOVED
        /*
		loadedClip.contourMinBlobSize = minBlobSize;
		loadedClip.contourTargetColor = targetColor;
		loadedClip.contourTargetThreshold = contourThreshold;
		*/
        loadedClip.faceCoord = facePosition;
        
        //SM ADDED
        loadedClip.skinBrightWeight = skinBrightWeight;
        loadedClip.skinHueWeight = skinHueWeight;
        loadedClip.skinSatWeight = skinSatWeight;
        loadedClip.skinLowerThreshold = skinThresholdLower;
        loadedClip.skinUpperThreshold = skinThresholdUpper;
		loadedClip.skinTargetColor = targetColor;
	}

	if(player.isLoaded() &&
	  (player.isFrameNew() ||
	   renderer.nearClip != minDepth ||
	   renderer.farClip != maxDepth ||
	   renderer.colorMatrixRotate != rotate ||
	   renderer.colorMatrixTranslate != translate ||
	   renderer.scale != scale))
	{
		loadedClip.minDepth = renderer.nearClip = minDepth;
		loadedClip.maxDepth = renderer.farClip = maxDepth;
		loadedClip.adjustRotate = renderer.colorMatrixRotate = rotate;
		loadedClip.adjustTranslate = renderer.colorMatrixTranslate = translate;
		loadedClip.adjustScale = renderer.scale = scale;
		
        filler.close( renderer.getDepthImage() );
		renderer.update();
	}
	
	if(saveAlignment && player.isLoaded()){
		loadedClip.saveAdjustmentToXML();
		[clipTable reloadData];
	}
}

- (void)draw
{

	if(selectColor){
		if(player.isLoaded()){
            
            //SM ADDED
			ofPushMatrix();
//			ofTranslate(200,0);
            shaderSkinDetection.begin();
            
            shaderSkinDetection.setUniformTexture("imgSampler",*player.getVideoPlayer(), 0);
            shaderSkinDetection.setUniform3f("samplePointColor", targetColor.r, targetColor.g,targetColor.b);
            shaderSkinDetection.setUniform3f("weights", skinHueWeight, skinSatWeight, skinBrightWeight);
            shaderSkinDetection.setUniform1f("lowerThreshold", skinThresholdLower);
            shaderSkinDetection.setUniform1f("upperThreshold", skinThresholdUpper);
            shaderSkinDetection.setUniform1i("redGreenDebug", 1);
            
			videoRect = ofRectangle(0,0,1920,1080);
			ofRectangle screenRect(200,0,ofGetWidth()-200,ofGetHeight());
			
			videoRect.scaleTo(screenRect);

			player.getVideoPlayer()->draw(videoRect);
            
            shaderSkinDetection.end();
        
			ofPopMatrix();
		}
	}
	else if(selectFace){
		ofPushMatrix();
		ofTranslate(200,0);
		
		ofImage depthImage;
		compressor.convertTo8BitImage(player.getDepthSequence()->getPixels(), depthImage);

		depthImage.draw(0,0);
		
		ofPushStyle();
		ofSetColor(255,100,0);
		ofNoFill();
		ofCircle(facePosition, 10);
		ofPopStyle();
		
		ofPopMatrix();
	}
	else {
		framebuffer.begin();
		ofClear(0);
		
		ofPushStyle();
		camRect = ofRectangle (200,0,ofGetWidth()-200,ofGetHeight());
		ofSetColor(0);
		ofRect(camRect);
		
		glEnable(GL_DEPTH_TEST);
		cam.begin(camRect);
		ofSetColor(255);
		if(player.isLoaded()){
			renderer.drawMesh();
		}
		
		glEnable(GL_DEPTH_TEST);
		
		ofNoFill();
		ofPushMatrix();
		ofSetHexColor(0x69aade);
        ofRotate(-90, 0,1,0);      
		ofTranslate(minDepth, 0, 0);

//#if (OF_VERSION_PATCH > 4)
		//ofDrawPlane(0, 0, 800, 800);
        ofDrawGridPlane(800);
		ofPopMatrix();
		
		ofPushMatrix();
		ofSetHexColor(0xFFFFFF - 0x69aade);
        ofRotate(-90, 0,1,0);
		ofTranslate(maxDepth, 0, 0);
		//ofDrawPlane(0, 0, 800, 800);
        ofDrawGridPlane(800);        
		ofPopMatrix();
		
		cam.end();
		
		glDisable(GL_DEPTH_TEST);
		
		ofPopStyle();
		framebuffer.end();
		
		framebuffer.getTextureReference().draw(0,ofGetHeight(), ofGetWidth(), -ofGetHeight());
	}
	
	if(showHistogram){
		if(!calculatedHistogram){
			[self calculateHistogram];
		}
		ofPolyline histLine;
		map<unsigned short, int>::iterator histit;
		
		for(histit = hist.begin(); histit != hist.end(); histit++){
			if(useLog){
				histLine.addVertex(ofVec2f(ofMap(histit->first, minSequenceDepth, maxSequenceDepth, 200, ofGetWidth(), true),
										   ofMap(log(histit->second), 0, log(maxOccurrences), ofGetHeight(), ofGetHeight()-300,true)));
			}
			else{
				histLine.addVertex(ofVec2f(ofMap(histit->first, minSequenceDepth, maxSequenceDepth, 200, ofGetWidth(), true),
										   ofMap((histit->second), 0, (maxOccurrences), ofGetHeight(), ofGetHeight()-300,true)));
			}
		}
		
		ofSetColor( ofColor::greenYellow );
		float minDepthLine = ofMap(minDepth, minSequenceDepth, maxSequenceDepth, 200, ofGetWidth(), true);
		float maxDepthLine = ofMap(maxDepth, minSequenceDepth, maxSequenceDepth, 200, ofGetWidth(), true);
		ofSetColor( ofColor::rosyBrown );
		
		ofLine(minDepthLine, ofGetHeight()-300, minDepthLine, ofGetHeight());
		ofLine(maxDepthLine, ofGetHeight()-300, maxDepthLine, ofGetHeight());
		ofSetColor(255);
		histLine.draw();
	}
	
	//draw color
	ofPushStyle();
	ofSetColor(targetColor);
	ofRect(ofGetWidth()-100,100,50,50);
	ofPopStyle();
}

- (void) loadClipForAlignment:(id)sender
{

	if(clipTable.selectedRow >= 0){

		colorReplacementFolder = ofFilePath::addTrailingSlash( string([[colorReplacementField stringValue] UTF8String]) );
		exportFolder = ofFilePath::addTrailingSlash( [[exportFolderField stringValue] UTF8String] );
		ofBuffer savedExportBuf;
		savedExportBuf.append( exportFolder );
		ofBuffer savedColorBuf;
		savedColorBuf.append( colorReplacementFolder );
		
		ofBufferToFile("SavedExportFolder.txt", savedExportBuf);
		ofBufferToFile("ColorReplacementFolder.txt", savedColorBuf);

		CloudsClip& clip = parser.getAllClips()[ clipTable.selectedRow ];
		player.setAlternativeVideoFolder( string([[colorReplacementField stringValue] UTF8String]), true);
		
		if(player.setup(clip.getSceneFolder())){
			
			if(!player.alternativeVideoIsConfirmed()){
				ofSystemAlertDialog("Error confirming alternative clip " + clip.getSceneFolder() + " Could not find clip " + ofFilePath::getFileName(player.getScene().videoPath) );
				return;
			}
			
			showHistogram = false;
			calculatedHistogram = false;
			histogram.clear();
			
			renderer.setup(player.getScene().calibrationFolder);
			renderer.setRGBTexture(*player.getVideoPlayer());
			renderer.setDepthImage(player.getDepthPixels());
			player.getVideoPlayer()->setFrame( clip.startFrame );
		
			loadedClip = clip;
			loadedClip.loadAdjustmentFromXML( true );
			translate = loadedClip.adjustTranslate;
			rotate = loadedClip.adjustRotate;
			scale = loadedClip.adjustScale;
			minDepth = loadedClip.minDepth;
			maxDepth = loadedClip.maxDepth;
            
            //SM REMOVED
            /*
			minBlobSize = loadedClip.contourMinBlobSize;
			contourThreshold = loadedClip.contourTargetThreshold;
			contours.setTargetColor(targetColor);
            */
            
            //SM ADDED
			targetColor = loadedClip.skinTargetColor;
			facePosition = loadedClip.faceCoord;
            skinHueWeight = loadedClip.skinHueWeight;
            skinBrightWeight = loadedClip.skinBrightWeight;
            skinSatWeight = loadedClip.skinSatWeight;
            skinThresholdUpper = loadedClip.skinUpperThreshold;
            skinThresholdLower = loadedClip.skinLowerThreshold;
			
			player.play();
		}
	}
}

- (IBAction) exportSelection:(id)sender
{
	startExport = true;
}

- (IBAction) cancelExport:(id)sender
{
	currentClipIndex = 0;
	
	for(int i = 0; i < exportManagers.size(); i++){
		cout << "STOPPING THREAD " << i << endl;
		exportManagers[i]->waitForThread(true);
		[progressBars[i] setDoubleValue:0 ];
	}
	
	[totalProgress setDoubleValue:0];


	selectedClips.clear();
	exporting = false;
}

- (void)exit
{

}

- (void)keyPressed:(int)key
{
	if(key == ' ' && player.isLoaded()){
		if(player.getVideoPlayer()->isPlaying()){
			player.getVideoPlayer()->stop();
		}
		else{
			player.getVideoPlayer()->play();
		}
	}
	
	if(key == 'S'){
		cout << "SHADER RELOAD" << endl;
		renderer.reloadShader();
        shaderSkinDetection.load(getDataPath() + "shaders/skinDetector");
	}
	
	if(key == 'H'){
		if(!calculatedHistogram){
			[self calculateHistogram];
		}
		showHistogram = !showHistogram;
	}
}

- (void)keyReleased:(int)key
{
	
}

- (void)mouseMoved:(NSPoint)p
{
	
}

- (void)mouseDragged:(NSPoint)p button:(int)button
{
//	if(selectColor && player.isLoaded() && ofRectangle(200,0,player.getVideoPlayer()->getWidth(),player.getVideoPlayer()->getHeight()).inside(p.x, p.y)){
//		targetColor = player.getVideoPlayer()->getPixelsRef().getColor( p.x-200, p.y );
//		contours.setTargetColor(targetColor);
//	}
	[self runSelectionsAtPoint:ofVec2f(p.x,p.y) ];
}

- (void)mousePressed:(NSPoint)p button:(int)button
{
	[self runSelectionsAtPoint:ofVec2f(p.x,p.y) ];
}

- (void) runSelectionsAtPoint:(ofVec2f) p
{
	if(selectColor && player.isLoaded() && videoRect.inside(p.x, p.y)){
		
		float xScale = player.getVideoPlayer()->getWidth()  / videoRect.width;
		float yScale = player.getVideoPlayer()->getHeight() / videoRect.height;
		
        targetColor = player.getVideoPlayer()->getPixelsRef().getColor( (p.x-videoRect.x )*xScale,
                                                                       (p.y-videoRect.y)*yScale);
        

        /*
		ofFloatColor c = player.getVideoPlayer()->getPixelsRef().getColor( (p.x-videoRect.x )*xScale,
																	    (p.y-videoRect.y)*yScale);
        
        ofFloatColor c1 = player.getVideoPlayer()->getPixelsRef().getColor( (p.x-videoRect.x +1 )*xScale,
                                                                         (p.y-videoRect.y)*yScale);
        ofFloatColor c2 = player.getVideoPlayer()->getPixelsRef().getColor( (p.x-videoRect.x  )*xScale,
                                                                          (p.y-videoRect.y +1)*yScale);
        ofFloatColor c3 = player.getVideoPlayer()->getPixelsRef().getColor( (p.x-videoRect.x  )*xScale,
                                                                           (p.y-videoRect.y -1)*yScale);
        ofFloatColor c4 = player.getVideoPlayer()->getPixelsRef().getColor( (p.x-videoRect.x -1  )*xScale,
                                                                           (p.y-videoRect.y )*yScale);
        
        ofFloatColor c5 = player.getVideoPlayer()->getPixelsRef().getColor( (p.x-videoRect.x +1 )*xScale,
                                                                          (p.y-videoRect.y +1)*yScale);
        ofFloatColor c6 = player.getVideoPlayer()->getPixelsRef().getColor( (p.x-videoRect.x -1 )*xScale,
                                                                           (p.y-videoRect.y +1)*yScale);
        ofFloatColor c7 = player.getVideoPlayer()->getPixelsRef().getColor( (p.x-videoRect.x +1 )*xScale,
                                                                           (p.y-videoRect.y -1)*yScale);
        ofFloatColor c8 = player.getVideoPlayer()->getPixelsRef().getColor( (p.x-videoRect.x -1  )*xScale,
                                                                           (p.y-videoRect.y -1)*yScale);
        
        targetColor = ( c + c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 ) / 9;
        */

	}
	else if(selectFace && player.isLoaded() && ofRectangle(0,0,640,480).inside(p.x-200,p.y) ){
		facePosition = ofVec2f(p.x-200, p.y);
		cout << "setting face position to " << selectFace << endl;
	}
}

- (void)mouseReleased:(NSPoint)p button:(int)button
{
	
}

- (void)windowResized:(NSSize)size
{
	framebuffer.allocate(size.width, size.height, GL_RGB, 4);
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return parser.getAllClips().size();
}

- (void)calculateHistogram
{
	int skipPixels = 4;
	int skipFrames = 2;
	ofShortPixels pixels;
	
	hist.clear();
	
//	int firstFrame = player.getDepthSequence()->getCurrentFrame();
	player.getVideoPlayer()->setFrame( loadedClip.startFrame );
	player.update();
	unsigned long startTime = player.getVideoPlayer()->getPosition() * player.getVideoPlayer()->getDuration() * 1000;
	player.getVideoPlayer()->setFrame( loadedClip.endFrame );
	player.update();
	unsigned long endTime = player.getVideoPlayer()->getPosition() * player.getVideoPlayer()->getDuration() * 1000;
	
	int firstFrame = player.getDepthSequence()->frameForTime( startTime );
	int lastFrame  = player.getDepthSequence()->frameForTime( endTime );
	
	cout << "calculating from time " << startTime << " to " << endTime << "; frame " << firstFrame << " " << lastFrame << endl;
	for(int i = firstFrame; i < lastFrame; i += skipFrames){
		player.getDepthSequence()->getPixelsAtFrame(i, pixels);
		for(int p = 0; p < pixels.getWidth()*pixels.getHeight(); p += skipPixels){
			if(pixels.getPixels()[p] != 0){
				hist[ pixels.getPixels()[p] ]++;
			}
		}
	}
	
	maxOccurrences = 0;
	minSequenceDepth = 1343;
	maxSequenceDepth = 1343;
	
	map<unsigned short, int>::iterator histit;
	for(histit = hist.begin(); histit != hist.end(); histit++){
		maxOccurrences = MAX(maxOccurrences,histit->second);
		minSequenceDepth = MIN(histit->first, minSequenceDepth);
		maxSequenceDepth = MAX(histit->first, maxSequenceDepth);
	}
	
	cout << "depth range [" << minSequenceDepth << " " << maxSequenceDepth << "] max occurrence is " << maxOccurrences << endl;
	calculatedHistogram = true;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{

	if([@"clip" isEqualToString:aTableColumn.identifier]){
		return [NSString stringWithUTF8String: parser.getAllClips()[rowIndex].getLinkName().c_str() ];
	}
	if([@"frames" isEqualToString:aTableColumn.identifier]){
		string frameDescriptor = "[ " + ofToString(parser.getAllClips()[rowIndex].startFrame) + " - " + ofToString(parser.getAllClips()[rowIndex].endFrame) + " ]";
		return [NSString stringWithUTF8String: frameDescriptor.c_str() ];
	}
	else if([@"align" isEqualToString:aTableColumn.identifier]){
		return ofFile::doesFileExist(parser.getAllClips()[rowIndex].getAdjustmentXML()) ? @"YES" : @"NO";
	}
	else if([@"file" isEqualToString:aTableColumn.identifier]){
		vector<string> components = ofSplitString( ofFilePath::removeTrailingSlash(parser.getAllClips()[rowIndex].getSceneFolder()), "/" );
		string takeName = components[components.size()-1];
		return  [NSString stringWithUTF8String:(takeName + " - " + ofFilePath::getBaseName(parser.getAllClips()[rowIndex].sourceVideoFilePath)).c_str() ];
	}
	else if([@"depth" isEqualToString:aTableColumn.identifier]){
		CloudsClip& clip = parser.getAllClips()[rowIndex];
		clip.loadAdjustmentFromXML();
		if(clip.minDepth == 300 && clip.maxDepth == 1200){
			return @"N/S";
		}
		return [NSString stringWithUTF8String: ("[" + ofToString(clip.minDepth, 1) + " - " + ofToString(clip.maxDepth, 1) + "]" ).c_str() ];
	}
	else if([@"texture" isEqualToString:aTableColumn.identifier]){
		CloudsClip& clip = parser.getAllClips()[rowIndex];
		clip.loadAdjustmentFromXML();		
		if(clip.adjustRotate.x == 0 && clip.adjustRotate.y == 0){
			return @"N/S";
		}
		return [NSString stringWithFormat: @"x:%.02f y:%.02f", clip.adjustRotate.x, clip.adjustRotate.y ];
	}
	else if([@"skin" isEqualToString:aTableColumn.identifier]){
		CloudsClip& clip = parser.getAllClips()[rowIndex];
		//clip.loadAdjustmentFromXML();
		if( clip.skinTargetColor == ofFloatColor(1.0,0.0,0.0) ){
			return @"N/S";
		}
		return [NSString stringWithFormat:@"%.01f,%.01f,%.01f",
					clip.skinTargetColor.r,
					clip.skinTargetColor.g,
					clip.skinTargetColor.b];
	}
	else if([@"head" isEqualToString:aTableColumn.identifier]){
		CloudsClip& clip = parser.getAllClips()[rowIndex];
		//clip.loadAdjustmentFromXML();
		if( clip.faceCoord == ofVec2f(320.,110.) ){
			return @"N/S";
		}

		return [NSString stringWithFormat:@"%.01f,%.01f",
				clip.faceCoord.x,
				clip.faceCoord.y];
	}
	else if([@"pairings" isEqualToString:aTableColumn.identifier]){
		return ofFile::doesFileExist(parser.getAllClips()[rowIndex].getSceneFolder() + "pairings.xml") ? @"YES" : @"NO";
	}
	else if([@"exported" isEqualToString:aTableColumn.identifier]){
		return ofFile::doesFileExist(exportFolder + "/" + parser.getAllClips()[rowIndex].getCombinedPNGExportFolder()+"/_calibration.xml") ? @"YES" : @"NO";
	}
	else{
		return @"IDENTIFER ERROR";
	}

//	string keyword = parser.getAllKeywords()[rowIndex];
//	
//	if([@"occurrence" isEqualToString:aTableColumn.identifier]){
//		return [NSNumber numberWithInt: parser.occurrencesOfKeyword(keyword)];
//	}
//	else{
//		return [NSString stringWithCString:keyword.c_str() encoding:NSUTF8StringEncoding];
//	}
	
}

@end