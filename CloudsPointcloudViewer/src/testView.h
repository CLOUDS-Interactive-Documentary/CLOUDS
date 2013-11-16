#pragma once

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsFCPParser.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsVisualSystemRGBD.h"


@interface testView : ofxCocoaGLView <NSTableViewDataSource, NSTableViewDelegate> {
	IBOutlet NSTableView* clipTable;
    IBOutlet NSTextField* interventionTextBox;
    
	
	CloudsFCPParser parser;
	CloudsVisualSystemRGBD rgbdVisualSystem;
	
	CloudsClip currentClip;
	int currentClipIndex;

}

@property (assign) IBOutlet NSTableView *clipTable;
@property (assign) IBOutlet NSTextField *interventionTextBox;
//@property (assign) 

- (void)setup;
- (void)update;
- (void)draw;
- (void)exit;

- (void)keyPressed:(int)key;
- (void)keyReleased:(int)key;
- (void)mouseMoved:(NSPoint)p;
- (void)mouseDragged:(NSPoint)p button:(int)button;
- (void)mousePressed:(NSPoint)p button:(int)button;
- (void)mouseReleased:(NSPoint)p button:(int)button;
- (void)windowResized:(NSSize)size;
- (void)addIntervention:(id)sender;
- (IBAction)loadClipFromTable:(id)sender;



- (std::string)convertString:(NSString *)string;

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;

@end
