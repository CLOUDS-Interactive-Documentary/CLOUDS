//
//  CloudsVisualSystemPreset.cpp
//  CLOUDS
//
//  Created by James George on 6/19/13.
//
//

#include "CloudsVisualSystemPreset.h"
#include "ofxXmlSettings.h"
#include "CloudsGlobal.h"

CloudsVisualSystemPreset::CloudsVisualSystemPreset(){

	enabledScreen = false;
	enabledOculus = false;
	indefinite = false;
	
	defaultedToFamily = false;
	randomlySelected = false;
	missingContent = false;

//	hasSound = false;
    system = NULL;
    
    soundAllowVO = false;
    soundExcludeVO = false;
    
    interlude = false;
    
	duration = 60;
	introDuration = 0;
	outroDuration = 0;
	
	hasFiles = false;
}

string CloudsVisualSystemPreset::getID(){
	return systemName + "_" + presetName;
}

bool CloudsVisualSystemPreset::hasSound(){
    return soundAllowVO || soundExcludeVO;
}

bool CloudsVisualSystemPreset::enabled(){
    return  enabledScreen || enabledOculus;
}
void CloudsVisualSystemPreset::loadTimeInfo(){
	

	ofxXmlSettings timeInfo;
	string directory = GetCloudsVisualSystemDataPath(systemName) + "Presets/" + trim(presetName);

	if(!ofDirectory(directory).exists()){
		ofLogError() << "Preset " << systemName << " / " << presetName << " has no directory";
		return;
	}
	
	string path = directory + "/TimeInfo.xml";
	if(timeInfo.loadFile(path) ){
		timeInfo.pushTag("timeinfo");
		indefinite = timeInfo.getValue("indefinite", false);
		duration = timeInfo.getValue("duration", 60);
		introDuration = timeInfo.getValue("introDuration", 0);
		outroDuration = timeInfo.getValue("outroDuration", 0);
	}
	else{
		ofLogError() << "Preset " << systemName << " / " << presetName << " has no time info at path " << path << ". creating default";
		timeInfo.addTag("timeinfo");
		timeInfo.pushTag("timeinfo");
		timeInfo.addValue("duration", duration);
		timeInfo.addValue("indefinite", indefinite);
		timeInfo.addValue("introDuration", introDuration);
		timeInfo.addValue("outroDuration", outroDuration);
		timeInfo.saveFile(path);
	}
}

void CloudsVisualSystemPreset::checkHasFiles(){

	hasFiles = ofFile(GetCloudsVisualSystemDataPath(systemName) + "Presets/" + presetName).exists();
}

void CloudsVisualSystemPreset::eraseFiles(){

	string path = GetCloudsVisualSystemDataPath(systemName) + "Presets/" + presetName;
	if(ofFile(path).remove(true)){		
		ofLogWarning("CloudsVisualSystemPreset::eraseFiles") << "Deleted preset at path " << path << endl;
	}
	else{
		ofLogError("CloudsVisualSystemPreset::eraseFiles") << "Delete failed at path " << path << endl;
	}

}
