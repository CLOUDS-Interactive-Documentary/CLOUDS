/* RTcmix - Copyright (C) 2004  The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/
#ifndef _XMOUSE_H_
#define _XMOUSE_H_
#include <RTcmixMouse.h>
#include <X11/Xlib.h>

class XMouse : public RTcmixMouse {
public:
	XMouse();

	virtual int show();

protected:
	virtual ~XMouse();

	// RTcmixMouse reimplementations

	virtual inline double getPositionX() const
	{
		return _xraw * _xfactor;
	}

	virtual inline double getPositionY() const
	{
		return 1.0 - (_yraw * _yfactor);
	}

	virtual void doConfigureXLabel(const int id, const char *prefix,
                                 const char *units, const int precision);
	virtual void doConfigureYLabel(const int id, const char *prefix,
                                 const char *units, const int precision);
	virtual void doUpdateXLabelValue(const int id, const double value);
	virtual void doUpdateYLabelValue(const int id, const double value);

	virtual bool handleEvents();

private:
	Window createWindow(const int xpos, const int ypos,
					const unsigned int width, const unsigned int height);
	void setFactors();
	void drawWindowContent();
	void drawXLabels();
	void drawYLabels();
	bool handleKeyPress(XKeyEvent *kevt);

	int _screen;
	Display *_display;
	Window _window;
	const char *_windowname;

	int _xraw;
	int _yraw;
	double _xfactor;
	double _yfactor;

	GC _gc;
	char *_fontName;
	int _labelXpos;
	int _labelYpos;
	int _fontAscent;
	int _lineHeight;
	int _maxLabelChars;
	int _charWidth;
};

#endif // _XMOUSE_H_
