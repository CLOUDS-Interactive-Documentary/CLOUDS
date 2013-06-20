#pragma once

#include "ofMain.h"
#include "CloudsFCPParser.h"
#include <algorithm> 
//#include "../../../CloudsLibrary/VisualSystems"
#include "CloudsVisualSystemRezanator.h"
#include "VoroParticle.h"
#include "Voro.h"


class testApp : public ofBaseApp{
  public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);


    CloudsFCPParser parser;
    ofTrueTypeFont font;
    vector<string> keyWords;

  
    float maxD;
    float minD;
    float maxClip;
    float minClip;
    float maxDist;
    float minDist;
   // CloudsVisualSystemVoro voro;
};
