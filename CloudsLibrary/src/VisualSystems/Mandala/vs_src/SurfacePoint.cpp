//
//  SurfacePoint.cpp
//  MandalaComponents
//
//  Created by laserstorm on 11/6/13.
//
//

#include "SurfacePoint.h"


SurfacePoint::SurfacePoint()
{
	surface = NULL;
	outpos = NULL;
	outnorm = NULL;
};
SurfacePoint::~SurfacePoint(){}

void SurfacePoint::setSurface( ofxSimpleSurface* s )
{
	surface = s;
}

void SurfacePoint::setTargetPosition( ofVec3f* v)
{
	outpos = v;
}
void SurfacePoint::setTargetNormal( ofVec3f* v)
{
	outnorm = v;
}

void SurfacePoint::update()
{
	if(surface!=NULL)
	{
		surface->getSurfacePositionAndNormal( position, normal, uv.x - floor(uv.x), uv.y - floor(uv.y) );
	}
	if(outpos != NULL)
	{
		*outpos = position;
	}
	
	if(outnorm != NULL)
	{
		*outnorm = normal;
	}
}

void SurfacePoint::operator = (SurfacePoint sp)
{
	position = sp.position;
	normal = sp.normal;
	uv = sp.uv;
	outpos = sp.outpos;
	surface = sp.surface;
}