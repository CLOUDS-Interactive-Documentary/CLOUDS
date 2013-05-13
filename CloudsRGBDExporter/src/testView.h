#pragma once

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsFCPParser.h"
#include "ofxRGBDGPURenderer.h"
#include "ofxRGBDPlayer.h"
#include "CloudsClipExportManager.h"

@interface testView : ofxCocoaGLView <NSTableViewDataSource, NSTableViewDelegate> {
	
	IBOutlet NSTableView* clipTable;

	IBOutlet NSProgressIndicator* totalProgress;
	IBOutlet NSProgressIndicator* clipProgress1;
	IBOutlet NSProgressIndicator* clipProgress2;
	IBOutlet NSProgressIndicator* clipProgress3;
	IBOutlet NSProgressIndicator* clipProgress4;
	IBOutlet NSProgressIndicator* clipProgress5;
	IBOutlet NSProgressIndicator* clipProgress6;
	IBOutlet NSProgressIndicator* clipProgress7;
	IBOutlet NSProgressIndicator* clipProgress8;

	NSProgressIndicator* progressBars[8];
	
	CloudsFCPParser parser;
	ofxRGBDPlayer player;
	ofxRGBDGPURenderer renderer;
	vector<CloudsClipExportManager*> exportManagers;
	
	bool exporting;
	bool startExport;
	
	vector<CloudsClip> selectedClips;

	int currentClipIndex;
}

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

- (IBAction) exportSelection:(id)sender;
- (IBAction) cancelExport:(id)sender;

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;

@end
