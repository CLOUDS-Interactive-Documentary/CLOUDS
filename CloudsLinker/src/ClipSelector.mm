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
    
    cout << "setting action " << [clipTable description] << endl;
    
    [clipTable setTarget:self];
    [clipTable setDoubleAction:@selector(playDoubleClickedRow:)];
		
//    [keywordTable setTarget:self];
//    [keywordTable setDoubleAction:@selector(playDoubleClickedRow:)];

	[keywordTable reloadData];
	
    [metaTable setTarget:self];
    [metaTable setDoubleAction:@selector(playDoubleClickedRow:)];

	[linkTable setTarget:self];
    [linkTable setDoubleAction:@selector(playDoubleClickedRow:)];

    [suppressedTable setTarget:self];
    [suppressedTable setDoubleAction:@selector(playDoubleClickedRow:)];


	[self updateTables];
}


- (IBAction) saveLinks:(id)sender
{
//	if(ofDirectory("../../../CloudsData/links/").exists()){
//		parser->saveLinks("../../../CloudsData/links/clouds_link_db.xml");
//	}
//	else{
//		parser->saveLinks("clouds_link_db.xml");
//	}
	[testViewParent saveLinks:self];
}

- (IBAction) playDoubleClickedRow:(id)sender
{
	
	CloudsClip clip;
	
	if(sender == clipTable && clipTable.selectedRow >= 0){
        clip = [self selectedClip];
	}
    else if(sender == linkTable && linkTable.selectedRow >= 0){
		CloudsLink&  link = [self selectedLink];
        clip = parser->getClipWithLinkName(link.targetName);
	}
    else if(sender == suppressedTable && suppressedTable.selectedRow >= 0){
        CloudsLink& link = [self selectedSuppression];
        cout << "getting supressed " << link.targetName << endl;
        clip = parser->getClipWithLinkName(link.targetName);
	}
    else if(sender == metaTable && metaTable.selectedRow >= 0){
        clip = [self selectedMeta];
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
        return currentMetaLinks.size();
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
        else if([@"Meta Links" isEqualToString:aTableColumn.identifier]){
            return [NSNumber numberWithInt:parser->getNumMetaDataConnections(m)];

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
    else if(aTableView == metaTable){

        string metaTableSourceEntry = currentMetaLinks[rowIndex].getLinkName();
//        cout<<metaTableSourceEntry<<endl;
        if(currentMetaLinks[rowIndex].startFrame != -1 && currentMetaLinks[rowIndex].endFrame != -1){
            metaTableSourceEntry += "[" + ofToString(currentMetaLinks[rowIndex].startFrame) + " - " + ofToString(currentMetaLinks[rowIndex].endFrame) + "]";
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
		//TODO:
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    if(aNotification.object == keywordTable){
		[clipTable selectRowIndexes:nil
			   byExtendingSelection:NO];
		
		[self updateTables];
    }
    else if(aNotification.object == clipTable){
		
        if([self isClipSelected]){
			CloudsClip m = [self selectedClip];
			string keywordList = "";
			currentKeywords.stringValue = [NSString stringWithUTF8String:ofJoinString(m.keywords, ",").c_str()];
			startQuestion.stringValue = [NSString stringWithUTF8String:m.getStartingQuestion().c_str()];
			
			dontUpdateClips = true;
			[self updateTables];
			dontUpdateClips = false;
		}
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
    else if(aNotification.object == metaTable){
        if(metaTable.selectedRow < 0){
            return;
        }
		
    }
    else if(aNotification.object == suppressedTable){
        if(suppressedTable.selectedRow<0){
            return;
        }
		
//		cout << "deselecting from link table";
//		[linkTable selectRowIndexes:[NSIndexSet indexSet] byExtendingSelection:NO];
		
		//linkTable.selectedRow = -1;
    }
}

- (CloudsClip&) selectedMeta
{
    if(currentMetaLinks.size() > 0){
        CloudsClip& c=currentMetaLinks[metaTable.selectedRow];
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

- (CloudsClip) selectedClip
{
	if(![self isClipSelected]){
		return CloudsClip();
	}
	
    if(selectedClips.size() > 0){
        return selectedClips[clipTable.selectedRow];
//        cout<<"Selected clip from source table "<<c.getLinkName()<<","<<c.hasStartingQuestion()<<endl;
    }
    else{
        return  parser->getAllClips()[clipTable.selectedRow] ;
//        cout<<"Selected clip from all clips"<<c.getLinkName()<<","<<c.hasStartingQuestion()<<endl;
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


- (IBAction) setQuestionText:(id)sender{
    //button pressed
    CloudsClip m = [self selectedClip];
	
	//needs to be the Clip referenced by the main database to have tehq uestion stored
    //get the txt from textfield, get currently select clip, and set it
    CloudsClip& n = parser->getClipWithLinkName(m.getLinkName());
    string q = [startQuestion.stringValue UTF8String];
    
    n.setStartingQuestion(q);
    
	cout<<"Set the question for clip"<<n.getLinkName()<<"::"<<n.getStartingQuestion()<<endl;
	
	[self updateTables];
    [self saveLinks:self];
}

- (IBAction) linkFromMetaTable:(id)sender
{
	if(metaTable.selectedRow >= 0 && [self isClipSelected]){
		
        string linkSourceName = [self selectedClip].getLinkName();
        string linkTargetName = currentMetaLinks[metaTable.selectedRow].getLinkName();
		
		parser->unsuppressConnection(linkSourceName, linkTargetName);
		parser->addLink(linkSourceName, linkTargetName);
		
		[self updateTables];
		[self saveLinks:self];
	}
}

- (IBAction) deleteLink:(id)sender
{
    if(linkTable.selectedRow >= 0 && [self isClipSelected]){
        parser->removeLink( [self selectedClip].getLinkName(), linkTable.selectedRow );
		
		[self updateTables];
		[self saveLinks:self];
    }
}

- (IBAction) deleteSupression:(id)sender
{
    if(suppressedTable.selectedRow >= 0 && [self isClipSelected]){
        parser->unsuppressConnection( [self selectedClip].getLinkName(), suppressedTable.selectedRow);
		
		[self updateTables];
		[self saveLinks:self];
    }
}

- (IBAction)suppressFromMetaTable:(id)sender
{
    if(metaTable.selectedRow >= 0 && [self isClipSelected] ){
        
        string suppressSourceName = [self selectedClip].getLinkName();
        string suppressTargetName = currentMetaLinks[metaTable.selectedRow].getLinkName();
        
        parser->removeLink(suppressSourceName, suppressTargetName);
		parser->suppressConnection(suppressSourceName, suppressTargetName);
				
		[self updateTables];
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
		
		[self updateTables];
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
		
		[self updateTables];
        [self saveLinks:self];
	}
}

- (IBAction) showQuestionsChanged:(id)sender
{
//	cout << "CALLING SHOW QUESTIONS " << endl;

//	selectedKeywords.clear();
//	selectedClips.clear();
//
//	if(showOnlyQuestions.state == NSOnState){
//		[keywordTable selectRowIndexes:nil
//                  byExtendingSelection:NO];
//		
//		for(int i = 0; i < parser->getAllClips().size(); i++ ){
//			if(parser->getAllClips()[i].hasStartingQuestion()){
//				selectedClips.push_back( parser->getAllClips()[i] );
//			}
//		}
//	}
	
	[self updateTables];
}

- (bool) isClipSelected
{
    return clipTable.selectedRow >= 0;
}

- (bool) isKeywordSelected
{
	return keywordTable.selectedRow >= 0;
}

- (void) updateTables
{
	currentClipLinks.clear();
	currentSuppressedLinks.clear();
	currentMetaLinks.clear();
	
	if([self isClipSelected]){
		CloudsClip clip = [self selectedClip];
		currentClipLinks = parser->getLinksForClip(clip.getLinkName());
		currentSuppressedLinks = parser->getSuppressionsForClip(clip.getLinkName());
		currentMetaLinks = parser->getMetaDataConnections(clip);
	}
	
	[self updateSelectedClips];
	
	if(!dontUpdateClips) [clipTable reloadData];
    [linkTable reloadData];
    [suppressedTable reloadData];
    [metaTable reloadData];
}

- (void) updateSelectedClips
{
	
	selectedClips.clear();
	selectedKeywords.clear();
	if(showOnlyQuestions.state == NSOnState){

		[keywordTable selectRowIndexes:nil
				  byExtendingSelection:NO];
		
		for(int i = 0; i < parser->getAllClips().size(); i++ ){
			if(parser->getAllClips()[i].hasStartingQuestion()){
				selectedClips.push_back( parser->getAllClips()[i] );
			}
		}
	}
	else if(keywordTable.selectedRow >= 0){
		
		NSUInteger idx = [keywordTable.selectedRowIndexes firstIndex];
		while (idx != NSNotFound) {
			// do work with "idx"
			selectedKeywords.push_back(parser->getAllKeywords()[idx]);
			
			// get the next index in the set
			idx = [keywordTable.selectedRowIndexes indexGreaterThanIndex:idx];
		}
		selectedClips = parser->getClipsWithKeyword(selectedKeywords);
	}
}

@end
