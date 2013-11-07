//
//  CloudsWebSocketConnection.cpp
//  CloudsStoryEngine
//
//  Created by James George on 10/15/13.
//
//

#include "CloudsWebSocketConnection.h"
CloudsWebSocketConnection::CloudsWebSocketConnection(){
	
}

void CloudsWebSocketConnection::setup(){
	
	ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    options.port = 9093;
    options.protocol = "of-protocol";
    options.bBinaryProtocol = true;
    
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
	
	char message[1024];
	sprintf(message, "{ \"clip\" : { \"name\" : \"%s\", \"id\" : \"%s\", \"duration\" : %f } }",
			args.chosenClip.person.c_str(),
			args.chosenClip.getLinkName().c_str(),
			args.chosenClip.getDuration() );
	server.send( message );
	cout << " message " << message << endl;
}

void CloudsWebSocketConnection::visualSystemBegan(CloudsVisualSystemEventArgs& args){
	
}

void CloudsWebSocketConnection::visualSystemEnded(CloudsVisualSystemEventArgs& args){
	
}

void CloudsWebSocketConnection::questionAsked(CloudsQuestionEventArgs& args){
	
}

void CloudsWebSocketConnection::topicChanged(CloudsTopicEventArgs& args){
	char message[1024];
	sprintf(message, "{ \"topic\" : \"%s\" }", args.topic.c_str() );
	server.send( message );
	cout << " message " << message << endl;
}

void CloudsWebSocketConnection::preRollRequested(CloudsPreRollEventArgs& args){
	
}
