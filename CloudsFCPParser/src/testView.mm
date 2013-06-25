#import "testView.h"
#import "ViewerApp.h"
#include "CloudsClip.h"

@implementation testView

@synthesize movieFileMissing;
@synthesize clipEndFrame;
@synthesize preview;

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
    //keywordTableSource.tableColumns

//    for (NSTableColumn *tableColumn in linkerA->keywordTable.tableColumns) {
//        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
//        [tableColumn setSortDescriptorPrototype:sortDescriptor];
//    }
//    
//    for (NSTableColumn *tableColumn in keywordTableTarget.tableColumns ) {
//        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
//        [tableColumn setSortDescriptorPrototype:sortDescriptor];
//    }
//    
//    for (NSTableColumn *tableColumn in clipTableSource.tableColumns ) {
//        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
//        [tableColumn setSortDescriptorPrototype:sortDescriptor];
//    }
//    
//    for (NSTableColumn *tableColumn in clipTableTarget.tableColumns ) {
//        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
//        [tableColumn setSortDescriptorPrototype:sortDescriptor];
//    }
//    
//    [clipTableSource setDoubleAction:@selector(playDoubleClickedRow:)];
//	[playlistTable setDoubleAction:@selector(playDoubleClickedRow:)];
 //   [clipTableTarget setDoubleAction:@selector(playDoubleClickedRow:)];
	
    //	visualizer.database = &parser;
//    visualizer.setup(parser);
    //SURYA MOD:
    //	ofAddListener(storyEngine.getEvents().storyBegan, &visualizer, &CloudsClusterVisualiser::storyBegan);
    //	ofAddListener(storyEngine.getEvents().clipBegan, &visualizer, &CloudsClusterVisualiser::clipBegan);
//	ofAddListener(storyEngine.getEvents().storyBegan, &visualizer, &CloudsFCPVisualizer::storyBegan);
//	ofAddListener(storyEngine.getEvents().clipBegan, &visualizer, &CloudsFCPVisualizer::clipBegan);
    //    ofAddListener(storyEngine.getEvents().clipEnded, &visualizer, &CloudsFCPVisualizer::clipEnded);
	
//	visualizer.setupPhysics();
    
//	storyEngine.setup();
//	storyEngine.network = &parser;
//	storyEngine.maxTimesOnTopic = 4;
//	storyEngine.fixedClipDelay = .1;
	
//	float randomClip = ofRandom(parser.getAllClips().size() );
	
//	cout << "seeding random " << randomClip << "/" << parser.getAllClips().size() << endl;
	
	//storyEngine.seedWithClip(parser.getRandomClip(false,true));
//	[playlistTable reloadData];
	
//	[self playCurrentPlaylist:self];
	
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
    
	
//	gui = new ofxUICanvas(0,0,200,ofGetHeight());
//	gui->addSlider("spring strength", 0.001, .5, &visualizer.springStrength);
//	gui->addSlider("spring rest length", 1, 50, &visualizer.restLength);
//	gui->addSlider("node repulsion", 1, 50, &visualizer.repulsionForce);
//	gui->addSlider("node size min", .5, 100, &visualizer.minRadius);
//	gui->addSlider("node size max", .5, 100, &visualizer.maxRadius);
    //    gui->addSlider("zoom", 0.1, 1, &visualizer.zoom);
    //   gui->addSlider("current Scale", .5, 500, &visualizer.currentScale);
	
//	gui->loadSettings("settings.xml");
//	gui->disable();
    
    linkerA.parser = &parser;
    linkerB.parser = &parser;
	[linkerA setup];
    [linkerB setup];
    linkerA.testViewParent = self;
    linkerB.testViewParent = self;
    
//	exporter.saveGephiCSV(parser);
    
}

//-(IBAction)previewLinks:(id)sender
//{
//    if(linkTable.selectedRow>=0){
//        string linkPreviewClip = currentClipLinks[linkTable.selectedRow].targetName;
//        CloudsClip clip = parser.getClipWithLinkName(linkPreviewClip);
//        
//        [self playClip:clip];
//    
//    }
//}

//-(IBAction)previewSuppressed:(id)sender
//{
//    if(suppressedTable.selectedRow>=0){
//        string suppressedPreviewClip = currentSuppressedLinks[suppressedTable.selectedRow].targetName;
//        
//        CloudsClip clip = parser.getClipWithLinkName(suppressedPreviewClip);
//		cout << "PLAYING CLIP " << suppressedPreviewClip << " clip name returned " << clip.getLinkName() << " clip " << clip.sourceVideoFilePath << endl;
//		
//        [self playClip:clip];
//        
//    }
//    
//}

-(IBAction)stopPreview:(id)sender
{
    preview.stop();
}

//- (IBAction) showQuestiosnChanged:(id)sender
//{
//	cout << "CALLING SHOW QUESTIONS " << endl;
	
//	if(showOnlyQuestions.state == NSOnState){
//		[keywordTableSource selectRowIndexes:nil
//                        byExtendingSelection:NO];
//        selectedKeywordsSource.clear();
//		selectedClipsSource.clear();
//		for(int i = 0; i < parser.getAllClips().size(); i++ ){
//			if(parser.getAllClips()[i].hasStartingQuestion()){
//                cout<<"Question Loop: "<<parser.getAllClips()[i].getLinkName()<<endl;
//				selectedClipsSource.push_back( parser.getAllClips()[i] );
//			}
//		}
//        
//	}
//	else{
//		selectedKeywordsSource.clear();
//		selectedClipsSource.clear();
//	}
//	
//	[keywordTableSource reloadData];
//	[clipTableSource reloadData];
//}

//- (IBAction) regenerateGraph:(id)sender
//{
//	timeOfNextStory = ofGetElapsedTimef() + 5;
//	/*
//     string seedKeywordString = [seedKeyword.stringValue UTF8String];
//     if(seedKeywordString != ""){
//     visualizer.clear();
//     visualizer.addTagToPhysics(seedKeywordString);
//     }
//	 */
//	preview.stop();
//	
//	if(playlistTable.selectedRow >= 0){
//		[playlistTable deselectAll:self];
//	}
//	
//	if(clipTableSource.selectedRow >= 0){
//		visualizer.clear();
//		CloudsClip& clip = [self selectedClip];
//		//visualizer.addLinksToPhysics(clip);
//		//TODO: selected topic
//		storyEngine.seedWithClip(clip);
//		[self playCurrentPlaylist:self];
//	}
//    
//    if(clipTableTarget.selectedRow >= 0){
//		visualizer.clear();
//		CloudsClip& clip = [self selectedClip];
//		//visualizer.addLinksToPhysics(clip);
//		//TODO: selected topic
//        storyEngine.seedWithClip(clip);
//		[self playCurrentPlaylist:self];
//	}
//    
//    
//}

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
    
//	if(playlistTable.selectedRow == storyEngine.getClipHistory().size()-1){
//        
//		if(!storyEngine.playNextClip()){
//			return;
//		}
//		[playlistTable reloadData];
//	}
//	
//    
//    
//	if(playlistTable.selectedRow < storyEngine.getClipHistory().size()){
//        [playlistTable selectRowIndexes:[[NSIndexSet alloc] initWithIndex:playlistTable.selectedRow+1]
//                   byExtendingSelection:NO];
//		[self playCurrentPlaylist:playlistTable];
//	}
}

- (IBAction) prevOnPlaylist:(id)sender
{
//	if(playlistTable.selectedRow >= 0){
//		[playlistTable selectRowIndexes:[[NSIndexSet alloc] initWithIndex:playlistTable.selectedRow-1]
//				   byExtendingSelection:NO];
//		[self playCurrentPlaylist:playlistTable];
//	}
}

- (IBAction) playCurrentPlaylist:(id)sender
{
    
//	if(playlistTable.selectedRow < 0){
//		[playlistTable selectRowIndexes:[[NSIndexSet alloc] initWithIndex:0]
//				   byExtendingSelection:NO];
//	}
//	
//	[self playDoubleClickedRow:playlistTable];
//	onPlaylist = true;
}

- (IBAction) addClip:(id)sender
{
//	storyEngine.playNextClip();
//	[playlistTable reloadData];
}

- (IBAction) unloadVideo:(id)sender
{
	preview.stop();
}

//- (IBAction)suppressLinkModifier:(id)sender
//{
//    if(linkTable.selectedRow>=0 &&suppressedTable.selectedRow<0){
//        
//        string linkSourceName = currentClipLinks[linkTable.selectedRow].sourceName;
//        string linkTargetName = currentClipLinks[linkTable.selectedRow].targetName;
//        
//        cout<<"swapping link for suppression : clip " << linkSourceName << " going to " << linkTargetName << endl;
//        parser.removeLink(linkSourceName, linkTargetName);
//        
//        //        [clipTableSource.resignFirstResponder
//        
//        [self suppressLink:parser.getClipWithLinkName(linkSourceName)
//                    toClip:parser.getClipWithLinkName(linkTargetName) ];
//        
//        currentClipLinks = parser.getLinksForClip(linkSourceName);
//        currentSuppressedLinks = parser.getSuppressionsForClip(linkSourceName);
//        
//        
//        [clipTableTarget reloadData];
//        [clipTableSource reloadData];
//        [linkTable reloadData];
//        [suppressedTable reloadData];
//        [self saveLinks:self];
//        //      parser.suppressConnection(cur)
//    }
//    else if(suppressedTable.selectedRow>=0&&linkTable.selectedRow<0){
//        cout<<"suppressedTable"<<endl;
//        string suppressedSourceName = currentSuppressedLinks[suppressedTable.selectedRow].sourceName;
//        string suppressedTargetName = currentSuppressedLinks[suppressedTable.selectedRow].targetName;
//        
//        cout<<" swaping suppression for link: clip " << suppressedSourceName << " going to " << suppressedTargetName << endl;
//        parser.unsuppressConnection(suppressedSourceName, suppressedTargetName);
//        
//        [self linkClip:parser.getClipWithLinkName(suppressedSourceName)
//                toClip:parser.getClipWithLinkName(suppressedTargetName) ];
//        currentClipLinks = parser.getLinksForClip(suppressedSourceName);
//        currentSuppressedLinks = parser.getSuppressionsForClip(suppressedSourceName);
//        
//        [clipTableTarget reloadData];
//        [clipTableSource reloadData];
//        [linkTable reloadData];
//        [suppressedTable reloadData];
//        [self saveLinks:self];
//    }
//
//}

- (void)update
{
//    if(updatePhysics){
//        visualizer.updatePhysics();
//    }
	
	if(movieFileMissing){
		if(ofGetElapsedTimef() > timeOfNextStory && onPlaylist){
			[self nextOnPlaylist:self];
		}
	}
	//auto progressive story just gooes from one clip to the next
//	if(autoProgressStory){
//		if(ofGetElapsedTimef() > timeOfNextStory){
//			timeOfNextStory = ofGetElapsedTimef() + .5;
//			[self nextOnPlaylist:self];
//		}
//	}
//	//most of the time we want to wait for a clip to finish or be interrupted by user interaction
	if(preview.isLoaded()) {
		preview.update();
		if(preview.getCurrentFrame() >= clipEndFrame){
			preview.stop();
			if(onPlaylist){
				[self nextOnPlaylist:self];
			}
		}
	}
    
//    if(visualizer.getPathChanged()){
//		[playlistTable reloadData];
//	}
}

- (void)draw
{
    
	ofBackground(0);
	
    if(preview.isLoaded() && preview.isPlaying()){
        preview.draw(0, 0, 960, 540);
    }
	
//    //	ofBackgroundGradient(ofColor::black,
//    //						 ofColor::darkGray*.15,
//    //						 OF_GRADIENT_LINEAR);
//	
////    visualizer.drawPhysics();
//    
////	string debug = "";
////	debug += "Current Topic: " + storyEngine.getCurrentTopic() + " (" + ofToString(storyEngine.getTimesOnTopic()) + "/" + ofToString(storyEngine.maxTimesOnTopic) + ")\n";
////	debug += "Watched " + ofxTimecode::timecodeForSeconds( storyEngine.getTotalSecondsWatched() ) + " from " + ofToString( storyEngine.getClipHistory().size()  ) + "\n";
////	debug += "Covered " + ofToString( storyEngine.getClipHistory().size() ) + " / " + ofToString( parser.getAllClips().size() ) + "\n";
//	
//	ofPushStyle();
//	ofSetColor(210);
////	font.drawString(debug, 30,30);
//	ofPopStyle();
}

- (void) exit
{
	NSLog(@"exit!!");
//	gui->saveSettings("settings.xml");
}

- (void)keyPressed:(int)key
{
    //if(key == ' ') updatePhysics = !updatePhysics;
//    if(key == ' '){
//        [self playDoubleClickedRow: clipTableSource];
//    }
	
//	if(key == 'h'){
//		cout << "toggling vis" << endl;
////		gui->disable();
//	}
//	if(key == 's'){
////		gui->enable();
//	}
}

- (IBAction) createLink:(id)sender
{
//    NSLog(@"creating link. edge selected? %@", visualizer.isEdgeSelected() ? @"YES" : @"NO" );
//	
//	if(visualizer.isEdgeSelected() && !visualizer.isSelectedEdgeLink() ){
//		
//		NSLog(@"Edge is selected!!");
//		
//		[self linkClip:visualizer.getEdgeSource() toClip:visualizer.getEdgeDestination() ];
//		visualizer.linkedEdge();
//		[clipTableSource reloadData];
//        [clipTableTarget reloadData];
//		[linkTable reloadData];
//        
//	}
//	else if(clipLoaded && clipTableSource.selectedRow >= 0){
//		
//		NSLog(@"clip is loaded and row is selected");
//		
//		[self linkClip:currentPlayingClip toClip:[self selectedClip] ];
//		
//		currentClipLinks = parser.getLinksForClip(currentPlayingClip.getLinkName());
//        
//		[clipTableSource reloadData];
//        [clipTableTarget reloadData];
//		[suppressedTable reloadData];
//		[linkTable reloadData];
//        
//		cout << "after creating link the current clip has " << currentClipLinks.size() << endl;
//	}
}

//CONFUSING: delete link is called from the link table, remove link called from the graph
//- (IBAction) deleteLink:(id)sender
//{
//    if(linkTable.selectedRow >= 0){
//        cout << "removing link " << linkTable.selectedRow << " from " << currentPlayingClip.getLinkName() << endl;
//        parser.removeLink(currentPlayingClip.getLinkName(), linkTable.selectedRow);
//        currentClipLinks = parser.getLinksForClip(currentPlayingClip.getLinkName());
//        
//        [linkTable reloadData];
//		[suppressedTable reloadData];
//		[clipTableSource reloadData];
//        [clipTableTarget reloadData];
//		
//		
//		[self saveLinks:self];
//    }
//}

//- (IBAction) removeLink:(id)sender
//{
//	if(	visualizer.isSelectedEdgeLink() ){
//		cout << "removing link!" << endl;
//		parser.removeLink(visualizer.getEdgeSource().getLinkName(),
//						  visualizer.getEdgeDestination().getLinkName() );
//		
//		visualizer.unlinkEdge();
//		[self saveLinks:self];
//	}
//}


//- (IBAction) suppressLink:(id)sender
//{
//	if(visualizer.isEdgeSelected()){
//		if( visualizer.isSelectedEdgeLink() ){
//			parser.removeLink(visualizer.getEdgeSource().getLinkName(),
//							  visualizer.getEdgeDestination().getLinkName() );
//		}
//		
//		NSLog(@"Edge is selected!!");
//		
//		[self suppressLink:visualizer.getEdgeSource() toClip:visualizer.getEdgeDestination() ];
//		visualizer.suppressEdge();
//		
//		[clipTableSource reloadData];
//        [clipTableTarget reloadData];
//		[linkTable reloadData];
//        [suppressedTable reloadData];
//		
//	}
//}

//- (IBAction) unsuppressLink:(id)sender
//{
//	if(	visualizer.isSelectedEdgeSuppressed() ){
//		cout << "removing suppression!" << endl;
//		parser.unsuppressConnection(visualizer.getEdgeSource().getLinkName(),
//									visualizer.getEdgeDestination().getLinkName());
//		
//		visualizer.unsuppressEdge();
//		[self saveLinks:self];
//        
//	}
//}

-(IBAction)suppressFromLeftToRight:(id)sender{
    if([linkerB isClipSelected]&&[linkerA isClipSelected]){
        [self suppressLink:[linkerA selectedClip]
					toClip:[linkerB selectedClip]];
        [linkerA updateTables];
        [linkerB updateTables];
    }
}

-(IBAction)suppressFromRighttoLeft:(id)sender{
    if([linkerA isClipSelected]&&[linkerB isClipSelected]){
        [self suppressLink:[linkerB selectedClip]
					toClip:[linkerA selectedClip]];
        [linkerA updateTables];
        [linkerB updateTables];
    }
    
    
}
-(IBAction)linkFromLeftToRight:(id)sender{
    if([linkerB isClipSelected]&&[linkerA isClipSelected]){
        [self linkClip:[linkerA selectedClip]
				toClip:[linkerB selectedClip]];
        [linkerA updateTables];
        [linkerB updateTables];
    }
}

-(IBAction)linkFromRightToLeft:(id)sender{
    if([linkerA isClipSelected]&&[linkerB isClipSelected]){
        [self linkClip:[linkerB selectedClip]
				toClip:[linkerA selectedClip]];
        [linkerA updateTables];
        [linkerB updateTables];
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
		
		parser.unsuppressConnection(l);
		
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

//- (IBAction) linkLast:(id)sender
//{
//	if(playlistTable.numberOfRows > 1){
//		CloudsClip& a = storyEngine.getClipHistory()[ playlistTable.selectedRow-1 ];
//		CloudsClip& b = storyEngine.getClipHistory()[ playlistTable.selectedRow ];
//		if(!parser.clipLinksTo(a.getLinkName(), b.getLinkName())){
//			[self linkClip:a toClip:b];
//		}
//		else{
//			parser.removeLink(a.getLinkName(), b.getLinkName());
//		}
//		visualizer.linkPathEdge(playlistTable.selectedRow-1);
//		[self saveLinks:self];
//	}
	
//}

//- (IBAction) suppressLast:(id)sender
//{
//	if(playlistTable.numberOfRows > 1){
//		CloudsClip& a = storyEngine.getClipHistory()[ playlistTable.selectedRow-1 ];
//		CloudsClip& b = storyEngine.getClipHistory()[ playlistTable.selectedRow ];
//        
//		if(!parser.linkIsSuppressed(a.getLinkName(), b.getLinkName())){
//			cout << "link is not currently suppressed, suppressing!" << endl;
//			[self suppressLink:a toClip:b];
//		}
//		else{
//			cout << "link is suppressed -- getting rid of it!" << endl;
//			parser.unsuppressConnection(a.getLinkName(), b.getLinkName());
//		}
//		visualizer.suppressPathEdge(playlistTable.selectedRow-1);
//		[self saveLinks:self];
//	}
//}

//- (IBAction) setStartText:(id)sender{
//    //button pressed
//    CloudsClip& m = [self selectedClip];
//    CloudsClip& n = parser.getClipWithLinkName(m.getLinkName());
//    string q =   std::string([startQuestion.stringValue UTF8String]);
//    
//    n.setStartingQuestion(q);
//    cout<<"Set the question for clip"<<n.getLinkName()<<"::"<<n.getStartingQuestion()<<endl;
//    //get the txt from textfield, get currently select clip, and set it
//    
//    
//}

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
	
//	CloudsClip clip;
//	if(sender == clipTableSource && clipTableSource.selectedRow >= 0){
//        clip = [self selectedClip];
//        //		playingPlaylist = false;
//	}
//	else if(sender == playlistTable && playlistTable.selectedRow >= 0){
//		clip = [self selectedClipFromPlaylist];
//        //		playingPlaylist = true;
//	}
//	else{
//		//bail!
//		return;
//	}
	
  //  [self playClip:clip];
    
    /*
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
     
     if(movieFileMissing){
     storyStartTime = ofGetElapsedTimef();
     timeOfNextStory = storyStartTime + (clip.endFrame - clip.startFrame) / 24.0;
     }
     else{
     preview.setFrame(clip.startFrame);
     preview.play();
     }
     */
    
   	clipLoaded = YES;
	
//	currentClipLabel.stringValue = [NSString stringWithUTF8String:clip.getLinkName().c_str()];
//	currentClipLinks = parser.getLinksForClip(clip.getLinkName());
//	currentSuppressedLinks = parser.getSuppressionsForClip(clip.getLinkName());
    
    //	cout << "current clips has " << currentClipLinks.size() << " links" << endl;
    
//	[linkTable reloadData];
//    [suppressedTable reloadData];
    
}

- (void) playClip:(CloudsClip&) clip
{
	onPlaylist = false; //will get set to true again if we are coming from a playlist
	if(currentPlayingClip.getLinkName() == clip.getLinkName() && !autoProgressStory){
		if(preview.isPlaying()){
			preview.stop();
		}
		else{
			preview.play();
		}
		return;
	}
	
	cout << " clip time size " << clip.startFrame << " " << clip.endFrame << endl;
	preview.stop();
	
	string clipFilePath = clip.sourceVideoFilePath;
	if(clipFilePath  == "" || autoProgressStory){
		return;
	}
	
	ofSleepMillis(250);
	if( !ofFile(clipFilePath).exists() ){
        
		//ofStringReplace(clipFilePath, "Nebula_backup", "Seance");
		//ofStringReplace(clipFilePath, "Nebula", "Seance");
        ofStringReplace(clipFilePath, "Nebula", "Nebula_helper");
        
	}
    
	if( preview.loadMovie(clipFilePath) ){
		movieFileMissing = false;
	}
	else{
		ofLogError() << "Clip " << clipFilePath << " failed to load.";
		movieFileMissing = true;
        //		return;
	}
	
	if(movieFileMissing){
		storyStartTime = ofGetElapsedTimef();
		timeOfNextStory = storyStartTime + (clip.endFrame - clip.startFrame) / 24.0;
	}
	else{
		preview.setFrame(clip.startFrame);
		preview.play();
	}
	
	currentPlayingClip = clip;
	clipEndFrame = clip.endFrame;
	
}

- (CloudsClip&) selectedClip
{
    
//    if(selectedClipsSource.size() > 0){
//        CloudsClip& c=selectedClipsSource[clipTableSource.selectedRow];
//        cout<<"Selected clip from source table "<<c.getLinkName()<<","<<c.hasStartingQuestion()<<endl;
//        return c;
//    }
//    else if(selectedClipsTarget.size() > 0){
//        CloudsClip& c = selectedClipsTarget[clipTableSource.selectedRow];
//        cout<<"Selected clip from target table"<<c.getLinkName()<<","<<c.hasStartingQuestion()<<endl;
//        return c;
//    }
//    else{
//        CloudsClip& c =parser.getAllClips()[clipTableSource.selectedRow] ;
//        cout<<"Selected clip from all clips"<<c.getLinkName()<<","<<c.hasStartingQuestion()<<endl;
//        return c;
//    }
    
    
}

- (CloudsClip&) selectedClipFromPlaylist
{
	//return visualizer.pathByClip[ playlistTable.selectedRow ];
//	return storyEngine.getClipHistory()[ playlistTable.selectedRow ];
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
//	visualizer.physics.setWorldMax(ofVec2f(size.width, size.height));
//	visualizer.width  = size.width;
//	visualizer.height = size.height;
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
        parser.parseClusterMap("../../../CloudsData/gephi/CLOUDS_test_5_26_13.SVG");
	}
	else{
		cout << "SETTING UP IN DATA DIRECTORY" << endl;
		parser.setup("xml");
		parser.parseLinks("clouds_link_db.xml");
        parser.parseClusterMap("CLOUDS_test_5_26_13.SVG");
	}
    
    //    parser.parseLinks("../../../CloudsLibrary/data/links/clouds_link_db.xml");
    
//    [keywordTableSource reloadData];
//    [clipTableSource reloadData];
//    [keywordTableTarget reloadData];
//    [clipTableTarget reloadData];
    
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
//    //   cout << "returning some rows " << (NSInteger)parser.getAllKeywords().size() << endl;
//    if(aTableView == keywordTableSource){
//        return (NSInteger)parser.getAllKeywords().size();
//    }
//    else if(aTableView == keywordTableTarget){
//        return (NSInteger)parser.getAllKeywords().size();
//    }
//    else if(aTableView == clipTableSource){
//        return (NSInteger) (selectedClipsSource.size() == 0) ? parser.getAllClips().size() : selectedClipsSource.size();
//    }
//    else if(aTableView == clipTableTarget){
//        return (NSInteger) (selectedClipsTarget.size() == 0) ? parser.getAllClips().size() : selectedClipsTarget.size();
//    }
//    else if(aTableView == linkTable){
//        return currentClipLinks.size();
//    }
//    else if(aTableView == suppressedTable){
//        return currentSuppressedLinks.size();
//    }
//	else if(aTableView == playlistTable){
////		return storyEngine.getClipHistory().size();
//	}
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
//    
//    if(aTableView == keywordTableSource){
//        string keyword = parser.getAllKeywords()[rowIndex];
//        if([@"occurrence" isEqualToString:aTableColumn.identifier]){
//            return [NSNumber numberWithInt: parser.occurrencesOfKeyword(keyword)];
//        }
//        else{
//            return [NSString stringWithCString:keyword.c_str() encoding:NSUTF8StringEncoding];
//        }
//    }
//    else if(aTableView == keywordTableTarget){
//        string keyword = parser.getAllKeywords()[rowIndex];
//        if([@"occurrence" isEqualToString:aTableColumn.identifier]){
//            return [NSNumber numberWithInt: parser.occurrencesOfKeyword(keyword)];
//        }
//        else{
//            return [NSString stringWithCString:keyword.c_str() encoding:NSUTF8StringEncoding];
//        }
//    }
//    else if(aTableView == clipTableSource){
//		CloudsClip& m = (selectedClipsSource.size() == 0) ? parser.getAllClips()[rowIndex] : selectedClipsSource[rowIndex];
//        //        cout<<"Selected Clips size: "<<selectedClips.size()<<endl;
//        //		NSLog(@"Identifer is %@", aTableColumn.identifier);
//		if([@"Links" isEqualToString:aTableColumn.identifier]){
//			return [NSNumber numberWithInt: parser.getLinksForClip( m.getLinkName() ).size()];
//		}
//        else if([@"Suppressions" isEqualToString:aTableColumn.identifier]){
//            return [NSNumber numberWithInt:parser.getSuppressionsForClip(m.getLinkName()).size()];
//        }
//        else if([@"Starting Question" isEqualToString:aTableColumn.identifier]){
//            //return [NSNumber numberWithInt:parser.getSuppressionsForClip(m.getLinkName()).size()];
//            //string
//            string s = m.getStartingQuestion();
//            //cout<<"Starting: "<<s<<endl;
//            return [NSString stringWithUTF8String:s.c_str()];
//        }
//		else {
//			//CloudsClip& m = [self selectedClip];
//			string linkString = m.person + " - " + m.name + " - " + m.clip + ": [" + ofToString(m.startFrame) + "," + ofToString(m.endFrame) + "]";
//			return [NSString stringWithUTF8String:linkString.c_str()];
//		}
//    }
//    else if(aTableView == clipTableTarget){
//		CloudsClip& m = (selectedClipsTarget.size() == 0) ? parser.getAllClips()[rowIndex] : selectedClipsTarget[rowIndex];
//        //        cout<<"Selected Clips size: "<<selectedClips.size()<<endl;
//        //		NSLog(@"Identifer is %@", aTableColumn.identifier);
//		if([@"Links" isEqualToString:aTableColumn.identifier]){
//			return [NSNumber numberWithInt: parser.getLinksForClip( m.getLinkName() ).size()];
//		}
//        else if([@"Suppressions" isEqualToString:aTableColumn.identifier]){
//            return [NSNumber numberWithInt:parser.getSuppressionsForClip(m.getLinkName()).size()];
//        }
//        else if([@"Starting Question" isEqualToString:aTableColumn.identifier]){
//            //return [NSNumber numberWithInt:parser.getSuppressionsForClip(m.getLinkName()).size()];
//            //string
//            string s = m.getStartingQuestion();
//            //cout<<"Starting: "<<s<<endl;
//            return [NSString stringWithUTF8String:s.c_str()];
//        }
//		else {
//			//CloudsClip& m = [self selectedClip];
//			string linkString = m.person + " - " + m.name + " - " + m.clip + ": [" + ofToString(m.startFrame) + "," + ofToString(m.endFrame) + "]";
//			return [NSString stringWithUTF8String:linkString.c_str()];
//		}
//    }
//    else if(aTableView == linkTable){
//        string clipTableSourceEntry = currentClipLinks[rowIndex].targetName;
//        if(currentClipLinks[rowIndex].startFrame != -1 && currentClipLinks[rowIndex].endFrame != -1){
//            clipTableSourceEntry += "[" + ofToString(currentClipLinks[rowIndex].startFrame) + " - " + ofToString(currentClipLinks[rowIndex].endFrame) + "]";
//        }
//        return [NSString stringWithUTF8String:clipTableSourceEntry.c_str()];
//    }
//    else if(aTableView == suppressedTable){
//        string suppressedTableEntry = currentSuppressedLinks[rowIndex].targetName;
//        if(currentSuppressedLinks[rowIndex].startFrame != -1 && currentSuppressedLinks[rowIndex].endFrame!= -1){
//            suppressedTableEntry += "[" + ofToString(currentSuppressedLinks[rowIndex].startFrame) + " - " + ofToString(currentSuppressedLinks[rowIndex].endFrame) + "]";
//        }
//        return [NSString stringWithUTF8String:suppressedTableEntry.c_str()];
//    }
//	else if(aTableView == playlistTable){
//		//return [NSString stringWithUTF8String: visualizer.pathByClip[rowIndex].getLinkName().c_str()];
//		return [NSString stringWithUTF8String: storyEngine.getClipHistory()[rowIndex].getLinkName().c_str()];
//	}
}

//- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
//{
//    if(aNotification.object == keywordTableSource && showOnlyQuestions.state ==  NSOffState){
//        
//        
//        selectedKeywordsSource.clear();
//        selectedClipsSource.clear();
//        
//        if(keywordTableSource.selectedRow >= 0){
//            
//            NSUInteger idx = [keywordTableSource.selectedRowIndexes firstIndex];
//            while (idx != NSNotFound) {
//                // do work with "idx"
//                selectedKeywordsSource.push_back(parser.getAllKeywords()[idx]);
//                
//                // get the next index in the set
//                idx = [keywordTableSource.selectedRowIndexes indexGreaterThanIndex:idx];
//            }
//            selectedClipsSource = parser.getClipsWithKeyword(selectedKeywordsSource);
//        }
//        
//        [clipTableSource reloadData];
//    }
//    else if(aNotification.object == keywordTableTarget && showOnlyQuestions.state ==  NSOffState){
//        
//        
//        selectedKeywordsTarget.clear();
//        selectedClipsTarget.clear();
//        
//        if(keywordTableTarget.selectedRow >= 0){
//            
//            NSUInteger idx = [keywordTableTarget.selectedRowIndexes firstIndex];
//            while (idx != NSNotFound) {
//                // do work with "idx"
//                selectedKeywordsTarget.push_back(parser.getAllKeywords()[idx]);
//                
//                // get the next index in the set
//                idx = [keywordTableTarget.selectedRowIndexes indexGreaterThanIndex:idx];
//            }
//            selectedClipsTarget = parser.getClipsWithKeyword(selectedKeywordsTarget);
//        }
//        
//        [clipTableTarget reloadData];
//    }
//    else if(aNotification.object == clipTableSource){
//        if(clipTableSource.selectedRow < 0){
//            return;
//        }
//        
//        //CloudsClip& m = (selectedKeywordsSource.size() == 0) ? parser.getAllClips()[clipTableSource.selectedRow] : selectedClipsSource[clipTableSource.selectedRow];
//        CloudsClip& m = [self selectedClip];
//        
//        string keywordList = "";
//        currentKeywordsSource.stringValue = [NSString stringWithUTF8String:ofJoinString(m.keywords, ",").c_str()];
//        
//        linkText.stringValue = [NSString stringWithUTF8String:m.getLinkName().c_str()];
//        //        suppressedTable.stringValue = [NSString stringWithUTF8String:parser.]
//        startQuestion.stringValue = [NSString stringWithUTF8String:m.getStartingQuestion().c_str()];
//    }
//    else if(aNotification.object == clipTableTarget){
//        if(clipTableTarget.selectedRow < 0){
//            return;
//        }
//        
//        //CloudsClip& m = (selectedKeywordsSource.size() == 0) ? parser.getAllClips()[clipTableSource.selectedRow] : selectedClipsSource[clipTableSource.selectedRow];
//        CloudsClip& m = [self selectedClip];
//        
//        string keywordList = "";
//        currentKeywordsTarget.stringValue = [NSString stringWithUTF8String:ofJoinString(m.keywords, ",").c_str()];
//        
//        linkText.stringValue = [NSString stringWithUTF8String:m.getLinkName().c_str()];
//        //        suppressedTable.stringValue = [NSString stringWithUTF8String:parser.]
//        startQuestion.stringValue = [NSString stringWithUTF8String:m.getStartingQuestion().c_str()];
//    }
//    else if(aNotification.object == linkTable){
//        if(linkTable.selectedRow < 0){
//            return;
//        }
//        
//        vector<CloudsClip>& searchClips = (selectedClipsSource.size() == 0) ? parser.getAllClips() : selectedClipsSource;
//        //CloudsClip& m = [self selectedClip];
//        string targetClip = currentClipLinks[ linkTable.selectedRow ].targetName;
//        for(int i = 0; i < searchClips.size(); i++){
//            if(searchClips[i].name == targetClip){
//                NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:i];
//                [clipTableSource selectRowIndexes:indexSet byExtendingSelection:NO];
//                break;
//            }
//        }
//    }
//    else if(aNotification.object == suppressedTable){
//        if(suppressedTable.selectedRow<0){
//            return;
//        }
//        
//        
//        
//    }
//}

//- (NSArray *)tokenField:(NSTokenField *)tokenField
//completionsForSubstring:(NSString *)substring
//           indexOfToken:(NSInteger)tokenIndex
//    indexOfSelectedItem:(NSInteger *)selectedIndex
//{
    
//    //    cout << "asking for completions..." << endl;
//    NSMutableArray* completions = [NSMutableArray array];
//    for(int i = 0; i < parser.getAllKeywords().size(); i++){
//        NSString* stringKeyword = [NSString stringWithUTF8String:parser.getAllKeywords()[i].c_str()];
//        if([stringKeyword rangeOfString:substring].location == 0  && ![self hasKeyword:stringKeyword]){
//            [completions addObject:stringKeyword];
//        }
//    }
//    
//    //    cout << "found " << completions.count << endl;
//    return completions;
//}

//- (BOOL) hasKeyword:(NSString*) keyword
//{
//    NSString* currentString = currentKeywordsSource.stringValue;
//    NSArray* stringSet = [currentString componentsSeparatedByCharactersInSet: currentKeywordsSource.tokenizingCharacterSet];
//    for( NSString* str in stringSet){
//        if([keyword isEqualToString:str]){
//            return YES;
//        }
//    }
//    return NO;
//}

//- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors
//{
//    NSArray *newDescriptors = [tableView sortDescriptors];
//    NSLog(@"sort descriptor %@", [newDescriptors objectAtIndex:0]);
//    if(tableView == keywordTableSource){
//        parser.sortKeywordsByOccurrence( [ [[newDescriptors objectAtIndex:0] key]  isEqualToString:@"occurrence"] );
//    }
//    
//    //    [results sortUsingDescriptors:newDescriptors];
//    //"results" is my NSMutableArray which is set to be the data source for the NSTableView object.
//    [tableView reloadData];
//}

//- (BOOL)tableView:(NSTableView *)aTableView shouldEditTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
//{
//    return NO;
//}

@end