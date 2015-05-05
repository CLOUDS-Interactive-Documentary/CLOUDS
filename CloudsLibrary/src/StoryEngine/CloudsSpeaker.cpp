//
//  CloudsSpeaker.h
//  CloudsStoryEngine
//
//  Created by James George on 9/1/13.
//
//

#include "CloudsSpeaker.h"
#include "ofxXmlSettings.h"
#include "CloudsGlobal.h"

map<string,CloudsSpeaker> CloudsSpeaker::speakers = map<string,CloudsSpeaker>();

void CloudsSpeaker::populateSpeakers(){
	
	speakers.clear();
	
	//parse speaker xml
	ofxXmlSettings speakerxml;
	if(!speakerxml.loadFile(GetCloudsDataPath() + "language/" + GetLanguage()+ "/bio/people.xml")){
		ofLogError("CloudsSpeaker::populateSpeakers") << "People XML failed to load";
		return;
	}
	speakerxml.pushTag("clouds");
	int numSpeakers = speakerxml.getNumTags("person");
	for(int i = 0; i < numSpeakers; i++){
		CloudsSpeaker speaker;
		speaker.fcpID = speakerxml.getAttribute("person", "id", "", i);
		if(speaker.fcpID == ""){
			ofLogError("Speaker XML") << "Error parsing entry " << i << " no FCP ID attribute";
			continue;
		}
		speakerxml.pushTag("person", i);
		
		speaker.lastName = speakerxml.getValue("last", "");
		speaker.firstName = speakerxml.getValue("first", "");
		speaker.gender = speakerxml.getValue("gender", "");
		speaker.twitterHandle = speakerxml.getValue("twitter", "");
		speaker.title = speakerxml.getValue("title", "");
		
		speaker.location1 = speakerxml.getValue("location1", "");
		speaker.location2 = speakerxml.getValue("location2", "");
		
		speaker.byline1 = speakerxml.getValue("byline1", "");
		speaker.byline2 = speakerxml.getValue("byline2", "");
		speaker.voiceOverOnly = speakerxml.getValue("voiceover", "false") == "true";
        
		speakerxml.popTag();//Person
		
		speakers[speaker.fcpID] = speaker;
	}
	
	speakerxml.popTag();//clouds

	
}

void CloudsSpeaker::printInfo(){
	
	cout << "SPEAKER: " << endl;
	cout << "	name:		" << firstName << " " << lastName << endl;
	cout << "	gender:		" << gender << endl;

	cout << "	twitterHandle	" << twitterHandle << endl;
	cout << "	title			" << title << endl;
	
	cout << "	location1	" << location1 << endl;
	cout << "	location2:	" << location2 << endl;
	
	cout << "	byline1		" << byline1 << endl;
	cout << "	byline2		" << byline2 << endl;
}

void CloudsSpeaker::exportSpeakerXMLTemplate(){
	
	ofxXmlSettings xmltemplate;
	map<string,CloudsSpeaker>::iterator it;
	xmltemplate.addTag("clouds");
	xmltemplate.pushTag("clouds");
	int personnum = 0;
	for(it = speakers.begin(); it != speakers.end(); it++){
		xmltemplate.addTag("person");
		xmltemplate.addAttribute("person", "id", it->first, personnum);
		xmltemplate.pushTag("person", personnum);
		xmltemplate.addValue("first", it->second.firstName);
		xmltemplate.addValue("last", it->second.lastName);
		xmltemplate.addValue("twitter", "");
		xmltemplate.addValue("byline1", "");
		xmltemplate.addValue("byline2", "");
		xmltemplate.addValue("voiceover", "");

		xmltemplate.popTag();//person;
		personnum++;
	}

	xmltemplate.saveFile(GetCloudsDataPath() + "/SpeakerTemplate.xml");
}