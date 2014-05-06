#pragma once

#include "ofMain.h"
#include "CloudsPortal.h"

class CloudsPortalEventArgs : public ofEventArgs
{
  public:
	CloudsPortalEventArgs(string question = "", bool animate = true)
		: question(question), animate(animate){}
	
	string question;
	bool animate;
};

class CloudsVisualSystemEvents {
public:
    ofEvent<CloudsPortalEventArgs> portalHoverBegan;
    ofEvent<CloudsPortalEventArgs> portalHoverEnded;
};
