#import "testView.h"
#include "CloudsVisualSystem.h"
#include "CloudsInputKinectOSC.h"


@implementation testView

- (void)setup
{
	currentVisualSystem = NULL;
	
	visualSystems.loadPresets();
    visualSystems.loadCachedDataForSystems();
	for(int i = 0; i < visualSystems.getPresets().size(); i++){
		if(visualSystems.getPresets()[i].enabledOculus){
			riftIndeces.push_back(i);
		}
	}
	
    [presetTable setTarget:self];
	[presetTable setDoubleAction:@selector(playPlaylist:)];
	[presetTable reloadData];

	maxSystemDuration = 50.0;
	
    mixer.setup();
    mixer.setDiageticVolume(1);
	
	string path = "index_save.txt";
	if(ofFile(path).exists()){
		ofBuffer b = ofBufferFromFile(path);
		NSMutableIndexSet* set = [NSMutableIndexSet indexSet];
		
		vector<string> components = ofSplitString(b.getText(), ",");
		for(int i = 0; i < components.size(); i++){
			[set addIndex:ofToInt(components[i])];
		}
		[presetTable selectRowIndexes:set byExtendingSelection:NO];
	}
}


- (IBAction) playPlaylist:(id)sender
{
	shouldPlayPlaylist = true;
	
	//save a file of indices
	vector<string> indeces;

	NSUInteger idx = [presetTable.selectedRowIndexes firstIndex];
	while (idx != NSNotFound) {
		indeces.push_back( ofToString(idx) );
		idx = [presetTable.selectedRowIndexes indexGreaterThanIndex:idx];
	}
	
	ofBuffer buf(ofJoinString(indeces, ","));
	string path = "index_save.txt";
	ofBufferToFile(path, buf);

}

- (void)update
{

	if(shouldPlayPlaylist){
		
		isPlayingPlaylist = false;
		shouldPlayPlaylist = false;
		playlist.clear();
		currentVisualSystem = NULL;
		visualSystems.DeallocateSystems();

		if(presetTable.selectedRow >= 0){
			
			NSUInteger idx = [presetTable.selectedRowIndexes firstIndex];
			while (idx != NSNotFound) {
			
				CloudsVisualSystemPreset& preset = visualSystems.getPresets()[ riftIndeces[idx] ];
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
	
	if(playlistIndex >= playlist.size()){
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
	if(isPlayingPlaylist){
		[self gotoNextSystem];
	}
}

- (void)mouseReleased:(NSPoint)p button:(int)button
{
	
}

- (void)windowResized:(NSSize)size
{
	
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
//	return visualSystems.getPresets().size();
	return riftIndeces.size();
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
	
//	int presetIndex = rowIndex;
	int presetIndex = riftIndeces[rowIndex];
	
	if([@"system" isEqualToString:aTableColumn.identifier]){
		return [NSString stringWithUTF8String: visualSystems.getPresets()[presetIndex].systemName.c_str()];
	}
	else if([@"preset" isEqualToString:aTableColumn.identifier]){
		return [NSString stringWithUTF8String: visualSystems.getPresets()[presetIndex].presetName.c_str()];
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