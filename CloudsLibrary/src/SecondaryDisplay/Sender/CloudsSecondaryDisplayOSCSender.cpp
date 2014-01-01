//
//  CloudsSecondaryDisplayOSCSender.cpp
//  CloudsStoryEngine
//
//  Created by James George on 12/23/13.
//
//

#include "CloudsSecondaryDisplayOSCSender.h"

CloudsSecondaryDisplayOSCSender::CloudsSecondaryDisplayOSCSender(){
	
}

void CloudsSecondaryDisplayOSCSender::setup(){
	sender.setup("localhost", 12346);
}

void CloudsSecondaryDisplayOSCSender::actCreated(CloudsActEventArgs& args){
	
}

void CloudsSecondaryDisplayOSCSender::actBegan(CloudsActEventArgs& args){
	ofxOscMessage m;
	m.setAddress("/actBegan");
	m.addStringArg("began");
	sender.sendMessage(m);
}

void CloudsSecondaryDisplayOSCSender::actEnded(CloudsActEventArgs& args){
	ofxOscMessage m;
	m.setAddress("/actEnded");
	m.addStringArg("ended");
	sender.sendMessage(m);
}

void CloudsSecondaryDisplayOSCSender::clipBegan(CloudsClipEventArgs& args){
	
}

void CloudsSecondaryDisplayOSCSender::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	
}

void CloudsSecondaryDisplayOSCSender::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	
}

void CloudsSecondaryDisplayOSCSender::questionAsked(CloudsQuestionEventArgs& args){
	
}

void CloudsSecondaryDisplayOSCSender::topicChanged(CloudsTopicEventArgs& newTopic){
	
}

void CloudsSecondaryDisplayOSCSender::preRollRequested(CloudsPreRollEventArgs& clip){
	
}

void CloudsSecondaryDisplayOSCSender::sendClip(CloudsClip& clip){
	
	ofxOscMessage m;
	m.setAddress("/clip");
	m.addStringArg(clip.person);//final cut person id
	m.addStringArg(clip.getID());//clip id
	m.addFloatArg(clip.getDuration());//duraiton
	m.addStringArg(currentTopic); //topic
	m.addStringArg(clip.projectExampleTitle); //example
	//m.addStringArg(lastQuestionAsked); //question
	if(clip.hasQuestion()){
		m.addStringArg( clip.getQuestionForTopic(clip.getTopicsWithQuestions()[0]) );
	}
	else{
		m.addStringArg(lastQuestionAsked); //question
	}
	sender.sendMessage(m);
}
