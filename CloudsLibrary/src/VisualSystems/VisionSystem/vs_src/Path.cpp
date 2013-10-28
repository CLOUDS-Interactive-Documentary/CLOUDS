//
//  Path.cpp
//  yearLong
//
//  Created by Surya on 01/04/13.
//
//

#include "Path.h"


Path::Path(){
    
}

Path::Path(int x, int y, int x1 ,int y1){
    radius = 5;
    start = ofVec2f(x,y);
    end = ofVec2f(x1,y1);
    //end.he
}


void Path::display(){
    ofSetColor(255);
    ofLine(start.x, start.y, end.x, end.y);
}