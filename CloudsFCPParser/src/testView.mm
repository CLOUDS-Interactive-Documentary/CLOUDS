#import "testView.h"
#import "ViewerApp.h"
#include "CloudsClip.h"

@implementation testView
@synthesize movieFileMissing;
@synthesize clipEndFrame;
@synthesize preview;
@synthesize playingPlaylist;

- (void)setup
{
	ofSeedRandom();
	
    ofBackground(220);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
	ofSetFrameRate(30);
	
	movieFileMissing = false;
	autoProgressStory = false;
	onPlaylist = false;
	
	playingPlaylist = false;
	currentPlaylistIndex = 0;
	timeOfNextStory = 7;
	
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
	
//	visualizer.database = &parser;
    visualizer.setup(parser);
	ofAddListener(storyEngine.getEvents().storyBegan, &visualizer, &CloudsFCPVisualizer::storyBegan);
	ofAddListener(storyEngine.getEvents().clipChanged, &visualizer, &CloudsFCPVisualizer::clipChanged);
	
	visualizer.setupPhysics();

	storyEngine.setup();
	storyEngine.network = &parser;
	storyEngine.maxTimesOnTopic = 2;
	
	
	float randomClip = ofRandom(parser.getAllClips().size() );
	
	cout << "seeding random " << randomClip << "/" << parser.getAllClips().size() << endl;
	
	storyEngine.seedWithClip(parser.getAllClips()[ int(randomClip) ]);
	[playlistTable reloadData];
	
	[self playCurrentPlaylist:self];
	
	font.loadFont("materiapro_light.ttf", 12);
	
	cout << ofToString(parser.percentOfClipsLinked(), 4) << " percent clips linked!" << endl;
	
	//visualizer.setupGrid();
    //visualizer.exportForGraphviz();
	//visualizer.addTagToPhysics("technological progress");
	//visualizer.addLinksToPhysics(parser.getAllClips()[0]);
	
	// Create a matrix for D3 chord visualization
    //CloudsD3Exporter d3Exporter;
    //d3Exporter.saveChordMatrix(parser);
	
	//visualizer.addAllClipsWithAttraction();
    //inpoint.setup();
    //important file!

	
	gui = new ofxUICanvas(0,0,200,ofGetHeight());
	gui->addSlider("spring strength", 0.001, .5, &visualizer.springStrength);
	gui->addSlider("spring rest length", 1, 50, &visualizer.restLength);
	gui->addSlider("node repulsion", 1, 50, &visualizer.repulsionForce);
	gui->addSlider("node size min", .5, 100, &visualizer.minRadius);
	gui->addSlider("node size max", .5, 100, &visualizer.maxRadius);
	
	gui->loadSettings("settings.xml");
	gui->disable();
	
	exporter.saveGephiCSV(parser);
}

- (IBAction) regenerateGraph:(id)sender
{
	timeOfNextStory = ofGetElapsedTimef() + 5;
	/*
	string seedKeywordString = [seedKeyword.stringValue UTF8String];
	if(seedKeywordString != ""){
		visualizer.clear();
		visualizer.addTagToPhysics(seedKeywordString);
	}
	 */
	preview.stop();
	
	if(playlistTable.selectedRow >= 0){
		[playlistTable deselectAll:self];
	}
	
	if(clipTable.selectedRow >= 0){
		visualizer.clear();
		CloudsClip& clip = [self selectedClip];
		//visualizer.addLinksToPhysics(clip);
		//TODO: selected topic
		storyEngine.seedWithClip(clip);
		[self playCurrentPlaylist:self];
	}
}

- (float) clipPercentComplete
{

	if(movieFileMissing){
		return (ofGetElapsedTimef() - storyStartTime) / (timeOfNextStory - storyStartTime);
	}
	else if(preview.isLoaded() && preview.isPlaying()){
		return (1.0*preview.getCurrentFrame() - currentPlayingClip.startFrame) / (currentPlayingClip.endFrame - currentPlayingClip.startFrame);
	}
	return 0;
}

- (IBAction) nextOnPlaylist:(id)sender
{
//	if(playlistTable.selectedRow < visualizer.pathByClip.size()){

	if(playlistTable.selectedRow == storyEngine.getClipHistory().size()-1){
		if(!storyEngine.selectNewClip()){
			return;
		}
		[playlistTable reloadData];
	}
	
	if(playlistTable.selectedRow < storyEngine.getClipHistory().size()){
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

	if(playlistTable.selectedRow < 0){
		[playlistTable selectRowIndexes:[[NSIndexSet alloc] initWithIndex:0]
				   byExtendingSelection:NO];
	}
	
	[self playDoubleClickedRow:playlistTable];
	onPlaylist = true;
}

- (IBAction) addClip:(id)sender
{
	storyEngine.selectNewClip();
	[playlistTable reloadData];
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
	
	if(movieFileMissing){
		if(ofGetElapsedTimef() > timeOfNextStory){
			[self nextOnPlaylist:self];
		}
	}
	//auto progressive story just gooes from one clip to the next
	if(autoProgressStory){
		if(ofGetElapsedTimef() > timeOfNextStory){
			timeOfNextStory = ofGetElapsedTimef() + .5;
			[self nextOnPlaylist:self];
		}
	}
	//most of the time we want to wait for a clip to finish or be interrupted by user interaction
	else if(preview.isLoaded()) {
		preview.update();
		if(preview.getCurrentFrame() >= clipEndFrame){
			preview.stop();
			if(onPlaylist){
				[self nextOnPlaylist:self];
			}
		}
	}

    if(visualizer.getPathChanged()){
		[playlistTable reloadData];
	}
}

- (void)draw
{

	ofBackground(0);
	
//	ofBackgroundGradient(ofColor::black,
//						 ofColor::darkGray*.15,
//						 OF_GRADIENT_LINEAR);
	
    visualizer.drawPhysics();

	string debug = "";
	debug += "Current Topic: " + storyEngine.getCurrentTopic() + " (" + ofToString(storyEngine.getTimesOnTopic()) + "/" + ofToString(storyEngine.maxTimesOnTopic) + ")\n";
	debug += "Watched " + ofxTimecode::timecodeForSeconds( storyEngine.getTotalSecondsWatched() ) + " from " + ofToString( storyEngine.getClipHistory().size()  ) + "\n";
	debug += "Covered " + ofToString( storyEngine.getClipHistory().size() ) + " / " + ofToString( parser.getAllClips().size() ) + "\n";
	
	ofPushStyle();
	ofSetColor(210);
	font.drawString(debug, 30,30);
	ofPopStyle();
}

- (void) exit
{
	NSLog(@"exit!!");
	gui->saveSettings("settings.xml");
}

- (void)keyPressed:(int)key
{
    //if(key == ' ') updatePhysics = !updatePhysics;
    
    if(key == ' '){
        [self playDoubleClickedRow: clipTable];
    }
	
	if(key == 'h'){
		cout << "toggling vis" << endl;
		gui->disable();
	}
	if(key == 's'){
		gui->enable();
	}
}

- (IBAction) createLink:(id)sender
{
    NSLog(@"creating link. edge selected? %@", visualizer.isEdgeSelected() ? @"YES" : @"NO" );
	
	if(visualizer.isEdgeSelected() && !visualizer.isSelectedEdgeLink() ){
		
		NSLog(@"Edge is selected!!");
		
		[self linkClip:visualizer.getEdgeSource() toClip:visualizer.getEdgeDestination() ];
		visualizer.linkedEdge();
		
		[clipTable reloadData];
		[linkTable reloadData];

	}
	else if(clipLoaded && clipTable.selectedRow >= 0){
		
		NSLog(@"clip is loaded and row is selected");
		
		[self linkClip:currentPlayingClip toClip:[self selectedClip] ];
		
		currentClipLinks = parser.getLinksForClip(currentPlayingClip.getLinkName());

		[clipTable reloadData];
		[linkTable reloadData];

		cout << "after creating link the current clip has " << currentClipLinks.size() << endl;		
	}
}

//CONFUSING: delete link is called from the link table, remove link called from the graph
- (IBAction) deleteLink:(id)sender
{
    if(linkTable.selectedRow >= 0){
        cout << "removing link " << linkTable.selectedRow << " from " << currentPlayingClip.getLinkName() << endl;
        parser.removeLink(currentPlayingClip.getLinkName(), linkTable.selectedRow);
        currentClipLinks = parser.getLinksForClip(currentPlayingClip.getLinkName());
        
        [linkTable reloadData];
		[self saveLinks:self];
    }
}

- (IBAction) removeLink:(id)sender
{
	if(	visualizer.isSelectedEdgeLink() ){
		cout << "removing link!" << endl;
		parser.removeLink(visualizer.getEdgeSource().getLinkName(),
						  visualizer.getEdgeDestination().getLinkName() );
		
		visualizer.unlinkEdge();
		[self saveLinks:self];
	}
}


- (IBAction) suppressLink:(id)sender
{
	if(visualizer.isEdgeSelected()){
		if( visualizer.isSelectedEdgeLink() ){
			parser.removeLink(visualizer.getEdgeSource().getLinkName(),
							  visualizer.getEdgeDestination().getLinkName() );
		}
		
		NSLog(@"Edge is selected!!");
		
		[self suppressLink:visualizer.getEdgeSource() toClip:visualizer.getEdgeDestination() ];
		visualizer.suppressEdge();
		
		[clipTable reloadData];
		[linkTable reloadData];
		
	}
}

- (IBAction) unsuppressLink:(id)sender
{
	if(	visualizer.isSelectedEdgeSuppressed() ){
		cout << "removing suppression!" << endl;
		parser.unsuppressConnection(visualizer.getEdgeSource().getLinkName(),
									visualizer.getEdgeDestination().getLinkName());
		
		visualizer.unsuppressEdge();
		[self saveLinks:self];

	}
}

- (void) linkClip:(CloudsClip) source toClip:(CloudsClip) target
{
	if(source.getLinkName() != target.getLinkName()){
		
		NSLog(@"creating link for %s + %s", source.getLinkName().c_str(), target.getLinkName().c_str());
		
		CloudsLink l;
		l.sourceName = source.getLinkName();
		l.targetName = target.getLinkName();
		l.startFrame = -1;
		l.endFrame = -1;
		
		//TODO figure out frame numbers
		parser.addLink(l);
		
		//save
		[self saveLinks:self];
	}
	else{
		NSLog(@"failed clip for %s + %s", source.getLinkName().c_str(), target.getLinkName().c_str());
	}
}

- (void) suppressLink:(CloudsClip)source toClip:(CloudsClip) target
{
	if(source.getLinkName() != target.getLinkName()){
		
		NSLog(@"suppressing link between %s + %s", source.getLinkName().c_str(), target.getLinkName().c_str());
		
		CloudsLink l;
		l.sourceName = source.getLinkName();
		l.targetName = target.getLinkName();
		l.startFrame = -1;
		l.endFrame = -1;

		parser.suppressConnection(l);

		[self saveLinks:self];
	}
}

- (IBAction) linkLast:(id)sender
{
	if(playlistTable.numberOfRows > 1){
		CloudsClip& a = storyEngine.getClipHistory()[ playlistTable.selectedRow-1 ];
		CloudsClip& b = storyEngine.getClipHistory()[ playlistTable.selectedRow ];
		if(!parser.clipLinksTo(a.getLinkName(), b.getLinkName())){
			[self linkClip:a toClip:b];
		}
		else{
			parser.removeLink(a.getLinkName(), b.getLinkName());
		}
		visualizer.linkPathEdge(playlistTable.selectedRow-1);
		[self saveLinks:self];
	}
	
}

- (IBAction) suppressLast:(id)sender
{
	if(playlistTable.numberOfRows > 1){
		CloudsClip& a = storyEngine.getClipHistory()[ playlistTable.selectedRow-1 ];
		CloudsClip& b = storyEngine.getClipHistory()[ playlistTable.selectedRow ];

		if(!parser.linkIsSuppressed(a.getLinkName(), b.getLinkName())){
			cout << "link is not currently suppressed, suppressing!" << endl;
			[self suppressLink:a toClip:b];
		}
		else{
			cout << "link is suppressed -- getting rid of it!" << endl;
			parser.unsuppressConnection(a.getLinkName(), b.getLinkName());
		}
		visualizer.suppressPathEdge(playlistTable.selectedRow-1);
		[self saveLinks:self];
	}
}

- (IBAction) setStartText:(id)sender{
    //button pressed
   CloudsClip& m = [self selectedClip];
   string q =   std::string([startQuestion.stringValue UTF8String]);

    m.setStartingQuestion(q );
    cout<<"Set the question for clip"<<m.getLinkName()<<"::"<<m.getStartingQuestion()<<endl;
    //get the txt from textfield, get currently select clip, and set it
    
    
}

- (IBAction) saveLinks:(id)sender
{
	if(ofDirectory("../../../CloudsData/links/").exists()){
		parser.saveLinks("../../../CloudsData/links/clouds_link_db.xml");
	}
	else{
		parser.saveLinks("clouds_link_db.xml");
	}
}

- (IBAction) playDoubleClickedRow:(id)sender
{
	
	onPlaylist = false; //will get set to true again if we are coming from a playlist
	
	CloudsClip clip;
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
	
	if(currentPlayingClip.getLinkName() == clip.getLinkName() && !autoProgressStory){
		if(preview.isPlaying()){
			preview.stop();
		}
		else{
			preview.play();
		}
		return;
	}
	
	preview.stop();
	
	string clipFilePath = clip.sourceVideoFilePath;
	if(clipFilePath  == "" || autoProgressStory){
		return;
	}
	
	ofSleepMillis(250);
	if( !ofFile(clipFilePath).exists() ){

		ofStringReplace(clipFilePath, "Nebula_backup", "Seance");		
		ofStringReplace(clipFilePath, "Nebula", "Seance");

	}
		
	if( preview.loadMovie(clipFilePath) ){
		movieFileMissing = false;
	}
	else{
		ofLogError() << "Clip " << clipFilePath << " failed to load.";
		movieFileMissing = true;
//		return;
	}
	
	
	clipLoaded = YES;
	currentClipLabel.stringValue = [NSString stringWithUTF8String:clip.getLinkName().c_str()];
	currentClipLinks = parser.getLinksForClip(clip.getLinkName());
	
//	cout << "current clips has " << currentClipLinks.size() << " links" << endl;
	
	currentPlayingClip = clip;
	
	clipEndFrame = clip.endFrame;

	if(movieFileMissing){
//		storyStartTime = ofGetElapsedTimef();
//		timeOfNextStory = storyStartTime + (clip.endFrame - clip.startFrame) / 24.0;
	}
	else{
		preview.setFrame(clip.startFrame);
		preview.play();
	}

	[linkTable reloadData];

}

- (CloudsClip&) selectedClip
{
    return (selectedKeywords.size() == 0) ? parser.getAllClips()[clipTable.selectedRow] : selectedClips[clipTable.selectedRow];
}

- (CloudsClip&) selectedClipFromPlaylist
{
	//return visualizer.pathByClip[ playlistTable.selectedRow ];
	return storyEngine.getClipHistory()[ playlistTable.selectedRow ];
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
	visualizer.physics.setWorldMax(ofVec2f(size.width, size.height));
	visualizer.width  = size.width;
	visualizer.height = size.height;
}

- (IBAction) setXMLFolder:(id)sender
{
    
}

- (IBAction) refreshXML:(id)sender
{
//    parser.setup("xml");
//    parser.parseLinks("clouds_link_db.xml");
	
	if(ofDirectory("../../../CloudsData/").exists()){
		parser.setup("../../../CloudsData/fcpxml/");
		parser.parseLinks("../../../CloudsData/links/clouds_link_db.xml");

	}
	else{
		cout << "SETTING UP IN DATA DIRECTORY" << endl;
		parser.setup("xml");
		parser.parseLinks("clouds_link_db.xml");
	}

//    parser.parseLinks("../../../CloudsLibrary/data/links/clouds_link_db.xml");
    
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
		//return visualizer.pathByClip.size();
		return storyEngine.getClipHistory().size();
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
		CloudsClip& m = (selectedKeywords.size() == 0) ? parser.getAllClips()[rowIndex] : selectedClips[rowIndex];
//		NSLog(@"Identifer is %@", aTableColumn.identifier);
		if([@"Links" isEqualToString:aTableColumn.identifier]){
			return [NSNumber numberWithInt: parser.getLinksForClip( m.getLinkName() ).size()];
		}
		else {
			//CloudsClip& m = [self selectedClip];
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
		//return [NSString stringWithUTF8String: visualizer.pathByClip[rowIndex].getLinkName().c_str()];
		return [NSString stringWithUTF8String: storyEngine.getClipHistory()[rowIndex].getLinkName().c_str()];		
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

        //CloudsClip& m = (selectedKeywords.size() == 0) ? parser.getAllClips()[clipTable.selectedRow] : selectedClips[clipTable.selectedRow];
        CloudsClip& m = [self selectedClip];
        
        string keywordList = "";
        currentKeywords.stringValue = [NSString stringWithUTF8String:ofJoinString(m.keywords, ",").c_str()];
        //linkText.stringValue = [NSString stringWithUTF8String:("link:" + m.person + " - " + m.name).c_str()];
        linkText.stringValue = [NSString stringWithUTF8String:m.getLinkName().c_str()];
        
        startQuestion.stringValue = [NSString stringWithUTF8String:m.getStartingQuestion().c_str()];
    }
    else if(aNotification.object == linkTable){
        if(linkTable.selectedRow < 0){
            return;
        }
        
        vector<CloudsClip>& searchClips = (selectedKeywords.size() == 0) ? parser.getAllClips() : selectedClips;
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