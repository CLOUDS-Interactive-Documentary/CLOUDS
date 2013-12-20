//
//  ClipSelector.h
//  CloudsFCPParser
//
//  Created by Surya Mattu on 24/06/13.
//
//

#pragma once
#import <Foundation/Foundation.h>
#include "CloudsFCPParser.h"
#include <set>
@class testView;

@interface ClipSelector : NSObject <NSTableViewDataSource,
									NSTableViewDelegate,
									NSTokenFieldDelegate,
									NSComboBoxDataSource> {

    IBOutlet NSTableView* keywordTable;
    IBOutlet NSTableView* clipTable;
    IBOutlet NSTableView* linkTable;
    IBOutlet NSTableView* suppressedTable;
    IBOutlet NSTableView* metaTable;
    IBOutlet NSTokenField* currentKeywords;
    IBOutlet NSTextField* revokedKeywords;
    IBOutlet NSTextField* sharedKeywords;
    
	IBOutlet NSComboBox* projectExamples;
    IBOutlet NSButton* showOnlyQuestions;
    
    vector<CloudsClip> selectedClips;
    vector<string> selectedKeywords;
    
    vector<CloudsLink> currentClipLinks;
    vector<CloudsLink> currentSuppressedLinks;
    vector<CloudsClip> currentMetaLinks;
	
    CloudsFCPParser* parser;
	CloudsClip dummyClip;
    testView* testViewParent;
	
	BOOL dontUpdateKeywords;
}

@property(nonatomic, readwrite) CloudsFCPParser* parser;
@property(nonatomic, assign) testView* testViewParent;


- (void) setup;

- (void) updateTables;
- (void) updateSelectedClips;
- (void) updateSelectedClip;
- (void) updateSharedKeywords;
- (void) deselectCurrentClip;

- (IBAction) saveLinks:(id)sender;
- (IBAction) updateKeywords:(id)sender;

- (IBAction) applySpecialKeyword:(id<NSUserInterfaceItemIdentification>)sender;
- (IBAction) attachProjectExample:(id)sender;

- (bool) isClipSelected;
- (bool) isKeywordSelected;

- (CloudsClip&) selectedClip;
- (CloudsLink&) selectedLink;
- (CloudsLink&) selectedSuppression;
- (CloudsClip&) selectedMeta;
- (void) selectClip:(CloudsClip&) clip;

- (IBAction) playDoubleClickedRow:(id)sender;

- (IBAction) linkFromMetaTable:(id)sender;
- (IBAction) suppressFromMetaTable:(id)sender;

- (IBAction) deleteLink:(id)sender;
- (IBAction) deleteSupression:(id)sender;

- (IBAction) suppressCurrentLink:(id)sender;
- (IBAction) linkCurrentSuppression:(id)sender;
- (IBAction) showQuestionsChanged:(id)sender;

- (BOOL) tableView:(NSTableView *)aTableView shouldEditTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;
- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;

- (BOOL) hasKeyword:(NSString*) keyword;
- (NSArray *)tokenField:(NSTokenField *)tokenField
completionsForSubstring:(NSString *)substring
           indexOfToken:(NSInteger)tokenIndex
    indexOfSelectedItem:(NSInteger *)selectedIndex;

//combo box
- (NSInteger)numberOfItemsInComboBox:(NSComboBox *)aComboBox;
- (id)comboBox:(NSComboBox *)aComboBox objectValueForItemAtIndex:(NSInteger)index;

- (NSUInteger)comboBox:(NSComboBox *)aComboBox indexOfItemWithStringValue:(NSString *)string;
- (NSString *)comboBox:(NSComboBox *)aComboBox completedString:(NSString *)string;
@end
