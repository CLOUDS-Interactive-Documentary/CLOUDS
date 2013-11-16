#import "AppDelegate.h"

@implementation AppDelegate

@synthesize window;

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
}

- (IBAction)addIntervention:(id)sender{
//    NSLog("Im here");
    [ofApp addIntervention:self];
}
@end
