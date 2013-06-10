//
//  ClusterData.h
//  CloudsSVGTest
//
//  Created by Surya Mattu on 09/06/13.
//
//

#ifndef __CloudsSVGTest__ClusterData__
#define __CloudsSVGTest__ClusterData__

#include <iostream>
#include "ofMain.h"

class ClusterData {
public:
	
	ClusterData();
	
    string Id;
    ofColor Color;
	int FillOpacity;
    float Radius;
    ofVec2f Centre;
    //vector<string> RelatedIds;

	
};
#endif /* defined(__CloudsSVGTest__ClusterData__) */
