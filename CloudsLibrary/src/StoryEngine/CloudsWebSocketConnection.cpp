//
//  CloudsWebSocketConnection.cpp
//  CloudsStoryEngine
//
//  Created by James George on 10/15/13.
//
//

#include "CloudsWebSocketConnection.h"
#include "CloudsGlobal.h"
CloudsWebSocketConnection::CloudsWebSocketConnection(){
	
}

void CloudsWebSocketConnection::setup(){
	
	ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    options.port = 9093;
    options.protocol = "of-protocol";
    options.bBinaryProtocol = true;
    
    options.documentRoot = ofToDataPath(GetCloudsDataPath() + "secondaryDisplay/web");
    cout << options.documentRoot << endl;
    options.sslCertPath = ofToDataPath(GetCloudsDataPath() + "secondaryDisplay/ssl/libwebsockets-test-server.pem");
    options.sslKeyPath = ofToDataPath(GetCloudsDataPath() + "secondaryDisplay/ssl/libwebsockets-test-server.key.pem");

    
    
    
    bool connected = server.setup( options );
    if(connected){
		cout << "Websockets connected!!" << endl;
	}
	
    // this adds your app as a listener for the server
    server.addListener(this);
    
    // setup message queue
    
    
//    messages.push_back("WebSocket server setup at "+ofToString( server.getPort() ) + ( server.usingSSL() ? " with SSL" : " without SSL") );
    
}


#pragma mark websockets

void CloudsWebSocketConnection::onConnect( ofxLibwebsockets::Event& args ){
	ofLogNotice("CloudsWebSocketConnection::onConnect");
}

void CloudsWebSocketConnection::onOpen( ofxLibwebsockets::Event& args ){
}

void CloudsWebSocketConnection::onClose( ofxLibwebsockets::Event& args ){
	ofLogNotice("CloudsWebSocketConnection::onClose");
}

void CloudsWebSocketConnection::onIdle( ofxLibwebsockets::Event& args ){
	
}

void CloudsWebSocketConnection::onMessage( ofxLibwebsockets::Event& args ){
	
}

void CloudsWebSocketConnection::onBroadcast( ofxLibwebsockets::Event& args ){
	
}

#pragma mark CloudsEvents
void CloudsWebSocketConnection::actCreated(CloudsActEventArgs& args){
	ofLogNotice("CloudsWebSocketConnection::actCreated");
}

void CloudsWebSocketConnection::actBegan(CloudsActEventArgs& args){
	ofLogNotice("CloudsWebSocketConnection::actBegan");
}

void CloudsWebSocketConnection::actEnded(CloudsActEventArgs& args){
	
}

void CloudsWebSocketConnection::clipBegan(CloudsClipEventArgs& args){
	ofLogNotice("CloudsWebSocketConnection::clipBegan");
	string example = "";
	
//	if( args.chosenClip.hasSpecialKeyword("#example") ){
	if( args.chosenClip.hasProjectExample ){
		example = args.chosenClip.example.title;
	}
	
	char message[1024];
    //{"clip" : { "name" : "%s", "id" : "%s", "duration" : %f,"topic" : "%s" "example" : "%s" } }"
	sprintf(message,
			"{ \"clip\" : \
				{ \"name\" : \"%s\", \
				\"id\" : \"%s\", \
				\"duration\" : %f, \
				\"topic\" : \"%s\", \
				\"example\" : \"%s\", \
				\"question\" : \"%s\" \
			} }",
			args.chosenClip.person.c_str(),
			args.chosenClip.getLinkName().c_str(),
			args.chosenClip.getDuration(),
			args.currentTopic.c_str(),
			args.chosenClip.hasSpecialKeyword("example") ? "Drawn" : "",
			"How do you sketch with code?");
//			example.c_str(),
//			lastQuestionAsked.c_str());
	server.send( message );
	
	cout << " message " << message << endl;
}

void CloudsWebSocketConnection::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	
}

void CloudsWebSocketConnection::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	
}

void CloudsWebSocketConnection::questionAsked(CloudsQuestionEventArgs& args){
	lastQuestionAsked = args.question;
}

void CloudsWebSocketConnection::topicChanged(CloudsTopicEventArgs& args){
//	char message[1024];
//	sprintf(message, "{ \"topic\" : \"%s\" }", args.topic.c_str() );
//	server.send( message );
//	cout << " message " << message << endl;
	 
//	currentTopic = args.topic;
	
}

void CloudsWebSocketConnection::preRollRequested(CloudsPreRollEventArgs& args){
	
}
