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

#include "CloudsVHXEventArgs.h"

//--------------------------------------------------------------
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
        VERIFY_PACKAGE
    };
    
    enum State {
        INACTIVE,
        PURCHASE,
        RENTAL,
        EXPIRED
    };
    
    CloudsVHXAuth();
    ~CloudsVHXAuth();
    
    bool setup();
    void exit();
    
    void update(ofEventArgs& args);

    void clearTokens();
    void requestToken();
    void refreshToken();
    void requestCode();
    void linkCode();
    void verifyPackage();

    ofEvent<CloudsVHXEventArgs> requestTokenComplete;
    ofEvent<CloudsVHXEventArgs> refreshTokenComplete;
    ofEvent<CloudsVHXEventArgs> requestCodeComplete;
    ofEvent<CloudsVHXEventArgs> linkCodeComplete;
    ofEvent<CloudsVHXEventArgs> verifyPackageComplete;
    
    ofEvent<CloudsVHXEventArgs> codeExpired;
    ofEvent<CloudsVHXEventArgs> packageExpired;
    
    Mode mode;
    State state;
    
protected:
    void threadedFunction();
    
    CloudsVHXEventArgs completeArgs;
    bool bNotifyComplete;
    
private:
    string _keysPath;
    string _tokensPath;
    
    ofxSSL _ssl;
    string _clientId;
    string _clientSecret;

    string _packageId;
    time_t _packageExpiry;
    
    string _accessToken;
    string _refreshToken;
    time_t _tokenExpiry;
    
    string _code;
    time_t _codeExpiry;
    
};
