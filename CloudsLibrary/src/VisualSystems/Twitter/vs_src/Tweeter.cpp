//
//  Tweeter.cpp
//  Twitter
//
//  Created by Surya Mattu on 23/11/13.
//
//

#include "Tweeter.h"

Tweeter::Tweeter(){
    name = " ";
    ID = -1 ;
    position = ofVec3f(-1, -1,-1);
}

Tweeter::Tweeter(string _name, int _id){
    name = _name;
    ID = _id;
    position = ofVec3f(-1, -1,-1);
}




