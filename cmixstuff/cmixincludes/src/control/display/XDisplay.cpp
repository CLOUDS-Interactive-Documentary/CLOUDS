/* RTcmix - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <XDisplay.h>
#include <X11/cursorfont.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

//#define DEBUG

static const char *kLabelFontName = "fixed";	// platform-specific
static const int kExtraLineHeight = 3;


XDisplay::XDisplay() : RTcmixDisplay()
{
	_display = NULL;
	_window = None;
	_labelXpos = kLabelFromLeft;
	_labelYpos = kLabelFromTop;
	_maxLabelChars = kWholeLabelLength;	// defined in labels.h
	_lineHeight = 0;
	_charWidth = 0;
	_fontName = (char *) kLabelFontName;
	_windowname = "RTcmix Display";
}

XDisplay::~XDisplay()
{
	if (_display && _window) {
		XUnmapWindow(_display, _window);
		XDestroyWindow(_display, _window);
	}
}

void XDisplay::doConfigureLabel(const int id, const char *prefix,
		const char *units, const int precision)
{
	_prefix[id] = new char [strlen(prefix) + 1];
	strcpy(_prefix[id], prefix);
	if (units) {
		_units[id] = new char [strlen(units) + 1];
		strcpy(_units[id], units);
	}
	_label[id] = new char [kWholeLabelLength];
	_label[id][0] = 0;
	_precision[id] = precision;
}

void XDisplay::doUpdateLabelValue(const int id, const double value)
{
	const char *units = _units[id] ? _units[id] : "";
	snprintf(_label[id], kWholeLabelLength, "%s: %.*f %s",
				_prefix[id], _precision[id], value, units);
	drawLabel(id);
}

int XDisplay::show()
{
	const int xpos = 100;	// NB: window manager sets position
	const int ypos = 100;
	const unsigned int width = 200;
	const unsigned int height = 200;
	_display = XOpenDisplay(NULL);
	if (_display != NULL) {
		_screen = XDefaultScreen(_display);
		_gc = XDefaultGC(_display, _screen);
		_window = createWindow(xpos, ypos, width, height);
		if (_window != None)
			return 0;
	}
	return -1;
}

Window XDisplay::createWindow(
		const int xpos,
		const int ypos,
		const unsigned int width,
		const unsigned int height)
{
	Window parent = XDefaultRootWindow(_display);
	Cursor cursor = XCreateFontCursor(_display, XC_crosshair);
   const unsigned int borderwidth = 0;

	XSetWindowAttributes attr;
	attr.backing_store = WhenMapped;
	attr.event_mask = ExposureMask | PointerMotionMask | ButtonMotionMask
				| ButtonPressMask | ButtonReleaseMask | StructureNotifyMask;
	attr.background_pixel = XWhitePixel(_display, _screen);
	attr.cursor = cursor;
	const unsigned long valuemask = CWBackingStore | CWEventMask | CWBackPixel
							| CWCursor;

	Window window = XCreateWindow(_display, parent,
							xpos, ypos, width, height,
							borderwidth, CopyFromParent, InputOutput,
							CopyFromParent, valuemask, &attr);
//FIXME: is this right?
	if (window == None) {
		fprintf(stderr, "Error creating display window.\n");
		return None;
	}

	XFontStruct *font = XLoadQueryFont(_display, _fontName);
	if (font == NULL) {
		fprintf(stderr, "Display window font not found.\n");
		return None;
	}
	XSetFont(_display, _gc, font->fid);
	_charWidth = XTextWidth(font, "X", 1);
   _lineHeight = font->ascent + font->descent;
   _fontAscent = font->ascent;

	XStoreName(_display, window, _windowname);
	XMapWindow(_display, window);

	return window;
}

void XDisplay::drawLabel(const int id)
{
	assert(id < _labelCount);

	// Clear rect enclosing label.
	int width = _maxLabelChars * _charWidth;
	int ypos = _labelYpos + (id * (_lineHeight + kExtraLineHeight));
	XClearArea(_display, _window, _labelXpos, ypos, width, _lineHeight, False);
#ifdef DEBUG
	XDrawRectangle(_display, _window, _gc, _labelXpos, ypos, width, _lineHeight);
	printf("drawLabel: xpos=%d, ypos=%d, width=%d, height=%d\n",
				_labelXpos, ypos, width, _lineHeight);
#endif

	// Draw label.
	ypos += _fontAscent;
	XDrawString(_display, _window, _gc,
					_labelXpos, ypos,
					_label[id], strlen(_label[id]));
	XFlush(_display);
}

void XDisplay::drawLabels()
{
	if (_labelCount > 0) {
		// Clear rect enclosing all labels.
		int height = _labelCount * (_lineHeight + kExtraLineHeight);
		int width = _maxLabelChars * _charWidth;
		int ypos = _labelYpos;
		XClearArea(_display, _window, _labelXpos, ypos, width, height, False);
#ifdef DEBUG
		XDrawRectangle(_display, _window, _gc, _labelXpos, ypos, width, height);
		printf("drawLabel: xpos=%d, ypos=%d, width=%d, height=%d\n",
					_labelXpos, ypos, width, height);
#endif

		// Draw all labels.
		ypos += _fontAscent;
		for (int i = 0; i < _labelCount; i++) {
			XDrawString(_display, _window, _gc,
							_labelXpos, ypos + (i * (_lineHeight + kExtraLineHeight)),
							_label[i], strlen(_label[i]));
		}
		XFlush(_display);
	}
}

void XDisplay::drawWindowContent()
{
	drawLabels();
}

// NOTE: This code is adapted from control/mouse/XMouse.cpp.  Here, all we're
// doing is displaying values sent to us.
bool XDisplay::handleEvents()
{
	XEvent event;
	const unsigned long evtmask =
		  ExposureMask
		| PointerMotionMask
		| StructureNotifyMask;

	bool keepgoing = true;
	while (XCheckWindowEvent(_display, _window, evtmask, &event)) {
		switch (event.type) {
			case MotionNotify:
				break;
			case Expose:
				drawWindowContent();
				break;
			case ConfigureNotify:
				break;
			default:
				break;
		}
	}
	return keepgoing;
}

