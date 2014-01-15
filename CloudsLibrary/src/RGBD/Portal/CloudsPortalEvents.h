#pragma once

#include "ofMain.h"
#include "CloudsPortal.h"

class CloudsPortalEventArgs : public ofEventArgs{
public:
    CloudsPortalEventArgs(CloudsPortal& portal, string question)
    : portal(portal), question(question){}
	
    CloudsPortal& portal;
	string question;
};

class CloudsVisualSystemEvents {
public:
    ofEvent<CloudsPortalEventArgs> portalHoverBegan;
    ofEvent<CloudsPortalEventArgs> portalHoverEnded;
};
