//
//  CloudsVHXRequest.cpp
//  CLOUDS
//
//  Created by Elias Zananiri on 2015-04-21.
//
//

#include "ofxCrypto.h"
#include "ofxJSONElement.h"

#include "CloudsCrypto.h"
#include "CloudsVHXRequest.h"
#include "CloudsGlobal.h"
#include "CloudsClip.h"

unsigned long long CloudsVHXUrlTimeLimit = 10 * 60 * 1000;  // 10 mins

string CloudsVHXRequest::userpwd = "";

CloudsVHXRequest::CloudsVHXRequest()
    : ofThread()
    , bNotifyComplete(false)
{
    ofxSSL::appendData = true;
    
    if (userpwd.size() == 0) {
        // Load and decrypt the user password from file.
        string path = GetCloudsDataPath(true) + "vhx/usrpwd.bin";
        ofFile file(path, ofFile::ReadOnly, true);
        if (!file.exists()) {
            ofLogError("CloudsVHXRequest::CloudsVHXRequest") << "Cannot open file at " << path;
            return false;
        }
        
        int numChars = 0;
        file.read((char *)(&numChars), sizeof(int));
        if (numChars > 0) {
            char chars[numChars];
            file.read(&chars[0], sizeof(char) * numChars);
            string encoded;
            encoded.assign(chars, numChars);
            string decoded = ofxCrypto::base64_decode(encoded);
            int pos = decoded.find(CloudsSalt);
            userpwd = decoded.substr(0, pos) + decoded.substr(pos + CloudsSalt.size());
        }
    }
    
    ofAddListener(ofEvents().update, this, &CloudsVHXRequest::update);
}

CloudsVHXRequest::~CloudsVHXRequest()
{
    ofRemoveListener(ofEvents().update, this, &CloudsVHXRequest::update);
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
    
    bNotifyComplete = false;
    startThread();
}

void CloudsVHXRequest::update(ofEventArgs& args)
{
    if (bNotifyComplete) {
        ofNotifyEvent(completeEvent, completeArgs);
        bNotifyComplete = false;
    }
}

void CloudsVHXRequest::threadedFunction()
{
    ofLogVerbose("CloudsVHXRequest::threadedFunction") << "Fetching package video with url " << url;
    
    ssl.setup();
    ssl.setURL(url);
    ssl.setOpt(CURLOPT_CAINFO, ofToDataPath(GetCloudsDataPath(true) + "vhx/cacert.pem"));
    ssl.setOpt(CURLOPT_USERPWD, userpwd);
    ssl.setOpt(CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    
    ssl.perform();
    
    string response = ssl.getResponseBody();
    ofLogVerbose("CloudsVHXRequest::threadedFunction") << "Response:" << endl << response;
    
    completeArgs.success = false;
    
    ofxJSONElement json;
    if (json.parse(response) && json.isArray()) {
        const ofxJSONElement& element = json[0];
        if (!element.isNull() && element.isMember("_links")) {
            completeArgs.success = true;
            completeArgs.result = element["_links"]["source"]["href"].asString();
        }
        else {
            ofLogError("CloudsVHXRequest::threadedFunction") << "Unexpected JSON format:" << endl << response;
        }
    }
    else {
        ofLogError("CloudsVHXRequest::threadedFunction") << "Unable to parse JSON:" << endl << response;
    }
    
    ssl.clear();

    bNotifyComplete = true;
}