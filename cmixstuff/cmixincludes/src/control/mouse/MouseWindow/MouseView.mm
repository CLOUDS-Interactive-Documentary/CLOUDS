#import <Cocoa/Cocoa.h>
#include <string.h>
#include <assert.h>
#import "MouseView.h"

const int kWholeLabelLength = 48;	// total number of chars in whole label
const float kLabelFromLeft = 10.0f;	// pixels from left border of window
const float kLabelFromTop = 10.0f;
const int kExtraLineHeight = 8.0f;
#define kFontName @"Lucida Grande"
const int kFontSize = 14;

@implementation MouseView

- (id) initWithFrame: (NSRect) frame
{
	[super initWithFrame:frame];

	[[NSNotificationCenter defaultCenter] addObserver:self
		selector:@selector(viewResized:)
		name:NSViewFrameDidChangeNotification object:self];
	[self setFactors];

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

	setMouseViewInstance((void *) self);

	return self;
}

- (void) awakeFromNib
{
	[self updateTrackingAreas];
}

- (void) initLabels
{
	xlabelCount = 0;
	for (int i = 0; i < kNumLabels; i++) {
		xprefix[i] = nil;
		xunits[i] = nil;
		xstring[i] = [[NSMutableString alloc] init];
	}
	xlabel = new char [kWholeLabelLength];
	xlabel[0] = 0;

	ylabelCount = 0;
	for (int i = 0; i < kNumLabels; i++) {
		yprefix[i] = nil;
		yunits[i] = nil;
		ystring[i] = [[NSMutableString alloc] init];
	}
	ylabel = new char [kWholeLabelLength];
	ylabel[0] = 0;
}

- (void) dealloc
{
	[super dealloc];
	for (int i = 0; i < kNumLabels; i++) {
		delete [] xprefix[i];
		delete [] xunits[i];
		[xstring[i] dealloc];
	}
	delete [] xlabel;
	for (int i = 0; i < kNumLabels; i++) {
		delete [] yprefix[i];
		delete [] yunits[i];
		[ystring[i] dealloc];
	}
	delete [] ylabel;
	[stringAttributes dealloc];
}

- (void) handleTimer: (NSTimer *) timer
{
	if (updatePending)
#ifdef USE_UPDATE_RECT
		[self setNeedsDisplayInRect:labelRect];
#else
		[self setNeedsDisplay:YES];
#endif
}

- (void) viewResized: (NSNotification *) notification
{
	[self setFactors];
	[self updateLabelRect];
}

- (void) updateTrackingAreas
{
	if (trackingArea) {
		[self removeTrackingArea:trackingArea];
		//[dealloc trackingArea]; // no, because set to autorelease
	}
	trackingArea = [[[NSTrackingArea alloc] initWithRect:[self visibleRect]
			options: NSTrackingMouseMoved | NSTrackingActiveAlways
			owner:self
			userInfo:nil] autorelease];
	[self addTrackingArea:trackingArea];
}

- (void) updateLabelRect
{
#ifdef USE_UPDATE_RECT
	NSRect bounds = [self bounds];
	float width = kWholeLabelLength * charWidth;
	float height = (xlabelCount + ylabelCount) * lineHeight;
	labelRect.origin.x = kLabelFromLeft;
	labelRect.origin.y = bounds.size.height - (kLabelFromTop + height);
	labelRect.size.width = kLabelFromLeft + width;
	labelRect.size.height = height;
#endif
}

// Set prefix string for xlabel with <idx>, allocate a new xlabel, and increment
// the count of xlabels in use.
- (void) configureXLabelPrefix: (int) idx  prefix: (char *) thePrefix
{
	assert(idx >= 0 && idx < kNumLabels);
	xprefix[idx] = new char [strlen(thePrefix) + 1];
	strcpy(xprefix[idx], thePrefix);
	xlabelCount++;
	[self updateLabelRect];
}

// Set (optional) units string for xlabel with <idx>.
// NOTE: This will have no effect if we don't receive a prefix for this label.
- (void) configureXLabelUnits: (int) idx  units: (char *) theUnits
{
	assert(idx >= 0 && idx < kNumLabels);
	xunits[idx] = new char [strlen(theUnits) + 1];
	strcpy(xunits[idx], theUnits);
}

// Set precision for xlabel with <idx>.
// NOTE: This will have no effect if we don't receive a prefix for this label.
- (void) configureXLabelPrecision: (int) idx  precision: (int) thePrecision
{
	assert(idx >= 0 && idx < kNumLabels);
	xprecision[idx] = thePrecision;
}

// Set prefix string for ylabel with <idx>, allocate a new ylabel, and increment
// the count of ylabels in use.
- (void) configureYLabelPrefix: (int) idx  prefix: (char *) thePrefix
{
	assert(idx >= 0 && idx < kNumLabels);
	yprefix[idx] = new char [strlen(thePrefix) + 1];
	strcpy(yprefix[idx], thePrefix);
	ylabelCount++;
	[self updateLabelRect];
}

// Set (optional) units string for ylabel with <idx>.
// NOTE: This will have no effect if we don't receive a prefix for this label.
- (void) configureYLabelUnits: (int) idx  units: (char *) theUnits
{
	assert(idx >= 0 && idx < kNumLabels);
	yunits[idx] = new char [strlen(theUnits) + 1];
	strcpy(yunits[idx], theUnits);
}

// Set precision for ylabel with <idx>.
// NOTE: This will have no effect if we don't receive a prefix for this label.
- (void) configureYLabelPrecision: (int) idx  precision: (int) thePrecision
{
	assert(idx >= 0 && idx < kNumLabels);
	yprecision[idx] = thePrecision;
}

- (void) updateXLabelValue: (int) idx  value: (double) theValue
{
	assert(idx >= 0 && idx < kNumLabels);
	char *unitstr = xunits[idx] ? xunits[idx] : (char *) "";
	snprintf(xlabel, kWholeLabelLength, "%s: %.*f %s",
			xprefix[idx], xprecision[idx], theValue, unitstr);
	NSString *tmp = [[NSString alloc] initWithCString:xlabel encoding:NSASCIIStringEncoding];
	[xstring[idx] setString: tmp];
	[tmp dealloc];
}

- (void) updateYLabelValue: (int) idx  value: (double) theValue
{
	assert(idx >= 0 && idx < kNumLabels);
	char *unitstr = yunits[idx] ? yunits[idx] : (char *) "";
	snprintf(ylabel, kWholeLabelLength, "%s: %.*f %s",
			yprefix[idx], yprecision[idx], theValue, unitstr);
	NSString *tmp = [[NSString alloc] initWithCString:ylabel encoding:NSASCIIStringEncoding];
	[ystring[idx] setString: tmp];
	[tmp dealloc];
}

- (void) doUpdate
{
	updatePending = YES;
}

- (void) drawAxis
{
	NSRect bounds = [self bounds];

	[[NSColor whiteColor] set];
	NSRectFill(bounds);

	[[NSColor grayColor] set];
	float hcenter = bounds.size.width / 2.0;
	float vcenter = bounds.size.height / 2.0;
	NSBezierPath *path = [NSBezierPath bezierPath];
	[path moveToPoint: NSMakePoint(0, vcenter)];
	[path lineToPoint: NSMakePoint(NSMaxX(bounds), vcenter)];
	[path moveToPoint: NSMakePoint(hcenter, 0)];
	[path lineToPoint: NSMakePoint(hcenter, NSMaxY(bounds))];
	[path setLineWidth: 1.0];
	[path stroke];

	NSFrameRect(bounds);
}

- (void) drawLabels
{
	NSRect bounds = [self bounds];
	float ypos = bounds.size.height - (kLabelFromTop + fontAscent);

	int line = 0;
	for (int i = 0; i < xlabelCount; i++) {
		[xstring[i] drawAtPoint: NSMakePoint(kLabelFromLeft, ypos - (line * lineHeight)) withAttributes: stringAttributes];
		line++;
	}
	for (int i = 0; i < ylabelCount; i++) {
		[ystring[i] drawAtPoint: NSMakePoint(kLabelFromLeft, ypos - (line * lineHeight)) withAttributes: stringAttributes];
		line++;
	}

#ifdef DEBUG
	NSFrameRect(labelRect);
#endif

	updatePending = NO;
}

- (void) drawRect: (NSRect) rect
{
	[self drawAxis];
	[self drawLabels];
}

- (void) mouseEntered: (NSEvent *) event
{
//	printf("mouseEntered\n");
}

- (void) mouseMoved: (NSEvent *) event
{
	NSPoint mouseLoc = [self convertPoint:[event locationInWindow] fromView:nil];
	const double xscaled = mouseLoc.x * xfactor;
	const double yscaled = mouseLoc.y * yfactor;
	sendCoordinates(xscaled, yscaled);
}

- (void) setFactors
{
	NSRect bounds = [self bounds];
	const float width = bounds.size.width;
	const float height = bounds.size.height;
	xfactor = 1.0 / (double) (width - 1.0);
	yfactor = 1.0 / (double) (height - 1.0);
}

- (void) resetCursorRects
{
	[super resetCursorRects];
	[self addCursorRect: [self bounds] cursor: [NSCursor crosshairCursor]];
}


// These definitions let us call our methods from C++.
void MouseViewConfigureXLabelPrefix(void *self, int idx, char *thePrefix) {
	return [(id)self configureXLabelPrefix:idx prefix:thePrefix];
}
void MouseViewConfigureYLabelPrefix(void *self, int idx, char *thePrefix) {
	return [(id)self configureYLabelPrefix:idx prefix:thePrefix];
}
void MouseViewConfigureXLabelUnits(void *self, int idx, char *theUnits) {
	return [(id)self configureXLabelUnits:idx units:theUnits];
}
void MouseViewConfigureYLabelUnits(void *self, int idx, char *theUnits) {
	return [(id)self configureYLabelUnits:idx units:theUnits];
}
void MouseViewConfigureXLabelPrecision(void *self, int idx, int thePrecision) {
	return [(id)self configureXLabelPrecision:idx precision:thePrecision];
}
void MouseViewConfigureYLabelPrecision(void *self, int idx, int thePrecision) {
	return [(id)self configureYLabelPrecision:idx precision:thePrecision];
}
void MouseViewUpdateXLabelValue(void *self, int idx, double theValue) {
	return [(id)self updateXLabelValue:idx value:theValue];
}
void MouseViewUpdateYLabelValue(void *self, int idx, double theValue) {
	return [(id)self updateYLabelValue:idx value:theValue];
}
void MouseViewDoUpdate(void *self) {
	return [(id)self doUpdate];
}

@end
