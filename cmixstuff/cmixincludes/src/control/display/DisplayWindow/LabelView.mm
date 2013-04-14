/* RTcmix - Copyright (C) 2011  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#import <Cocoa/Cocoa.h>
#include <string.h>
#include <assert.h>
#import "LabelView.h"

const int kWholeLabelLength = 64;	// total number of chars in whole label
const float kLabelFromLeft = 10.0f;	// pixels from left border of window
const float kLabelFromTop = 10.0f;
const int kExtraLineHeight = 8.0f;
#define kFontName @"Lucida Grande"
const int kFontSize = 18;

@implementation LabelView

- (id) initWithFrame: (NSRect) frame
{
	[super initWithFrame:frame];

	[self initLabels];

	NSFont *font = [NSFont fontWithName:kFontName size:kFontSize];
	fontAscent = [font ascender];
	lineHeight = fontAscent + [font descender] + [font leading] + kExtraLineHeight;
	NSRect wRect = [font boundingRectForGlyph:(NSGlyph) '0'];
	charWidth = wRect.size.width;	// rough estimate

	stringAttributes = [[NSMutableDictionary alloc] init];
	[stringAttributes setObject:font forKey:NSFontAttributeName];
	
	updatePending = NO;
	timer = [NSTimer scheduledTimerWithTimeInterval: 0.1
					target: self
					selector: @selector(handleTimer:)
					userInfo: nil
					repeats: YES];

	setLabelViewInstance((void *) self);

	return self;
}

- (void) initLabels
{
	labelCount = 0;
	for (int i = 0; i < kNumLabels; i++) {
		prefix[i] = nil;
		units[i] = nil;
		string[i] = [[NSMutableString alloc] init];
	}
	label = new char [kWholeLabelLength];
	label[0] = 0;
}

- (void) dealloc
{
	[super dealloc];
	for (int i = 0; i < kNumLabels; i++) {
		delete [] prefix[i];
		delete [] units[i];
		[string[i] dealloc];
	}
	delete [] label;
	[stringAttributes dealloc];
}

- (void) handleTimer: (NSTimer *) timer
{
	if (updatePending)
#ifdef USE_UPDATE_RECT
		// problem with this: resizing window causes updates to fail,
		// because labelRect not updated for new window size; for
		// something as simple as this app, it hardly matters
		// whether we use an update rect or the whole view.
		[self setNeedsDisplayInRect:labelRect];
#else
		[self setNeedsDisplay:YES];
#endif
}

- (void) updateLabelRect
{
#ifdef USE_UPDATE_RECT
	NSRect bounds = [self bounds];
	float width = kWholeLabelLength * charWidth;
	float height = labelCount * lineHeight;
	labelRect.origin.x = kLabelFromLeft;
	labelRect.origin.y = bounds.size.height - (kLabelFromTop + height);
	labelRect.size.width = kLabelFromLeft + width;
	labelRect.size.height = height;
#endif
}

// Set prefix string for label with <idx>, allocate a new label, and increment
// the count of labels in use.
- (void) configureLabelPrefix: (int) idx  prefix: (char *) thePrefix
{
	assert(idx >= 0 && idx < kNumLabels);
	prefix[idx] = new char [strlen(thePrefix) + 1];
	strcpy(prefix[idx], thePrefix);
	labelCount++;
	[self updateLabelRect];
}

// Set (optional) units string for label with <idx>.
// NOTE: This will have no effect if we don't receive a prefix for this label.
- (void) configureLabelUnits: (int) idx  units: (char *) theUnits
{
	assert(idx >= 0 && idx < kNumLabels);
	units[idx] = new char [strlen(theUnits) + 1];
	strcpy(units[idx], theUnits);
}

// Set precision for label with <idx>.
// NOTE: This will have no effect if we don't receive a prefix for this label.
- (void) configureLabelPrecision: (int) idx  precision: (int) thePrecision
{
	assert(idx >= 0 && idx < kNumLabels);
	precision[idx] = thePrecision;
}

- (void) updateLabelValue: (int) idx  value: (double) theValue
{
	assert(idx >= 0 && idx < kNumLabels);
	char *unitstr = units[idx] ? units[idx] : (char *)"";
	snprintf(label, kWholeLabelLength, "%s: %.*f %s",
			prefix[idx], precision[idx], theValue, unitstr);
	NSString *tmp = [[NSString alloc] initWithCString:label encoding:NSASCIIStringEncoding];
	[string[idx] setString: tmp];
	[tmp dealloc];
}

- (void) doUpdate
{
	updatePending = YES;
}

- (void) drawLabels
{
	NSRect bounds = [self bounds];
	float ypos = bounds.size.height - (kLabelFromTop + fontAscent);

	int line = 0;
	for (int i = 0; i < labelCount; i++) {
		[string[i] drawAtPoint: NSMakePoint(kLabelFromLeft, ypos - (line * lineHeight))
			withAttributes: stringAttributes];
		line++;
	}

#ifdef DEBUG
	NSFrameRect(bounds);
	NSFrameRect(labelRect);
#endif

	updatePending = NO;
}

- (void) drawRect: (NSRect) rect
{
	[self drawLabels];
}


// These definitions let us call our methods from C++.
void LabelViewConfigureLabelPrefix(void *self, int idx, char *thePrefix) {
	return [(id)self configureLabelPrefix:idx prefix:thePrefix];
}
void LabelViewConfigureLabelUnits(void *self, int idx, char *theUnits) {
	return [(id)self configureLabelUnits:idx units:theUnits];
}
void LabelViewConfigureLabelPrecision(void *self, int idx, int thePrecision) {
	return [(id)self configureLabelPrecision:idx precision:thePrecision];
}
void LabelViewUpdateLabelValue(void *self, int idx, double theValue) {
	return [(id)self updateLabelValue:idx value:theValue];
}
void LabelViewDoUpdate(void *self) {
	return [(id)self doUpdate];
}

@end
