#pragma once

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsFCPParser.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsVisualSystemRGBD.h"
#include "CloudsHUDController.h"
#include "CloudsTransitionController.h"
#include "CloudsSound.h"

@interface testView : ofxCocoaGLView <NSTableViewDataSource, NSTableViewDelegate> {

	IBOutlet NSTableView* clipTable;
	IBOutlet NSTableView* trackTable;
	
    IBOutlet NSTextField* interventionTextBox;
    IBOutlet NSTextField* speakerVolTextBox;
    IBOutlet NSTextField* trackVolTextBox;

	
	CloudsFCPParser parser;
	CloudsHUDController hud;
	CloudsVisualSystemRGBD rgbdVisualSystem;
	
	CloudsClip* currentClip;
	int currentClipIndex;
	
	CloudsVisualSystem::RGBDTransitionType type;
	CloudsTransitionController transitionController;

	CloudsSound sound;

}

@property (assign) IBOutlet NSTableView *clipTable;
@property (assign) IBOutlet NSTableView *trackTable;

@property (assign) IBOutlet NSTextField *interventionTextBox;
@property (assign) IBOutlet NSTextField *speakerVolTextBox;
@property (assign) IBOutlet NSTextField *trackVolTextBox;

- (void)setup;
- (void)update;
- (void)draw;
- (void)exit;

- (void)updateTransitions;

- (void)keyPressed:(int)key;
- (void)keyReleased:(int)key;
- (void)mouseMoved:(NSPoint)p;
- (void)mouseDragged:(NSPoint)p button:(int)button;
- (void)mousePressed:(NSPoint)p button:(int)button;
- (void)mouseReleased:(NSPoint)p button:(int)button;
- (void)windowResized:(NSSize)size;

- (IBAction)addIntervention:(id)sender;

- (void)updateSpeakerVolumeTextField:(id)sender;
- (void)updateTrackVolumeTextField:(id)sender;

- (IBAction)updateSpeakerVolume:(id)sender;
- (IBAction)updateTrackVoume:(id)sender;

- (IBAction)loadClipFromTable:(id)sender;
- (IBAction)loadTrackFromTable:(id)sender;


- (std::string)convertString:(NSString *)string;

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;

@end
