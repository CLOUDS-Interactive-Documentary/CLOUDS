#import "testView.h"

@implementation testView

- (void)setup
{
    ofBackground(0);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    updatePhysics = true;
    [self refreshXML:self];
    
    
    for (NSTableColumn *tableColumn in keywordTable.tableColumns ) {
        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
        [tableColumn setSortDescriptorPrototype:sortDescriptor];
    }
    for (NSTableColumn *tableColumn in linkTable.tableColumns ) {
        NSSortDescriptor *sortDescriptor = [NSSortDescriptor sortDescriptorWithKey:tableColumn.identifier ascending:YES selector:@selector(compare:)];
        [tableColumn setSortDescriptorPrototype:sortDescriptor];
    }
 
    visualizer.database = &parser;
//    visualizer.setupGrid();
    visualizer.setup();
    visualizer.exportForGraphviz();
//    visualizer.createIterativePhysics();
}

- (void)update
{
    if(updatePhysics){
        visualizer.updatePhysics();
    }
    
    if(preview.isLoaded()){
        preview.update();
    }
}

- (void)draw
{
//    visualizer.drawPhysics();
//    visualizer.drawGrid();
    if(preview.isLoaded() && preview.isPlaying()){
        preview.draw(0, 0, 1280, 720);
    }
}

- (void)exit
{
	
}

- (void)keyPressed:(int)key
{
    //if(key == ' ') updatePhysics = !updatePhysics;
    if(key == ' '){
        if(linkTable.selectedRow >= 0){
            ClipMarker& m = (selectedKeywords.size() == 0) ? parser.getAllClips()[linkTable.selectedRow] : selectedClips[linkTable.selectedRow];
            
            if( m.filePath != "" && ofFile(m.filePath).exists() ){
                if(preview.loadMovie(m.filePath)){
                    preview.setFrame(m.startFrame);
                    preview.play();
                }
                else{
                    NSLog(@"movie failed %s", m.filePath.c_str());
                }
            }
            else{
                NSLog(@"movie path does not exist %s", m.filePath.c_str());
            }
        }
    }
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
    
    [keywordTable reloadData];
    [linkTable reloadData];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
 //   cout << "returning some rows " << (NSInteger)parser.getAllKeywords().size() << endl;
    if(aTableView == keywordTable){
        return (NSInteger)parser.getAllKeywords().size();
    }
    else{
        return (NSInteger) (selectedKeywords.size() == 0) ? parser.getAllClips().size() : selectedClips.size();
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
    else{
    
        ClipMarker& m = (selectedKeywords.size() == 0) ? parser.getAllClips()[rowIndex] : selectedClips[rowIndex];
        string linkString = m.person + " - " + m.name + " - " + m.clip + ": [" + ofToString(m.startFrame) + "," + ofToString(m.endFrame) + "]";
        return [NSString stringWithUTF8String:linkString.c_str()];
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
            
        [linkTable reloadData];
        
        /*
        string selection = parser.getAllKeywords()[keywordTable.selectedRow] ;
        NSString* nsStringSelection = [NSString stringWithCString:selection.c_str()
                                                         encoding:NSUTF8StringEncoding];

        if(![self hasKeyword:nsStringSelection]){
            [currentKeywords setStringValue: [currentKeywords.stringValue stringByAppendingFormat:@", %@", nsStringSelection]];
        }
        */
    }
    else{
        if(linkTable.selectedRow < 0){
            return;
        }
        //ClipMarker& m = parser.getAllClips()[linkTable.selectedRow];
        ClipMarker& m = (selectedKeywords.size() == 0) ? parser.getAllClips()[linkTable.selectedRow] : selectedClips[linkTable.selectedRow];
        string keywordList = "";
        currentKeywords.stringValue = [NSString stringWithUTF8String:ofJoinString(m.keywords, ",").c_str()];
        linkText.stringValue = [NSString stringWithUTF8String:("link:" + m.person + " - " + m.name).c_str()];
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

@end