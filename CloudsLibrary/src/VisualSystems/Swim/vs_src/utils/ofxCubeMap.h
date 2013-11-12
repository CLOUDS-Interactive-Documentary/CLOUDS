/*
 *  Inspired by Cinder CubeMap David Wicks on 2/27/09.
 *  Ported to oF by brenfer on 7/2/10.
 *  Updated for of 007 and extended by James George 2/13/11
 *
 *  Made working-happily by Joshy-pants and James George
 *
 *  Not copyrighted, no rights reserved, long live Christiania.
 *
 */

#ifndef	CUBE_MAP_H
#define CUBE_MAP_H

#include "ofMain.h"

class ofxCubeMap
{
public:
	
	unsigned int textureObject;	
	
	//these should all be the same size
	// and all power of two, i.e. 256, 512, 1024 (I think)
	void loadImages( string pos_x, string pos_y, string pos_z, string neg_x,string neg_y,string neg_z)
	{	
		ofImage images[6];	
		bool loaded1 = images[0].loadImage(pos_x);
		bool loaded2 = images[1].loadImage(neg_x);
		bool loaded3 = images[2].loadImage(pos_y);
		bool loaded4 = images[3].loadImage(neg_y);
		bool loaded5 = images[4].loadImage(pos_z);
		bool loaded6 = images[5].loadImage(neg_z);
		

		loadFromOfImages(images[0],
						 images[2],
						 images[4],
						 images[1],
						 images[3],
						 images[5]);
		
	}
	
	void loadFromOfImages( ofImage pos_x, ofImage pos_y, ofImage pos_z, ofImage neg_x,ofImage neg_y,ofImage neg_z);
	
	void bindMulti( int pos )
	{
		glActiveTexture(GL_TEXTURE0 + pos );
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, textureObject);
	}

	void bind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, textureObject);
	}

	void unbind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, 0 );
	}

	void enableFixedMapping()
	{
		glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP );
		glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP );
		glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP );
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glEnable( GL_TEXTURE_CUBE_MAP );
	}

	void disableFixedMapping()
	{
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable( GL_TEXTURE_CUBE_MAP );
	}
};

#endif
