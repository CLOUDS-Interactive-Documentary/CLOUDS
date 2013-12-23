//
//  Astrolabe.cpp
//  Astrolabe
//
//  Created by laserstorm on 12/20/13.
//
//

#include "Astrolabe.h"


Astrolabe::Astrolabe()
{
	adoptedVbo = NULL;
	ease = &easesine;
}
Astrolabe::~Astrolabe()
{
	clear();
}

void Astrolabe::clear()
{
	vbo.clear();
	vertices.clear();
	normals.clear();
}

void Astrolabe::setupMesh( float _lowRadian, float _hiRadian, float _innerRadius, float _outerRadius, float _thickness)
{
	r0 = _lowRadian;
	r1 = _hiRadian;
	
	innerRadius = _innerRadius;
	outerRadius = _outerRadius;
	thickness = _thickness;
	
	
	//set the profile curve
	int subDivisions = abs(r1 - r0) / .05;
	profile.resize(subDivisions);
	
	for( int i=0; i<profile.size(); i++)
	{
		float radian = ofMap(i, 0, profile.size()-1, r0, r1, true);
		profile[i].set( sin(radian), 0, cos(radian) );
	}

	buildTheMesh();
};

void Astrolabe::updateRotations()
{
	//states for tween different attributes...
	
	//rotation tween in x or z & y
	for( map<string, ofxTween>::iterator it = rot.begin(); it!= rot.end(); it++ )
	{
		if( it->second.isCompleted() )
		{
			float start = fmod(it->second.getTarget(0), 360.f);
			
			TweenInfo ti = tweenInfo[it->first];
			
			if( ti.reverse )
			{
				tweenInfo[it->first].step = -tweenInfo[it->first].step;
			}
			
			float scl = ti.scale;
			
			it->second.setParameters(*ease, ofxTween::easeOut, start, start + ti.step * scl, ti.duration * scl, ti.delay * scl );
		}
	}
	
	eularAngels.x = rot.find("x") != rot.end() ? rot["x"].update() : eularAngels.x;
	eularAngels.y = rot.find("y") != rot.end() ? rot["y"].update() : eularAngels.y;
	eularAngels.z = rot.find("z") != rot.end() ? rot["z"].update() : eularAngels.z;
	
	setOrientation( eularAngels );
}

void Astrolabe::addRotationTween( string axis, float startVal, float step, float duration, float delay, float increment )
{
	tweenInfo[axis].set( step, duration, increment );
	rot[axis].setParameters(*ease, ofxTween::easeInOut, startVal, startVal+step, duration, delay );
}
void Astrolabe::setRotationTween( string axis, float startVal, float step, float duration, float delay, float increment )
{
	addRotationTween( axis, startVal, step, duration, delay, increment );
};

void Astrolabe::draw()
{
	ofSetColor( color );
	
	ofPushMatrix();
	ofMultMatrix( getGlobalTransformMatrix() );
	
	if(adoptedVbo != NULL)
	{
		adoptedVbo->draw(GL_QUADS, 0, numVertices );
	}
	else if( vbo.getIsAllocated() )
	{
		vbo.draw(GL_QUADS, 0, numVertices );
	}
	
	ofPopMatrix();
}

ofVec3f Astrolabe::normalFromThreePoints(ofVec3f p0, ofVec3f p1, ofVec3f p2)
{
	return (p2 - p1).cross( p0 - p1).normalized();
}

void Astrolabe::buildTheMesh()
{
	if(profile.size() > 1)
	{
		//create our mesh
		numVertices = 16 * profile.size() + 8;//top, bottom, front, back, lofts & caps
		vertices.resize( numVertices );
		normals.resize( numVertices );
		
		//add the vertices
		ofVec3f halfThick(0,thickness * .5 , 0);
		
		for(int i=0; i<profile.size()-1; i++)
		{
			ofVec3f v0 = profile[i] * innerRadius;
			ofVec3f v1 = profile[i] * outerRadius;
			ofVec3f v2 = profile[i+1] * outerRadius;
			ofVec3f v3 = profile[i+1] * innerRadius;
			
			vertices[i*16 + 0] =  v0 + halfThick;
			vertices[i*16 + 1] =  v1 + halfThick;
			vertices[i*16 + 2] =  v2 + halfThick;
			vertices[i*16 + 3] =  v3 + halfThick;
			
			vertices[i*16 + 4] = v0 - halfThick;
			vertices[i*16 + 5] = v3 - halfThick;
			vertices[i*16 + 6] = v2 - halfThick;
			vertices[i*16 + 7] = v1 - halfThick;
			
			vertices[i*16 + 8] = v1 + halfThick;
			vertices[i*16 + 9] = v1 - halfThick;
			vertices[i*16 + 10] = v2 - halfThick;
			vertices[i*16 + 11] = v2 + halfThick;
			
			vertices[i*16 + 12] = v3 + halfThick;
			vertices[i*16 + 13] = v3 - halfThick;
			vertices[i*16 + 14] = v0 - halfThick;
			vertices[i*16 + 15] = v0 + halfThick;
			
			
			//normals
			fill (normals.begin()+(i*16), normals.begin()+(i*16+3), ofVec3f(0,1,0));
			fill (normals.begin()+(i*16+4), normals.begin()+(i*16+7), ofVec3f(0,-1,0));
			
			normals[i*16 + 8] = profile[i];
			normals[i*16 + 9] = profile[i];
			normals[i*16 + 10] = profile[i+1];
			normals[i*16 + 11] = profile[i+1];
			
			normals[i*16 + 12] = -profile[i+1];
			normals[i*16 + 13] = -profile[i+1];
			normals[i*16 + 14] = -profile[i];
			normals[i*16 + 15] = -profile[i];
			
		}
		
		//cap the ends
		//front
		ofVec3f v0 = profile.front() * innerRadius + halfThick;
		ofVec3f v1 = profile.front() * outerRadius + halfThick;
		ofVec3f v2 = profile.front() * outerRadius - halfThick;
		ofVec3f v3 = profile.front() * innerRadius - halfThick;
		
		*(vertices.end()-8) = v0;
		*(vertices.end()-7) = v3;
		*(vertices.end()-6) = v2;
		*(vertices.end()-5) = v1;
		
		fill (normals.end()-8, normals.end()-5, normalFromThreePoints(v0,v2,v1) );
		
		//back
		v0 = profile.back() * innerRadius + halfThick;
		v1 = profile.back() * outerRadius + halfThick;
		v2 = profile.back() * outerRadius - halfThick;
		v3 = profile.back() * innerRadius - halfThick;
		
		*(vertices.end()-4) = v0;
		*(vertices.end()-3) = v1;
		*(vertices.end()-2) = v2;
		*(vertices.end()-1) = v3;
		
		fill (normals.end()-4, normals.end()-1, normalFromThreePoints(v0,v1,v2) );
		
		//build the vbo
		vbo.clear();
		vbo.setVertexData( &vertices[0], vertices.size(), GL_DYNAMIC_DRAW );
		vbo.setNormalData( &normals[0], normals.size(), GL_DYNAMIC_DRAW );
	}
}

void Astrolabe::setVbo( ofVbo& _vbo, int _numVertices )
{
	numVertices = _numVertices;
	adoptedVbo = &_vbo;
}

void Astrolabe::setTweenIncrement(string axis, int increment)
{
	if(rot.find(axis) != rot.end() )
	{
		rot[axis].setDuration( increment );
		tweenInfo[axis].duration = increment;
	}
}
void Astrolabe::setTweenDelay(string axis, int delay)
{
	if(rot.find(axis) != rot.end() )
	{
		tweenInfo[axis].delay = delay;
	}
}

void Astrolabe::setTweenScale(string axis, float scale)
{
	if(rot.find(axis) != rot.end() )
	{
		tweenInfo[axis].scale = scale;
	}
}

void Astrolabe::setTweenReverse( string axis, bool reverse )
{
	if(rot.find(axis) != rot.end() )
	{
		tweenInfo[axis].reverse = reverse;
	}
}

void Astrolabe::getTweenReverse( string axis )
{
	return 	rot.find(axis) != rot.end() ? tweenInfo[axis].reverse : false;
}