/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#include <XMouse.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

//#define DEBUG

const char *kLabelFontName = "fixed";	// platform-specific


XMouse::XMouse() : RTcmixMouse()
{
	_display = NULL;
	_window = None;
	_xraw = -INT_MAX;		// forces getpos* to return negative
	_yraw = INT_MAX;
	_labelXpos = kLabelFromLeft;
	_labelYpos = kLabelFromTop;
	_maxLabelChars = kWholeLabelLength;	// defined in labels.h
	_lineHeight = 0;
	_charWidth = 0;
	_fontName = (char *) kLabelFontName;
	_windowname = "RTcmix Mouse Input";
}

XMouse::~XMouse()
{
	shutdownEventLoop();
	if (_display && _window) {
		XUnmapWindow(_display, _window);
		XDestroyWindow(_display, _window);
		XCloseDisplay(_display);
	}
}

void XMouse::doConfigureXLabel(const int id, const char *prefix,
		const char *units, const int precision)
{
	_xprefix[id] = strdup(prefix);
	if (units)
		_xunits[id] = strdup(units);
	_xlabel[id] = new char [kWholeLabelLength];
	_xlabel[id][0] = 0;
	_xprecision[id] = precision;
}

void XMouse::doConfigureYLabel(const int id, const char *prefix,
		const char *units, const int precision)
{
	_yprefix[id] = strdup(prefix);
	if (units)
		_yunits[id] = strdup(units);
	_ylabel[id] = new char [kWholeLabelLength];
	_ylabel[id][0] = 0;
	_yprecision[id] = precision;
}

void XMouse::doUpdateXLabelValue(const int id, const double value)
{
	const char *units = _xunits[id] ? _xunits[id] : "";
	snprintf(_xlabel[id], kWholeLabelLength, "%s: %.*f %s",
				_xprefix[id], _xprecision[id], value, units);
	drawXLabels();
}

void XMouse::doUpdateYLabelValue(const int id, const double value)
{
	const char *units = _yunits[id] ? _yunits[id] : "";
	snprintf(_ylabel[id], kWholeLabelLength, "%s: %.*f %s",
				_yprefix[id], _yprecision[id], value, units);
	drawYLabels();
}

int XMouse::show()
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
		if (_window != None) {
			setFactors();           // must do after creating window
			return 0;
		}
	}
	return -1;
}

Window XMouse::createWindow(
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
	attr.event_mask = KeyPressMask | ExposureMask | PointerMotionMask
	                    | ButtonMotionMask | ButtonPressMask
	                    | ButtonReleaseMask | StructureNotifyMask;
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
		fprintf(stderr, "Error creating mouse window.\n");
		return None;
	}

	XFontStruct *font = XLoadQueryFont(_display, _fontName);
	if (font == NULL) {
		fprintf(stderr, "Mouse window font not found.\n");
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

void XMouse::setFactors()
{
	XWindowAttributes attr;
	XGetWindowAttributes(_display, _window, &attr);
	_xfactor = 1.0 / (double) (attr.width - 1);
	_yfactor = 1.0 / (double) (attr.height - 1);
}

void XMouse::drawXLabels()
{
	if (_xlabelCount > 0) {
		// Clear rect enclosing all X labels.
		int height = _xlabelCount * _lineHeight;
		int width = _maxLabelChars * _charWidth;
		int ypos = _labelYpos;
		XClearArea(_display, _window, _labelXpos, ypos, width, height, False);
#ifdef DEBUG
		XDrawRectangle(_display, _window, _gc, _labelXpos, ypos, width, height);
		printf("drawXLabel: xpos=%d, ypos=%d, width=%d, height=%d\n",
					_labelXpos, ypos, width, height);
#endif

		// Draw all X labels.
		ypos += _fontAscent;
		int line = 0;
		for (int i = 0; i < _xlabelCount; i++) {
			XDrawString(_display, _window, _gc,
							_labelXpos, ypos + (line * _lineHeight),
							_xlabel[i], strlen(_xlabel[i]));
			line++;
		}
		XFlush(_display);
	}
}

void XMouse::drawYLabels()
{
	if (_ylabelCount > 0) {
		// Clear rect enclosing all Y labels.
		int height = _ylabelCount * _lineHeight;
		int width = _maxLabelChars * _charWidth;
		int ypos = _labelYpos + (_xlabelCount * _lineHeight);
		XClearArea(_display, _window, _labelXpos, ypos, width, height, False);
#ifdef DEBUG
		XDrawRectangle(_display, _window, _gc, _labelXpos, ypos, width, height);
		printf("drawYLabel: xpos=%d, ypos=%d, width=%d, height=%d\n",
					_labelXpos, ypos, width, height);
#endif

		// Draw all Y labels.
		ypos += _fontAscent;
		int line = 0;
		for (int i = 0; i < _ylabelCount; i++) {
			XDrawString(_display, _window, _gc,
							_labelXpos, ypos + (line * _lineHeight),
							_ylabel[i], strlen(_ylabel[i]));
			line++;
		}
		XFlush(_display);
	}
}

void XMouse::drawWindowContent()
{
	drawXLabels();
	drawYLabels();
}

bool XMouse::handleKeyPress(XKeyEvent *kevt)
{
#ifdef NOTYET
	KeySym keysym = XLookupKeysym(kevt, 0);
	if (keysym == XK_q)	// regardless of modifier key
		return false;
#endif
	return true;
}

bool XMouse::handleEvents()
{
	XEvent event;
	const unsigned long evtmask =
		  KeyPressMask
		| ExposureMask
		| PointerMotionMask
		| StructureNotifyMask;

	bool keepgoing = true;
	while (XCheckWindowEvent(_display, _window, evtmask, &event)) {
		switch (event.type) {
			case MotionNotify:
				_xraw = event.xmotion.x;
				_yraw = event.xmotion.y;
				break;
			case Expose:
				drawWindowContent();
				break;
			case ConfigureNotify:
				setFactors();
				break;
			case KeyPress:
				keepgoing = handleKeyPress(&event.xkey);
				break;
			default:
				break;
		}
	}
	return keepgoing;
}

