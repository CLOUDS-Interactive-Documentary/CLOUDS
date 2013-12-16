//
//  CloudsWebSocketConnection.h
//  CloudsStoryEngine
//
//  Created by James George on 10/15/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxLibwebsockets.h"
#include "CloudsEvents.h"

class CloudsWebSocketConnection {
  public:
	CloudsWebSocketConnection();
	void setup();
	
	ofxLibwebsockets::Server server;
	
	void onConnect( ofxLibwebsockets::Event& args );
	void onOpen( ofxLibwebsockets::Event& args );
	void onClose( ofxLibwebsockets::Event& args );
	void onIdle( ofxLibwebsockets::Event& args );
	void onMessage( ofxLibwebsockets::Event& args );
	void onBroadcast( ofxLibwebsockets::Event& args );

	
	void actCreated(CloudsActEventArgs& args);
	void actBegan(CloudsActEventArgs& args);
	void actEnded(CloudsActEventArgs& args);
	void clipBegan(CloudsClipEventArgs& args);
	void visualSystemBegan(CloudsVisualSystemEventArgs& args);
	void visualSystemEnded(CloudsVisualSystemEventArgs& args);
    void questionAsked(CloudsQuestionEventArgs& args);
	void topicChanged(CloudsTopicEventArgs& newTopic);
    void preRollRequested(CloudsPreRollEventArgs& clip);
	
	string lastQuestionAsked;
};