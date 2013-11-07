//
//  MandalaComponent.cpp
//  MandalaComponents
//
//  Created by laserstorm on 11/6/13.
//
//

#include "MandalaComponent.h"

MandalaComponent::MandalaComponent( ofxSimpleSurface* _s,  ofVboMesh* _m, string _name)
{
	componentCount++;
	
	getUniqueName( _name );
	setSurface(_s);
	setMesh(_m);
	
	cout << "name: " << name << " : "<< componentCount <<  endl;
	
	bDraw = true;
};
MandalaComponent::~MandalaComponent()
{
};

void MandalaComponent::setup( ofxSimpleSurface* _s )
{
	setSurface(_s);
}

void MandalaComponent::setSurface( ofxSimpleSurface* _s )
{
	s = _s;
	if(s!=NULL)
	{
		pOnS.setSurface( _s );
	}
}

void MandalaComponent::setMesh( ofVboMesh* _m )
{
	m=_m;
}

void MandalaComponent::setUV( float u, float v)
{
	pOnS.uv.set( u,v );
}

void MandalaComponent::setUV( ofVec2f uv)
{
	pOnS.uv = uv;
}

ofVec2f& MandalaComponent::getUV()
{
	return pOnS.uv;
}

//void MandalaComponent::setScale(float x, float y, float z)
//{
//	scl.set(x,y,z);
//}
//void MandalaComponent::setScale( ofVec3f _scale )
//{
//	scl = _scale;
//}


void MandalaComponent::update()
{
	//get out point on surface
	pOnS.update();
	
	//update transforms
	resetTransform();
	
	setPosition(pOnS.position + pOnS.normal * offset);
	
	setScale( scl );
	
	lookAt( getPosition() + pOnS.normal * 10. );
}

void MandalaComponent::draw()
{
	if(bDraw && m!=NULL)
	{
		transformGL();
		m->draw();
		restoreTransformGL();
	}
}
void MandalaComponent::drawWireframe()
{
	
	if(m!=NULL)
	{
		transformGL();
		m->drawWireframe();
		restoreTransformGL();
	}
}
void MandalaComponent::drawPoints()
{
	if(m!=NULL)
	{
		transformGL();
		m->drawVertices();
		restoreTransformGL();
	}
}

string MandalaComponent::getUniqueName(string _name)
{
	//		static int number_of_MandalaComponents=0;
	name = _name + ofToString(componentCount);
	return name;
}
