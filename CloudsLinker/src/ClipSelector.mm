//
//  ClipSelector.m
//  CloudsFCPParser
//
//  Created by Surya Mattu on 24/06/13.
//
//

#import "ClipSelector.h"
#import "testView.h"

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
    
    [clipTable setTarget:self];
    [clipTable setDoubleAction:@selector(playDoubleClickedRow:)];
		
	[keywordTable reloadData];
	
    [metaTable setTarget:self];
    [metaTable setDoubleAction:@selector(playDoubleClickedRow:)];

	[linkTable setTarget:self];
    [linkTable setDoubleAction:@selector(playDoubleClickedRow:)];

    [suppressedTable setTarget:self];
    [suppressedTable setDoubleAction:@selector(playDoubleClickedRow:)];

	[self updateTables];
	[projectExamples reloadData];
}


- (IBAction) saveLinks:(id)sender
{
	[testViewParent saveLinks:self];
}

- (void) specialKeywords:(id)sender
{
    CloudsClip* clip = [self selectedClip];
    vector<string> test = clip->getSpecialKeywords();
    
    for( int i =0 ;i<test.size();i++){
        cout<<"special keywords are: "<<test[i]<<endl;
    }
}

- (IBAction) playDoubleClickedRow:(id)sender
{
	
	CloudsClip* clip;
	
	if(sender == clipTable && clipTable.selectedRow >= 0){
        clip = [self selectedClip];
	}
    else if(sender == linkTable && linkTable.selectedRow >= 0){
		CloudsLink&  link = [self selectedLink];
        clip = parser->getClipWithLinkName(link.targetName);
		[testViewParent selectClip:clip inAlternateTable:self];
	}
    else if(sender == suppressedTable && suppressedTable.selectedRow >= 0){
        CloudsLink& link = [self selectedSuppression];
        cout << "getting supressed " << link.targetName << endl;
        clip = parser->getClipWithLinkName(link.targetName);
		[testViewParent selectClip:clip inAlternateTable:self];
	}
    else if(sender == metaTable && metaTable.selectedRow >= 0){
        clip = [self selectedMeta];
		[testViewParent selectClip:clip inAlternateTable:self];
	}
	else{
		//bail!
		return;
	}

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
    else if(aTableView == metaTable){
        return [self isClipSelected] ? currentMetaLinks.size() : 0;
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
		CloudsClip* m = (selectedClips.size() == 0) ? parser->getAllClips()[rowIndex] : selectedClips[rowIndex];
        
		if([@"Links" isEqualToString:aTableColumn.identifier]){
			return [NSNumber numberWithInt: parser->getLinksForClip( m->getLinkName() ).size()];
		}
        else if([@"Suppressions" isEqualToString:aTableColumn.identifier]){
           return [NSNumber numberWithInt:parser->getSuppressionsForClip(m->getLinkName()).size()];
        }
//        else if([@"Starting Question" isEqualToString:aTableColumn.identifier]){
//            string s = m.getStartingQuestion();
//            return [NSString stringWithUTF8String:s.c_str()];
//        }
        else if([@"Meta Links" isEqualToString:aTableColumn.identifier]){
			return @"";
//            return [NSNumber numberWithInt:parser->getNumMetaDataConnections(m)];
        }
		else {
			//CloudsClip& m = [self selectedClip];
			string linkString = m->person + " - " + m->name + " - " + m->clip + ": [" + ofToString(m->startFrame) + "," + ofToString(m->endFrame) + "]";
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
    else if(aTableView == metaTable){

        string metaTableSourceEntry = currentMetaLinks[rowIndex]->getLinkName();

        if(currentMetaLinks[rowIndex]->startFrame != -1 && currentMetaLinks[rowIndex]->endFrame != -1){
            metaTableSourceEntry += "[" + ofToString(currentMetaLinks[rowIndex]->startFrame) + " - " + ofToString(currentMetaLinks[rowIndex]->endFrame) + "]";
        }
        return [NSString stringWithUTF8String:metaTableSourceEntry.c_str()];
    }
    else if(aTableView == suppressedTable){
        string suppressedTableEntry = currentSuppressedLinks[rowIndex].targetName;
        if(currentSuppressedLinks[rowIndex].startFrame != -1 && currentSuppressedLinks[rowIndex].endFrame!= -1){
            suppressedTableEntry += "[" + ofToString(currentSuppressedLinks[rowIndex].startFrame) + " - " + ofToString(currentSuppressedLinks[rowIndex].endFrame) + "]";
        }
        return [NSString stringWithUTF8String:suppressedTableEntry.c_str()];
    }
}

- (BOOL) tableView:(NSTableView *)aTableView shouldEditTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	return NO;
}

- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors
{
	NSArray *newDescriptors = [tableView sortDescriptors];
	
    NSLog(@"sort descriptor %@", [newDescriptors objectAtIndex:0]);
    if(tableView == keywordTable){
        parser->sortKeywordsByOccurrence( [ [[newDescriptors objectAtIndex:0] key]  isEqualToString:@"occurrence"] );
    }

    //    [results sortUsingDescriptors:newDescriptors];
    //"results" is my NSMutableArray which is set to be the data source for the NSTableView object.
//    [tableView reloadData];
	[testViewParent updateViews];
	
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    if(aNotification.object == keywordTable){
		[self deselectCurrentClip];
		
		dontUpdateKeywords = true;
		[self updateTables];
		dontUpdateKeywords = false;
    }
    else if(aNotification.object == clipTable){
		[metaTable deselectAll:self];
		[linkTable deselectAll:self];
		[suppressedTable deselectAll:self];
		
		//[self performSelectorOnMainThread:@selector(updateSelectedClip) withObject:nil waitUntilDone:NO];
		[self updateSelectedClip];
    }
    else if(aNotification.object == linkTable){
		
        if(linkTable.selectedRow < 0){
            return;
        }
        
        vector<CloudsClip*>& searchClips = (selectedClips.size() == 0) ? parser->getAllClips() : selectedClips;
        string targetClip = currentClipLinks[ linkTable.selectedRow ].targetName;
        for(int i = 0; i < searchClips.size(); i++){
            if(searchClips[i]->name == targetClip){
                NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:i];
				[self deselectCurrentClip];
                break;
            }
        }
    }
    else if(aNotification.object == metaTable){
        if(metaTable.selectedRow < 0){
            return;
        }
		
		[self updateSharedKeywords];
    }
    else if(aNotification.object == suppressedTable){
        if(suppressedTable.selectedRow<0){
            return;
        }
		
    }
}

- (NSInteger)numberOfItemsInComboBox:(NSComboBox *)aComboBox
{
	if(parser == NULL) return 0;
	return parser->getProjectExamples().size();
}

- (id)comboBox:(NSComboBox *)aComboBox objectValueForItemAtIndex:(NSInteger)index
{
	if(parser == NULL) return @"";
	return [NSString stringWithUTF8String:parser->getProjectExamples()[index].title.c_str() ];
}

- (NSUInteger)comboBox:(NSComboBox *)aComboBox indexOfItemWithStringValue:(NSString *)string
{
	if(parser == NULL) return 0;
	
	for(int i = 0; i < parser->getProjectExamples().size(); i++){
		if(parser->getProjectExamples()[i].title.find([string UTF8String]) != string::npos){
			return i;
		}
	}
	return 0;
}

- (NSString *)comboBox:(NSComboBox *)aComboBox completedString:(NSString *)string
{
	if(parser == NULL) return string;
	NSLog(@"Searching completed string %@", string);
	for(int i = 0; i < parser->getProjectExamples().size(); i++){
		cout << "Find for " << parser->getProjectExamples()[i].title << " is " << parser->getProjectExamples()[i].title.find([string UTF8String]) << endl;
		if(ofToLower(parser->getProjectExamples()[i].title).find(ofToLower([string UTF8String])) == 0){
			cout << "FOUND STRING " << parser->getProjectExamples()[i].title << endl;
			return [NSString stringWithUTF8String:parser->getProjectExamples()[i].title.c_str()];
		}
	}
	return string;
}

- (CloudsClip*) selectedMeta
{
    if(currentMetaLinks.size() > 0){
        CloudsClip* c = currentMetaLinks[metaTable.selectedRow];
//        cout<<"Selected clip from source table "<<c.getLinkName()<<","<<c.hasStartingQuestion()<<endl;
        return c;
    }
}

- (CloudsLink&) selectedSuppression
{
    if(currentSuppressedLinks.size() > 0){
        CloudsLink& c = currentSuppressedLinks[suppressedTable.selectedRow];
//        cout<<"Selected clip from link table "<<c.targetName<<endl;
        return c;
    }
}

- (CloudsLink&) selectedLink
{
    if(currentClipLinks.size() > 0){
        CloudsLink& c=currentClipLinks[linkTable.selectedRow];
//        cout<<"Selected clip from link table "<<c.targetName<<endl;
        return c;
    }
}

- (CloudsClip*) selectedClip
{
	if(![self isClipSelected]){
		return &dummyClip;
	}
	
    if(selectedClips.size() > 0){
        return parser->getClipWithID( selectedClips[clipTable.selectedRow]->getID() );
//        cout<<"Selected clip from source table "<<c.getLinkName()<<","<<c.hasStartingQuestion()<<endl;
    }
    else{
        return  parser->getAllClips()[clipTable.selectedRow];
//        cout<<"Selected clip from all clips"<<c.getLinkName()<<","<<c.hasStartingQuestion()<<endl;
    }
}

- (void) selectClip:(CloudsClip*) clip
{
	string clipID = clip->getID();
	for(int i = 0; i < parser->getAllClips().size(); i++){
		if(parser->getAllClips()[i]->getID() == clipID){
			[clipTable selectRowIndexes:[NSIndexSet indexSetWithIndex:i]
				   byExtendingSelection:NO];
			[clipTable scrollRowToVisible:i];
			break;
		}
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

- (void) updateKeywords:(id)sender
{

    if([self isClipSelected]){
		
		string keywordString = [currentKeywords.stringValue UTF8String];
		
//		CloudsClip& n = parser->getClipWithLinkName([self selectedClip].getLinkName());
		CloudsClip* n = [self selectedClip];
		
        vector<string> newKeywords = ofSplitString(keywordString, ",");
        n->setDesiredKeywords(newKeywords);
        
        cout<<"Keywords for clip: "<< n->getLinkName()<<" ::"<<keywordString <<endl;
		
        string revokedList = ofJoinString(n->getRevokedKeywords(), ",");
        revokedKeywords.stringValue = [NSString stringWithUTF8String:revokedList.c_str()];
		parser->refreshAllKeywords();
		
		[testViewParent updateViews];

		[self saveLinks:self];

    }
    else{
        ofLogError()<<"No clip selected!"<<endl;
    }
}

- (IBAction) applySpecialKeyword:(id<NSUserInterfaceItemIdentification>)sender
{
	NSLog(@" tag %@", sender.identifier);
	if([self isClipSelected]){
		[self selectedClip]->addKeyword( "#" + string( [sender.identifier UTF8String] ) );
		[testViewParent updateViews];
		[self saveLinks:self];
	}
}

- (IBAction) attachProjectExample:(id)sender
{
	if([self isClipSelected]){
		string selectedProjectExample = parser->getProjectExamples()[ projectExamples.indexOfSelectedItem ].title;
		cout << "project example set to " << selectedProjectExample << endl;

		[self selectedClip]->setProjectExample( selectedProjectExample );
		[self updateSelectedClip];
		[testViewParent updateViews];
		[self saveLinks:self];
	}
}


- (IBAction) linkFromMetaTable:(id)sender
{
	if(metaTable.selectedRow >= 0 && [self isClipSelected]){
		
        string linkSourceName = [self selectedClip]->getLinkName();
        string linkTargetName = currentMetaLinks[metaTable.selectedRow]->getLinkName();
		
		parser->unsuppressConnection(linkSourceName, linkTargetName);
		parser->addLink(linkSourceName, linkTargetName);
		
		[testViewParent updateViews];
		[self saveLinks:self];
	}
}

- (IBAction) deleteLink:(id)sender
{
    if(linkTable.selectedRow >= 0 && [self isClipSelected]){
        parser->removeLink( [self selectedClip]->getLinkName(), linkTable.selectedRow );
		
		[testViewParent updateViews];
		[self saveLinks:self];
    }
}

- (IBAction) deleteSupression:(id)sender
{
    if(suppressedTable.selectedRow >= 0 && [self isClipSelected]){
        parser->unsuppressConnection( [self selectedClip]->getLinkName(), suppressedTable.selectedRow);
		
		[testViewParent updateViews];
		[self saveLinks:self];
    }
}

- (IBAction)suppressFromMetaTable:(id)sender
{
    if(metaTable.selectedRow >= 0 && [self isClipSelected] ){
        
        string suppressSourceName = [self selectedClip]->getLinkName();
        string suppressTargetName = currentMetaLinks[metaTable.selectedRow]->getLinkName();
        
		cout << "Suppress meta clip: " << suppressSourceName << " -> " << suppressTargetName << endl;
		
        parser->removeLink(suppressSourceName, suppressTargetName);
		parser->suppressConnection(suppressSourceName, suppressTargetName);
				
		[testViewParent updateViews];
        [self saveLinks:self];
    }
}

- (IBAction) suppressCurrentLink:(id)sender
{
	if(linkTable.selectedRow >= 0){
		string suppressSourceName = currentClipLinks[linkTable.selectedRow].sourceName;
		string suppressTargetName = currentClipLinks[linkTable.selectedRow].targetName;
		
		parser->removeLink(suppressSourceName, suppressTargetName);
		parser->suppressConnection(suppressSourceName, suppressTargetName);
		
		[testViewParent updateViews];
        [self saveLinks:self];
	}
}

- (IBAction) linkCurrentSuppression:(id)sender;
{
	if(suppressedTable.selectedRow >= 0){
		
        string suppressSourceName = currentSuppressedLinks[suppressedTable.selectedRow].sourceName;
        string suppressTargetName = currentSuppressedLinks[suppressedTable.selectedRow].targetName;
		
		parser->unsuppressConnection(suppressSourceName, suppressTargetName);
		parser->addLink(suppressSourceName, suppressTargetName);
		
		[testViewParent updateViews];
		
        [self saveLinks:self];
	}
}

- (IBAction) showQuestionsChanged:(id)sender
{

	[testViewParent updateViews];
}

- (bool) isClipSelected
{
    return clipTable.selectedRow >= 0 && ((selectedClips.size() > 0 && clipTable.selectedRow < selectedClips.size()) ||
										  (selectedClips.size() == 0 && clipTable.selectedRow < parser->getAllClips().size()));
}

- (bool) isKeywordSelected
{
	return keywordTable.selectedRow >= 0;
}

- (void) deselectCurrentClip
{
	[clipTable deselectAll:self];
	[metaTable deselectAll:self];
	[linkTable deselectAll:self];
	[suppressedTable deselectAll:self];
}

- (void) updateTables
{
	[self updateSelectedClips];
	[self updateSelectedClip];
	
	[clipTable reloadData];
	if(!dontUpdateKeywords) [keywordTable reloadData];
}

- (void) updateSelectedClips
{
	selectedClips.clear();
	selectedKeywords.clear();
	
	if(showOnlyQuestions.state == NSOnState){

		[keywordTable deselectAll:self];
		
		for(int i = 0; i < parser->getAllClips().size(); i++ ){
			if(parser->getAllClips()[i]->hasStartingQuestion()){
				selectedClips.push_back( parser->getAllClips()[i] );
			}
		}
	}
	else if(keywordTable.selectedRow >= 0){
		NSUInteger idx = [keywordTable.selectedRowIndexes firstIndex];
		while (idx != NSNotFound) {
			// do work with "idx"
			selectedKeywords.push_back(parser->getAllKeywords()[idx]);
			cout << "selected keyword " << parser->getAllKeywords()[idx] << endl;
			
			// get the next index in the set
			idx = [keywordTable.selectedRowIndexes indexGreaterThanIndex:idx];
		}
		if(selectedKeywords.size() > 0){
			selectedClips = parser->getClipsWithKeyword(selectedKeywords);
			if(selectedClips.size() == 0){
				ofSystemAlertDialog("No clips found for Keywords: " + ofJoinString(selectedKeywords,","));
				[keywordTable deselectAll:self];				
			}
		}
	}
}

- (void) updateSelectedClip
{

	if([self isClipSelected]){
				
		CloudsClip* m = [self selectedClip];
		string revokedList = "";
		string keywords = ofJoinString(m->getKeywords(), ",") +","+ofJoinString(m->getSpecialKeywords(),",");
		currentKeywords.stringValue = [NSString stringWithUTF8String:keywords.c_str()];
		
//		startQuestion.stringValue = [NSString stringWithUTF8String:m.getStartingQuestion().c_str()];
		revokedList = ofJoinString(m->getRevokedKeywords(), ",");
		revokedKeywords.stringValue = [NSString stringWithUTF8String:revokedList.c_str()];
		
		currentClipLinks = parser->getLinksForClip(m->getLinkName());
		currentSuppressedLinks = parser->getSuppressionsForClip(m->getLinkName());
		currentMetaLinks = parser->getMetaDataConnections(m);
		
		[self updateSharedKeywords];
	}
	else{
		
		currentKeywords.stringValue = @"";
		revokedKeywords.stringValue = @"";
//		startQuestion.stringValue = @"";
		sharedKeywords.stringValue = @"";
		
		currentClipLinks.clear();
		currentSuppressedLinks.clear();
		currentMetaLinks.clear();
	}
	
    [linkTable reloadData];
    [suppressedTable reloadData];
    [metaTable reloadData];
	
}

- (void) updateSharedKeywords
{
	if([self isClipSelected] && metaTable.selectedRow >= 0 && metaTable.selectedRow < currentMetaLinks.size()){
		CloudsClip* clip = [self selectedClip];
		CloudsClip* metaClip = [self selectedMeta];
		sharedKeywords.stringValue = [NSString stringWithUTF8String: ofJoinString(parser->getSharedKeywords(clip, metaClip), ", ").c_str() ];
	}
	else{
		sharedKeywords.stringValue = @"";
	}
	
}

@end
