#pragma once

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsFCPParser.h"
#include "CloudsVisualSystemManager.h"

@interface testView : ofxCocoaGLView <NSTableViewDataSource, NSTableViewDelegate, NSTokenFieldDelegate> {
	IBOutlet NSTokenField* currentKeywords;
	
	IBOutlet NSTableView* presetTable;
	IBOutlet NSTableView* clipTable;
	IBOutlet NSTableView* allKeywordTable;
	IBOutlet NSTableView* allClipTable;
	
	IBOutlet NSTextField* keywordPercent;
	IBOutlet NSTextField* clipPercent;
	
	IBOutlet NSTextField* notesText;
	IBOutlet NSComboBox* grade;
	IBOutlet NSButton* enabledBox;
	
	bool shouldPlaySelectedRow;
	
	CloudsFCPParser parser;
	CloudsVisualSystemManager visualSystems;
	CloudsVisualSystem* currentVisualSystem;
	CloudsVisualSystemPreset* selectedPreset;
	vector<CloudsClip> associatedClips;
	vector<string> associatedKeywords;
	
	float percentKeywordsTagged;
	float percentClipsTagged;
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

- (void) updateCounts;

- (IBAction) updatePresets:(id)sender;
- (IBAction) updateKeywords:(id)sender;
- (IBAction) suppressClip:(id)sender;
- (IBAction) unsuppressClip:(id)sender;

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification;
- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors;

- (void)playDoubleClickedRow:(id)sender;

- (NSArray *)tokenField:(NSTokenField *)tokenField
completionsForSubstring:(NSString *)substring
		   indexOfToken:(NSInteger)tokenIndex
	indexOfSelectedItem:(NSInteger *)selectedIndex;

- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor;
- (NSArray *)tokenField:(NSTokenField *)tokenField shouldAddObjects:(NSArray *)tokens atIndex:(NSUInteger)index;
- (void) updateAssociatedClips;
- (BOOL) hasKeyword:(NSString*) keyword;
- (vector<string>) entries:(vector<string>&)a sharedWith:(vector<string>&)b;

@end
