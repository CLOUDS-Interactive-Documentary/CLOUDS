//
//  ColorManager.h
//  MazeGenerator
//
//  Created by Gal Sasson on 10/25/13.
//
//

#ifndef __MazeGenerator__ColorManager__
#define __MazeGenerator__ColorManager__

#include <ofMain.h>

class ParamManager
{
public:
	static ParamManager& getInstance()
	{
		static ParamManager instance;
		return instance;
	}
    
    ofFloatColor groundColor;
    ofFloatColor wallColor;
    ofFloatColor sideWallsColor;
    
    ofColor getGroundColor();
    ofColor getWallColor();
    ofColor getSideWallsColor();
    
    float cameraSpeed;
    float cameraAngle;
    float showAhead;
    
private:
    ParamManager();
    ~ParamManager();
    
    ParamManager(ParamManager const&) {cout<<"copy constructor\n";}
    void operator=(ParamManager const&) {cout<<"assignment operator called\n";}
    
    
};

#endif /* defined(__MazeGenerator__ColorManager__) */
