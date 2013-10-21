//
//  Cog.cpp
//  MandalaVisualSystem
//
//  Created by laserstorm on 10/12/13.
//
//

#include "Cog.h"

Cog::Cog( float _radius, float _thickness, float _startU, float _sweepU, float _startV, float _sweepV)
{
	m = NULL;
	shader != NULL;
	bDrawWireframe = false;
	
	setup( _radius, _thickness, _startU, _sweepU, _startV, _sweepV);
}

Cog::Cog( float _radius, float _thickness, float _startU, float _sweepU, float _startV, float _sweepV, int _subdU, int _subdV)
{
	m = NULL;
	shader != NULL;
	bDrawWireframe = false;
	
	setup( _radius, _thickness, _startU, _sweepU, _startV, _sweepV, _subdU, _subdV);
}

Cog::Cog()
{
	m = NULL;
	shader != NULL;
	bDrawWireframe = false;
	
	setup( 10, 2, 0, .5, 0.25, .75);
}

Cog::~Cog()
{
	clearMesh();
}


void Cog::setup(float _radius,float _thickness, float _startU, float _sweepU, float _startV, float _sweepV)
{
	frontTexture = NULL;
	sideTexture = NULL;
	
	edgeIndexCount = 0;
	bDrawBorders = false;
	bDrawMesh = true;
	edgeLinewidth = 1.;
	thickness = _thickness;
	radius = _radius;
	
	subdScl = 10;
	
	frontColor.set(1, 1, 1, 1);
	sideColor.set(1, 1, 1, 1);
	
	//get our subd for u and v
	minU = TWO_PI * _startU;
	maxU = TWO_PI * (_startU + _sweepU);
	minV = PI * _startV;
	maxV = PI * (_startV + _sweepV);
	
	sweepU = maxU - minU;
	sweepV = maxV - minV;
	
	subdU = radiansToSdubd( sweepU );
	subdV = radiansToSdubd( sweepV );
	
	setupMesh();
}

void Cog::setup(float _radius,float _thickness, float _startU, float _sweepU, float _startV, float _sweepV, int _subdU, int _subdV)
{
	frontTexture = NULL;
	sideTexture = NULL;
	
	edgeIndexCount = 0;
	bDrawBorders = false;
	bDrawMesh = true;
	edgeLinewidth = 1.;
	thickness = _thickness;
	radius = _radius;
	
	subdScl = 10;
	
	frontColor.set(1, 1, 1, 1);
	sideColor.set(1, 1, 1, 1);
	
	//get our subd for u and v
	minU = TWO_PI * _startU;
	maxU = TWO_PI * (_startU + _sweepU);
	minV = PI * _startV;
	maxV = PI * (_startV + _sweepV);
	
	sweepU = maxU - minU;
	sweepV = maxV - minV;
	
	subdU = _subdU;
	subdV = _subdV;
	
	setupMesh();
}

void Cog::draw( ofShader* _shader, int renderMode)
{
	if(_shader == NULL)	_shader = shader;
	
	if(_shader != NULL && m != NULL)
	{
		ofPushMatrix();
		//			ofMultMatrix( getGlobalTransformMatrix() );
		
		_shader->begin();
		_shader->setUniform1i("renderMode", renderMode );
		_shader->setUniform1f( "time", ofGetElapsedTimef() );
		_shader->setUniform1f( "radius", radius );
		_shader->setUniform2f("radianOffset", radianOffset.x, radianOffset.y );
		_shader->setUniform2f("phiTheta", (minU + maxU)*.5, (minV + maxV)*.5 );
		_shader->setUniform2f("sweep", sweepU, sweepV);
		_shader->setUniform2f("startSweep", minU, minV);
		_shader->setUniform4f("frontColor", frontColor.r, frontColor.g, frontColor.b, frontColor.a );
		_shader->setUniform4f("sideColor", sideColor.r, sideColor.g, sideColor.b, sideColor.a );
		
		if(frontTexture != NULL)
		{
			_shader->setUniform1i("usingFrontTexture", 1);
			_shader->setUniformTexture("frontTexture", *frontTexture, 0);
			_shader->setUniform2f("frontTexDim", frontTexture->getWidth(), frontTexture->getHeight() );
		}
		else
		{
			_shader->setUniform1i("usingFrontTexture", 0);
		}
		if(sideTexture != NULL){
			_shader->setUniform1i("usingSideTexture", 1);
			_shader->setUniformTexture("sideTexture", *sideTexture, 1);
			_shader->setUniform2f("sideTexDim", sideTexture->getWidth(), sideTexture->getHeight() );
		}
		else{
			_shader->setUniform1i("usingSideTexture", 0);
		}
		
		if(bDrawMesh)
		{
			bDrawWireframe ? m->drawWireframe() : m->draw();
		}
		
		if (bDrawBorders) {
			//I'm not really liking the way this is set up
			glNormal3f(0, 0, 1);
			glLineWidth( edgeLinewidth );
			ofSetColor(frontColor);
			edges.drawElements(GL_LINES, edgeIndexCount );
		}
		
		
		_shader->end();
		ofPopMatrix();
	}
}

void Cog::drawBorders(ofShader* _shader )
{
	if(_shader == NULL)	_shader = shader;
	
	if(_shader != NULL && m != NULL)
	{
		ofPushMatrix();
		//			ofMultMatrix( getGlobalTransformMatrix() );
		
		_shader->begin();
		_shader->setUniform1f( "time", ofGetElapsedTimef() );
		_shader->setUniform1f( "radius", radius );
		_shader->setUniform2f("radianOffset",  + radianOffset.x,  + radianOffset.y );
		_shader->setUniform2f("sweep", sweepU, sweepV);
		_shader->setUniform2f("startSweep", minU, minV);
		
		glNormal3f(0, 0, 1);
		glLineWidth( edgeLinewidth );
		edges.drawElements(GL_LINES, edgeIndexCount );
		
		_shader->end();
		ofPopMatrix();
	}
}


//MESH METHODS
void Cog::setupMesh()
{
	clearMesh();
	
	m = createCog( minU, maxU, minV, maxV, thickness, subdU, subdV );
}

void Cog::clearMesh()
{
	//delete or pointed mesh
	if(m!=NULL)
	{
		m->clear();
	}
	delete m;
	m = NULL;
	
	
	//clear the box edge mesh
	edges.clear();
}

void Cog::clear()
{
	clearMesh();
}

int Cog::radiansToSdubd( float radians )
{
	//TODO: incorperate radius here
	return int(radians * 10);
}

ofVec3f Cog::normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2)
{
	return (p2 - p1).cross( p0 - p1).normalized();
}

ofVboMesh* Cog::createCog( float uRadiansMin, float uRadiansMax, float vRadiansMin, float vRadiansMax, float thickness, int subdX, int subdY )
{
	return createCogMesh(uRadiansMin, uRadiansMax, vRadiansMin, vRadiansMax, 0, thickness, subdX, subdY, 1 );
}

ofVboMesh* Cog::createCogMesh( float low_w, float hi_w, float low_h, float hi_h, float low_d, float hi_d, int _subx, int _suby, int _subz )
{
	ofVboMesh* m = new ofVboMesh;
	
	int subx = max( 1, _subx );
	int suby = max( 1, _suby );
	int subz = max( 1, _subz );
	
	
	vector<ofVec3f> v;
	vector<ofVec2f> uv;
	vector<ofIndexType> indices;
	
	float stepX = (hi_w - low_w)/subx;
	float stepY = (hi_h - low_h)/suby;
	float stepZ = (hi_d - low_d)/subz;
	
	float uvStepX = 1./subx;
	float uvStepY = 1./suby;
	float uvStepZ = 1./subz;
	
	vector< vector< int > > tempIndices;
	
	//front
	tempIndices.resize(subx+1);
	for (int i=0; i<tempIndices.size(); i++)
	{
		tempIndices[i].resize(suby+1);
		for (int j=0; j<tempIndices[i].size(); j++)
		{
			tempIndices[i][j] = v.size();
			v.push_back( ofVec3f( i*stepX + low_w, j*stepY + low_h, hi_d ) );
			uv.push_back( ofVec2f(i*uvStepX, j*uvStepY) );
		}
	}
	for (int i = 0; i<tempIndices.size()-1; i++)
	{
		for(int j=0; j<tempIndices[i].size()-1; j++)
		{
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i+1][j+1] );
			indices.push_back( tempIndices[i][j+1] );
			
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i+1][j] );
			indices.push_back( tempIndices[i+1][j+1] );
		}
		tempIndices[i].clear();
	}
	tempIndices.clear();
	
	//back
	tempIndices.resize(subx+1);
	for (int i=0; i<tempIndices.size(); i++)
	{
		tempIndices[i].resize(suby+1);
		for (int j=0; j<tempIndices[i].size(); j++)
		{
			tempIndices[i][j] = v.size();
			v.push_back( ofVec3f( i*stepX + low_w, j*stepY + low_h, low_d ) );
			uv.push_back( ofVec2f(i*uvStepX, j*uvStepY) );
		}
	}
	for (int i = 0; i<tempIndices.size()-1; i++)
	{
		for(int j=0; j<tempIndices[i].size()-1; j++)
		{
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i][j+1] );
			indices.push_back( tempIndices[i+1][j+1] );
			
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i+1][j+1] );
			indices.push_back( tempIndices[i+1][j] );
		}
		tempIndices[i].clear();
	}
	tempIndices.clear();
	
	
	//left
	tempIndices.resize(suby+1);
	for (int i=0; i<tempIndices.size(); i++)
	{
		tempIndices[i].resize(subz+1);
		for (int j=0; j<tempIndices[i].size(); j++)
		{
			tempIndices[i][j] = v.size();
			v.push_back( ofVec3f( hi_w, i*stepY + low_h, j*stepZ + low_d ) );
			uv.push_back( ofVec2f(i*uvStepY, j*uvStepZ) );
		}
	}
	for (int i = 0; i<tempIndices.size()-1; i++)
	{
		for(int j=0; j<tempIndices[i].size()-1; j++)
		{
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i+1][j+1] );
			indices.push_back( tempIndices[i][j+1] );
			
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i+1][j] );
			indices.push_back( tempIndices[i+1][j+1] );
		}
		tempIndices[i].clear();
	}
	tempIndices.clear();
	
	//right
	tempIndices.resize(suby+1);
	for (int i=0; i<tempIndices.size(); i++)
	{
		tempIndices[i].resize(subz+1);
		for (int j=0; j<tempIndices[i].size(); j++)
		{
			tempIndices[i][j] = v.size();
			v.push_back( ofVec3f( low_w, i*stepY + low_h, j*stepZ + low_d ) );
			uv.push_back( ofVec2f(i*uvStepY, j*uvStepZ) );
		}
	}
	for (int i = 0; i<tempIndices.size()-1; i++)
	{
		for(int j=0; j<tempIndices[i].size()-1; j++)
		{
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i][j+1] );
			indices.push_back( tempIndices[i+1][j+1] );
			
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i+1][j+1] );
			indices.push_back( tempIndices[i+1][j] );
		}
		tempIndices[i].clear();
	}
	tempIndices.clear();
	
	//top
	tempIndices.resize(subx+1);
	for (int i=0; i<tempIndices.size(); i++)
	{
		tempIndices[i].resize(subz+1);
		for (int j=0; j<tempIndices[i].size(); j++)
		{
			tempIndices[i][j] = v.size();
			v.push_back( ofVec3f(  i*stepX + low_w, hi_h, j*stepZ + low_d ) );
			uv.push_back( ofVec2f(i*uvStepX, j*uvStepZ) );
		}
	}
	for (int i = 0; i<tempIndices.size()-1; i++)
	{
		for(int j=0; j<tempIndices[i].size()-1; j++)
		{
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i][j+1] );
			indices.push_back( tempIndices[i+1][j+1] );
			
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i+1][j+1] );
			indices.push_back( tempIndices[i+1][j] );
		}
		tempIndices[i].clear();
	}
	tempIndices.clear();
	
	
	//bottom
	tempIndices.resize(subx+1);
	for (int i=0; i<tempIndices.size(); i++)
	{
		tempIndices[i].resize(subz+1);
		for (int j=0; j<tempIndices[i].size(); j++)
		{
			tempIndices[i][j] = v.size();
			v.push_back( ofVec3f(  i*stepX + low_w, low_h, j*stepZ + low_d ) );
			uv.push_back( ofVec2f(i*uvStepX, j*uvStepZ) );
		}
	}
	for (int i = 0; i<tempIndices.size()-1; i++)
	{
		for(int j=0; j<tempIndices[i].size()-1; j++)
		{
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i+1][j+1] );
			indices.push_back( tempIndices[i][j+1] );
			
			indices.push_back( tempIndices[i][j] );
			indices.push_back( tempIndices[i+1][j] );
			indices.push_back( tempIndices[i+1][j+1] );
		}
		tempIndices[i].clear();
	}
	tempIndices.clear();
	
	//compute smoothed normals( one normal per vertex, not per face)
	vector<ofVec3f> n(v.size());
	ofVec3f norm;
	for (int i=0; i<indices.size(); i+=3)
	{
		norm = normalFrom3Points( v[indices[i]], v[indices[i+1]], v[indices[i+2]]);
		n[indices[i]] += norm;
		n[indices[i+1]] += norm;
		n[indices[i+2]] += norm;
	}
	
	for (int i=0; i<n.size(); i++)
	{
		n[i].normalize();
	}
	
	m->addVertices( v );
	m->addNormals( n );
	m->addTexCoords( uv );
	m->addIndices( indices );
	
	
	//create an edge mesh to draw the box outline
	edges.clear();
	vector<ofVec3f> edgeVerts;
	vector<ofVec2f> edgeUVs;
	vector<ofIndexType> edgeIndices;
	
	//it's probably easier to reconstrcut them than to sample the existing mesh

	for (int i=0; i<=subx; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<subx)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( i*stepX + low_w, low_h, low_d ) );
		edgeUVs.push_back( ofVec2f(i*uvStepX, 0 ) );
		
	}
	for (int i=0; i<=subx; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<subx)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( i*stepX + low_w, hi_h, low_d ) );
		edgeUVs.push_back( ofVec2f(i*uvStepX, 1 ) );
		
	}
	for (int i=0; i<=subx; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<subx)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( i*stepX + low_w, low_h, hi_d ) );
		edgeUVs.push_back( ofVec2f(i*uvStepX, 0 ) );
		
	}
	for (int i=0; i<=subx; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<subx)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( i*stepX + low_w, hi_h, hi_d ) );
		edgeUVs.push_back( ofVec2f(i*uvStepX, 1 ) );
		
	}
	
	
	
	
	for (int i=0; i<=suby; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<suby)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( low_w, i*stepY + low_h, low_d ) );
		edgeUVs.push_back( ofVec2f( 0, i * uvStepY ) );
		
	}
	for (int i=0; i<=suby; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<suby)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( hi_w, i*stepY + low_h, low_d ) );
		edgeUVs.push_back( ofVec2f( 1, i * uvStepY ) );
		
	}
	for (int i=0; i<=suby; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<suby)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( low_w, i*stepY + low_h, hi_d ) );
		edgeUVs.push_back( ofVec2f( 0, i * uvStepY ) );
		
	}
	for (int i=0; i<=suby; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<suby)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( hi_w, i*stepY + low_h, hi_d ) );
		edgeUVs.push_back( ofVec2f( 1, i * uvStepY ) );
		
	}
	
	
	
	for (int i=0; i<=subz; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<subz)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( low_w, low_h, i*stepZ + low_d ) );
		edgeUVs.push_back( ofVec2f( 0, i * uvStepY ) );
	}
	for (int i=0; i<=subz; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<subz)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( hi_w, low_h, i*stepZ + low_d ) );
		edgeUVs.push_back( ofVec2f( 1, i * uvStepY ) );
	}
	for (int i=0; i<=subz; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<subz)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( low_w, hi_h, i*stepZ + low_d ) );
		edgeUVs.push_back( ofVec2f( 0, i * uvStepY ) );
	}
	for (int i=0; i<=subz; i++)
	{
		edgeIndices.push_back(edgeVerts.size());
		if(i<subz)	edgeIndices.push_back(edgeVerts.size()+1);
		else	edgeIndices.push_back(edgeVerts.size());
		
		edgeVerts.push_back( ofVec3f( hi_w, hi_h, i*stepZ + low_d ) );
		edgeUVs.push_back( ofVec2f( 1, i * uvStepY ) );
	}
	
	edges.setVertexData( &edgeVerts[0], edgeVerts.size(), GL_STATIC_DRAW );
	edges.setTexCoordData( &edgeUVs[0], edgeUVs.size(), GL_STATIC_DRAW );
	edges.setIndexData( &edgeIndices[0], edgeIndices.size(), GL_STATIC_DRAW );
	
	edgeIndexCount = edgeIndices.size();
	
	return m;
}









