//
//  CloudsIntervention.h
//  CloudsPointcloudViewer
//
//  Created by Surya Mattu on 15/11/13.
//
//

#ifndef __CloudsPointcloudViewer__CloudsIntervention__
#define __CloudsPointcloudViewer__CloudsIntervention__
#include "ofMain.h"

class CloudsIntervention{
public:
    CloudsIntervention(){};
    CloudsIntervention(string _name) : name(_name){
    }
    
    string name;
    
    
};

#endif /* defined(__CloudsPointcloudViewer__CloudsIntervention__) */
