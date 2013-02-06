#pragma once

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsFCPParser.h"

@interface testView : ofxCocoaGLView {
    IBOutlet NSTokenField* tokenList;
    IBOutlet NSTextField* freeText;
    
    CloudsFCPParser parser;
}

- (void)setup;
- (void)update;
- (void)draw;
- (void)exit;

- (IBAction) setXMLFolder:(id)sender;
- (IBAction) refreshXML:(id)sender;

- (void)keyPressed:(int)key;
- (void)keyReleased:(int)key;
- (void)mouseMoved:(NSPoint)p;
- (void)mouseDragged:(NSPoint)p button:(int)button;
- (void)mousePressed:(NSPoint)p button:(int)button;
- (void)mouseReleased:(NSPoint)p button:(int)button;
- (void)windowResized:(NSSize)size;

@end
