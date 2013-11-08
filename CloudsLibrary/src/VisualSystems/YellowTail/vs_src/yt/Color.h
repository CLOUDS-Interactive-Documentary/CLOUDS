/*
 *  Color.h
 *  yellowtail
 *
 *  Created by Max Hawkins on 12/15/10.
 *  Copyright 2010 Carnegie Mellon University. All rights reserved.
 *
 */

#ifndef YTCOLOR_H
#define YTCOLOR_H


struct Color {
public:
	int r, g, b, a;
	
	Color() {
		r = 255;
		g = 255;
		b = 255;
		a = 255;
	}
	
	Color(int _r, int _g, int _b, int _a) {
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
};


#endif