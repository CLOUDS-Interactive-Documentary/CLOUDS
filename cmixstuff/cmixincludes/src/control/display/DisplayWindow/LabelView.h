/* RTcmix - Copyright (C) 2011  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#import <Cocoa/Cocoa.h>
#import "LabelView-CInterface.h"

// Max. number of labels. Must match ../labels.h.
const int kNumLabels = 12;

@interface LabelView : NSView {
	int labelCount;
	float lineHeight;
	float fontAscent;
	float charWidth;
#ifdef USE_UPDATE_RECT
	NSRect labelRect;
#endif
	char *label;
	char *prefix[kNumLabels];
	char *units[kNumLabels];
	int precision[kNumLabels];
	NSMutableString *string[kNumLabels];
	NSMutableDictionary *stringAttributes;
	NSTimer *timer;
	BOOL updatePending;
}

- (id) initWithFrame: (NSRect) frame;
- (void) initLabels;
- (void) dealloc;
- (void) handleTimer: (NSTimer *) timer;
- (void) updateLabelRect;
- (void) configureLabelPrefix: (int) idx  prefix: (char *) thePrefix;
- (void) configureLabelUnits: (int) idx  units: (char *) theUnits;
- (void) configureLabelPrecision: (int) idx  precision: (int) thePrecision;
- (void) updateLabelValue: (int) idx  value: (double) theValue;
- (void) doUpdate;
- (void) drawLabels;
- (void) drawRect: (NSRect) rect;

@end
