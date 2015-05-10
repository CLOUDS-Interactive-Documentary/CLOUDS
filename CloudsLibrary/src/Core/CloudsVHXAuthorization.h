#pragma once

#include "ofMain.h"
#include "ofxSSL.h"

class CloudsVHXAuthorization {

  public:
	CloudsVHXAuthorization();
    void setup();
    ofxSSL ssl;
    
  private:
    static string userpwd;
    static string clientsecret;
    
};
