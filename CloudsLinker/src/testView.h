#pragma once

#define CLOUDS_NO_VS 1

#include "ofMain.h"
#include "ofxCocoaGLView.h"
#include "CloudsFCPParser.h"
#include "CloudsD3Exporter.h"
#include "ClipSelector.h"

//@class ViewerApp;
@interface testView : ofxCocoaGLView <NSTableViewDataSource, NSTableViewDelegate, NSTokenFieldDelegate> {

    IBOutlet ClipSelector* linkerA;
    IBOutlet ClipSelector* linkerB;
    
    CloudsFCPParser parser;
	ofVideoPlayer preview;
	CloudsD3Exporter exporter;
    
    vector<string> selectedKeywordsSource;
    vector<string> selectedKeywordsTarget;
    CloudsClip currentPlayingClip;
    BOOL clipLoaded;
    int clipEndFrame;
	
    vector<CloudsClip> selectedClipsTarget;
    vector<CloudsClip> selectedClipsSource;
    vector<CloudsLink> currentClipLinks;
    vector<CloudsLink> currentSuppressedLinks;
	ofTrueTypeFont font;
	
	bool movieSuccessfullyLoaded;
}

@property(nonatomic,readonly) int clipEndFrame;
@property(nonatomic,readonly) ofVideoPlayer& preview;

- (void) setup;
- (void) update;
- (void) draw;
- (void) exit;

- (void) playClip:(CloudsClip&)clip;

- (void) updateViews;

- (IBAction) setXMLFolder:(id)sender;
- (IBAction) refreshXML:(id)sender;
- (IBAction) saveLinks:(id)sender;

- (IBAction) togglePlay:(id)sender;

- (IBAction)linkFromLeftToRight:(id)sender;
- (IBAction)linkFromRightToLeft:(id)sender;
- (IBAction)suppressClips:(id)sender;

- (float) clipPercentComplete;

- (void) linkClip:(CloudsClip) source toClip:(CloudsClip) target;
- (void) suppressLink:(CloudsClip)source toClip:(CloudsClip) target;

- (void)keyPressed:(int)key;
- (void)keyReleased:(int)key;
- (void)mouseMoved:(NSPoint)p;
- (void)mouseDragged:(NSPoint)p button:(int)button;
- (void)mousePressed:(NSPoint)p button:(int)button;
- (void)mouseReleased:(NSPoint)p button:(int)button;
- (void)windowResized:(NSSize)size;
@end
