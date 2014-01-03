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
	if(!speakerxml.loadFile(GetCloudsDataPath()+"secondaryDisplay/web/xml/people.xml")){
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
		
		speakerxml.popTag();//Person
		
		speakers[speaker.fcpID] = speaker;
//		cout << "ADDED SPEAKER " << speaker.fcpID << endl;
///		speaker.printInfo();
	}
	
	speakerxml.popTag();//clouds
	
	//	speakers["Aaron"]		= CloudsSpeaker("Aaron", "Aaron", "Koblin", "male");
	//	speakers["Andres"]		= CloudsSpeaker("Andres", "Andres", "Colubri", "male");
	//	speakers["Binx"]		= CloudsSpeaker("Binx", "Rachel", "Binx", "female");
	//	speakers["Casey"]		= CloudsSpeaker("Casey", "Casey", "Reas", "male");
	//	speakers["Chris"]		= CloudsSpeaker("Chris", "Chris", "Sugrue", "female");
	//	speakers["Diederick"]	= CloudsSpeaker("Diederick", "Diederick", "Huijbers", "male");
	//	speakers["Elliot"]		= CloudsSpeaker("Elliot", "Elliot", "Woods", "male");
	//	speakers["Elliot_UK"]	= CloudsSpeaker("Elliot_UK", "Elliot", "Woods", "male");
	//	speakers["Fernanda"]	= CloudsSpeaker("Fernanda", "Fernanda", "Viegas", "female");
	//	speakers["Golan"]		= CloudsSpeaker("Golan", "Golan", "Levin", "male");
	//	speakers["Golan_AMA"]	= CloudsSpeaker("Golan_AMA", "Golan", "Levin", "male");
	//	speakers["Greg"]		= CloudsSpeaker("Greg", "Greg", "Borenstein", "male");
	//	speakers["JTNimoy"]		= CloudsSpeaker("JTNimoy", "Josh", "Nimoy", "male");
	//	speakers["Javier"]		= CloudsSpeaker("Javier", "Javier", "", "male");
	//	speakers["Jen"]			= CloudsSpeaker("Jen", "Jen", "Lowe", "female");
	//	speakers["Jer"]			= CloudsSpeaker("Jer", "Jer", "Thorpe", "male");
	//	speakers["Joel"]		= CloudsSpeaker("Joel", "Joel", "Lewis", "male");
	//	speakers["Julia"]		= CloudsSpeaker("Julia", "Julia", "Kaganskiy", "female");
	//	speakers["Julian"]		= CloudsSpeaker("Julian", "Julian", "Oliver", "male");
	//	speakers["Karolina"]	= CloudsSpeaker("Karolina", "Karolina", "Sobecka", "female");
	//	speakers["Karsten"]		= CloudsSpeaker("Karsten", "Karsten", "Schmidt", "male");
	//	speakers["Kevin"]		= CloudsSpeaker("Kevin", "Kevin", "Slavin", "male");
	//	speakers["Kyle_CH"]		= CloudsSpeaker("Kyle_CH", "Kyle", "Chayka", "male");
	//	speakers["Kyle_MC"]		= CloudsSpeaker("Kyle_MC", "Kyle", "McDonald", "male");
	//	speakers["Lauren"]		= CloudsSpeaker("Lauren", "Lauren", "McCarthy", "female");
	//	speakers["Lindsay"]		= CloudsSpeaker("Lindsay", "Lindsay", "Howard", "female");
	//	speakers["Maeda"]		= CloudsSpeaker("John", "John", "Maeda", "male");
	//	speakers["Marcus"]		= CloudsSpeaker("Marcus", "Marcus", "Wendt", "male");
	//	speakers["Marius"]		= CloudsSpeaker("Marius", "Marius", "Watz", "male");
	//	speakers["Martin"]		= CloudsSpeaker("Martin", "Martin", "Wattenberg", "male");
	//	speakers["Nervous_Jesse"]		= CloudsSpeaker("Jesse", "Louis-Rosenberg", "", "male");
	//	speakers["Nervous_Jessica"]		= CloudsSpeaker("Jessica", "Jessica", "Rosencrantz", "female");
	//	speakers["Paola"]		= CloudsSpeaker("Paola", "Paola", "Antonelli", "female");
	//	speakers["Philip"]		= CloudsSpeaker("Philip", "Philip", "Whitfield", "male");
	//	speakers["Ramsey"]		= CloudsSpeaker("Ramsey", "Ramsey", "Nasser", "male");
	//	speakers["Regine"]		= CloudsSpeaker("Regine", "Regine", "Debatty", "female");
	//	speakers["Shantell"]	= CloudsSpeaker("Shantell", "Shantell", "Martin", "female");
	//	speakers["Shiffman"]	= CloudsSpeaker("Daniel", "Daniel", "Shiffman", "male");
	//	speakers["Sophy"]		= CloudsSpeaker("Sophy", "Sophy", "Yuditskaya", "female");
	//	speakers["Theo"]		= CloudsSpeaker("Theo", "Theo", "Watson", "male");
	//	speakers["Vera"]		= CloudsSpeaker("Vera", "Vera", "Glahn", "female");
	//	speakers["Zach"]		= CloudsSpeaker("Zach", "Zach", "Lieberman", "male");

	//clouds
//	<person id="Aaron">
//	<first>Aaron</first>
//	<last>Koblin</last>
//	<title>Digital Media Artist</title>
//	<twitter>@aaronkoblin</twitter>
//	<location1>Santa Monica, California</location1>
//	<location2>San Francisco, California</location2>
//	<byline1>As Creative Director of the Data Arts Team at Google, Aaron Koblin develops innovative uses of data visualization, crowdsourcing, and web-based storytelling. </byline1>
//	<byline2>He works with community generated data to reflect on cultural trends and the changing relationship between humans and the systems they create.
//	</byline2>
//	<resume> ... </resume>
//	</person>
	//all people
//	map<string,CloudsSpeaker>::iterator it;
//	for(it = speakers.begin(); it != speakers.end(); it++){
//		cout << it->second.firstName + " "  + it->second.lastName << endl;
//	}
	
	//export xml template
	
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
		xmltemplate.addValue("resume", "");

		xmltemplate.popTag();//person;
		personnum++;
	}

	xmltemplate.saveFile(GetCloudsDataPath() + "/SpeakerTemplate.xml");
}