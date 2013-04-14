/* RTcmix - Copyright (C) 2005  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _XDISPLAY_H_
#define _XDISPLAY_H_
#include <RTcmixDisplay.h>
#include <X11/Xlib.h>

class XDisplay : public RTcmixDisplay {
public:
	XDisplay();
	virtual ~XDisplay();
	virtual int show();

protected:
	// RTcmixDisplay reimplementations
	virtual void doConfigureLabel(const int id, const char *prefix,
                                 const char *units, const int precision);
	virtual void doUpdateLabelValue(const int id, const double value);
	virtual bool handleEvents();

private:
	Window createWindow(const int xpos, const int ypos,
					const unsigned int width, const unsigned int height);
	void drawWindowContent();
	void drawLabel(const int id);
	void drawLabels();

	int _screen;
	Display *_display;
	Window _window;
	const char *_windowname;

	GC _gc;
	char *_fontName;
	int _labelXpos;
	int _labelYpos;
	int _fontAscent;
	int _lineHeight;
	int _maxLabelChars;
	int _charWidth;
};

#endif // _XDISPLAY_H_
