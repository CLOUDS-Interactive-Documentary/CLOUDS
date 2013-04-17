//
//  ViewerApp.h
//  CloudsFCPParser
//
//  Created by James George on 3/14/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "testView.h"

@interface ViewerApp : ofxCocoaGLView
{
	IBOutlet testView* mainview;
	ofImage movieNotFound;
}

- (void)setup;
- (void)update;
- (void)draw;
- (void)exit;

- (IBAction) togglePlay:(id)sender;

- (void)keyPressed:(int)key;
- (void)keyReleased:(int)key;
- (void)mouseMoved:(NSPoint)p;
- (void)mouseDragged:(NSPoint)p button:(int)button;
- (void)mousePressed:(NSPoint)p button:(int)button;
- (void)mouseReleased:(NSPoint)p button:(int)button;
- (void)windowResized:(NSSize)size;

@end

