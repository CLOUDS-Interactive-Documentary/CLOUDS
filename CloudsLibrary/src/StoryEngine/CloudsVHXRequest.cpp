//
//  CloudsVHXRequest.cpp
//  CLOUDS
//
//  Created by Elias Zananiri on 2015-04-21.
//
//

#include "ofxJSONElement.h"

#include "CloudsVHXRequest.h"
#include "CloudsGlobal.h"
#include "CloudsClip.h"

unsigned long long CloudsVHXUrlTimeLimit = 10 * 60 * 1000;  // 10 mins

string CloudsVHXRequest::userpwd = "";

CloudsVHXRequest::CloudsVHXRequest()
    : ofThread()
{
    if (userpwd.size() == 0) {
        // Load and decrypt the user password from file.
        ofBuffer buffer = ofBufferFromFile(GetCloudsDataPath() + "vhx/userpwd.txt");
        userpwd = buffer.getFirstLine();
    }
}

CloudsVHXRequest::~CloudsVHXRequest()
{
    
}

void CloudsVHXRequest::fetchSourceUrl(const string& vhxId)
{
    if (isThreadRunning()) {
        ofLogError("CloudsVHXRequest::requestSourceURL") << "Thread is already running, abort!";
        return;
    }
    
    if (vhxId == "") {
        ofLogError("CloudsVHXRequest::requestSourceURL") << "Clip has no VHX ID set, abort!";
        return;
    }
    
    url = "https://api.vhx.tv/videos/" + vhxId + "/files";
    
    startThread();
}

void CloudsVHXRequest::threadedFunction()
{
    ofLogVerbose("CloudsVHXRequest::threadedFunction") << "Fetching package video with url " << url;
    
    ssl.setup();
    ssl.setURL(url);
    ssl.setOpt(CURLOPT_CAINFO, ofToDataPath(GetCloudsDataPath() + "vhx/cacert.pem"));
    ssl.setOpt(CURLOPT_USERPWD, userpwd);
    ssl.setOpt(CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    
    ssl.perform();
    
    string response = ssl.getResponseBody();
    ofLogVerbose("CloudsVHXRequest::threadedFunction") << "Response:" << endl << response;
    
    CloudsVHXEventArgs args;
    args.success = false;
    
    ofxJSONElement json;
    if (json.parse(response) && json.isArray()) {
        const ofxJSONElement& element = json[0];
        if (!element.isNull() && element.isMember("_links")) {
            args.success = true;
            args.result = element["_links"]["source"]["href"].asString();
        }
        else {
            ofLogError("CloudsVHXRequest::threadedFunction") << "Unexpected JSON format " << response;
        }
    }
    else {
        ofLogError("CloudsVHXRequest::threadedFunction") << "Unable to parse JSON " << response;
    }
    
    ssl.clear();

    ofNotifyEvent(completeEvent, args);
}