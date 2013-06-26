#import "testView.h"
#include "CloudsVisualSystem.h"

@implementation testView

- (void)setup
{
	currentVisualSystem = NULL;
	
	parser.loadFromFiles();
	
	visualSystems.populateVisualSystems();
    [presetTable setTarget:self];
	[presetTable setDoubleAction:@selector(playDoubleClickedRow:)];
	[presetTable reloadData];
}

- (void)update
{
    if(shouldPlaySelectedRow){
        if(currentVisualSystem != NULL){
            currentVisualSystem->stopSystem();
        }
        currentVisualSystem = visualSystems.getPresets()[presetTable.selectedRow].system;

        currentVisualSystem->playSystem();
        currentVisualSystem->loadPresetGUISFromName(visualSystems.getPresets()[presetTable.selectedRow].presetName);
        shouldPlaySelectedRow = false;     
    }        
    ofShowCursor();
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

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return visualSystems.getPresets().size();
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex{
	
	if([@"preset" isEqualToString:aTableColumn.identifier]){
		return [NSString stringWithUTF8String: visualSystems.getPresets()[rowIndex].presetName.c_str()];
	}
	else{
		return [NSString stringWithUTF8String: visualSystems.getPresets()[rowIndex].system->getSystemName().c_str()];
	}
}

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification{
	
}

- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange: (NSArray *)oldDescriptors{
	
}

- (void)playDoubleClickedRow:(id)sender
{
    shouldPlaySelectedRow = true;
    

}

@end