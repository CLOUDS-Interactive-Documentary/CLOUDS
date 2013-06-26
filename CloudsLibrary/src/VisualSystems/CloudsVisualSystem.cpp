
#include "CloudsVisualSystem.h"
static bool confirmedDataPath = false;
static bool usingDevelopmentFolder = false;
static ofFbo sharedRenderTarget;

ofFbo& CloudsVisualSystem::getSharedRenderTarget(){
    if(!sharedRenderTarget.isAllocated() ||
       sharedRenderTarget.getWidth() != ofGetWidth() ||
       sharedRenderTarget.getHeight() != ofGetHeight())
    {
        sharedRenderTarget.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 4);
    }
    return sharedRenderTarget;
}

CloudsVisualSystem::CloudsVisualSystem(){
	isPlaying = false;
	sharedRenderer = NULL;
}

CloudsVisualSystem::~CloudsVisualSystem(){
	
}

string CloudsVisualSystem::getVisualSystemDataPath(){
    return getDataPath() + "visualsystems/"+getSystemName()+"/";
}

string CloudsVisualSystem::getDataPath()
{
	if(!confirmedDataPath){
		usingDevelopmentFolder = ofDirectory("../../../CloudsData/").exists();
		if(!usingDevelopmentFolder){
			ofDirectory("CloudsData/").create();
		}
		confirmedDataPath = true;
	}
    return usingDevelopmentFolder ? "../../../CloudsData/" : "CloudsData/";
}

bool CloudsVisualSystem::isReleventToKeyword(string keyword){
	return ofContains(relevantKeywords, keyword);
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

void CloudsVisualSystem::setCurrentKeyword(string keyword){
	currentKeyword = keyword;
}

string CloudsVisualSystem::getCurrentKeyword(){
	return currentKeyword;
}

void CloudsVisualSystem::setCurrentTopic(string topic){
	currentTopic = topic;
}

string CloudsVisualSystem::getCurrentTopic(){
	return currentTopic;
}

vector<string>& CloudsVisualSystem::getRelevantKeywords(){
	return relevantKeywords;
}

void CloudsVisualSystem::setRenderer(CloudsRGBDCombinedRender& newRenderer){
	sharedRenderer = &newRenderer;
}

//void CloudsVisualSystem::setCamera(CloudsCamera& camera){
//	sharedCamera = &camera;
//}

void CloudsVisualSystem::setupSpeaker(string speakerFirstName,
									  string speakerLastName,
									  string quoteName)
{
	this->speakerFirstName = speakerFirstName;
	this->speakerLastName = speakerLastName;
	this->quoteName = quoteName;
	hasSpeaker = true;
	
}

void CloudsVisualSystem::speakerEnded(){
	hasSpeaker = false;
}
