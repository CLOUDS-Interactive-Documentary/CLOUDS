#pragma once

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsFCPParser.h"
#include "CloudsFCPVisualizer.h"
#include "ofxTimeline.h"
#include "ofxUI.h"

@class ViewerApp;
@interface testView : ofxCocoaGLView <NSTableViewDataSource, NSTableViewDelegate, NSTokenFieldDelegate> {
    IBOutlet NSTableView* keywordTable;
    IBOutlet NSTableView* clipTable;
    IBOutlet NSTableView* linkTable;
    IBOutlet NSTextField* currentClipLabel;
    
    IBOutlet NSTokenField* currentKeywords;
    IBOutlet NSTextField* linkText;
	
	IBOutlet NSTextField* seedKeyword;
	
	IBOutlet ViewerApp* viewerApp;
	
    CloudsFCPParser parser;
    CloudsFCPVisualizer visualizer;
    
    bool updatePhysics;
    vector<string> selectedKeywords;
    ClipMarker currentPlayingClip;
    BOOL clipLoaded;
	
    vector<ClipMarker> selectedClips;
    vector<CloudsLink> currentClipLinks;

    int clipEndFrame;
    ofVideoPlayer preview;
    ofxTimeline inpoint;

	float physSpringTension;
	float physSpringLength;
	ofxUICanvas* gui;

}

@property(nonatomic,readonly) int clipEndFrame;
@property(nonatomic,readonly) ofVideoPlayer& preview;

- (void)setup;
- (void)update;
- (void)draw;
- (void)exit;

- (IBAction) setXMLFolder:(id)sender;
- (IBAction) refreshXML:(id)sender;
- (IBAction) createLink:(id)sender;
- (IBAction) playDoubleClickedRow:(id)sender;
- (IBAction) saveLinks:(id)sender;
- (IBAction) deleteLink:(id)sender;
- (IBAction) regenerateGraph:(id)sender;
- (IBAction) unloadVideo:(id)sender;

- (void)keyPressed:(int)key;
- (void)keyReleased:(int)key;
- (void)mouseMoved:(NSPoint)p;
- (void)mouseDragged:(NSPoint)p button:(int)button;
- (void)mousePressed:(NSPoint)p button:(int)button;
- (void)mouseReleased:(NSPoint)p button:(int)button;
- (void)windowResized:(NSSize)size;

- (BOOL)tableView:(NSTableView *)aTableView shouldEditTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;

- (ClipMarker&) selectedClip;

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification;
- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors;
- (NSArray *)tokenField:(NSTokenField *)tokenField completionsForSubstring:(NSString *)substring indexOfToken:(NSInteger)tokenIndex indexOfSelectedItem:(NSInteger *)selectedIndex;


@end
