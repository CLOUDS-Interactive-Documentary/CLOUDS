#import "testView.h"
#include "CloudsGlobal.h"
#include "CloudsSpeaker.h"

@implementation testView
@synthesize clipTable;
@synthesize interventionTextBox;
- (void)setup
{

	ofSetVerticalSync(true);
	
	ofBackground(22);

	
	parser.loadFromFiles();

	
	if(ofFile::doesFileExist(getDataPath() + "CloudsMovieDirectory.txt")){
		parser.setCombinedVideoDirectory(ofBufferFromFile(getDataPath() + "CloudsMovieDirectory.txt").getText());
        cout<<"Clouds Directory is pointing to "<<ofBufferFromFile(getDataPath() + "CloudsMovieDirectory.txt").getText()<<endl;
	}
	else{
		ofSystemAlertDialog("Could not find movie file path. Create a file called CloudsMovieDirectory.txt that contains one line, the path to your movies folder");
	}


	[clipTable setTarget:self];
	[clipTable setDoubleAction:@selector(loadClipFromTable:)];
	[clipTable reloadData];
    [interventionTextBox setTarget:self];   
	
//	rgbdVisualSystem.setRenderer(renderer);
	rgbdVisualSystem.setup();
//	renderer.setShaderPath( renderer.G() + "shaders/rgbdcombined");

	rgbdVisualSystem.playSystem();
#ifdef OCULUS_RIFT
	rgbdVisualSystem.loadPresetGUISFromName("RGBDOC");
#else
	rgbdVisualSystem.loadPresetGUISFromName("RGBDMain");
#endif
	srand(ofGetSeconds());
	
	[self loadClip: parser.getRandomClip(true, false)];
	
}

- (void)update
{
//	renderer.update();
	if(rgbdVisualSystem.getRGBDVideoPlayer().isDone()){
		rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().setPosition(0);
		rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().play();
	}
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
	if(clip.hasCombinedVideo && rgbdVisualSystem.getRGBDVideoPlayer().setup( clip.combinedVideoPath, clip.combinedCalibrationXMLPath) ){
		
		rgbdVisualSystem.getRGBDVideoPlayer().swapAndPlay();
		rgbdVisualSystem.setupSpeaker( CloudsSpeaker::speakers[clip.person].firstName,
									   CloudsSpeaker::speakers[clip.person].lastName,
									   clip.name );
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
	if(key == ' ' && rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().isLoaded() ){
		if(rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().isPlaying()){
			rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().stop();
		}
		else{
			rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().play();
		}
	}
	
	if(key == 'P'){
		rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().play();
	}
	
	if(key == 'H'){
		ofShowCursor();
	}
	
	if(key == 'Q'){
		CloudsClip& clip = parser.getRandomClip(false,true);
		if(clip.hasQuestion()){
			rgbdVisualSystem.addQuestion(clip, clip.getTopicsWithQuestions()[0], clip.getQuestions()[0] );
		}
		else {
			cout << "clip " << clip.getLinkName() << " does not have a question!" << endl;
		}
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

- (std::string)convertString:(NSString *)string
{
    std::string cppString([string UTF8String], [string lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
    return cppString;
}

-(void)addIntervention:(id)sender{

    const char* interventionName =[interventionTextBox.stringValue UTF8String ];
    string name = interventionName;
    if(clipTable.selectedRow >= 0){
        CloudsClip& clip =parser.getAllClips()[[clipTable selectedRow]];
        cout<<" Adding intervention : "<<name<<" to clip "<<clip.getLinkName()<<endl;
    }
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