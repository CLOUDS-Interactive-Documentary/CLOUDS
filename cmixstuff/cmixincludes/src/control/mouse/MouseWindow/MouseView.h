#import <Cocoa/Cocoa.h>
#import "MouseView-CInterface.h"

// Max. number of labels. Must match ../labels.h.
const int kNumLabels = 4;

@interface MouseView : NSView {
	int xlabelCount;
	int ylabelCount;
	float lineHeight;
	float fontAscent;
	float charWidth;
	NSRect labelRect; 
	char *xlabel;
	char *ylabel;
	char *xprefix[kNumLabels];
	char *yprefix[kNumLabels];
	char *xunits[kNumLabels];
	char *yunits[kNumLabels];
	int xprecision[kNumLabels];
	int yprecision[kNumLabels];
	double xfactor;
	double yfactor;
	NSMutableString *xstring[kNumLabels];
	NSMutableString *ystring[kNumLabels];
	NSMutableDictionary *stringAttributes;
	NSTimer *timer;
	NSTrackingArea *trackingArea;
	BOOL updatePending;
}

- (id) initWithFrame: (NSRect) frame;
- (void) initLabels;
- (void) dealloc;
- (void) handleTimer: (NSTimer *) timer;
- (void) updateLabelRect;
- (void) configureXLabelPrefix: (int) idx  prefix: (char *) thePrefix;
- (void) configureYLabelPrefix: (int) idx  prefix: (char *) thePrefix;
- (void) configureXLabelUnits: (int) idx  units: (char *) theUnits;
- (void) configureYLabelUnits: (int) idx  units: (char *) theUnits;
- (void) configureXLabelPrecision: (int) idx  precision: (int) thePrecision;
- (void) configureYLabelPrecision: (int) idx  precision: (int) thePrecision;
- (void) updateXLabelValue: (int) idx  value: (double) theValue;
- (void) updateYLabelValue: (int) idx  value: (double) theValue;
- (void) doUpdate;
- (void) drawAxis;
- (void) drawLabels;
- (void) drawRect: (NSRect) rect;
- (void) mouseMoved: (NSEvent *) event;
- (void) setFactors;

@end
