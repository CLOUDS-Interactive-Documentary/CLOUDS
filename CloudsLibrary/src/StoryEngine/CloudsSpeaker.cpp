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
	
	cout << "POPULATING SPEAKERS" << endl;
	speakers.clear();
	
	speakers["Aaron"]		= CloudsSpeaker("Aaron", "Aaron", "Koblin", "male");
	speakers["Andres"]		= CloudsSpeaker("Andres", "Andres", "Colubri", "male");
	speakers["Binx"]		= CloudsSpeaker("Binx", "Rachel", "Binx", "female");
	speakers["Casey"]		= CloudsSpeaker("Casey", "Casey", "Reas", "male");
	speakers["Chris"]		= CloudsSpeaker("Chris", "Chris", "Sugrue", "female");
	speakers["Diederick"]	= CloudsSpeaker("Diederick", "Diederick", "Huijbers", "male");
	speakers["Elliot"]		= CloudsSpeaker("Elliot", "Elliot", "Woods", "male");
	speakers["Elliot_UK"]	= CloudsSpeaker("Elliot_UK", "Elliot", "Woods", "male");
	speakers["Fernanda"]	= CloudsSpeaker("Fernanda", "Fernanda", "Viegas", "female");
	speakers["Golan"]		= CloudsSpeaker("Golan", "Golan", "Levin", "male");
	speakers["Golan_AMA"]	= CloudsSpeaker("Golan_AMA", "Golan", "Levin", "male");
	speakers["Greg"]		= CloudsSpeaker("Greg", "Greg", "Borenstein", "male");
	speakers["JTNimoy"]		= CloudsSpeaker("JTNimoy", "Josh", "Nimoy", "male");
	speakers["Javier"]		= CloudsSpeaker("Javier", "Javier", "", "male");
	speakers["Jen"]			= CloudsSpeaker("Jen", "Jen", "Lowe", "female");
	speakers["Jer"]			= CloudsSpeaker("Jer", "Jer", "Thorpe", "male");
	speakers["Joel"]		= CloudsSpeaker("Joel", "Joel", "Lewis", "male");
	speakers["Julia"]		= CloudsSpeaker("Julia", "Julia", "Kaganskiy", "female");
	speakers["Julian"]		= CloudsSpeaker("Julian", "Julian", "Oliver", "male");
	speakers["Karolina"]	= CloudsSpeaker("Karolina", "Karolina", "Sobecka", "female");
	speakers["Karsten"]		= CloudsSpeaker("Karsten", "Karsten", "Schmidt", "male");
	speakers["Kevin"]		= CloudsSpeaker("Kevin", "Kevin", "Slavin", "male");
	speakers["Kyle_CH"]		= CloudsSpeaker("Kyle_CH", "Kyle", "Chayka", "male");
	speakers["Kyle_MC"]		= CloudsSpeaker("Kyle_MC", "Kyle", "McDonald", "male");
	speakers["Lauren"]		= CloudsSpeaker("Lauren", "Lauren", "McCarthy", "female");
	speakers["Lindsay"]		= CloudsSpeaker("Lindsay", "Lindsay", "Howard", "female");
	speakers["Maeda"]		= CloudsSpeaker("John", "John", "Maeda", "male");
	speakers["Marcus"]		= CloudsSpeaker("Marcus", "Marcus", "Wendt", "male");
	speakers["Marius"]		= CloudsSpeaker("Marius", "Marius", "Watz", "male");
	speakers["Martin"]		= CloudsSpeaker("Martin", "Martin", "Wattenberg", "male");
	speakers["Nervous_Jesse"]		= CloudsSpeaker("Jesse", "Louis-Rosenberg", "", "male");
	speakers["Nervous_Jessica"]		= CloudsSpeaker("Jessica", "Jessica", "Rosencrantz", "female");
	speakers["Paola"]		= CloudsSpeaker("Paola", "Paola", "Antonelli", "female");
	speakers["Philip"]		= CloudsSpeaker("Philip", "Philip", "Whitfield", "male");
	speakers["Ramsey"]		= CloudsSpeaker("Ramsey", "Ramsey", "Nasser", "male");
	speakers["Regine"]		= CloudsSpeaker("Regine", "Regine", "Debatty", "female");
	speakers["Shantell"]	= CloudsSpeaker("Shantell", "Shantell", "Martin", "female");
	speakers["Shiffman"]	= CloudsSpeaker("Daniel", "Daniel", "Shiffman", "male");
	speakers["Sophy"]		= CloudsSpeaker("Sophy", "Sophy", "Yuditskaya", "female");
	speakers["Theo"]		= CloudsSpeaker("Theo", "Theo", "Watson", "male");
	speakers["Vera"]		= CloudsSpeaker("Vera", "Vera", "Glahn", "female");
	speakers["Zach"]		= CloudsSpeaker("Zach", "Zach", "Lieberman", "male");

	//all people
//	map<string,CloudsSpeaker>::iterator it;
//	for(it = speakers.begin(); it != speakers.end(); it++){
//		cout << it->second.firstName + " "  + it->second.lastName << endl;
//	}
	
	//export xml template

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

//	<clouds>
//	<person id="Lauren">
//	<first>Lauren</first>
//	<last>McCarthy</last>
//	<title></title>
//	<twitter>@lau</twitter>
//	<location>Brooklyn, NY</location>
//	<byline1></byline1>
//	<byline2></byline2>
//	<resume> ... </resume>
//	</person>
//	</clouds>
	
	
}


