//
//  ClipSelector.h
//  CloudsFCPParser
//
//  Created by Surya Mattu on 24/06/13.
//
//

#pragma once
#import <Foundation/Foundation.h>
//#import <Cocoa/Cocoa.h>

//#include "ofMain.h"
#include "CloudsFCPParser.h"
@class testView;

@interface ClipSelector : NSObject <NSTableViewDataSource, NSTableViewDelegate, NSTokenFieldDelegate> {

    IBOutlet NSTableView* keywordTable;
    IBOutlet NSTableView* clipTable;
    IBOutlet NSTableView* linkTable;
    IBOutlet NSTableView* suppressedTable;
    IBOutlet NSTableView* metaTable;
    IBOutlet NSTokenField* currentKeywords;

    IBOutlet NSTextField* startQuestion;
    IBOutlet NSButton* showOnlyQuestions;
    
    vector<CloudsClip> selectedClips;
    vector<string> selectedKeywords;
    
    vector<CloudsLink> currentClipLinks;
    vector<CloudsLink> currentSuppressedLinks;
    vector<CloudsClip> currentMetaLinks;
	
    CloudsFCPParser* parser;
    testView* testViewParent;
	
	BOOL dontUpdateClips;
}
@property(nonatomic, readwrite) CloudsFCPParser* parser;
@property(nonatomic, assign) testView* testViewParent;

- (void) setup;
//- (void) playClip:(CloudsClip&) clip;
- (void) updateTables;
- (void) updateSelectedClips;

- (IBAction) saveLinks:(id)sender;

- (bool) isClipSelected;
- (bool) isKeywordSelected;

- (CloudsClip) selectedClip;
- (CloudsLink&) selectedLink;
- (CloudsLink&) selectedSuppression;
- (CloudsClip&) selectedMeta;

- (IBAction) playDoubleClickedRow:(id)sender;
- (IBAction) setQuestionText:(id)sender;

- (IBAction) linkFromMetaTable:(id)sender;
- (IBAction) suppressFromMetaTable:(id)sender;

- (IBAction) deleteLink:(id)sender;
- (IBAction) deleteSupression:(id)sender;

- (IBAction) suppressCurrentLink:(id)sender;
- (IBAction) linkCurrentSuppression:(id)sender;
- (IBAction) showQuestionsChanged:(id)sender;

- (BOOL) tableView:(NSTableView *)aTableView shouldEditTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification;
- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors;
- (NSArray *)tokenField:(NSTokenField *)tokenField completionsForSubstring:(NSString *)substring indexOfToken:(NSInteger)tokenIndex indexOfSelectedItem:(NSInteger *)selectedIndex;


@end
