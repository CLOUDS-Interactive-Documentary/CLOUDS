//
//  CloudsVHXAuth.h
//  vhxTest
//
//  Created by Elias Zananiri on 2015-05-06.
//
//

#pragma once

#include "ofMain.h"
#include "ofxSSL.h"

class CloudsVHXEventArgs
: ofEventArgs
{
public:
    CloudsVHXEventArgs()
    : ofEventArgs(), success(false), result("")
    {}
    
    bool success;
    string result;
};

class CloudsVHXAuth
: public ofThread
{
public:
    enum Mode {
        WAITING,
        REQUEST_TOKEN,
        REFRESH_TOKEN,
        REQUEST_CODE,
        LINK_CODE,
    };
    
    CloudsVHXAuth();
    ~CloudsVHXAuth();
    
    void setup();
    void exit();
    
    void update(ofEventArgs& args);
    
    void requestToken();
    void refreshToken();
    void requestCode();
    void linkCode();
    
    ofEvent<CloudsVHXEventArgs> requestTokenComplete;
    ofEvent<CloudsVHXEventArgs> refreshTokenComplete;
    ofEvent<CloudsVHXEventArgs> requestCodeComplete;
    ofEvent<CloudsVHXEventArgs> linkCodeComplete;
    
protected:
    void threadedFunction();
    
    Mode mode;
    CloudsVHXEventArgs completeArgs;
    bool bNotifyComplete;
    
private:
    ofxSSL _ssl;
    string _clientId;
    string _clientSecret;
    
    string _accessToken;
    string _refreshToken;
    float _tokenExpiry;
    string _code;
    float _codeExpiry;
};
