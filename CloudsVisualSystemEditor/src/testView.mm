#import "testView.h"
#include "CloudsVisualSystem.h"
#include "CloudsInputKinectOSC.h"


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

	
	if(ofFile::doesFileExist(GetCloudsDataPath() + "CloudsMovieDirectory.txt")){
		parser.setCombinedVideoDirectory(ofBufferFromFile(GetCloudsDataPath() + "CloudsMovieDirectory.txt").getText());
        cout<<"Clouds Directory is pointing to "<<ofBufferFromFile(GetCloudsDataPath	() + "CloudsMovieDirectory.txt").getText()<<endl;
	}
	else{
		ofSystemAlertDialog("Could not find movie file path. Create a file called CloudsMovieDirectory.txt that contains one line, the path to your movies folder");
	}
	
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

	filterEnabledBox.state = NSOffState;
	filterOculusBox.state  = NSOffState;
	filterGradeABox.state  = NSOffState;
	
	[self updateFilters:self];
	
    [presetTable setTarget:self];
	[presetTable setDoubleAction:@selector(playDoubleClickedRow:)];
	[presetTable reloadData];
    
    [allClipTable reloadData];
	[allKeywordTable reloadData];
    
    [clipTable setDoubleAction:@selector(loadClipFromTable:)];
	[allClipTable setDoubleAction:@selector(loadClipFromTable:)];
	
	[self updateCounts];

//    cout << " APP POINTER NULL? " << (ofGetAppPtr() == NULL ? "YES" : "NO") << endl;
	//TEST FACTORY
//	vector<string> systemNames;
//	systemNames.push_back("ExampleVectorMath");
//	systemNames.push_back("ExampleBox2D");
//	vector<CloudsVisualSystem*> systems = CloudsVisualSystemManager::InstantiateSystems(systemNames);
//	for(int i = 0; i < systems.size(); i++){
//		cout << "*** SYSTEM TEST " << systems[i]->getSystemName() << endl;
//	}
	
    mixer.setup();
    mixer.setDiageticVolume(1);

}

- (void)update
{
	
	if(runningTest){
		
		if(ofGetElapsedTimef() - lastSystemStartTime > 5){
			
			if(currentVisualSystem != NULL){
				cout << "5) (" << currentTestPresetIndex << "/" << testPresetIndeces.size() << ") STOPPING SYSTEM " << currentVisualSystem->getSystemName() << endl;
				currentVisualSystem->stopSystem();
			}

			if(testBatchIndex == testBatch.size() || testBatch.size() == 0){
				
				for(int i = 0; i < testBatch.size(); i++){
					cout << "5) CALLING EXIT " << testBatch[i]->getSystemName() << endl;
					testBatch[i]->exit();
				}
				
				cout << "DELETING PRESET BATCH" << endl;
				testBatch.clear();
//				currentVisualSystem = ofPtr<CloudsVisualSystem>( (CloudsVisualSystem*)(NULL) );
				
				int i = 0;
				for(i = currentTestPresetIndex; i < MIN(testPresetIndeces.size(),currentTestPresetIndex+2); i++){
					
					CloudsVisualSystemPreset& preset = visualSystems.getPresets()[ testPresetIndeces[i] ];
					cout << "******* 1) (" << i << "/" << testPresetIndeces.size() << ") INSTANTIATING " << preset.systemName << " : " << preset.presetName << endl;
					ofPtr<CloudsVisualSystem> vs = CloudsVisualSystemManager::InstantiateSystem( preset.systemName );
					cout << "******* 2) (" << i << "/" << testPresetIndeces.size() << ") SETTING UP " << preset.systemName << " : " << preset.presetName << endl;
					if(vs == NULL){
						cout << "******* 3) (" << i << "/" << testPresetIndeces.size() << ") PRESET NOT REGISTERED " << preset.systemName << " : " << preset.presetName << endl;
						continue;
					}
					vs->setup();
					cout << "******* 3) (" << i << "/" << testPresetIndeces.size() << ") LOADING PRESET " << preset.systemName << " : " << preset.presetName << endl;
					vs->loadPresetGUISFromName(preset.presetName);
					
					testBatch.push_back(vs);
				}
				
				if(i == testPresetIndeces.size()){
					runningTest = false;
				}
				
				testBatchIndex = 0;
			}
			
			
			if(testBatchIndex < testBatch.size()){
				currentVisualSystem = testBatch[testBatchIndex];
				cout << "4) PLAYING SYSTEM " << currentVisualSystem->getSystemName() << endl;
				currentVisualSystem->playSystem();
				lastSystemStartTime = ofGetElapsedTimef();
				
				testBatchIndex++;
				currentTestPresetIndex++;
                if(currentTestPresetIndex == testPresetIndeces.size()-1){
                    cout<<"TEST COMPLETE : Reshufling and looping test"<<endl;
                    currentTestPresetIndex = 0;
                    random_shuffle(testPresetIndeces.begin(), testPresetIndeces.end());
                }
			}
			
		}
	}
    else if(shouldPlaySelectedRow){
        
		if(currentVisualSystem != NULL){
            currentVisualSystem->stopSystem();
			currentVisualSystem->exit();
        }

		cout << "loading system " << visualSystems.getPresets()[ self.selectedPresetIndex ].systemName << " preset " << visualSystems.getPresets()[self.selectedPresetIndex].presetName << endl;
		
        currentVisualSystem = CloudsVisualSystemManager::InstantiateSystem( visualSystems.getPresets()[ self.selectedPresetIndex ].systemName );
		
		if(currentVisualSystem != NULL){
			currentVisualSystem->setup();
			string presetName = visualSystems.getPresets()[ self.selectedPresetIndex ].presetName;
			if(presetName == "+New Preset"){
				currentVisualSystem->loadPresetGUISFromName("");
			}
			else if(presetName == "+Current State"){
				currentVisualSystem->loadPresetGUISFromName("Working");
			}
			else {
				currentVisualSystem->loadPresetGUISFromName(presetName);
			}
			currentVisualSystem->playSystem();
		}
		else{
			ofSystemAlertDialog(visualSystems.getPresets()[self.selectedPresetIndex].systemName + " is not registered system");
		}
		
		shouldPlaySelectedRow = false;	
    }
}

- (void) loadClipFromTable:(id)sender
{
    if(sender == clipTable){
        cout<<"Sender for this was clipTable"<<endl;
        if(clipTable.selectedRow >= 0){
            cout<<associatedClips[ clipTable.selectedRow ].getLinkName()<<endl;
            [self loadClip: associatedClips[ clipTable.selectedRow ] ];
            
        }
    }
    else if(sender == allClipTable){
        if(allClipTable.selectedRow >= 0){
            cout<<"Sender for this was clipTable"<<endl;
            cout<<parser.getAllClips()[ allClipTable.selectedRow ].getLinkName()<<endl;
            [self loadClip: parser.getAllClips()[ allClipTable.selectedRow ] ];
            
        }
    }
}

- (IBAction)loadClip:(CloudsClip&)clip
{
	cout<<"Im in load clip"<<endl;
	if(clip.hasMediaAsset && clip.voiceOverAudio && CloudsVisualSystem::getRGBDVideoPlayer().setupVO(clip.voiceOverAudioPath) ){
		
		CloudsVisualSystem::getRGBDVideoPlayer().swapAndPlay();
//		CloudsVisualSystem::setupSpeaker( CloudsSpeaker::speakers[clip.person].firstName,
//									  CloudsSpeaker::speakers[clip.person].lastName,
//									  clip.name );
		
		currentClip = clip;
	}
	else if(clip.hasMediaAsset && CloudsVisualSystem::getRGBDVideoPlayer().setup( clip.combinedVideoPath, clip.combinedCalibrationXMLPath,1,clip.speakerVolume) ){
		cout<<"clip.speakerVolume : "<<clip.speakerVolume<<endl;
		CloudsVisualSystem::getRGBDVideoPlayer().swapAndPlay();
//		CloudsVisualSystem::setupSpeaker( CloudsSpeaker::speakers[clip.person].firstName,
//                                      CloudsSpeaker::speakers[clip.person].lastName,
//                                      clip.name );
		currentClip = clip;
		
	}
	else{
		ofLogError() << "CloudsPlaybackController::playClip -- folder " << clip.combinedVideoPath << " is not valid";
	}	
}

- (void)draw
{

}

- (void)exit
{
	if(currentVisualSystem != NULL){
		currentVisualSystem->stopSystem();
		currentVisualSystem->exit();
	}	
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
	
	if(self.selectedPresetIndex >= 0){

        [self updatePresetsForSystem: visualSystems.getPresets()[self.selectedPresetIndex].systemName ];

	}
}

- (void) updatePresetsForSystem:(string) systemName
{
    ofPtr<CloudsVisualSystem> system = CloudsVisualSystemManager::InstantiateSystem( systemName );
    if(system != NULL){
        cout << "updating presets for " << system->getSystemName() << endl;
        visualSystems.updatePresetsForSystem( system );
        
        string refreshFlagPath = GetCloudsVisualSystemDataPath(systemName) + "Presets/Working/_refreshme.txt";
        if(ofFile::doesFileExist(refreshFlagPath)){
            ofFile::removeFile(refreshFlagPath);
        }
        
    }
    
    [clipTable reloadData];
    [suppressedClipTable reloadData];
    [presetTable reloadData];
    [allKeywordTable reloadData];
    [allClipTable reloadData];
}

- (IBAction) deletePreset:(id)sender
{
	if(self.selectedPresetIndex >= 0){
		visualSystems.deletePreset( self.selectedPresetIndex );
		
		[self updateAssociatedClips];
		
		[presetTable reloadData];
		[allKeywordTable reloadData];
		[allClipTable reloadData];
	}
}

- (IBAction) updateKeywords:(id)sender
{
	if(self.selectedPresetIndex >= 0){
		
		associatedKeywords = ofSplitString([currentKeywords.stringValue UTF8String], ",", true, true);
		visualSystems.setKeywordsForPreset(*selectedPreset, associatedKeywords);
		
		//associatedClips = parser.getClipsWithKeyword(associatedKeywords);
		[self updateAssociatedClips];
		
		selectedPreset->enabled = (enabledBox.state == NSOnState);
		selectedPreset->oculusCompatible = (oculusBox.state == NSOnState);
		selectedPreset->hasSound = (soundBox.state == NSOnState);
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
        
		NSUInteger idx = [clipTable.selectedRowIndexes firstIndex];
		while (idx != NSNotFound) {
			
			visualSystems.suppressClip(visualSystems.getPresets()[self.selectedPresetIndex].getID(),
									   associatedClips[idx].getLinkName());
			
			// get the next index in the set
			idx = [clipTable.selectedRowIndexes indexGreaterThanIndex:idx];
		}
		
		
        cout<<"Clip: "<<associatedClips[clipTable.selectedRow].getLinkName()<<" suppressed for Visual System: "<<visualSystems.getPresets()[self.selectedPresetIndex].getID()<<endl;
        visualSystems.savePresets();
		
		[self updateAssociatedClips];
		[self updateCurrentClipPresets];
		
        [clipTable selectRowIndexes:[NSIndexSet indexSet] byExtendingSelection:NO];

		[allClipTable reloadData];
//		[suppressedClipTable reloadData];	
    }
}

- (IBAction) unsuppressClip:(id)sender{
    if(suppressedClipTable.selectedRow >= 0){
		
		NSUInteger idx = [suppressedClipTable.selectedRowIndexes firstIndex];
		while (idx != NSNotFound) {
			
			visualSystems.unsuppressClip(visualSystems.getPresets()[self.selectedPresetIndex].getID(),
										 suppressedClips[idx].getLinkName());
			
			// get the next index in the set
			idx = [suppressedClipTable.selectedRowIndexes indexGreaterThanIndex:idx];
		}
		
        cout<<"Clip: "<<suppressedClips[suppressedClipTable.selectedRow].getLinkName()<<" unsuppressed for Visual System: "<<visualSystems.getPresets()[self.selectedPresetIndex].getID()<<endl;
        
		visualSystems.savePresets();
		
		[clipTable selectRowIndexes:[NSIndexSet indexSet] byExtendingSelection:NO];
		[self updateAssociatedClips];
		[self updateCurrentClipPresets];
		
		
    }
}

- (void) linkClipToPreset:(id)sender
{
	if(allClipTable.selectedRow >= 0 &&
	   presetTable.selectedRow  >= 0)
	{
		visualSystems.linkClip(visualSystems.getPresets()[self.selectedPresetIndex].getID(),
							   parser.getAllClips()[allClipTable.selectedRow].getLinkName());
		
		visualSystems.savePresets();

		[self updateAssociatedClips];
		[self updateCurrentClipPresets];
		
		[presetTable reloadData];
		[allClipTable reloadData];
		[clipPresetTable reloadData];
	}
}

- (int) selectedPresetIndex
{
	if(presetTable.selectedRow >= 0){
		return filteredPresetInds[ presetTable.selectedRow ];
	}
	return -1;
}

- (IBAction) exportStandalone:(id)sender {
	visualSystems.exportStandalonePresets();
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	if(aTableView == presetTable){
		return filteredPresetInds.size();
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
	else if(aTableView == clipPresetTable){
		return currentClipPresets.size();
	}
	return 0;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	
	if(aTableView == presetTable){
		int presetIndex = filteredPresetInds[ rowIndex ];
        
        //check to refresh
        string refreshFlagPath = GetCloudsVisualSystemDataPath(visualSystems.getPresets()[presetIndex].systemName) + "Presets/Working/_refreshme.txt";
        ofFile refreshFile(refreshFlagPath);
        if(refreshFile.exists()){
            [self updatePresetsForSystem:visualSystems.getPresets()[presetIndex].systemName];
        }
        
		if([@"system" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: visualSystems.getPresets()[presetIndex].systemName.c_str()];
		}
		else if([@"grade" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String:
					(string(visualSystems.getPresets()[presetIndex].hasFiles ? "" : "!!") +
						   (visualSystems.getPresets()[presetIndex].enabled ? "+" : "-") +
							visualSystems.getPresets()[presetIndex].grade +
							(visualSystems.getPresets()[presetIndex].oculusCompatible ? "Oc" : "") +
							(visualSystems.getPresets()[presetIndex].hasSound ? "Snd" : "")
					 ).c_str()];
		}
		else if([@"preset" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: visualSystems.getPresets()[presetIndex].presetName.c_str()];
		}
		else if( [@"keywords" isEqualToString:aTableColumn.identifier] ){
			return [NSString stringWithUTF8String: ofJoinString( visualSystems.keywordsForPreset(presetIndex), ",").c_str() ];
		}
		else if( [@"timing" isEqualToString:aTableColumn.identifier] ){
			if(visualSystems.getPresets()[presetIndex].indefinite){
				return @"indefinite";
			}
			else {
				return [NSString stringWithFormat:@"%.02fs", visualSystems.getPresets()[presetIndex].duration];
			}
		}
	}
	else if(aTableView == clipTable){
		if([@"clip" isEqualToString:aTableColumn.identifier]){
			return [NSString stringWithUTF8String: associatedClips[rowIndex].getLinkName().c_str() ];
		}
		else if([@"keyword" isEqualToString:aTableColumn.identifier]){
			if(visualSystems.isClipLinked(visualSystems.getPresets()[self.selectedPresetIndex].getID(),
										  associatedClips[rowIndex].getLinkName() )){
				return @"LINK";
			}
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
		else{
			CloudsClip& clip = parser.getAllClips()[rowIndex];
			vector<CloudsVisualSystemPreset> presets =
				visualSystems.getPresetsForKeywords( clip.getKeywords(), clip.getLinkName() );
			int numPresets = 0;
			for(int i = 0; i < presets.size(); i++){
				if(!visualSystems.isClipSuppressed(presets[i].getID(), clip.getLinkName()) &&
				   presets[i].enabled)
				{
					numPresets++;
				}
			}
			return [NSString stringWithFormat:@"%d", numPresets];
		}
	}
	else if(aTableView == clipPresetTable){
		return [NSString stringWithUTF8String: currentClipPresets[rowIndex].getID().c_str() ];
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
- (IBAction) runTestsOnPresets:(id)sender{

    string systemName = visualSystems.getPresets()[ self.selectedPresetIndex ].systemName ;
    
    cout << "loading system " << systemName<< endl;
    
    testPresetIndeces = visualSystems.getFilteredPresetIndeces(true, false, false, systemName);
    
    random_shuffle( testPresetIndeces.begin(),testPresetIndeces.end() );
	
	currentTestPresetIndex = 0;
	lastSystemStartTime = ofGetElapsedTimef()-5;
	runningTest = true;

}

- (IBAction) runTests:(id)sender
{
	testPresetIndeces = visualSystems.getFilteredPresetIndeces(true,false,false);
	random_shuffle( testPresetIndeces.begin(),testPresetIndeces.end() );
	
	currentTestPresetIndex = 0;
	lastSystemStartTime = ofGetElapsedTimef()-5;
	runningTest = true;
}

- (IBAction) updateFilters:(id)sender
{

	filteredPresetInds = visualSystems.getFilteredPresetIndeces(filterEnabledBox.state == NSOnState,
																filterOculusBox.state  == NSOnState,
																filterGradeABox.state  == NSOnState);

	[presetTable selectRowIndexes:[NSIndexSet indexSet] byExtendingSelection:NO];
	
	[clipTable reloadData];
	[suppressedClipTable reloadData];
	[presetTable reloadData];
	[allKeywordTable reloadData];

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
			soundBox.state = (selectedPreset->hasSound ? NSOnState : NSOffState);
		}
		else{
			currentKeywords.stringValue = @"";
			notesText.stringValue = @"";
			grade.stringValue = @"";
			enabledBox.state = NSOnState;
			oculusBox.state = NSOffState;
		}
	}
	else if(aNotification.object == allClipTable){
		if(allClipTable.selectedRow >= 0){
			[self updateCurrentClipPresets];
		}
	}
}

- (void)updateCurrentClipPresets
{
	if(allClipTable.selectedRow < 0 ){
		return;
	}
	
	CloudsClip& clip = parser.getAllClips()[allClipTable.selectedRow];
	vector<CloudsVisualSystemPreset> presets =
	visualSystems.getPresetsForKeywords( clip.getKeywords(), clip.getLinkName() );
	currentClipPresets.clear();
	for(int i = 0; i < presets.size(); i++){
		if(!visualSystems.isClipSuppressed(presets[i].getID(), clip.getLinkName()) &&
		   presets[i].enabled)
		{
			currentClipPresets.push_back( presets[i] );
		}
	}

	[allClipTable reloadData];
	[clipPresetTable reloadData];
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
	
	if(self.selectedPresetIndex >= 0){
		selectedPreset = &visualSystems.getPresets()[ self.selectedPresetIndex ];
		associatedKeywords = visualSystems.keywordsForPreset(*selectedPreset);
		associatedClips = parser.getClipsWithKeyword(associatedKeywords);

		//create a set of all the names
		set<string> clipIds;
		for(int i = 0; i < associatedClips.size(); i++){
			clipIds.insert(associatedClips[i].getLinkName());
		}
		
		//insert the links
		vector<string>& links = visualSystems.getLinksForPreset(selectedPreset->getID());
		for(int i = 0; i < links.size(); i++){
			if(clipIds.find(links[i])  == clipIds.end()){
				associatedClips.push_back( parser.getClipWithLinkName(links[i]) );
			}
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
	}
	
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

- (IBAction) playSelectedRow:(id)sender
{
    shouldPlaySelectedRow = true;
}

- (IBAction) playPreviousPreset:(id)sender
{
	[presetTable selectRowIndexes:[NSIndexSet indexSetWithIndex:MAX(presetTable.selectedRow-1,0)] byExtendingSelection:NO];
	shouldPlaySelectedRow = true;
}

- (IBAction) playNextPreset:(id)sender
{
	[presetTable selectRowIndexes:[NSIndexSet indexSetWithIndex:MIN(presetTable.selectedRow+1,filteredPresetInds.size()-1)] byExtendingSelection:NO];
	shouldPlaySelectedRow = true;

}

- (void)playDoubleClickedRow:(id)sender
{
    shouldPlaySelectedRow = true;
}

////void testView::audioRequested(float *output, int bufferSize, int nChannels)
//- (void)audioRequested: (float*) output bufferSize:(int)bufferSize nChannels:(int)nChannels
//{
//    cout<<"audio requested"<<endl;
//    mixer.fillBuffer(output, bufferSize, nChannels);
//}

@end