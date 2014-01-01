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
    ofxXmlSettings linksXML;
    
    if(!linksXML.loadFile(GetCloudsDataPath()+"secondaryDisplay/" + "OSC/OSCSettings.xml")){
        ofSystemAlertDialog("UNABLE TO LOAD LINKS! do not proceed");
		return;
	}
	
    linksXML.pushTag("OSC");
    string hostname;
    int port;
    hostname = linksXML.getValue("hostname", "");
    if (hostname == "") {
        ofLogError()<<"Check OSCSettings.xml in CloudsData/SecondaryDisplay/OSC"<<endl;
        return;
    }
    
    port = linksXML.getValue("port", -1);
    if (port == -1) {
        ofLogError()<<"Check OSCSettings.xml in CloudsData/SecondaryDisplay/OSC"<<endl;
        return;
    }

    cout<<"Setting up OSC Sender with IP: "<<hostname<<" and Port : "<<port<<endl;
    sender.setup(hostname, port);
    
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
    ofxOscMessage m;
	m.setAddress("/clip");
    
	m.addStringArg(args.chosenClip.person);//final cut person id
	m.addStringArg(args.chosenClip.getID());//clip id
	m.addFloatArg(args.chosenClip.getDuration());//duraiton
	m.addStringArg(currentTopic); //topic
	m.addStringArg(args.chosenClip.projectExampleTitle); //example
	//m.addStringArg(lastQuestionAsked); //question
	if(args.chosenClip.hasQuestion()){
		m.addStringArg( args.chosenClip.getQuestionForTopic(args.chosenClip.getTopicsWithQuestions()[0]) );
	}
	else{
		m.addStringArg(lastQuestionAsked); //question
	}
	sender.sendMessage(m);
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
