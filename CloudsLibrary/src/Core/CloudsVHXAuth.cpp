//
//  CloudsVHXAuth.cpp
//  vhxTest
//
//  Created by Elias Zananiri on 2015-05-06.
//
//

#include "CloudsVHXAuth.h"
#include "CloudsCrypto.h"

#include "ofxCrypto.h"
#include "ofxJSONElement.h"

CloudsVHXAuth::CloudsVHXAuth()
: ofThread()
, _accessToken("")
, _refreshToken("")
, _tokenExpiry(0)
, _code("")
, _codeExpiry(0)
{
    ofxSSL::appendData = true;

    setup();
}

CloudsVHXAuth::~CloudsVHXAuth()
{
    waitForThread();
    exit();
}

void CloudsVHXAuth::setup()
{
    // Load and decrypt the client info from file.
    //string path = GetCloudsDataPath() + "vhx/client.bin";
    string path = "client.bin";
    ofFile file(path, ofFile::ReadOnly, true);
    if (file.exists()) {
        int numChars = 0;
        file.read((char *)(&numChars), sizeof(int));
        if (numChars > 0) {
            char chars[numChars];
            file.read(&chars[0], sizeof(char) * numChars);
            string encoded;
            encoded.assign(chars, numChars);
            string decoded = ofxCrypto::base64_decode(encoded);
            int pos = decoded.find(CloudsSalt);
            string desalted = decoded.substr(0, pos) + decoded.substr(pos + CloudsSalt.size());
            vector<string> strs = ofSplitString(desalted, "\n");
            _clientId = strs[0];
            _clientSecret = strs[1];
        }
    }
    else {
        ofLogError("CloudsVHXAuth::CloudsVHXAuth") << "Cannot open file at " << path;
    }
    
    _tokenExpiry = 0;
    
    mode = WAITING;
    bNotifyComplete = false;
    ofAddListener(ofEvents().update, this, &CloudsVHXAuth::update);
}

void CloudsVHXAuth::exit()
{
    ofRemoveListener(ofEvents().update, this, &CloudsVHXAuth::update);
}

void CloudsVHXAuth::update(ofEventArgs& args)
{
    if (bNotifyComplete) {
        if (mode == REQUEST_TOKEN) {
            ofNotifyEvent(requestTokenComplete, completeArgs);
        }
        else if (mode == REFRESH_TOKEN) {
            ofNotifyEvent(refreshTokenComplete, completeArgs);
        }
        else if (mode == REQUEST_CODE) {
            ofNotifyEvent(requestCodeComplete, completeArgs);
        }
        else if (mode == LINK_CODE) {
            ofNotifyEvent(linkCodeComplete, completeArgs);
        }
        mode = WAITING;
        bNotifyComplete = false;
    }
    
    if (mode == WAITING && _tokenExpiry && ofGetElapsedTimef() > _tokenExpiry) {
        if (_refreshToken.empty()) {
            requestToken();
        }
        else {
            refreshToken();
        }
    }
}

void CloudsVHXAuth::requestToken()
{
    if (isThreadRunning()) {
        ofLogError("CloudsVHXAuth::requestToken") << "Thread is already running with mode " << mode;
        return;
    }
    if (mode != WAITING) {
        ofLogError("CloudsVHXAuth::requestToken") << "Mode " << mode << " is still active";
        return;
    }
    
    mode = REQUEST_TOKEN;
    startThread();
}

void CloudsVHXAuth::refreshToken()
{
    if (isThreadRunning()) {
        ofLogError("CloudsVHXAuth::refreshToken") << "Thread is already running with mode " << mode;
    }
    if (mode != WAITING) {
        ofLogError("CloudsVHXAuth::refreshToken") << "Mode " << mode << " is still active";
        return;
    }
    if (_refreshToken.empty()) {
        ofLogError("CloudsVHXAuth::refreshToken") << "No refresh token set!";
        return;
    }
    
    mode = REFRESH_TOKEN;
    startThread();
}

void CloudsVHXAuth::requestCode()
{
    if (isThreadRunning()) {
        ofLogError("CloudsVHXAuth::requestCode") << "Thread is already running with mode " << mode;
    }
    if (mode != WAITING) {
        ofLogError("CloudsVHXAuth::requestCode") << "Mode " << mode << " is still active";
        return;
    }
    
    mode = REQUEST_CODE;
    startThread();
}

void CloudsVHXAuth::linkCode()
{
    if (isThreadRunning()) {
        ofLogError("CloudsVHXAuth::linkCode") << "Thread is already running with mode " << mode;
    }
    if (mode != WAITING) {
        ofLogError("CloudsVHXAuth::linkCode") << "Mode " << mode << " is still active";
        return;
    }
    if (_code.empty()) {
        ofLogError("CloudsVHXAuth::linkCode") << "No code set!";
        return;
    }
    
    mode = LINK_CODE;
    startThread();
}

void CloudsVHXAuth::threadedFunction()
{
    if (mode == REQUEST_TOKEN || mode == REFRESH_TOKEN) {
        _ssl.setup();
        _ssl.setURL("https://api.vhx.tv/oauth/token");
        if (mode == REQUEST_TOKEN) {
            _ssl.addFormField("client_id", _clientId);
            _ssl.addFormField("client_secret", _clientSecret);
            _ssl.addFormField("grant_type", "client_credentials");
        }
        else {
            _ssl.addFormField("refresh_token", _refreshToken);
            _ssl.addFormField("grant_type", "refresh_token");
        }
        
        _ssl.perform();
        
        string response = _ssl.getResponseBody();
        ofLogNotice("CloudsVHXAuth::threadedFunction") << "Response:" << endl << response;
        
        completeArgs.success = false;
        
        ofxJSONElement json;
        if (json.parse(response)) {
            if (json.isMember("access_token")) {
                _accessToken = json["access_token"].asString();
                _refreshToken = json["refresh_token"].asString();
                _tokenExpiry = ofGetElapsedTimef() + json["expires_in"].asFloat();
                
                completeArgs.success = true;
                completeArgs.result = _accessToken;
            }
            else {
                ofLogError("CloudsVHXAuth::threadedFunction") << "Unexpected JSON format:" << endl << response;
            }
        }
        else {
            ofLogError("CloudsVHXAuth::threadedFunction") << "Unable to parse JSON:" << endl << response;
        }
        
        _ssl.clear();
        
        bNotifyComplete = true;
    }
    else if (mode == REQUEST_CODE) {
        _ssl.setup();
        _ssl.setURL("https://api.vhx.tv/oauth/codes");
        _ssl.addFormField("client_id", _clientId);
        _ssl.addFormField("client_secret", _clientSecret);

        _ssl.perform();
        
        string response = _ssl.getResponseBody();
        ofLogNotice("CloudsVHXAuth::threadedFunction") << "Response:" << endl << response;
        
        completeArgs.success = false;
        
        ofxJSONElement json;
        if (json.parse(response)) {
            if (json.isMember("code")) {
                _code = json["code"].asString();
                _codeExpiry = ofGetElapsedTimef() + json["expires_in"].asFloat();
                
                completeArgs.success = true;
                completeArgs.result = _code;
            }
            else {
                ofLogError("CloudsVHXAuth::threadedFunction") << "Unexpected JSON format:" << endl << response;
            }
        }
        else {
            ofLogError("CloudsVHXAuth::threadedFunction") << "Unable to parse JSON:" << endl << response;
        }
        
        _ssl.clear();
        
        bNotifyComplete = true;
    }
    else if (mode == LINK_CODE) {
        stringstream ss;
        ss << "http://www.vhx.tv/activate/clouds";
        ss << "?client_id=" << _clientId;
        ss << "&code=" << _code;
        ofLaunchBrowser(ss.str());
        
        ss.str("");
        ss << "https://api.vhx.tv/oauth/codes/" << _code;
        ss << "?client_id=" << _clientId;
        ss << "&client_secret=" << _clientSecret;
        
        completeArgs.success = false;

        bool bWaitForLink = true;
        while (bWaitForLink) {
            _ssl.setup();
            _ssl.setURL(ss.str());
            
            _ssl.perform();
            
            string response = _ssl.getResponseBody();
            ofLogNotice("CloudsVHXAuth::threadedFunction") << "Response:" << endl << response;
            
            ofxJSONElement json;
            if (json.parse(response)) {
                if (json.isMember("access_token")) {
                    _accessToken = json["access_token"].asString();
                    _refreshToken = json["refresh_token"].asString();
                    _tokenExpiry = ofGetElapsedTimef() + json["expires_in"].asFloat();
                    
                    completeArgs.success = true;
                    completeArgs.result = _accessToken;

                    bWaitForLink = false;
                }
                else {
                    ofLogVerbose("CloudsVHXAuth::threadedFunction") << "Unexpected JSON result:" << endl << response;
                }
            }
            else {
                ofLogError("CloudsVHXAuth::threadedFunction") << "Unable to parse JSON:" << endl << response;
                
                bWaitForLink = false;
            }
            
            _ssl.clear();
            
            sleep(10);
        }
        
        bNotifyComplete = true;
    }
    else {
        ofLogError("CloudsVHXAuth::threadedFunction") << "Mode " << mode << " is unrecongized!";
    }
}
