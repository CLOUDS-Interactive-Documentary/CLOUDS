#import "testView.h"

@implementation testView

- (void)setup
{
    parser.setup("xml");
    [keywordTable reloadData];
    
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
    
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
 //   cout << "returning some rows " << (NSInteger)parser.getAllKeywords().size() << endl;
    return (NSInteger)parser.getAllKeywords().size();
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    
    string keyword = parser.getAllKeywords()[rowIndex];
    if([@"quantity" isEqualToString:aTableColumn.identifier]){
        return [NSNumber numberWithInt: parser.occurrencesOfKeyword(keyword)];
    }
    else{
        return [NSString stringWithCString:keyword.c_str() encoding:NSUTF8StringEncoding];
    }
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    if(keywordTable.selectedRow < 0){
        return;
    }
    string selection = parser.getAllKeywords()[keywordTable.selectedRow] ;
    NSString* nsStringSelection = [NSString stringWithCString:selection.c_str()
                                                     encoding:NSUTF8StringEncoding];
    NSString* currentString = currentKeywords.stringValue;
    NSArray* stringSet = [currentString componentsSeparatedByCharactersInSet: currentKeywords.tokenizingCharacterSet];
    for( NSString* str in stringSet){
        NSLog(@" comparing %@ to %@", str, nsStringSelection);
        if([nsStringSelection isEqualToString:str]){
            return;
        }
    }
        
    [currentKeywords setStringValue: [currentString stringByAppendingFormat:@", %@", nsStringSelection]];
}

@end