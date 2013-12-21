/*
 *  CloudsPortal.h
 *  Empty
 *
 *  Created by Jim on 12/21/13.
 *  Copyright 2013 CLOUDS. All rights reserved.
 *
 */

#pragma once

#include "CloudsPortalRing.h"

class CloudsPortal {
  public:
	CloudsPortal();
	~CloudsPortal();	
	
	void setup();
	vector<CloudsPortalRing> rings;
	
};