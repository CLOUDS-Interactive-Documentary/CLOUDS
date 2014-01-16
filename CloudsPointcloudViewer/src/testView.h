#pragma once

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsFCPParser.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsVisualSystemRGBD.h"
#include "CloudsHUDController.h"
#include "CloudsTransitionController.h"

@interface testView : ofxCocoaGLView <NSTableViewDataSource, NSTableViewDelegate> {
	IBOutlet NSTableView* clipTable;
    IBOutlet NSTextField* interventionTextBox;
    IBOutlet NSTextField* speakerVolTextBox;
	
	CloudsFCPParser parser;
	CloudsHUDController hud;
	CloudsVisualSystemRGBD rgbdVisualSystem;
	
	CloudsClip currentClip;
	int currentClipIndex;
	
	CloudsVisualSystemRGBD rgbd;
	CloudsVisualSystem::RGBDTransitionType type;
	CloudsTransitionController transitionController;

}

@property (assign) IBOutlet NSTableView *clipTable;
@property (assign) IBOutlet NSTextField *interventionTextBox;
@property (assign) IBOutlet NSTextField *speakerVolTextBox;

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

- (void)updateSpeakerVolumeTextField;
- (IBAction)addIntervention:(id)sender;
- (IBAction)updateSpeakerVolume:(id)sender;
- (IBAction)loadClipFromTable:(id)sender;



- (std::string)convertString:(NSString *)string;

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;

@end
