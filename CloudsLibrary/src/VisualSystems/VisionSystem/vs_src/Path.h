//
//  Path.h
//  yearLong
//
//  Created by Surya on 01/04/13.
//
//

#ifndef __yearLong__Path__
#define __yearLong__Path__

#include <iostream>
#include "ofMain.h"

class Path{
public:
    Path();
    Path(int x, int y,int x1, int y1);
    
    ofVec2f start;
    ofVec2f end;
    float radius;
    
    void display();
    
};
#endif /* defined(__yearLong__Path__) */
