#import "testView.h"

@implementation testView
@synthesize clipTable;

- (void)setup
{
	
	ofBackground(22);
	minSequenceDepth = 100;
	maxSequenceDepth = 200;
	
	cout << "PARSING LINKS" << endl;
	if(ofDirectory("../../../CloudsData/").exists()){
		
		cout << "Found link in correct directory" << endl;
		parser.parseLinks("../../../CloudsData/links/clouds_link_db.xml");
		parser.setup("../../../CloudsData/fcpxml/");
	}
	else{
		cout << "SETTING UP IN DATA DIRECTORY" << endl;
		parser.parseLinks("clouds_link_db.xml");
		parser.setup("xml");
	}
	
	//    parser.parseLinks("../../../CloudsLibrary/data/links/clouds_link_db.xml");
    
    [clipTable reloadData];
    exportFolder = CloudsClip::relinkFilePath("/Volumes/Nebula/MediaPackages/_exports");
    cout << "Relinked Export Folder "<< exportFolder << endl;
	for(int i = 0; i < 8; i++){
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

	renderer.setShaderPath("../../../CloudsData/shaders/unproject");
	
	gui = new ofxUICanvas(0,0,200,ofGetHeight());
	
	gui->addButton("Reset Camera", &resetCamera);
	gui->addSlider("min depth", 300, 1000, &minDepth);
	gui->addSlider("max depth", 900, 1500, &maxDepth);
	gui->addSlider("x texture translate", -20, 20, &translate.x);
	gui->addSlider("y texture translate", -20, 20, &translate.y);
	gui->addSlider("x texture rotate", -5, 5, &rotate.x);
	gui->addSlider("y texture rotate", -5, 5, &rotate.y);
	gui->addSlider("x texture scale", .8, 1.2, &scale.x);
	gui->addSlider("y texture scale", .8, 1.2, &scale.y);
	gui->addToggle("pause", &pause);
	gui->addToggle("Show Histogram", &showHistogram);
	gui->addToggle("Show Log Histogram", &useLog);
	
	gui->addButton("Save Alignment", &saveAlignment);
	
	[clipTable setDoubleAction:@selector(loadClipForAlignment:)];

	cam.setup();
	cam.autosavePosition = true;
	cam.loadCameraPosition();
	
	framebuffer.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 4);
}

- (void)update
{
	
	if(startExport){
		[self cancelExport:self];
    
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
        encodingScript.append("#!/bin/bash");
        for(int i = 0; i < selectedClips.size(); i++){
            encodingScript.append( selectedClips[i].getFFMpegLine(exportFolder) );
        }
//        ofBufferToFile(exportFolder+"/script.sh", encodingScript);
        ofBufferToFile("~/Desktop/"+ofGetTimestampString()+".sh", encodingScript);
//        selectedClips.clear();
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
		cam.setPosition(0, 0, 0);
		cam.setOrientation(ofQuaternion());
		cam.rotate(180, ofVec3f(0,1,0));
		cam.setAnglesFromOrientation();	
	}
	
	player.update();
	if(player.isLoaded()){
		if(player.getVideoPlayer()->isPlaying() && pause){
			player.getVideoPlayer()->stop();
		}
		else if(!player.getVideoPlayer()->isPlaying() && !pause){
			player.getVideoPlayer()->play();
		}
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
		
		renderer.update();
	}
	
	if(saveAlignment && player.isLoaded()){
		loadedClip.saveAdjustmentToXML();
		[clipTable reloadData];
	}
}

- (void)draw
{

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
	ofTranslate(0, 0, minDepth);
	ofDrawPlane(0, 0, 800, 800);
	ofPopMatrix();
	
	ofPushMatrix();
	ofSetHexColor(0xFFFFFF - 0x69aade);
	ofTranslate(0, 0, maxDepth);
	ofDrawPlane(0, 0, 800, 800);
	ofPopMatrix();
	
	cam.end();
	
	
	glDisable(GL_DEPTH_TEST);
	
	ofPopStyle();
	framebuffer.end();
	
	framebuffer.getTextureReference().draw(0,ofGetHeight(), ofGetWidth(), -ofGetHeight());
		
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

}

- (void) loadClipForAlignment:(id)sender
{

	if(clipTable.selectedRow >= 0){
		cout << "loadedd clip at row " << clipTable.selectedRow << endl;
		CloudsClip& clip = parser.getAllClips()[ clipTable.selectedRow ];
		if(player.setup(clip.getSceneFolder())){
			showHistogram = false;
			calculatedHistogram = false;
			histogram.clear();
			
			renderer.setup(player.getScene().calibrationFolder);
			renderer.setRGBTexture(*player.getVideoPlayer());
			renderer.setDepthImage(player.getDepthPixels());
			player.getVideoPlayer()->setFrame( clip.startFrame );
		
			loadedClip = clip;
			loadedClip.loadAdjustmentFromXML();
			translate = loadedClip.adjustTranslate;
			rotate = loadedClip.adjustRotate;
			scale = loadedClip.adjustScale;
			minDepth = loadedClip.minDepth;
			maxDepth = loadedClip.maxDepth;
			
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
	
}

- (void)mousePressed:(NSPoint)p button:(int)button
{
	
}

- (void)mouseReleased:(NSPoint)p button:(int)button
{
	
}

- (void)windowResized:(NSSize)size
{
	
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
	else if([@"align" isEqualToString:aTableColumn.identifier]){
		return ofFile::doesFileExist(parser.getAllClips()[rowIndex].getAdjustmentXML()) ? @"YES" : @"NO";
	}
	else if([@"file" isEqualToString:aTableColumn.identifier]){
		return [NSString stringWithUTF8String: ofFilePath::getBaseName( parser.getAllClips()[rowIndex].sourceVideoFilePath ).c_str() ];
	}
	else if([@"depth" isEqualToString:aTableColumn.identifier]){
		CloudsClip& clip = parser.getAllClips()[rowIndex];
		clip.loadAdjustmentFromXML();
		return [NSString stringWithUTF8String: ("[" + ofToString(clip.minDepth, 1) + " - " + ofToString(clip.maxDepth, 1) + "]" ).c_str() ];
	}
	else if([@"pairings" isEqualToString:aTableColumn.identifier]){
		return ofFile::doesFileExist(parser.getAllClips()[rowIndex].getSceneFolder() + "pairings.xml") ? @"YES" : @"NO";
	}
	else if([@"exported" isEqualToString:aTableColumn.identifier]){
		return ofFile::doesFileExist(parser.getAllClips()[rowIndex].getCombinedPNGExportFolder()) ? @"YES" : @"NO";
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