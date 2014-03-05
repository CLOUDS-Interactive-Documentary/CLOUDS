#import "testView.h"
#include "CloudsVisualSystem.h"
#include "CloudsInputKinectOSC.h"


@implementation testView

- (void)setup
{
	currentVisualSystem = NULL;
	
	visualSystems.loadPresets();
    visualSystems.loadCachedDataForSystems();
		
    [presetTable setTarget:self];
	[presetTable setDoubleAction:@selector(playPlaylist:)];
	[presetTable reloadData];

	maxSystemDuration = 35.0;
	
    mixer.setup();
    mixer.setDiageticVolume(1);

}


- (IBAction) playPlaylist:(id)sender
{
	shouldPlayPlaylist = true;
}

- (void)update
{

	if(shouldPlayPlaylist){
		
		shouldPlayPlaylist = false;
		playlist.clear();
		currentVisualSystem = NULL;
		visualSystems.DeallocateSystems();

		if(presetTable.selectedRow >= 0){
			
			NSUInteger idx = [presetTable.selectedRowIndexes firstIndex];
			while (idx != NSNotFound) {
			
				CloudsVisualSystemPreset& preset = visualSystems.getPresets()[ idx ];
				CloudsVisualSystem* system = CloudsVisualSystemManager::InstantiateSystem( preset.systemName ) ;
				if(system == NULL){
					continue;
				}
				system->setup();
				system->loadPresetGUISFromName(preset.presetName);
				
				playlist.push_back( system );
				
				cout << "Selected preset " << preset.systemName << endl;
				
				// get the next index in the set
				idx = [presetTable.selectedRowIndexes indexGreaterThanIndex:idx];
			}
		}
		
		if(playlist.size() > 0){
			isPlayingPlaylist = true;
			playlistIndex = -1;
			[self gotoNextSystem];
		}
	}
	else if(isPlayingPlaylist){
		if( ofGetElapsedTimef() - systemStartTime > maxSystemDuration){
			[self gotoNextSystem];
		}
	}
	
	/*
	if(runningTest){
		
		if(ofGetElapsedTimef() - lastSystemStartTime > 1){
			
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


                currentVisualSystem = NULL;
				
				int i = 0;
				for(i = currentTestPresetIndex; i < MIN(testPresetIndeces.size(),currentTestPresetIndex+2); i++){
					
					CloudsVisualSystemPreset& preset = visualSystems.getPresets()[ testPresetIndeces[i] ];
					cout << "******* 1) (" << i << "/" << testPresetIndeces.size() << ") INSTANTIATING " << preset.systemName << " : " << preset.presetName << endl;
					CloudsVisualSystem* vs = CloudsVisualSystemManager::InstantiateSystem( preset.systemName );
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
            currentVisualSystem = NULL;
        }

		cout << "loading system " << visualSystems.getPresets()[ self.selectedPresetIndex ].systemName << " preset " << visualSystems.getPresets()[self.selectedPresetIndex].presetName << endl;
        
        visualSystems.DeallocateSystems();
        currentVisualSystem = CloudsVisualSystemManager::InstantiateSystem( visualSystems.getPresets()[ self.selectedPresetIndex ].systemName );
		
		///SCREENCAPTURE MODE
		currentVisualSystem->setNumSamples(4);
		currentVisualSystem->forceScreenResolution(1920, 1080);
		currentVisualSystem->setDrawToScreen(false);
		/////
		
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
	 */
	
}

- (void)draw
{
	if(currentVisualSystem != NULL){
		currentVisualSystem->selfPostDraw();
	}
}

- (void) gotoNextSystem
{
	if(currentVisualSystem != NULL){
		currentVisualSystem->stopSystem();
		currentVisualSystem->exit();
		currentVisualSystem = NULL;
	}
	
	playlistIndex++;
	
	if(playlistIndex == playlist.size()){
		//restart the same playlist
		shouldPlayPlaylist = true;
	}
	else{
		currentVisualSystem = playlist[playlistIndex];
		currentVisualSystem->playSystem();
		systemStartTime = ofGetElapsedTimef();
	}
}

- (void)exit
{
	if(currentVisualSystem != NULL){
		currentVisualSystem->stopSystem();
		currentVisualSystem->exit();
        visualSystems.DeallocateSystems();
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
	[self gotoNextSystem];
}

- (void)mouseReleased:(NSPoint)p button:(int)button
{
	
}

- (void)windowResized:(NSSize)size
{
	
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return visualSystems.getPresets().size();
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	
	int presetIndex = rowIndex;
	
	if([@"system" isEqualToString:aTableColumn.identifier]){
		return [NSString stringWithUTF8String: visualSystems.getPresets()[presetIndex].systemName.c_str()];
	}
//	else if([@"grade" isEqualToString:aTableColumn.identifier]){
//		return [NSString stringWithUTF8String:
//				(string(visualSystems.getPresets()[presetIndex].hasFiles ? "" : "!!") +
//					   (visualSystems.getPresets()[presetIndex].enabledScreen ? "Sc" : "") +
//						visualSystems.getPresets()[presetIndex].grade +
//						(visualSystems.getPresets()[presetIndex].enabledOculus ? "Oc" : "") +
//						(visualSystems.getPresets()[presetIndex].hasSound() ? "Snd" : "")
//				 ).c_str()];
//	}
	else if([@"preset" isEqualToString:aTableColumn.identifier]){
		return [NSString stringWithUTF8String: visualSystems.getPresets()[presetIndex].presetName.c_str()];
	}
//	else if( [@"keywords" isEqualToString:aTableColumn.identifier] ){
//		return [NSString stringWithUTF8String: ofJoinString( visualSystems.keywordsForPreset(presetIndex), ",").c_str() ];
//	}
//	else if( [@"timing" isEqualToString:aTableColumn.identifier] ){
//		if(visualSystems.getPresets()[presetIndex].indefinite){
//			return @"indefinite";
//		}
//		else {
//			return [NSString stringWithFormat:@"%.02fs", visualSystems.getPresets()[presetIndex].duration];
//		}
//
//	}
	return @"-";
	
}


@end