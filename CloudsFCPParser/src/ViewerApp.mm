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


- (void)update
{
	
}

- (void)draw
{
	if(mainview == NULL) return;
	
	ofCircle(mouseX, mouseY, 30);
	
	if(mainview.movieFileMissing){
		//movieNotFound.draw(0,0,960,540);
		
		ofDrawBitmapString("MOVIE MISSING", 10, 10);
	}
	
    if(mainview.preview.isLoaded() && mainview.preview.isPlaying()){
        mainview.preview.draw(0, 0, 960, 540);
    }
	ofPushStyle();
	ofSetColor(0xc965e2, 150);
	float percentDone = [mainview clipPercentComplete];
	ofRect(0, ofGetHeight()-10, 960*percentDone, 10);
	ofPopStyle();
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
