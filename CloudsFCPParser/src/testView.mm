#import "testView.h"
#import "ViewerApp.h"

@implementation testView
@synthesize clipEndFrame;
@synthesize preview;
@synthesize playingPlaylist;

- (void)setup
{
    ofBackground(220);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
	ofSetFrameRate(30);
	
	playingPlaylist = false;
	currentPlaylistIndex = 0;
	timeOfNextStory = 10;
	
    updatePhysics = true;
    
    [self refreshXML:self];
    clipEndFrame = 0;
    clipLoaded = NO;
    
    for (NSTableColumn *tableColumn in keywordTable.tableColumns ) {
        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
        [tableColumn setSortDescriptorPrototype:sortDescriptor];
    }
    
    for (NSTableColumn *tableColumn in clipTable.tableColumns ) {
        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
        [tableColumn setSortDescriptorPrototype:sortDescriptor];
    }
 
    [clipTable setDoubleAction:@selector(playDoubleClickedRow:)];
	[playlistTable setDoubleAction:@selector(playDoubleClickedRow:)];
	
    visualizer.database = &parser;
	storyEngine.setup();
	storyEngine.visualizer = &visualizer;
	
	//visualizer.setupGrid();
    //visualizer.exportForGraphviz();
	
    visualizer.setup();
	visualizer.setupPhysics();
	//visualizer.addTagToPhysics("technological progress");
	visualizer.addLinksToPhysics(parser.getAllClips()[0]);
	
	
	// Create a matrix for D3 chord visualization
	CloudsD3Exporter d3Exporter;
	d3Exporter.saveChordMatrix(parser);
	
	//visualizer.addAllClipsWithAttraction();
    //inpoint.setup();
    //important file!
	//gui = new ofxUICanvas(0,0,200,ofGetHeight());
}

- (IBAction) regenerateGraph:(id)sender
{
	/*
	string seedKeywordString = [seedKeyword.stringValue UTF8String];
	if(seedKeywordString != ""){
		visualizer.clear();
		visualizer.addTagToPhysics(seedKeywordString);
	}
	 */
	
	if(clipTable.selectedRow >= 0){
		visualizer.clear();
		ClipMarker& clip = [self selectedClip];
		visualizer.addLinksToPhysics(clip);
	}
}

- (IBAction) nextOnPlaylist:(id)sender
{
	if(playlistTable.selectedRow < visualizer.pathByClip.size()){
	   [playlistTable selectRowIndexes:[[NSIndexSet alloc] initWithIndex:playlistTable.selectedRow+1]
				  byExtendingSelection:NO];
		[self playCurrentPlaylist:playlistTable];
	}
}

- (IBAction) prevOnPlaylist:(id)sender
{
	if(playlistTable.selectedRow >= 0){
		[playlistTable selectRowIndexes:[[NSIndexSet alloc] initWithIndex:playlistTable.selectedRow-1]
				   byExtendingSelection:NO];
		[self playCurrentPlaylist:playlistTable];
	}
}

- (IBAction) playCurrentPlaylist:(id)sender
{
	[self playDoubleClickedRow:playlistTable];
}

- (IBAction) unloadVideo:(id)sender
{
	preview.stop();
}

- (void)update
{
    if(updatePhysics){
        visualizer.updatePhysics();
    }
	
	if(ofGetElapsedTimef() > timeOfNextStory){
		timeOfNextStory = ofGetElapsedTimef() + 1.5;
		storyEngine.selectNewClip();
	}
	
    if(visualizer.getPathChanged()){
		[playlistTable reloadData];
	}
}

- (void)draw
{
    visualizer.drawPhysics();
//    visualizer.drawGrid();
}

- (void)exit
{
	
}

- (void)keyPressed:(int)key
{
    //if(key == ' ') updatePhysics = !updatePhysics;
    
    if(key == ' '){
        [self playDoubleClickedRow: clipTable];
    }
}

- (IBAction) createLink:(id)sender
{
    NSLog(@"creating link");
    if(clipLoaded && clipTable.selectedRow >= 0){
        NSLog(@"clip is loaded and row is selected");
        ClipMarker& clip = [self selectedClip];
        if(clip.getLinkName() != currentPlayingClip.getLinkName()){
            
            NSLog(@"creating lip for %s + %s", clip.getLinkName().c_str(), currentPlayingClip.getLinkName().c_str());
            
            CloudsLink l;
            l.sourceName = currentPlayingClip.getLinkName();
            l.targetName = clip.getLinkName();
            l.startFrame = -1;
            l.endFrame = -1;
            
            //TODO figure out frame numbers
            parser.addLink(l);
            currentClipLinks = parser.getLinksForClip(currentPlayingClip.getLinkName());
            
            cout << "after creating link the current clip has " << currentClipLinks.size() << endl;
            
            [linkTable reloadData];
			
			//save
			[self saveLinks:self];
        }
    }
}

- (IBAction) saveLinks:(id)sender
{
    parser.saveLinks("clouds_link_db.xml");
}

- (IBAction) playDoubleClickedRow:(id)sender
{
	ClipMarker clip;
	if(sender == clipTable && clipTable.selectedRow >= 0){
        clip = [self selectedClip];
		playingPlaylist = false;
	}
	else if(sender == playlistTable && playlistTable.selectedRow >= 0){
		clip = [self selectedClipFromPlaylist];
		playingPlaylist = true;
	}
	else{
		//bail!
		return;
	}
	
	if(currentPlayingClip.getLinkName() == clip.getLinkName()){
		if(preview.isPlaying()){
			preview.stop();
		}
		else{
			preview.play();
		}
		return;
	}
	
	preview.stop();
	
	ofSleepMillis(500);
	if( clip.filePath != "" && ofFile(clip.filePath).exists() && preview.loadMovie(clip.filePath)){
		preview.setFrame(clip.startFrame);
		preview.play();
	}
	else {
		preview.loadMovie("/Users/focus/Desktop/CMUDemo/TAKE_02_25_14_49_09/color/MVI_7394.MOV");
		preview.play();
		
		NSLog(@"movie load failed %s", clip.filePath.c_str());
	}
	
	clipLoaded = YES;
	currentClipLabel.stringValue = [NSString stringWithUTF8String:clip.getLinkName().c_str()];
	currentClipLinks = parser.getLinksForClip(clip.getLinkName());
	
	cout << "current clips is of size " << currentClipLinks.size() << endl;
	currentPlayingClip = clip;
	
	clipEndFrame = clip.endFrame;
	
	[linkTable reloadData];

}

- (IBAction) deleteLink:(id)sender
{
    if(linkTable.selectedRow >= 0){
        cout << "removing link " << linkTable.selectedRow << " from " << currentPlayingClip.getLinkName() << endl;
        parser.removeLink(currentPlayingClip.getLinkName(), linkTable.selectedRow);
        currentClipLinks = parser.getLinksForClip(currentPlayingClip.getLinkName());
        
        [linkTable reloadData];
    }
}

- (ClipMarker&) selectedClip
{
    return (selectedKeywords.size() == 0) ? parser.getAllClips()[clipTable.selectedRow] : selectedClips[clipTable.selectedRow];
}

- (ClipMarker&) selectedClipFromPlaylist
{
	return visualizer.pathByClip[ playlistTable.selectedRow ];
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

- (IBAction) setXMLFolder:(id)sender
{
    
}

- (IBAction) refreshXML:(id)sender
{
    parser.setup("xml");
    parser.parseLinks("clouds_link_db.xml");
    
    [keywordTable reloadData];
    [clipTable reloadData];
    
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
 //   cout << "returning some rows " << (NSInteger)parser.getAllKeywords().size() << endl;
    if(aTableView == keywordTable){
        return (NSInteger)parser.getAllKeywords().size();
    }
    else if(aTableView == clipTable){
        return (NSInteger) (selectedKeywords.size() == 0) ? parser.getAllClips().size() : selectedClips.size();
    }
    else if(aTableView == linkTable){
        return currentClipLinks.size();
    }
	else if(aTableView == playlistTable){
		return visualizer.pathByClip.size();
	}
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    
    if(aTableView == keywordTable){
        string keyword = parser.getAllKeywords()[rowIndex];
        if([@"occurrence" isEqualToString:aTableColumn.identifier]){
            return [NSNumber numberWithInt: parser.occurrencesOfKeyword(keyword)];
        }
        else{
            return [NSString stringWithCString:keyword.c_str() encoding:NSUTF8StringEncoding];
        }
    }
    else if(aTableView == clipTable){
		ClipMarker& m = (selectedKeywords.size() == 0) ? parser.getAllClips()[rowIndex] : selectedClips[rowIndex];
//		NSLog(@"Identifer is %@", aTableColumn.identifier);
		if([@"Links" isEqualToString:aTableColumn.identifier]){
			return [NSNumber numberWithInt: parser.getLinksForClip( m.getLinkName() ).size()];
		}
		else {
			//ClipMarker& m = [self selectedClip];
			string linkString = m.person + " - " + m.name + " - " + m.clip + ": [" + ofToString(m.startFrame) + "," + ofToString(m.endFrame) + "]";
			return [NSString stringWithUTF8String:linkString.c_str()];
		}
    }
    else if(aTableView == linkTable){
        string clipTableEntry = currentClipLinks[rowIndex].targetName;
        if(currentClipLinks[rowIndex].startFrame != -1 && currentClipLinks[rowIndex].endFrame != -1){
            clipTableEntry += "[" + ofToString(currentClipLinks[rowIndex].startFrame) + " - " + ofToString(currentClipLinks[rowIndex].endFrame) + "]";
        }
        return [NSString stringWithUTF8String:clipTableEntry.c_str()];
    }
	else if(aTableView == playlistTable){
		return [NSString stringWithUTF8String: visualizer.pathByClip[rowIndex].getLinkName().c_str()];
	}
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    if(aNotification.object == keywordTable){
        selectedKeywords.clear();
        selectedClips.clear();
        if(keywordTable.selectedRow >= 0){
        
            NSUInteger idx = [keywordTable.selectedRowIndexes firstIndex];
            while (idx != NSNotFound) {
                // do work with "idx"
                selectedKeywords.push_back(parser.getAllKeywords()[idx]);
                
                // get the next index in the set
                idx = [keywordTable.selectedRowIndexes indexGreaterThanIndex:idx];
            }
            selectedClips = parser.getClipsWithKeyword(selectedKeywords);
        }
            
        [clipTable reloadData];
        
        /*
        string selection = parser.getAllKeywords()[keywordTable.selectedRow] ;
        NSString* nsStringSelection = [NSString stringWithCString:selection.c_str()
                                                         encoding:NSUTF8StringEncoding];

        if(![self hasKeyword:nsStringSelection]){
            [currentKeywords setStringValue: [currentKeywords.stringValue stringByAppendingFormat:@", %@", nsStringSelection]];
        }
        */
    }
    else if(aNotification.object == clipTable){
        if(clipTable.selectedRow < 0){
            return;
        }

        //ClipMarker& m = (selectedKeywords.size() == 0) ? parser.getAllClips()[clipTable.selectedRow] : selectedClips[clipTable.selectedRow];
        ClipMarker& m = [self selectedClip];
        
        string keywordList = "";
        currentKeywords.stringValue = [NSString stringWithUTF8String:ofJoinString(m.keywords, ",").c_str()];
        //linkText.stringValue = [NSString stringWithUTF8String:("link:" + m.person + " - " + m.name).c_str()];
        linkText.stringValue = [NSString stringWithUTF8String:m.getLinkName().c_str()];
    }
    else if(aNotification.object == linkTable){
        if(linkTable.selectedRow < 0){
            return;
        }
        
        vector<ClipMarker>& searchClips = (selectedKeywords.size() == 0) ? parser.getAllClips() : selectedClips;
        //ClipMarker& m = [self selectedClip];
        string targetClip = currentClipLinks[ linkTable.selectedRow ].targetName;
        for(int i = 0; i < searchClips.size(); i++){
            if(searchClips[i].name == targetClip){
                NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:i];
                [clipTable selectRowIndexes:indexSet byExtendingSelection:NO];
                break;
            }
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
    for(int i = 0; i < parser.getAllKeywords().size(); i++){
        NSString* stringKeyword = [NSString stringWithUTF8String:parser.getAllKeywords()[i].c_str()];
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

- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors
{
    NSArray *newDescriptors = [tableView sortDescriptors];
    NSLog(@"sort descriptor %@", [newDescriptors objectAtIndex:0]);
    if(tableView == keywordTable){
        parser.sortKeywordsByOccurrence( [ [[newDescriptors objectAtIndex:0] key]  isEqualToString:@"occurrence"] );
    }
    
//    [results sortUsingDescriptors:newDescriptors];
    //"results" is my NSMutableArray which is set to be the data source for the NSTableView object.
    [tableView reloadData];
}

- (BOOL)tableView:(NSTableView *)aTableView shouldEditTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    return NO;
}
@end