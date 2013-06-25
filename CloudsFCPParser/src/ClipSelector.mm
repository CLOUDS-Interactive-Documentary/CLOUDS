//
//  ClipSelector.m
//  CloudsFCPParser
//
//  Created by Surya Mattu on 24/06/13.
//
//

#import "ClipSelector.h"

@implementation ClipSelector
@synthesize parser;
@synthesize testViewParent;


-(void) setup
{
    for (NSTableColumn *tableColumn in keywordTable.tableColumns) {
        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
        [tableColumn setSortDescriptorPrototype:sortDescriptor];
    }
    
    for (NSTableColumn *tableColumn in clipTable.tableColumns ) {
        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
        [tableColumn setSortDescriptorPrototype:sortDescriptor];
    }
    
    cout << "setting action " << [clipTable description] << endl;
    
    [clipTable setTarget:self];
    [clipTable setDoubleAction:@selector(playDoubleClickedRow:)];
    [clipTable reloadData];
    [keywordTable reloadData];
    
}
//update tables
// [testViewParent updateViews];
- (IBAction) playDoubleClickedRow:(id)sender
{
	
	CloudsClip clip;
	if(sender == clipTable && clipTable.selectedRow >= 0){
        clip = [self selectedClip];
        currentClipLabel.stringValue = [NSString stringWithUTF8String:clip.getLinkName().c_str()];
        currentClipLinks = parser->getLinksForClip(clip.getLinkName());
        currentSuppressedLinks = parser->getSuppressionsForClip(clip.getLinkName());
        [linkTable reloadData];
        [suppressedTable reloadData];
	}
	else{
		//bail!
		return;
	}
	cout<<"im in playDoubleClickedRow in ClipSelector"<<endl;
    [testViewParent playClip:clip];
    
    
	

    
}


- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
    if(aTableView == keywordTable){
        if(parser == NULL) return 0;
        return (NSInteger)parser->getAllKeywords().size();
    }
    else if(aTableView == clipTable){
        if(parser == NULL) return 0;
        return (NSInteger) (selectedClips.size() == 0) ? parser->getAllClips().size() : selectedClips.size();
    }
    else if(aTableView == linkTable){
        return currentClipLinks.size();
    }
    else if(aTableView == suppressedTable){
        return currentSuppressedLinks.size();
    }
}


- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    
    if(aTableView == keywordTable){
        string keyword = parser->getAllKeywords()[rowIndex];
        if([@"occurrence" isEqualToString:aTableColumn.identifier]){
            return [NSNumber numberWithInt: parser->occurrencesOfKeyword(keyword)];
        }
        else{
            return [NSString stringWithCString:keyword.c_str() encoding:NSUTF8StringEncoding];
        }
    }
    else if(aTableView == clipTable){
		CloudsClip& m = (selectedClips.size() == 0) ? parser->getAllClips()[rowIndex] : selectedClips[rowIndex];

		if([@"Links" isEqualToString:aTableColumn.identifier]){
			return [NSNumber numberWithInt: parser->getLinksForClip( m.getLinkName() ).size()];
		}
        else if([@"Suppressions" isEqualToString:aTableColumn.identifier]){
            return [NSNumber numberWithInt:parser->getSuppressionsForClip(m.getLinkName()).size()];
        }
        else if([@"Starting Question" isEqualToString:aTableColumn.identifier]){
            //return [NSNumber numberWithInt:parser.getSuppressionsForClip(m.getLinkName()).size()];
            //string
            string s = m.getStartingQuestion();
            //cout<<"Starting: "<<s<<endl;
            return [NSString stringWithUTF8String:s.c_str()];
        }
		else {
			//CloudsClip& m = [self selectedClip];
			string linkString = m.person + " - " + m.name + " - " + m.clip + ": [" + ofToString(m.startFrame) + "," + ofToString(m.endFrame) + "]";
			return [NSString stringWithUTF8String:linkString.c_str()];
		}
    }

    else if(aTableView == linkTable){
        string clipTableSourceEntry = currentClipLinks[rowIndex].targetName;
        if(currentClipLinks[rowIndex].startFrame != -1 && currentClipLinks[rowIndex].endFrame != -1){
            clipTableSourceEntry += "[" + ofToString(currentClipLinks[rowIndex].startFrame) + " - " + ofToString(currentClipLinks[rowIndex].endFrame) + "]";
        }
        return [NSString stringWithUTF8String:clipTableSourceEntry.c_str()];
    }
    else if(aTableView == suppressedTable){
        string suppressedTableEntry = currentSuppressedLinks[rowIndex].targetName;
        if(currentSuppressedLinks[rowIndex].startFrame != -1 && currentSuppressedLinks[rowIndex].endFrame!= -1){
            suppressedTableEntry += "[" + ofToString(currentSuppressedLinks[rowIndex].startFrame) + " - " + ofToString(currentSuppressedLinks[rowIndex].endFrame) + "]";
        }
        return [NSString stringWithUTF8String:suppressedTableEntry.c_str()];
    }
\
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    if(aNotification.object == keywordTable && showOnlyQuestions.state ==  NSOffState){
        
        
        selectedKeywords.clear();
        selectedClips.clear();
        
        if(keywordTable.selectedRow >= 0){
            
            NSUInteger idx = [keywordTable.selectedRowIndexes firstIndex];
            while (idx != NSNotFound) {
                // do work with "idx"
                selectedKeywords.push_back(parser->getAllKeywords()[idx]);
                
                // get the next index in the set
                idx = [keywordTable.selectedRowIndexes indexGreaterThanIndex:idx];
            }
            selectedClips = parser->getClipsWithKeyword(selectedKeywords);
        }
        
        [clipTable reloadData];
    }
    else if(aNotification.object == clipTable){
        if(clipTable.selectedRow < 0){
            return;
        }
        
        
        CloudsClip& m = [self selectedClip];
        
        string keywordList = "";
        currentKeywords.stringValue = [NSString stringWithUTF8String:ofJoinString(m.keywords, ",").c_str()];
        
        linkText.stringValue = [NSString stringWithUTF8String:m.getLinkName().c_str()];
        //        suppressedTable.stringValue = [NSString stringWithUTF8String:parser.]
        startQuestion.stringValue = [NSString stringWithUTF8String:m.getStartingQuestion().c_str()];
        currentClipLabel.stringValue = [NSString stringWithUTF8String:m.getLinkName().c_str()];
        currentClipLinks = parser->getLinksForClip(m.getLinkName());
        currentSuppressedLinks = parser->getSuppressionsForClip(m.getLinkName());
        [linkTable reloadData];
        [suppressedTable reloadData];
    }
    else if(aNotification.object == linkTable){
        if(linkTable.selectedRow < 0){
            return;
        }
        
        vector<CloudsClip>& searchClips = (selectedClips.size() == 0) ? parser->getAllClips() : selectedClips;
        //CloudsClip& m = [self selectedClip];
        string targetClip = currentClipLinks[ linkTable.selectedRow ].targetName;
        for(int i = 0; i < searchClips.size(); i++){
            if(searchClips[i].name == targetClip){
                NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:i];
                [clipTable selectRowIndexes:indexSet byExtendingSelection:NO];
                break;
            }
        }
    }
    else if(aNotification.object == suppressedTable){
        if(suppressedTable.selectedRow<0){
            return;
        }
        
        
        
    }
}

- (CloudsClip&) selectedClip
{
    
    if(selectedClips.size() > 0){
        CloudsClip& c=selectedClips[clipTable.selectedRow];
        cout<<"Selected clip from source table "<<c.getLinkName()<<","<<c.hasStartingQuestion()<<endl;
        return c;
    }
    else{
        CloudsClip& c =parser->getAllClips()[clipTable.selectedRow] ;
        cout<<"Selected clip from all clips"<<c.getLinkName()<<","<<c.hasStartingQuestion()<<endl;

        return c;
    }
    


    
}


- (NSArray *)tokenField:(NSTokenField *)tokenField
completionsForSubstring:(NSString *)substring
           indexOfToken:(NSInteger)tokenIndex
    indexOfSelectedItem:(NSInteger *)selectedIndex
{
    
    //    cout << "asking for completions..." << endl;
    NSMutableArray* completions = [NSMutableArray array];
    for(int i = 0; i < parser->getAllKeywords().size(); i++){
        NSString* stringKeyword = [NSString stringWithUTF8String:parser->getAllKeywords()[i].c_str()];
        if([stringKeyword rangeOfString:substring].location == 0  && ![self hasKeyword:stringKeyword]){
            [completions addObject:stringKeyword];
        }
    }
    
    //    cout << "found " << completions.count << endl;
    return completions;
}

- (BOOL) hasKeyword:(NSString*) keyword
{
    NSString* currentString = currentKeywords.stringValue;
    NSArray* stringSet = [currentString componentsSeparatedByCharactersInSet: currentKeywords.tokenizingCharacterSet];
    for( NSString* str in stringSet){
        if([keyword isEqualToString:str]){
            return YES;
        }
    }
    return NO;
}

- (IBAction) createLink:(id)sender
{

		
		NSLog(@"clip is loaded and row is selected");
		
//		[testViewParent linkClip:testViewParent->currentPlayingClip toClip:[self selectedClip] ];
//		testViewParent->
//		currentClipLinks = parser->getLinksForClip(testViewParent->currentPlayingClip.getLinkName());
    
		[clipTable reloadData];
        [clipTable reloadData];
		[suppressedTable reloadData];
		[linkTable reloadData];
        
		cout << "after creating link the current clip has " << currentClipLinks.size() << endl;
	
}

//CONFUSING: delete link is called from the link table, remove link called from the graph
- (IBAction) deleteLink:(id)sender
{
    if(linkTable.selectedRow >= 0){
        cout << "removing link " << linkTable.selectedRow << " from " << [self selectedClip].getLinkName() << endl;
        parser->removeLink([self selectedClip].getLinkName() , linkTable.selectedRow);
        currentClipLinks = parser->getLinksForClip([self selectedClip].getLinkName());
        
        [linkTable reloadData];
		[suppressedTable reloadData];
        [clipTable reloadData];
		
		
		[self saveLinks:self];
    }
}

- (IBAction) setStartText:(id)sender{
    //button pressed
    CloudsClip& m = [self selectedClip];
    CloudsClip& n = parser->getClipWithLinkName(m.getLinkName());
    string q =   std::string([startQuestion.stringValue UTF8String]);
    
    n.setStartingQuestion(q);
    cout<<"Set the question for clip"<<n.getLinkName()<<"::"<<n.getStartingQuestion()<<endl;
    //get the txt from textfield, get currently select clip, and set it
    [clipTable reloadData];
    
}
- (IBAction) saveLinks:(id)sender
{
	if(ofDirectory("../../../CloudsData/links/").exists()){
		parser->saveLinks("../../../CloudsData/links/clouds_link_db.xml");
	}
	else{
		parser->saveLinks("clouds_link_db.xml");
	}
}
- (IBAction) suppressLink:(id)sender{
    cout<<"suppressedTable"<<endl;
    string suppressedSourceName = currentSuppressedLinks[suppressedTable.selectedRow].sourceName;
    string suppressedTargetName = currentSuppressedLinks[suppressedTable.selectedRow].targetName;
    
    cout<<"deleting suppression :" << suppressedSourceName << " going to " << suppressedTargetName << endl;
    parser->unsuppressConnection(suppressedSourceName, suppressedTargetName);
    
    currentClipLinks = parser->getLinksForClip(suppressedSourceName);
    currentSuppressedLinks = parser->getSuppressionsForClip(suppressedSourceName);
    
    [clipTable reloadData];
    [linkTable reloadData];
    [suppressedTable reloadData];
    [self saveLinks:self];
}
- (IBAction)suppressLinkModifier:(id)sender
{
    if(linkTable.selectedRow>=0 &&suppressedTable.selectedRow<0){
        
        string linkSourceName = currentClipLinks[linkTable.selectedRow].sourceName;
        string linkTargetName = currentClipLinks[linkTable.selectedRow].targetName;
        
        cout<<"swapping link for suppression : clip " << linkSourceName << " going to " << linkTargetName << endl;
        parser->removeLink(linkSourceName, linkTargetName);
        
        //        [clipTableSource.resignFirstResponder
        
        [testViewParent suppressLink:parser->getClipWithLinkName(linkSourceName)
                    toClip:parser->getClipWithLinkName(linkTargetName) ];
        
        currentClipLinks = parser->getLinksForClip(linkSourceName);
        currentSuppressedLinks = parser->getSuppressionsForClip(linkSourceName);
        
        
        [clipTable reloadData];
        [linkTable reloadData];
        [suppressedTable reloadData];
        [self saveLinks:self];
        //      parser.suppressConnection(cur)
    }
    else if(suppressedTable.selectedRow>=0&&linkTable.selectedRow<0){
        cout<<"suppressedTable"<<endl;
        string suppressedSourceName = currentSuppressedLinks[suppressedTable.selectedRow].sourceName;
        string suppressedTargetName = currentSuppressedLinks[suppressedTable.selectedRow].targetName;
        
        cout<<" swaping suppression for link: clip " << suppressedSourceName << " going to " << suppressedTargetName << endl;
        parser->unsuppressConnection(suppressedSourceName, suppressedTargetName);
        
        [testViewParent linkClip:parser->getClipWithLinkName(suppressedSourceName)
                toClip:parser->getClipWithLinkName(suppressedTargetName) ];
        currentClipLinks = parser->getLinksForClip(suppressedSourceName);
        currentSuppressedLinks = parser->getSuppressionsForClip(suppressedSourceName);
        
        [clipTable reloadData];
        [linkTable reloadData];
        [suppressedTable reloadData];
        [self saveLinks:self];
    }
    
    //  cout<<"button pressed"<< linkTable.selectedRow<<"::"<<suppressedTable.selectedRow<<endl;
}

- (IBAction) showQuestiosnChanged:(id)sender
{
	cout << "CALLING SHOW QUESTIONS " << endl;
	
	if(showOnlyQuestions.state == NSOnState){
		[keywordTable selectRowIndexes:nil
                        byExtendingSelection:NO];
        selectedKeywords.clear();
		selectedClips.clear();
		for(int i = 0; i < parser->getAllClips().size(); i++ ){
			if(parser->getAllClips()[i].hasStartingQuestion()){
                cout<<"Question Loop: "<<parser->getAllClips()[i].getLinkName()<<endl;
				selectedClips.push_back( parser->getAllClips()[i] );
			}
		}
        
	}
	else{
		selectedKeywords.clear();
		selectedClips.clear();
	}
	
	[keywordTable reloadData];
	[clipTable reloadData];
}

- (void) updateTables{
    [keywordTable reloadData];
	[clipTable reloadData];
    [linkTable reloadData];
    [suppressedTable reloadData];
}

@end
