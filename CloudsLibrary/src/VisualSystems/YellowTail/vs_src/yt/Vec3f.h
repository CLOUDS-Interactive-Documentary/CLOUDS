/*
 *  Vec3f.h
 *  YellowTail
 *
 *  Created by Lee Byron on 8/29/08.
 *  Copyright 2008 Carnegie Mellon University. All rights reserved.
 *
 */

#ifndef VEC3F_H
#define VEC3F_H

    
struct Vec3f
{
public:
	
	float x;
	float y;
	float p; // "pressure" mapped to local thickness
	
	Vec3f():
	x(0.0f),
	y(0.0f),
	p(0.0f)
	{
	}
	
	Vec3f(float _x, float _y, float _p):
	x(_x),
	y(_y),
	p(_p)
	{
	}
	
	void set(float _x, float _y, float _p)
	{
		x = _x;
		y = _y;
		p = _p;
	}
	
};
    

#endif