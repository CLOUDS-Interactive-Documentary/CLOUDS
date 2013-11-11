#import "testView.h"
#include "CloudsVisualSystem.h"

struct sortObject {
	CloudsFCPParser* parser;
	bool operator() (pair<int,string> keyA, pair<int,string> keyB) {
		return ( parser->getCohesionIndexForKeyword(keyA.second) > parser->getCohesionIndexForKeyword(keyB.second) );
	}
	
} cohesionSort;


bool clipsort(CloudsClip a, CloudsClip b){
	return a.getID().compare( b.getID() ) < 0;
}

@implementation testView

- (void)setup
{
//	currentVisualSystem = NULL;
	selectedPreset = NULL;
	
	parser.loadFromFiles();
	
//	visualSystems.populateVisualSystems();
	visualSystems.loadPresets();
	
	for (NSTableColumn *tableColumn in allKeywordTable.tableColumns) {
        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
        [tableColumn setSortDescriptorPrototype:sortDescriptor];
    }
	
	for (NSTableColumn *tableColumn in allClipTable.tableColumns) {
        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
        [tableColumn setSortDescriptorPrototype:sortDescriptor];
    }

	for(int i = 0; i < parser.getAllKeywords().size(); i++){
		sortedKeywordIndeces.push_back( i );
	}

    [presetTable setTarget:self];
	[presetTable setDoubleAction:@selector(playDoubleClickedRow:)];
	[presetTable reloadData];
	
//	[allClipTable setTarget:self];
//	[allClipTable setDoubleAction:@selector(playDoubleClickedRow:)];
	[allClipTable reloadData];
	[allKeywordTable reloadData];
	
	
	[self updateCounts];

	//TEST FACTORY
//	vector<string> systemNames;
//	systemNames.push_back("ExampleVectorMath");
//	systemNames.push_back("ExampleBox2D");
//	vector<CloudsVisualSystem*> systems = CloudsVisualSystemManager::InstantiateSystems(systemNames);
//	for(int i = 0; i < systems.size(); i++){
//		cout << "*** SYSTEM TEST " << systems[i]->getSystemName() << endl;
//	}
	
}

- (void)update
{
    if(shouldPlaySelectedRow){
        
		if(currentVisualSystem != NULL){
            currentVisualSystem->stopSystem();
			currentVisualSystem->exit();
        }
		cout << "loading system " << visualSystems.getPresets()[presetTable.selectedRow].systemName << " preset " << visualSystems.getPresets()[presetTable.selectedRow].presetName << endl;
		
        currentVisualSystem = CloudsVisualSystemManager::InstantiateSystem( visualSystems.getPresets()[presetTable.selectedRow].systemName );
		
		if(currentVisualSystem != NULL){
			currentVisualSystem->setup();
			currentVisualSystem->loadPresetGUISFromName(visualSystems.getPresets()[presetTable.selectedRow].presetName);
			currentVisualSystem->playSystem();
		}
		else{
			ofSystemAlertDialog(visualSystems.getPresets()[presetTable.selectedRow].systemName + " is not registered system");
		}
		
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

- (IBAction) updatePresets:(id)sender
{
//	visualSystems.loadPresets();
	
	if(presetTable.selectedRow >= 0){

		ofPtr<CloudsVisualSystem> system = CloudsVisualSystemManager::InstantiateSystem( visualSystems.getPresets()[presetTable.selectedRow].systemName );
		if(system != NULL){
			cout << "updating presets for " << system->getSystemName() << endl;
			visualSystems.updatePresetsForSystem( system );
		}
		
		[clipTable reloadData];
		[suppressedClipTable reloadData];
		[presetTable reloadData];
		[allKeywordTable reloadData];
		[allClipTable reloadData];
		
	}
	
}

- (IBAction) deletePreset:(id)sender
{
	if(presetTable.selectedRow >= 0){
		visualSystems.deletePreset( presetTable.selectedRow );
		
		[clipTable reloadData];
		[suppressedClipTable reloadData];
		[presetTable reloadData];
		[allKeywordTable reloadData];
		[allClipTable reloadData];
	}
}

- (IBAction) updateKeywords:(id)sender
{
	if(presetTable.selectedRow >= 0){
		
		associatedKeywords = ofSplitString([currentKeywords.stringValue UTF8String], ",", true, true);
		visualSystems.setKeywordsForPreset(*selectedPreset, associatedKeywords);
		
		//associatedClips = parser.getClipsWithKeyword(associatedKeywords);
		[self updateAssociatedClips];
		
		selectedPreset->enabled = (enabledBox.state == NSOnState);
		selectedPreset->oculusCompatible = (oculusBox.state == NSOnState);
		selectedPreset->comments = [notesText.stringValue UTF8String];
		selectedPreset->grade = [grade.stringValue UTF8String];
		
//		currentKeywords.stringValue = [NSString stringWithUTF8String: ofJoinString(associatedKeywords,",").c_str() ];
//		notesText.stringValue = [NSString stringWithUTF8String: selectedPreset->comments.c_str() ];
//		grade.stringValue = [NSString stringWithUTF8String: selectedPreset->grade.c_str() ];
//		enabledBox.state = selectedPreset->enabled;
		visualSystems.savePresets();
		
		[self updateCounts];
		
		[clipTable reloadData];
		[suppressedClipTable reloadData];
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
	[clipPercent setStringValue:[NSString stringWithFormat:@"%.02f%% Clips Tagged", percentClips*100]];
}

- (IBAction)suppressClip:(id)sender{
    if(clipTable.selectedRow >= 0){
        
		//TODO: multi selection
        visualSystems.suppressClip(visualSystems.getPresets()[presetTable.selectedRow].getID(), associatedClips[clipTable.selectedRow].getLinkName());
        
        cout<<"Clip: "<<associatedClips[clipTable.selectedRow].getLinkName()<<" suppressed for Visual System: "<<visualSystems.getPresets()[presetTable.selectedRow].getID()<<endl;
        visualSystems.savePresets();
		
		[self updateAssociatedClips];
//      [clipTable reloadData];
//		[suppressedClipTable reloadData];
		
    }
}

- (IBAction) unsuppressClip:(id)sender{
    if(suppressedClipTable.selectedRow >= 0){
		
		//TODO: multi selection
        visualSystems.unsuppressClip(visualSystems.getPresets()[presetTable.selectedRow].getID(), suppressedClips[suppressedClipTable.selectedRow].getLinkName());
        cout<<"Clip: "<<suppressedClips[suppressedClipTable.selectedRow].getLinkName()<<" unsuppressed for Visual System: "<<visualSystems.getPresets()[presetTable.selectedRow].getID()<<endl;
        visualSystems.savePresets();
		
		[self updateAssociatedClips];
		
    }
}



- (IBAction) exportStandalone:(id)sender {
	visualSystems.exportStandalonePresets();
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	if(aTableView == presetTable){
		return visualSystems.getPresets().size();
	}
	else if(aTableView == clipTable){
		return associatedClips.size();
	}
	else if(aTableView == suppressedClipTable){
		return suppressedClips.size();
	}
	else if(aTableView == allKeywordTable){
		return sortedKeywordIndeces.size();
	}
	else if(aTableView == allClipTable){
		return parser.getAllClips().size();
	}
	return 0;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex{
	
	if(aTableView == presetTable){
		if([@"system" isEqualToString:aTableColumn.identifier]){
//			return [NSString stringWithUTF8String: visualSystems.getPresets()[rowIndex].system->getSystemName().c_str()];
			return [NSString stringWithUTF8String: visualSystems.getPresets()[rowIndex].systemName.c_str()];
		}
		else if([@"grade" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String:
					(string(visualSystems.getPresets()[rowIndex].hasFiles ? "" : "!!") +
						   (visualSystems.getPresets()[rowIndex].enabled ? "+" : "-") +
							visualSystems.getPresets()[rowIndex].grade +
							(visualSystems.getPresets()[rowIndex].oculusCompatible ? "Oc" : "")).c_str()];
		}		
		else if([@"preset" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: visualSystems.getPresets()[rowIndex].presetName.c_str()];
		}
		else if( [@"keywords" isEqualToString:aTableColumn.identifier] ){
			return [NSString stringWithUTF8String: ofJoinString( visualSystems.keywordsForPreset(rowIndex), ",").c_str() ];
		}
		else if( [@"timing" isEqualToString:aTableColumn.identifier] ){
			if(visualSystems.getPresets()[rowIndex].indefinite){
				return @"indefinite";
			}
			else {
				return [NSString stringWithFormat:@"%.02fs", visualSystems.getPresets()[rowIndex].duration];
			}
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
	}
	else if(aTableView == suppressedClipTable){
		if([@"clip" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: suppressedClips[rowIndex].getLinkName().c_str() ];
		}
		else if([@"keyword" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: ofJoinString([self entries:suppressedClips[rowIndex].getKeywords()
																   sharedWith:associatedKeywords], ",").c_str() ];
		}		
	}
	else if(aTableView == allKeywordTable){
		int keywordIndex = sortedKeywordIndeces[rowIndex];
		if([@"keyword" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String:parser.getAllKeywords()[ keywordIndex ].c_str() ];
		}
		else if([@"cohesion" isEqualToString:aTableColumn.identifier]){
			return [NSNumber numberWithFloat: parser.getCohesionIndexForKeyword( parser.getAllKeywords()[ keywordIndex ] ) ];
		}
		else if([@"numclips" isEqualToString:aTableColumn.identifier]){
			return [NSNumber numberWithInt: parser.getClipsWithKeyword( parser.getAllKeywords()[ keywordIndex ] ).size() ];
		}
		else if([@"presets" isEqualToString:aTableColumn.identifier]){
			vector<CloudsVisualSystemPreset> presets = visualSystems.getPresetsForKeyword( parser.getAllKeywords()[ keywordIndex ] );
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
			CloudsClip& clip = parser.getAllClips()[rowIndex];
			vector<CloudsVisualSystemPreset> presets = visualSystems.getPresetsForKeywords( clip.getKeywords() );
			vector<string> ids;
			for(int i = 0; i < presets.size(); i++){
				if(!visualSystems.isClipSuppressed(presets[i].getID(), clip.getLinkName()) ){
					ids.push_back( presets[i].getID() );
				}
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
		
		[self updateAssociatedClips];
		
		if(selectedPreset != NULL){
			currentKeywords.stringValue = [NSString stringWithUTF8String: ofJoinString(associatedKeywords,",").c_str() ];
			notesText.stringValue = [NSString stringWithUTF8String: selectedPreset->comments.c_str() ];
			grade.stringValue = [NSString stringWithUTF8String: selectedPreset->grade.c_str() ];
			enabledBox.state = (selectedPreset->enabled ? NSOnState : NSOffState);
			oculusBox.state = (selectedPreset->oculusCompatible ? NSOnState : NSOffState);
		}
		else{
			currentKeywords.stringValue = @"";
			notesText.stringValue = @"";
			grade.stringValue = @"";
			enabledBox.state = NSOnState;
			oculusBox.state = NSOffState;
		}
	}
}

- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors{
	
	NSArray *newDescriptors = [tableView sortDescriptors];
    NSLog(@"sort descriptor %@", [newDescriptors objectAtIndex:0]);
	cohesionSort.parser = &parser;

    if(tableView == allKeywordTable){
		sortedKeywordIndeces.clear();
		
		NSString* descriptor = [[newDescriptors objectAtIndex:0] key];
		if( [@"cohesion" isEqualToString: descriptor] ){
			vector< pair<int,string> > keywordIndeces(parser.getAllKeywords().size());
			for(int i = 0; i < parser.getAllKeywords().size(); i++){
				keywordIndeces[i] = make_pair(i, parser.getAllKeywords()[i] );
			}
			
			sort(keywordIndeces.begin(), keywordIndeces.end(), cohesionSort);
			
			for(int i = 0; i < keywordIndeces.size(); i++){
				sortedKeywordIndeces.push_back( keywordIndeces[i].first );
			}
		}
		else if( [@"keyword" isEqualToString: descriptor] ){
			for(int i = 0; i < parser.getAllKeywords().size(); i++){
				sortedKeywordIndeces.push_back( i );
			}
		}
		else if( [@"clip" isEqualToString: descriptor] ){
			
		}
    }
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
	return true;
}

- (NSArray *)tokenField:(NSTokenField *)tokenField shouldAddObjects:(NSArray *)tokens atIndex:(NSUInteger)index
{
	return tokens;
}

- (void) updateAssociatedClips
{
	selectedPreset = NULL;
	associatedClips.clear();
	suppressedClips.clear();
	associatedKeywords.clear();
	
	if(presetTable.selectedRow >= 0){
		selectedPreset = &visualSystems.getPresets()[ presetTable.selectedRow ];
		associatedKeywords = visualSystems.keywordsForPreset(*selectedPreset);
		associatedClips = parser.getClipsWithKeyword(associatedKeywords);
	}
	
	for(int i = associatedClips.size() - 1; i >= 0; i-- ){
		if(visualSystems.isClipSuppressed(selectedPreset->getID(), associatedClips[i].getLinkName())){
//			cout << "adding suppressed clip " << associatedClips[i].getLinkName();
			suppressedClips.push_back(associatedClips[i]);
			associatedClips.erase(associatedClips.begin() + i);
		}
	}
	
	sort(associatedClips.begin(),associatedClips.end(), clipsort);
	sort(suppressedClips.begin(),suppressedClips.end(), clipsort);
	
	[clipTable reloadData];
	[suppressedClipTable reloadData];
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