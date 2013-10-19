//
//  CloudsVisualSystemPreset.cpp
//  CLOUDS
//
//  Created by James George on 6/19/13.
//
//

#include "CloudsVisualSystemPreset.h"
#include "CloudsVisualSystem.h"
#include "ofxXmlSettings.h"
#include "CloudsGlobal.h"

CloudsVisualSystemPreset::CloudsVisualSystemPreset(){
	system = NULL;
	enabled = true;
	indefinite = true;
	
	defaultedToFamily = false;
	randomlySelected = false;

	duration = 60;
	introDuration = 0;
	outroDuration = 0;
}

string CloudsVisualSystemPreset::getID(){
	return systemName + "_" + presetName;
}

void CloudsVisualSystemPreset::loadTimeInfo(){
	ofxXmlSettings timeInfo;
	string path = CloudsVisualSystem::getVisualSystemDataPath(systemName) + "Presets/" + presetName + "/TimeInfo.xml";
	if(timeInfo.loadFile(path) ){
		timeInfo.pushTag("timeinfo");
		indefinite = timeInfo.getValue("indefinite", true);
		duration = timeInfo.getValue("duration", 60);
		introDuration = timeInfo.getValue("introDuration", 0);
		outroDuration = timeInfo.getValue("outroDuration", 0);
	}
	else{
		ofLogError() << "Preset " << systemName << " / " << presetName << " has no time info at path" << path << ". creating default";
		if(system != NULL){
			timeInfo.addTag("timeinfo");
			timeInfo.pushTag("timeinfo");
			timeInfo.addValue("duration", duration);
			timeInfo.addValue("indefinite", indefinite);
			timeInfo.addValue("introDuration", introDuration);
			timeInfo.addValue("outroDuration", outroDuration);
			timeInfo.saveFile(path);
		}
	}
}
