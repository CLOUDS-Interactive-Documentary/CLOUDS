#pragma once

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsRGBDVideoPlayer.h"
#include "CloudsVisualSystemManager.h"
#include "CloudsGlobal.h"
#include "CloudsMixer.h"

@interface testView : ofxCocoaGLView <NSTableViewDataSource, NSTableViewDelegate> {
	IBOutlet NSTableView* presetTable;
	
    CloudsMixer mixer;
	CloudsVisualSystemManager visualSystems;
	CloudsVisualSystem* currentVisualSystem;
	vector< CloudsVisualSystem* > playlist;
	
	bool isPlayingPlaylist;
	bool shouldPlayPlaylist;
	int playlistIndex;
	vector<int> riftIndeces;
	
	float maxSystemDuration;
	float systemStartTime;
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

- (IBAction) playPlaylist:(id)sender;

- (void) gotoNextSystem;

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;

@end
