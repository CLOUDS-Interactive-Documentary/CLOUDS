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
	CloudsSpeaker(){
        voiceOverOnly = false;
    };
//	CloudsSpeaker(string fcpID,string firstName,string lastName,string gender)
//		:fcpID(fcpID),firstName(firstName),lastName(lastName),gender(gender)
//	{
//    }
	
	void printInfo();
	
	static map<string,CloudsSpeaker> speakers;
	static map<string,CloudsSpeaker> twitterHandleToSpeaker;
    
	static void populateSpeakers();
	static void exportSpeakerXMLTemplate();
	
	string fcpID;
	string lastName;
	string firstName;
	string gender;
	string twitterHandle;
	string title;
	
    bool voiceOverOnly;
    
	string location1; // Santa Monica, California
	string location2; // San Francisco, California
	
	string byline1;
	string byline2;
	
};

