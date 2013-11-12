/**
@author jtnimoy
*/

#ifndef _LINE_H_
#define _LINE_H_

#include "Ball.h"
#include "V3.h"

class EditLine {
	public:
		float x1;
		float y1;
		float x2;
		float y2;
		void initMem();
		EditLine();
		EditLine(float _x1,float _y1, float _x2,float _y2);
		~EditLine();
		void set1(float x,float y);
		void  set2(float x,float y);
		int whichSideY(float x,float y);
		int fixDirection();
		bool checkBallCollide(Ball *ball);
		float checkAngle( float point_x, float point_y,float line_x, float line_y, V3 lineVec);
		bool diffSign(float a,float b);

};

#endif

