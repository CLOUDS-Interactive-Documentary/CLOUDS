//
//  ViewerApp.m
//  CloudsFCPParser
//
//  Created by James George on 3/14/13.
//
//

#import "ViewerApp.h"

@implementation ViewerApp

- (void)setup
{
	
}

- (IBAction) togglePlay:(id)sender
{
	if(mainview.preview.isLoaded()){
		if(mainview.preview.isPlaying()){
			mainview.preview.stop();
		}
		else{
			mainview.preview.play();
		}
	}
}

- (IBAction) next:(id)sender
{
	
}

- (IBAction) prev:(id)sender
{
	
}

- (void)update
{
	if(mainview == NULL) return;
	
    if(mainview.preview.isLoaded()){
        mainview.preview.update();
        if(mainview.preview.getCurrentFrame() >= mainview.clipEndFrame){
            mainview.preview.stop();
			if(mainview.playingPlaylist){
				[mainview nextOnPlaylist:self];
			}
        }
    }
}

- (void)draw
{
	if(mainview == NULL) return;
	
	ofCircle(mouseX, mouseY, 30);
	
    if(mainview.preview.isLoaded() && mainview.preview.isPlaying()){
        mainview.preview.draw(0, 0, 960, 540);
    }
}

- (void)exit
{

}

- (void)keyPressed:(int)key{ }
- (void)keyReleased:(int)key{ }
- (void)mouseMoved:(NSPoint)p{ }
- (void)mouseDragged:(NSPoint)p button:(int)button{ }
- (void)mousePressed:(NSPoint)p button:(int)button{ }
- (void)mouseReleased:(NSPoint)p button:(int)button{ }
- (void)windowResized:(NSSize)size{ }

@end
