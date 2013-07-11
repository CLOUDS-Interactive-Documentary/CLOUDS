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
	system = NULL;
}

string CloudsVisualSystemPreset::getID(){
	return (system == NULL ? "null" : system->getSystemName() ) + "_" + presetName;
}