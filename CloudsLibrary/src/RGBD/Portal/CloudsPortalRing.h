/*
 *  CloudsPortalRing.h
 *  Empty
 *
 *  Created by Jim on 12/21/13.
 *  Copyright 2013 University of Washington. All rights reserved.
 *
 */

typedef struct {
	float curentRot;
	float
} PortalShard;

class CloudsPortalRing {
  public:
	CloudsPortalRing();
	void setup();
	vector<PortalShard> shards;
	float radius;
	float thickness;
	
};