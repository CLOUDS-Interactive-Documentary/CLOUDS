#import "testView.h"

@implementation testView
@synthesize clipTable;

- (void)setup
{

	ofSetVerticalSync(true);
	
	ofBackground(22);

	
	parser.loadFromFiles();

	
	if(ofFile::doesFileExist(CloudsVisualSystem::getDataPath() + "CloudsMovieDirectory.txt")){
		parser.setCombinedVideoDirectory(ofBufferFromFile(CloudsVisualSystem::getDataPath() + "CloudsMovieDirectory.txt").getText());
	}
	else{
		ofSystemAlertDialog("Could not find movie file path. Create a file called CloudsMovieDirectory.txt that contains one line, the path to your movies folder");
	}


	[clipTable setTarget:self];
	[clipTable setDoubleAction:@selector(loadClipFromTable:)];
	[clipTable reloadData];
	
	rgbdVisualSystem.setRenderer(renderer);
	rgbdVisualSystem.setup();
	renderer.setShaderPath( CloudsVisualSystem::getDataPath() + "shaders/rgbdcombined");

	rgbdVisualSystem.playSystem();
	rgbdVisualSystem.loadPresetGUISFromName("Test_");
	
	srand(ofGetSeconds());
	
	[self loadClip: parser.getRandomClip(true, false)];
	
}

- (void)update
{
	renderer.update();
}

- (void)draw
{

}

- (void) loadClipFromTable:(id)sender
{

	if(clipTable.selectedRow >= 0){

		[self loadClip: parser.getAllClips()[ clipTable.selectedRow ] ];
		
	}
}

- (IBAction)loadClip:(CloudsClip&)clip
{
	if(clip.hasCombinedVideo && renderer.setup( clip.combinedVideoPath, clip.combinedCalibrationXMLPath) ){
		renderer.getPlayer().play();
		rgbdVisualSystem.setupSpeaker(clip.person, "", clip.name);
		currentClip = clip;
	}
	else{
		ofLogError() << "CloudsPlaybackController::playClip -- folder " << clip.combinedVideoPath << " is not valid";
	}	
}

- (void)exit
{

}

- (void)keyPressed:(int)key
{
	if(key == ' ' && renderer.getPlayer().isLoaded()){
		if(renderer.getPlayer().isPlaying()){
			renderer.getPlayer().stop();
		}
		else{
			renderer.getPlayer().play();
		}
	}
	
	if(key == 'P'){
		renderer.getPlayer().play();
	}
	
	if(key == 'H'){
		ofShowCursor();
	}
	
	if(key == 'Q'){
		
		rgbdVisualSystem.addQuestion(parser.getRandomClip(true,true));
	}
	
	if(key == 'R'){
//		renderer.reloadShader();
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


- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{

	if([@"person" isEqualToString:aTableColumn.identifier]){
		return [NSString stringWithUTF8String: parser.getAllClips()[rowIndex].person.c_str() ];
	}
	else if([@"clip" isEqualToString:aTableColumn.identifier]){
		return [NSString stringWithUTF8String: parser.getAllClips()[rowIndex].name.c_str() ];
	}
	else if([@"combined" isEqualToString:aTableColumn.identifier]){
		return parser.getAllClips()[rowIndex].hasCombinedVideo ? @"YES" : @"NO";
	}
	return @"";
	

}

@end