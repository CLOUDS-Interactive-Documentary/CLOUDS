//
//  CloudsVisualization.cpp
//  CLOUDS
//
//  Created by James George on 4/23/13.
//
//

#include "CloudsVisualSystem.h"

CloudsVisualSystem::CloudsVisualSystem(){
	isPlaying = false;
}

CloudsVisualSystem::~CloudsVisualSystem(){
	
}

bool CloudsVisualSystem::isReleventToKeyword(string keyword){
	return false;
}

void CloudsVisualSystem::setup(){
	ofAddListener(ofEvents().exit, this, &CloudsVisualSystem::exit);
}

void CloudsVisualSystem::playSystem(){
	if(!isPlaying){
		ofRegisterMouseEvents(this);
		ofRegisterKeyEvents(this);
		ofAddListener(ofEvents().update, this, &CloudsVisualSystem::update);
		ofAddListener(ofEvents().draw, this, &CloudsVisualSystem::draw);
		
		isPlaying = true;
		
		begin();
	}
}

void CloudsVisualSystem::stopSystem(){
	if(isPlaying){
		
		end();
		
		ofUnregisterMouseEvents(this);
		ofUnregisterKeyEvents(this);
		ofRemoveListener(ofEvents().update, this, &CloudsVisualSystem::update);
		ofRemoveListener(ofEvents().draw, this, &CloudsVisualSystem::draw);
		
		isPlaying = false;
	}
}

float CloudsVisualSystem::getSecondsRemaining(){
	return secondsRemaining;
}

void CloudsVisualSystem::setSecondsRemaining(float seconds){
	secondsRemaining = seconds;
}
