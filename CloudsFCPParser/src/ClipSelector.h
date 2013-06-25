//
//  ClipSelector.h
//  CloudsFCPParser
//
//  Created by Surya Mattu on 24/06/13.
//
//

#import <Foundation/Foundation.h>
#include "CloudsFCPParser.h"
@class testView;

@interface ClipSelector : NSObject <NSTableViewDataSource, NSTableViewDelegate, NSTokenFieldDelegate> {

    IBOutlet NSTableView* keywordTable;
    IBOutlet NSTableView* clipTable;
    IBOutlet NSTableView* linkTable;
    IBOutlet NSTableView* suppressedTable;
    IBOutlet NSTokenField* currentKeywords;

    IBOutlet NSTextField* startQuestion;
    IBOutlet NSTextField* currentClipLabel;
    IBOutlet NSTextField* linkText;
    IBOutlet NSButton* showOnlyQuestions;
    
    vector<CloudsClip> selectedClips;
    vector<string> selectedKeywords;
    
    vector<CloudsLink> currentClipLinks;
    vector<CloudsLink> currentSuppressedLinks;
    CloudsFCPParser* parser;
    testView* testViewParent;
    
    
    
}

- (void)setup;
- (void) playClip:(CloudsClip&) clip;
- (void) updateTables;
@property(nonatomic, readwrite) CloudsFCPParser* parser;
@property(nonatomic, assign) testView* testViewParent;

- (IBAction) playDoubleClickedRow:(id)sender;
- (IBAction) createLink:(id)sender;
- (IBAction) suppressLink:(id)sender;
- (CloudsClip&) selectedClip;
- (IBAction) createLink:(id)sender;
- (IBAction) deleteLink:(id)sender;
- (void)keyPressed:(int)key;
-(IBAction)setStartText:(id)sender;
- (IBAction) saveLinks:(id)sender;
- (IBAction)suppressLinkModifier:(id)sender;
- (IBAction) showQuestiosnChanged:(id)sender;
- (BOOL)tableView:(NSTableView *)aTableView shouldEditTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;
-(bool) isClipSelected;
- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification;
- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors;
- (NSArray *)tokenField:(NSTokenField *)tokenField completionsForSubstring:(NSString *)substring indexOfToken:(NSInteger)tokenIndex indexOfSelectedItem:(NSInteger *)selectedIndex;


@end
