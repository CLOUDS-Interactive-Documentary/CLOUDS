#import "testView.h"

@implementation testView

- (void)setup
{
    parser.setup("xml");
    [keywordTable reloadData];
    [linkTable reloadData];
    
    cout << "done parsing " << endl;
    
}

- (void)update
{
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

- (IBAction) setXMLFolder:(id)sender
{
    
}

- (IBAction) refreshXML:(id)sender
{
    parser.setup("xml");
    [keywordTable reloadData];    
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
 //   cout << "returning some rows " << (NSInteger)parser.getAllKeywords().size() << endl;
    if(aTableView == keywordTable){
        return (NSInteger)parser.getAllKeywords().size();
    }
    else{
        return (NSInteger)parser.getAllClips().size();
    }
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    
    if(aTableView == keywordTable){
        string keyword = parser.getAllKeywords()[rowIndex];
        if([@"quantity" isEqualToString:aTableColumn.identifier]){
            return [NSNumber numberWithInt: parser.occurrencesOfKeyword(keyword)];
        }
        else{
            return [NSString stringWithCString:keyword.c_str() encoding:NSUTF8StringEncoding];
        }
    }
    else{
        ClipMarker& m = parser.getAllClips()[rowIndex];
        string linkString = m.person + " - " + m.name + " - " + m.clip + ": [" + ofToString(m.startFrame) + "," + ofToString(m.endFrame) + "]";
        return [NSString stringWithUTF8String:linkString.c_str()];
    }
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    if(aNotification.object == keywordTable){
        if(keywordTable.selectedRow < 0){
            return;
        }
        string selection = parser.getAllKeywords()[keywordTable.selectedRow] ;
        NSString* nsStringSelection = [NSString stringWithCString:selection.c_str()
                                                         encoding:NSUTF8StringEncoding];

        if(![self hasKeyword:nsStringSelection]){
            [currentKeywords setStringValue: [currentKeywords.stringValue stringByAppendingFormat:@", %@", nsStringSelection]];
        }
    }
    else{
        if(linkTable.selectedRow < 0){
            return;
        }
        ClipMarker& m = parser.getAllClips()[linkTable.selectedRow];
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

@end