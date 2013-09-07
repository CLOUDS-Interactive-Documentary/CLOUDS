//
//  CloudsSpeaker.h
//  CloudsStoryEngine
//
//  Created by James George on 9/1/13.
//
//

#pragma once
#include "ofMain.h"

class CloudsSpeaker {
  public:
	CloudsSpeaker(){};
	CloudsSpeaker(string fcpID,string firstName,string lastName,string gender)
		:fcpID(fcpID),firstName(firstName),lastName(lastName),gender(gender)
	{}
	
	static map<string,CloudsSpeaker> speakers;
	
	static void populateSpeakers();
	
	string fcpID;
	string lastName;
	string firstName;
	string gender;
};

