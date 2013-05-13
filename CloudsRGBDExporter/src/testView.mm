#import "testView.h"

@implementation testView

- (void)setup
{
	if(ofDirectory("../../../CloudsData/").exists()){
		parser.parseLinks("../../../CloudsData/links/clouds_link_db.xml");
		parser.setup("../../../CloudsData/fcpxml/");
	}
	else{
		cout << "SETTING UP IN DATA DIRECTORY" << endl;
		parser.parseLinks("clouds_link_db.xml");
		parser.setup("xml");
	}
	
	//    parser.parseLinks("../../../CloudsLibrary/data/links/clouds_link_db.xml");
    
    [clipTable reloadData];

	for(int i = 0; i < 8; i++){
		exportManagers.push_back(new CloudsClipExportManager());
		exportManagers[i]->setExportDirectory( "/Volumes/Seance/MediaPackages/_exports/" );
	}
}

- (void)update
{

	if(exporting){
		for(int i = 0; i < exportManagers.size(); i++){
			if(selectedClips.size() > 0  && exportManagers[i]->isDone()){
				cout << "EXPORTING CLIP " << selectedClips[0].getLinkName() << endl;;
				exportManagers[i]->exportClip(selectedClips[0]);
				selectedClips.erase(selectedClips.begin());
			}
			
//			cout << "EXPORTER " << i << " IS " << exportManagers[i]->percentComplete() << endl;
		}
	}
}

- (void)draw
{

}

- (IBAction) exportSelection:(id)sender
{
	
	cout << "export called!?!?" << endl;
	if(exporting){
		for(int i = 0; i < exportManagers.size(); i++){
			cout << "STOPPING THREAD " << i << endl;
			exportManagers[i]->waitForThread(true);
		}
	}
	
	selectedClips.clear();

	NSUInteger idx = [clipTable.selectedRowIndexes firstIndex];
	while (idx != NSNotFound) {
		// do work with "idx"
		selectedClips.push_back(parser.getAllClips()[idx]);
		
		// get the next index in the set
		idx = [clipTable.selectedRowIndexes indexGreaterThanIndex:idx];
	}
	
	exporting = selectedClips.size() > 0;
	cout << "exporting " << selectedClips.size() << endl;
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

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return parser.getAllClips().size();
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{

	return [NSString stringWithUTF8String: parser.getAllClips()[rowIndex].getLinkName().c_str() ];
	
//	string keyword = parser.getAllKeywords()[rowIndex];
//	
//	if([@"occurrence" isEqualToString:aTableColumn.identifier]){
//		return [NSNumber numberWithInt: parser.occurrencesOfKeyword(keyword)];
//	}
//	else{
//		return [NSString stringWithCString:keyword.c_str() encoding:NSUTF8StringEncoding];
//	}
	
}

@end