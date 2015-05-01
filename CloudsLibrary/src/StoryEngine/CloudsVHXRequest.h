//
//  CloudsVHXRequest.h
//  CLOUDS
//
//  Created by Elias Zananiri on 2015-04-21.
//
//

#pragma once

#include "ofMain.h"
#include "ofxSSL.h"

extern unsigned long long CloudsVHXUrlTimeLimit;

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

class CloudsVHXRequest
    : public ofThread
{
public:
    CloudsVHXRequest();
    ~CloudsVHXRequest();
    
    void fetchSourceUrl(const string& vhxId);
    
    ofEvent<CloudsVHXEventArgs> completeEvent;
    
protected:
    void update(ofEventArgs& args);
    void threadedFunction();

    ofxSSL ssl;
    string url;
    
    CloudsVHXEventArgs completeArgs;
    bool bNotifyComplete;

private:
    static string userpwd;
};
