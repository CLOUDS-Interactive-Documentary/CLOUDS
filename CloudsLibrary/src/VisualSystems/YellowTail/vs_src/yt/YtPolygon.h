/*
 *  Polygon.h
 *  YellowTail
 *
 *  Copyright 2008 Carnegie Mellon University. All rights reserved.
 *
 */

#ifndef YT_POLYGON_H
#define YT_POLYGON_H


    
struct YtPolygon
{
public:
	
	int pointCount;
	float xpoints[8];
	float ypoints[8];
	
	YtPolygon():
	pointCount(8)
	{
	}
	
};

#endif