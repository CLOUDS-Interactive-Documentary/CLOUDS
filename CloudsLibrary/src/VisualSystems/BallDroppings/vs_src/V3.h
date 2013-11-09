#ifndef V3_H
#define V3_H

/**
	V3 is a basic point math class
	functions thanks to OpenFrameworks and jttoolkit
*/

class V3{
	public:
		float x;
		float y;
		float z;
		V3();
		V3(float xx,float yy,float zz);
		float dot(V3 vec);
		void copyFrom(V3 that);
		void copyFrom(float,float,float);
		void copyFrom(float,float);
		V3 getRightNormal();
		V3 getLeftNormal();
		void normalize();
		float getLength();
		void scaleVec(float scalar);
		V3 minVecNew(V3 vec);
		void operator*=(float);
		void operator+=(V3 v);
		void lerpSelfTo(V3,float);
};

#endif
