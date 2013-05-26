#pragma once

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsFCPParser.h"
#include "ofxRGBDGPURenderer.h"
#include "ofxRGBDPlayer.h"
#include "CloudsClipExportManager.h"
#include "ofxGameCamera.h"
#include "ofxUI.h"

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
	
	ofxGameCamera cam;
	CloudsFCPParser parser;
	ofxRGBDPlayer player;
	ofxRGBDGPURenderer renderer;
	vector<CloudsClipExportManager*> exportManagers;
	
	bool exporting;
	bool startExport;
	bool saveAlignment;
	bool pause;
	bool resetCamera;
	
	ofFbo framebuffer;
	vector<CloudsClip> selectedClips;

	ofxUICanvas* gui;
	
	int currentClipIndex;
	
    string  exportFolder;
    
	//correction variables, loaded for each clip
	ofVec3f translate;
	ofVec3f rotate;
	ofVec2f scale;
	float minDepth;
	float maxDepth;
	ofRectangle camRect;

	CloudsClip loadedClip;
}

@property (assign) IBOutlet NSTableView *clipTable;

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

- (IBAction)loadClipForAlignment:(id)sender;
- (IBAction) exportSelection:(id)sender;
- (IBAction) cancelExport:(id)sender;

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;

@end
