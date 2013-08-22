#import "testView.h"
#include "CloudsVisualSystem.h"

@implementation testView

- (void)setup
{
	currentVisualSystem = NULL;
	selectedPreset = NULL;
	
	parser.loadFromFiles();
	
	visualSystems.populateVisualSystems();
	
    [presetTable setTarget:self];
	[presetTable setDoubleAction:@selector(playDoubleClickedRow:)];
	[presetTable reloadData];
	
//	[allClipTable setTarget:self];
//	[allClipTable setDoubleAction:@selector(playDoubleClickedRow:)];
	[allClipTable reloadData];
	[allKeywordTable reloadData];
	
	
	[self updateCounts];

}

- (void)update
{
    if(shouldPlaySelectedRow){
        
		if(currentVisualSystem != NULL){
            currentVisualSystem->stopSystem();
        }
		
        currentVisualSystem = visualSystems.getPresets()[presetTable.selectedRow].system;
        
        currentVisualSystem->playSystem();
        currentVisualSystem->loadPresetGUISFromName(visualSystems.getPresets()[presetTable.selectedRow].presetName);
		
        shouldPlaySelectedRow = false;	
    }
	
    //ofShowCursor();
}

- (void)draw
{
    
}

- (void)exit
{
	
}

- (void)keyPressed:(int)key
{
	
}

- (void)keyReleased:(int)key
{
	
}

- (void)mouseMoved:(NSPoint)p
{
	
}

- (void)mouseDragged:(NSPoint)p button:(int)button
{
	
}

- (void)mousePressed:(NSPoint)p button:(int)button
{
	
}

- (void)mouseReleased:(NSPoint)p button:(int)button
{
	
}

- (void)windowResized:(NSSize)size
{
	
}

- (IBAction) updateKeywords:(id)sender
{
	if(presetTable.selectedRow >= 0){
		
		associatedKeywords = ofSplitString([currentKeywords.stringValue UTF8String], ",", true, true);
		visualSystems.setKeywordsForPreset(*selectedPreset, associatedKeywords);
		associatedClips = parser.getClipsWithKeyword(associatedKeywords);
			
		[self updateCounts];
		
		[clipTable reloadData];
		[presetTable reloadData];
		[allKeywordTable reloadData];
		[allClipTable reloadData];
	}
}

- (void) updateCounts
{
	set<string> allKeywords = visualSystems.getAllKeywords();
	vector<string> allKeysVector( allKeywords.begin(), allKeywords.end() );
	float percentKeywords = 1.0 * allKeywords.size() / parser.getAllKeywords().size();
	float percentClips = 1.0 * parser.getClipsWithKeyword(allKeysVector).size() / parser.getAllClips().size();
	
	[keywordPercent setStringValue:[NSString stringWithFormat:@"%.02f%% Keywords Tagged", percentKeywords*100]];
	[clipPercent setStringValue: [NSString stringWithFormat:@"%.02f%% Clips Tagged", percentClips*100]];
}

-(IBAction) unsuppressClip:(id)sender{
    if(clipTable.selectedRow>=0){
        visualSystems.unsuppressClip(visualSystems.getPresets()[presetTable.selectedRow].getID(), associatedClips[clipTable.selectedRow].getLinkName());
        cout<<"Clip: "<<associatedClips[clipTable.selectedRow].getLinkName()<<" unsuppressed for Visual System: "<<visualSystems.getPresets()[presetTable.selectedRow].getID()<<endl;
        
        [clipTable reloadData];
    }
}

-(IBAction)suppressClip:(id)sender{
    if(clipTable.selectedRow>=0){
        
        visualSystems.suppressClip(visualSystems.getPresets()[presetTable.selectedRow].getID(), associatedClips[clipTable.selectedRow].getLinkName());
        
        cout<<"Clip: "<<associatedClips[clipTable.selectedRow].getLinkName()<<" suppressed for Visual System: "<<visualSystems.getPresets()[presetTable.selectedRow].getID()<<endl;
        
        [clipTable reloadData];
    }
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	if(aTableView == presetTable){
		return visualSystems.getPresets().size();
	}
	else if(aTableView == clipTable){
		return associatedClips.size();
	}
	else if(aTableView == allKeywordTable){
		return parser.getAllKeywords().size();
	}
	else if(aTableView == allClipTable){
		return parser.getAllClips().size();
	}
	return 0;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex{
	
	if(aTableView == presetTable){
		if([@"system" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: visualSystems.getPresets()[rowIndex].system->getSystemName().c_str()];
		}
		else if([@"preset" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: visualSystems.getPresets()[rowIndex].presetName.c_str()];
		}
		else if( [@"keywords" isEqualToString:aTableColumn.identifier] ){
			return [NSString stringWithUTF8String: ofJoinString( visualSystems.keywordsForPreset(rowIndex), ",").c_str() ];
		}
	}
	else if(aTableView == clipTable){
		if([@"clip" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: associatedClips[rowIndex].getLinkName().c_str() ];
		}
		else if([@"keyword" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: ofJoinString([self entries:associatedClips[rowIndex].getKeywords()
																   sharedWith:associatedKeywords], ",").c_str() ];
		}
        else if( [@"Suppressed" isEqualToString:aTableColumn.identifier] ){
            if( visualSystems.isClipSuppressed(selectedPreset->getID(), associatedClips[rowIndex].getLinkName())){
                string str = "Yes";
                return [NSString stringWithUTF8String:str.c_str()];
            }
            else{
                string str = "No";
                return [NSString stringWithUTF8String:str.c_str()];
            }
        }
	}
	else if(aTableView == allKeywordTable){
		if([@"keyword" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: parser.getAllKeywords()[rowIndex].c_str() ];
		}
		else if([@"presets" isEqualToString:aTableColumn.identifier]){
			vector<CloudsVisualSystemPreset> presets = visualSystems.getPresetsForKeyword( parser.getAllKeywords()[rowIndex] );
			vector<string> ids;
			for(int i = 0; i < presets.size(); i++){
				ids.push_back(presets[i].getID());
			}
			return [NSString stringWithUTF8String: ofJoinString(ids, ", ").c_str()];
		}
	}
	else if(aTableView == allClipTable){
		if([@"clip" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: parser.getAllClips()[rowIndex].getLinkName().c_str() ];
		}
		else if([@"presets" isEqualToString:aTableColumn.identifier]){
			vector<CloudsVisualSystemPreset> presets = visualSystems.getPresetsForKeywords(parser.getAllClips()[rowIndex].getKeywords());
			vector<string> ids;
			for(int i = 0; i < presets.size(); i++){
				ids.push_back(presets[i].getID());
			}
			return [NSString stringWithUTF8String: ofJoinString(ids, ", ").c_str()];
		}
	}

	return @"-";
}

- (vector<string>) entries:(vector<string>&)a sharedWith:(vector<string>&)b
{
	vector<string> shared;
	for(int i = 0; i < a.size(); i++){
		for(int j = 0; j < b.size(); j++){
			if(a[i] == b[j] && ! ofContains(shared, a[i])){
				shared.push_back(a[i]);
			}
		}
	}
	return shared;
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
	if(aNotification.object == presetTable){
		if(selectedPreset != NULL){
			vector<string> keywords = ofSplitString([currentKeywords.stringValue UTF8String], ",", true, true);
			visualSystems.setKeywordsForPreset(*selectedPreset, keywords);
			visualSystems.saveKeywords();
		}
		
		[self updateAssociatedClips];
		
		currentKeywords.stringValue = [NSString stringWithUTF8String: ofJoinString(associatedKeywords,",").c_str() ];
	}
}

- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors{
	
}

- (NSArray *)tokenField:(NSTokenField *)tokenField
completionsForSubstring:(NSString *)substring
		   indexOfToken:(NSInteger)tokenIndex
	indexOfSelectedItem:(NSInteger *)selectedIndex
{
    
    //cout << "asking for completions..." << endl;
    NSMutableArray* completions = [NSMutableArray array];
    for(int i = 0; i < parser.getAllKeywords().size(); i++){
        NSString* stringKeyword = [NSString stringWithUTF8String:parser.getAllKeywords()[i].c_str()];
        if([stringKeyword rangeOfString:substring].location == 0 &&
		   ![self hasKeyword:stringKeyword])
		{
            [completions addObject:stringKeyword];
        }
    }
    return completions;
}

- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor
{
	cout << "CONTROL TEXT END EDITING" << endl;
	
	if(presetTable.selectedRow >= 0){
	}
    
}

- (NSArray *)tokenField:(NSTokenField *)tokenField shouldAddObjects:(NSArray *)tokens atIndex:(NSUInteger)index
{
	cout << "SHOULD ADD OBJECTS " << [tokens description] << endl;
	if(presetTable.selectedRow >= 0){
		associatedKeywords = ofSplitString([currentKeywords.stringValue UTF8String], ",", true, true);
		visualSystems.setKeywordsForPreset(*selectedPreset, associatedKeywords);
		associatedClips = parser.getClipsWithKeyword(associatedKeywords);
		[clipTable reloadData];
		[presetTable reloadData];
	}
    
    //	[self updateAssociatedClips];
	
    //	if(presetTable.selectedRow >= 0){
    //		associatedKeywords = ofSplitString([currentKeywords.stringValue UTF8String], ",", true, true);
    //		visualSystems.setKeywordsForPreset(*selectedPreset, associatedKeywords);
    //		associatedClips = parser.getClipsWithKeyword(associatedKeywords);
    //		[clipTable reloadData];
    //	}
	
	return tokens;
}

- (void) updateAssociatedClips
{
	selectedPreset = NULL;
	associatedClips.clear();
	associatedKeywords.clear();
	
	if(presetTable.selectedRow >= 0){
		selectedPreset = &visualSystems.getPresets()[ presetTable.selectedRow ];
		associatedKeywords = visualSystems.keywordsForPreset(*selectedPreset);
		associatedClips = parser.getClipsWithKeyword(associatedKeywords);
	}
		
	[clipTable reloadData];
}

- (BOOL) hasKeyword:(NSString*)keyword
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

- (void)playDoubleClickedRow:(id)sender
{
    shouldPlaySelectedRow = true;
}

@end