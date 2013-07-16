#import "testView.h"
#include "CloudsClip.h"
#include "CloudsGlobal.h"

@implementation testView

@synthesize clipEndFrame;
@synthesize preview;

- (void)setup
{
	ofSeedRandom();
	
    ofBackground(220);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
	ofSetFrameRate(30);
	
	movieSuccessfullyLoaded = false;
    
    [self refreshXML:self];
    clipEndFrame = 0;
    clipLoaded = NO;
	
	
	font.loadFont("materiapro_light.ttf", 12);
	
	cout << ofToString(parser.percentOfClipsLinked(), 4) << " percent clips linked!" << endl;
	
    linkerA.testViewParent = self;
    linkerB.testViewParent = self;
    linkerA.parser = &parser;
    linkerB.parser = &parser;
	[linkerA setup];
    [linkerB setup];
    
//	exporter.saveGephiCSV(parser);
    
}


- (void) updateViews
{
	[linkerA updateTables];
	[linkerB updateTables];
}

- (IBAction)stopPreview:(id)sender
{
    preview.stop();
}

- (float) clipPercentComplete
{
    
	if(movieSuccessfullyLoaded && preview.isLoaded() && preview.isPlaying()){
		return (1.0*preview.getCurrentFrame() - currentPlayingClip.startFrame) / (currentPlayingClip.endFrame - currentPlayingClip.startFrame);
	}
	return 0;
}

- (IBAction) togglePlay:(id)sender
{
	if(preview.isLoaded()){
		if(preview.isPlaying()){
			preview.stop();
		}
		else{
			preview.play();
		}
	}
}


- (void)update
{
	
	//most of the time we want to wait for a clip to finish or be interrupted by user interaction
	if(preview.isLoaded()) {
		preview.update();
		if(preview.getCurrentFrame() >= clipEndFrame){
			preview.stop();
		}
	}
}

- (void)draw
{
	ofBackground(0);
    if(preview.isLoaded()){
		ofRectangle screenRect(0,0, ofGetWidth(), ofGetHeight());
		ofRectangle videoRect(0,0,1920,1080);
		videoRect.scaleTo(screenRect);
		
        preview.draw(videoRect);
		
		font.drawString(currentPlayingClip.getLinkName() + "\n" + ofJoinString(currentPlayingClip.getKeywords(), ", "), 10, ofGetHeight() - font.getLineHeight()*2 - 10);
    }	
}

- (void) exit
{
	NSLog(@"exit!!");
}

- (void)selectClip:(CloudsClip)clip inAlternateTable:(id)sender
{
	if(sender == linkerA){
		[linkerB selectClip:clip];
	}
	else{
		[linkerA selectClip:clip];
	}
}

- (void)keyPressed:(int)key
{
	
	if(key == ' '){
		[self togglePlay:self];
	}
	
	cout << "KEY " << key << endl;
}

-(IBAction)suppressClips:(id)sender
{
    if([linkerB isClipSelected]&&[linkerA isClipSelected]){
        [self suppressLink:[linkerA selectedClip]
					toClip:[linkerB selectedClip]];
        [linkerA updateTables];
        [linkerB updateTables];
		
		[self saveLinks:self];
    }
}

-(IBAction)linkFromLeftToRight:(id)sender
{
    if([linkerB isClipSelected]&&[linkerA isClipSelected]){
        [self linkClip:[linkerA selectedClip]
				toClip:[linkerB selectedClip]];
        [linkerA updateTables];
        [linkerB updateTables];
		
		[self saveLinks:self];
    }
}

-(IBAction)linkFromRightToLeft:(id)sender
{
    if([linkerA isClipSelected]&&[linkerB isClipSelected]){
        [self linkClip:[linkerB selectedClip]
				toClip:[linkerA selectedClip]];
        [linkerA updateTables];
        [linkerB updateTables];
		[self saveLinks:self];		
    }
}

- (void) linkClip:(CloudsClip) source toClip:(CloudsClip) target
{
	if(source.getLinkName() != target.getLinkName()){
		
		NSLog(@"creating link for %s + %s", source.getLinkName().c_str(), target.getLinkName().c_str());
		
		CloudsLink l;
		l.sourceName = source.getLinkName();
		l.targetName = target.getLinkName();
		l.startFrame = -1;
		l.endFrame = -1;
		
		parser.unsuppressConnection(l);
		
		parser.addLink(l);
		
		//save
		[self saveLinks:self];
	}
	else{
		NSLog(@"failed clip for %s + %s", source.getLinkName().c_str(), target.getLinkName().c_str());
	}
}

- (void) suppressLink:(CloudsClip)source toClip:(CloudsClip) target
{
	if(source.getLinkName() != target.getLinkName()){
		
		NSLog(@"suppressing link between %s + %s", source.getLinkName().c_str(), target.getLinkName().c_str());
		
		CloudsLink l;
		l.sourceName = source.getLinkName();
		l.targetName = target.getLinkName();
		l.startFrame = -1;
		l.endFrame = -1;
        
		parser.removeLink(source.getLinkName(),target.getLinkName());
		parser.suppressConnection(l);
        
		[self saveLinks:self];
	}
}

- (IBAction) saveLinks:(id)sender
{
	if(ofDirectory("../../../CloudsData/links/").exists()){
		parser.saveLinks("../../../CloudsData/links/clouds_link_db.xml");
        cout<<"saving!"<<endl;
	}
	else{
		parser.saveLinks("clouds_link_db.xml");
	}
}

- (void) playClip:(CloudsClip&) clip
{

	if(currentPlayingClip.getLinkName() == clip.getLinkName()){
		if(preview.isPlaying()){
			preview.stop();
		}
		else{
			if(preview.getCurrentFrame() >= clipEndFrame){
				preview.setFrame( clip.startFrame );
			}
			preview.play();
		}
		return;
	}
		
	preview.stop();
	
	string clipFilePath = relinkFilePath( clip.sourceVideoFilePath );
	movieSuccessfullyLoaded = preview.loadMovie(clipFilePath) ;
	if(!movieSuccessfullyLoaded){
		ofLogError() << "Clip " << clipFilePath << " failed to load.";
        return;
	}
	
	preview.setFrame(clip.startFrame);
	preview.play();
	
	currentPlayingClip = clip;
	clipEndFrame = clip.endFrame;	
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

	parser.loadFromFiles();
	    
}

@end