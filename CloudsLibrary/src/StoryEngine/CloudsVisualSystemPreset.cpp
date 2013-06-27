//
//  CloudsVisualSystemPreset.cpp
//  CLOUDS
//
//  Created by James George on 6/19/13.
//
//

#include "CloudsVisualSystemPreset.h"
#include "CloudsVisualSystem.h"

CloudsVisualSystemPreset::CloudsVisualSystemPreset(){
	
}

string CloudsVisualSystemPreset::getID(){
	return system->getSystemName() + "_" + presetName;
}