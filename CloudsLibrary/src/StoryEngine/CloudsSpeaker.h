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
	
	string fcpID;
	string lastName;
	string firstName;
	string gender;
};


static map<string,CloudsSpeaker> speakers;


static void populateSpeakers(){
	speakers.clear();
	
	speakers["Aaron"]		= CloudsSpeaker("Aaron", "Aaron", "Koblin", "male");
	speakers["Andres"]		= CloudsSpeaker("Andres", "Andres", "Colubri", "male");
	speakers["Binx"]		= CloudsSpeaker("Binx", "Rachel", "Binx", "female");
	speakers["Casey"]		= CloudsSpeaker("Casey", "Casey", "", "male");
	speakers["Chris"]		= CloudsSpeaker("Chris", "Chris", "Sugrue", "male");
	speakers["Diederick"]	= CloudsSpeaker("Diederick", "Diederick", "Huijbers", "male");
	speakers["Elliot"]		= CloudsSpeaker("Elliot", "Elliot", "Woods", "male");
	speakers["Elliot_UK"]	= CloudsSpeaker("Elliot_UK", "Elliot", "Woods", "male");
	speakers["Fernanda"]	= CloudsSpeaker("Fernanda", "Fernanda", "Viegas", "male");
	speakers["Golan"]		= CloudsSpeaker("Golan", "Golan", "Levin", "male");
	speakers["Golan_AMA"]	= CloudsSpeaker("Golan_AMA", "Golan", "Levin", "male");
	speakers["Greg"]		= CloudsSpeaker("Greg", "Greg", "Borenstein", "male");
	speakers["JTNimoy"]		= CloudsSpeaker("JTNimoy", "J", "", "male");
	speakers["Javier"]		= CloudsSpeaker("", "", "", "male");
	speakers["Jen"]			= CloudsSpeaker("", "", "", "male");
	speakers["Jer"]			= CloudsSpeaker("", "", "", "male");
	speakers["Joel"]		= CloudsSpeaker("", "", "", "male");
	speakers["Julia"]		= CloudsSpeaker("", "", "", "male");
	speakers["Julian"]		= CloudsSpeaker("", "", "", "male");
	speakers["Karolina"]	= CloudsSpeaker("", "", "", "male");
	speakers["Karsten"]		= CloudsSpeaker("", "", "", "male");
	speakers["Kevin"]		= CloudsSpeaker("", "", "", "male");
	speakers["Kyle_CH"]		= CloudsSpeaker("", "", "", "male");
	speakers["Kyle_MC"]		= CloudsSpeaker("", "", "", "male");
	speakers["Lauren"]		= CloudsSpeaker("", "", "", "male");
	speakers["Lindsay"]		= CloudsSpeaker("", "", "", "male");
	speakers["Maeda"]		= CloudsSpeaker("", "", "", "male");
	speakers["Marcus"]		= CloudsSpeaker("", "", "", "male");
	speakers["Marius"]		= CloudsSpeaker("", "", "", "male");
	speakers["Martin"]		= CloudsSpeaker("", "", "", "male");
	speakers["Nervous_Jesse"]		= CloudsSpeaker("", "", "", "male");
	speakers["Nervous_Jessica"]		= CloudsSpeaker("", "", "", "male");
	speakers["Paola"]		= CloudsSpeaker("", "", "", "male");
	speakers["Philip"]		= CloudsSpeaker("", "", "", "male");
	speakers["Ramsey"]		= CloudsSpeaker("", "", "", "male");
	speakers["Regine"]		= CloudsSpeaker("", "", "", "male");
	speakers["Shantell"]	= CloudsSpeaker("", "", "", "male");
	speakers["Shiffman"]	= CloudsSpeaker("", "", "", "male");
	speakers["Sophy"]		= CloudsSpeaker("", "", "", "male");
	speakers["Theo"]		= CloudsSpeaker("", "", "", "male");
	speakers["Vera"]		= CloudsSpeaker("", "", "", "male");
	speakers["Zach"]		= CloudsSpeaker("", "", "", "male");
}


