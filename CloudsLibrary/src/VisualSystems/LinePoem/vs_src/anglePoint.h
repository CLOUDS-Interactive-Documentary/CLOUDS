
#pragma once

#include "ofMain.h"

class anglePoint {

public: 
	float angle;
	
	//---------------------
    anglePoint() {
		angle = 0;
	}
	
	//	---------------------
    anglePoint(float _angle){
		angle = _angle;
	}
	
	//	---------------------
	void set(float _angle){
		angle = _angle;
	}
};

