#import "testView.h"
#include "CloudsGlobal.h"
#include "CloudsSpeaker.h"
#include "CloudsVisualSystem.h"

@implementation testView
@synthesize clipTable;
@synthesize trackTable;

@synthesize interventionTextBox;
@synthesize speakerVolTextBox;
@synthesize trackVolTextBox;

- (void)setup
{

	ofSetVerticalSync(true);
	
	ofBackground(22);

	parser.loadFromFiles();
	sound.setup();
	
	if(ofFile::doesFileExist(GetCloudsDataPath() + "CloudsMovieDirectory.txt")){
		parser.setCombinedVideoDirectory(ofBufferFromFile(GetCloudsDataPath() + "CloudsMovieDirectory.txt").getText());
        cout<<"Clouds Directory is pointing to "<<ofBufferFromFile(GetCloudsDataPath	() + "CloudsMovieDirectory.txt").getText()<<endl;
	}
	else{
		ofSystemAlertDialog("Could not find movie file path. Create a file called CloudsMovieDirectory.txt that contains one line, the path to your movies folder");
	}



	[clipTable setTarget:self];
	[clipTable setDoubleAction:@selector(loadClipFromTable:)];
	[clipTable reloadData];

	[trackTable setTarget:self];
	[trackTable setDoubleAction:@selector(loadTrackFromTable:)];
	[trackTable reloadData];

    [interventionTextBox setTarget:self];   
	[speakerVolTextBox setTarget:self];
	[trackVolTextBox setTarget:self];
    
//    rgbdVisualSystem.setNumSamples(4);
	rgbdVisualSystem.setDrawToScreen(false);
	rgbdVisualSystem.setup();
    
	hud.setup();

#ifdef OCULUS_RIFT
//  rgbdVisualSystem.hud = &hud;
//  rgbdVisualSystem.setupHUDGui();
//	rgbdVisualSystem.loadPresetGUISFromName("RGBD_OC_POINTS");
#else
//	rgbdVisualSystem.loadPresetGUISFromName("RGBDMain");
//	rgbdVisualSystem.loadPresetGUISFromName("Working");
#endif
	srand(ofGetSeconds());
	
	[self loadClip: parser.getRandomClip(true, false)];
	

	rgbdVisualSystem.playSystem();
	
	type = CloudsVisualSystem::FLY_THROUGH;
}

- (void)update
{
	
	[self updateTransitions ];
    
    rgbdVisualSystem.getRGBDVideoPlayer().forceStop = false;
    rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().setLoopState(OF_LOOP_NORMAL);
    rgbdVisualSystem.getRGBDVideoPlayer().maxVolume = 1.;
	
	hud.update();
	
}

- (void)draw
{
    ofBackground(0);
    
	//ofHideCursor();
	
	rgbdVisualSystem.selfPostDraw();

#ifndef OCULUS_RIFT
	//hud.draw();
#endif
    
    CloudsVisualSystem::getRGBDVideoPlayer().drawSubtitles(
        CloudsVisualSystem::getStaticRenderTarget().getWidth()/2.,
        CloudsVisualSystem::getStaticRenderTarget().getHeight()*0.8);

}

- (void) loadClipFromTable:(id)sender
{
	if(clipTable.selectedRow >= 0){
		[self loadClip: parser.getAllClips()[ clipTable.selectedRow ] ];
	}
}

- (IBAction)loadTrackFromTable:(id)sender
{
	if(trackTable.selectedRow >= 0){
		sound.playImmediately(sound.renderedTracks[ trackTable.selectedRow ]);
	}
}

- (IBAction)loadClip:(CloudsClip&)clip
{
	if(clip.hasMediaAsset && clip.voiceOverAudio && rgbdVisualSystem.getRGBDVideoPlayer().setupVO(clip.voiceOverAudioPath) ){
		
		rgbdVisualSystem.getRGBDVideoPlayer().swapAndPlay();
		rgbdVisualSystem.setupSpeaker(CloudsSpeaker::speakers[clip.person].firstName,
									  CloudsSpeaker::speakers[clip.person].lastName,
									  clip.name );
		
		currentClip = clip;
        // EZ: Temp to get HUD content
        hud.respondToClip(clip);
	}
	else if(clip.hasMediaAsset && rgbdVisualSystem.getRGBDVideoPlayer().setup( clip.combinedVideoPath, clip.combinedCalibrationXMLPath, clip.combinedSRTPath, 1,clip.speakerVolume) ){
		cout<<"clip.speakerVolume : "<<clip.speakerVolume<<endl;
		rgbdVisualSystem.getRGBDVideoPlayer().swapAndPlay();
		rgbdVisualSystem.setupSpeaker( CloudsSpeaker::speakers[clip.person].firstName,
									   CloudsSpeaker::speakers[clip.person].lastName,
									   clip.name );
		currentClip = clip;
        // EZ: Temp to get HUD content
        hud.respondToClip(clip);
	}
	else{
		ofLogError() << "CloudsPlaybackController::playClip -- folder " << clip.combinedVideoPath << " is not valid";
	}	
}

- (void)exit
{
    rgbdVisualSystem.exit();
    parser.saveSpeakersVolume(GetCloudsDataPath()+"sound/SpeakersVolume.txt");
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
	if(key == 'F')
	{
		ofToggleFullscreen();
	}
	
	if(key == 'O'){
		rgbdVisualSystem.StopEditTransitionMode();//<-- used to revert the camera  to the rgbd camera. it only matters in "Edit" mode
		transitionController.transitionToVisualSystem(1.0, 1.0);
	}
	if(key == 'I'){
		rgbdVisualSystem.StopEditTransitionMode();//<-- used to revert the camera  to the rgbd camera. it only matters in "Edit" mode
		transitionController.transitionToInterview(1.0, 1.0);
	}
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


- (void)updateTransitions
{
	transitionController.update();
	
	float crossfadeValue = transitionController.getFadeValue();
	rgbdVisualSystem.visualSystemFadeValue = crossfadeValue;
	
	//cout << "visual system fade value is " << rgbd.visualSystemFadeValue << endl;
	
	if(transitionController.transitioning){
		ofLogNotice("testApp::updateTransitions") << transitionController.getCurrentStateDescription() << " TRANSITIONING: " << transitionController.getInterviewTransitionPoint();
	}
	
	rgbdVisualSystem.updateTransition( transitionController.getInterviewTransitionPoint() );
	
	if(transitionController.isStateNew()){
		
		if(transitionController.getCurrentState() == TRANSITION_INTERVIEW_OUT){
			
			ofLogNotice("testApp::updateTransitions") << "Going to INTERVIEW OUT";
			
			//rgbd.startTransitionOut( type );
			rgbdVisualSystem.startCurrentTransitionOut();
		}
		else if(transitionController.getCurrentState() == TRANSITION_VISUALSYSTEM_IN){
			
			//ofLogNotice("testApp::updateTransitions") << "Going to VISUAL SYSTEM IN";
			
			rgbdVisualSystem.transtionFinished();
			rgbdVisualSystem.stopSystem();
		}
		else if(transitionController.getCurrentState() == TRANSITION_VISUALSYSTEM_OUT){
			// no need to do anything special, the crossfade value will take care of this
			ofLogNotice("testApp::updateTransitions") << "Going to VISUAL SYSTEM OUT";
		}
		else if(transitionController.getCurrentState() == TRANSITION_INTERVIEW_IN){
			
			ofLogNotice("testApp::updateTransitions") << "Going to INTERVIEW IN";
			
			rgbdVisualSystem.playSystem();
			//rgbd.startTransitionIn( type );
			rgbdVisualSystem.startCurrentTransitionIn();
		}
		else if(transitionController.getCurrentState() == TRANSITION_IDLE){
			
			ofLogNotice("testApp::updateTransitions") << "Going to IDLE";
			
			rgbdVisualSystem.transtionFinished();
		}
	}
}


- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	if(aTableView == clipTable){
		return parser.getAllClips().size();
	}
	else if(aTableView == trackTable){
		return sound.renderedTracks.size();
	}
}

- (std::string)convertString:(NSString *)string
{
    std::string cppString([string UTF8String], [string lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
    return cppString;
}

- (IBAction)addIntervention:(id)sender{

    const char* interventionName =[interventionTextBox.stringValue UTF8String ];
    string name = interventionName;

    if(clipTable.selectedRow >= 0){
        CloudsClip& clip =parser.getAllClips()[[clipTable selectedRow]];
        cout<<" Adding intervention : "<<name<<" to clip "<<clip.getLinkName()<<endl;
    }
}

- (IBAction)updateSpeakerVolume:(id)sender{

    float speakerVol = speakerVolTextBox.floatValue;

    if(clipTable.selectedRow >= 0){
        CloudsClip& clip =parser.getAllClips()[[clipTable selectedRow]];
        parser.setSpeakerVolume(clip.person, speakerVol);
        cout<<" Updating vol for speaker : "<<clip.person<<" new vol : "<<speakerVol<<endl;
        parser.saveSpeakersVolume(GetCloudsDataPath()+"sound/SpeakersVolume.txt");
        
        rgbdVisualSystem.getRGBDVideoPlayer().currentMaxVolume = rgbdVisualSystem.getRGBDVideoPlayer().maxVolume *  speakerVol;
    }
}

- (IBAction)updateTrackVoume:(id)sender
{
	if(trackTable.selectedRow >= 0){
		sound.setMixVolumeForTrack(sound.renderedTracks[ trackTable.selectedRow ], trackVolTextBox.floatValue);
		sound.saveMixLevels();
		[trackTable reloadData];
	}
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    if (aNotification.object == clipTable) {
        [self updateSpeakerVolumeTextField:self];
    }
	else if(aNotification.object == trackTable){
		[self updateTrackVolumeTextField:self];
	}
}

- (void) updateSpeakerVolumeTextField:(id)sender
{
	if(clipTable.selectedRow >= 0){
		CloudsClip& clip = parser.getAllClips()[[clipTable selectedRow]];
		speakerVolTextBox.floatValue = clip.getSpeakerVolume();
	}
}

- (void)updateTrackVolumeTextField:(id)sender
{
	if(trackTable.selectedRow >= 0){
		trackVolTextBox.floatValue = sound.mixVolumeForTrack(sound.renderedTracks[ trackTable.selectedRow ] );
	}

}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{

	if(aTableView == clipTable){
		if([@"person" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: parser.getAllClips()[rowIndex].person.c_str() ];
		}
		else if([@"clip" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: parser.getAllClips()[rowIndex].name.c_str() ];
		}
		else if([@"combined" isEqualToString:aTableColumn.identifier]){
			return parser.getAllClips()[rowIndex].hasMediaAsset ? @"YES" : @"NO";
		}
	}
	else if(aTableView == trackTable){
		if([@"track" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: sound.renderedTracks[rowIndex].c_str() ];
		}
		else if([@"volume" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: ofToString( sound.mixVolumeForTrack(sound.renderedTracks[rowIndex]) ).c_str() ];
		}

	}
	return @"";
	

}

@end