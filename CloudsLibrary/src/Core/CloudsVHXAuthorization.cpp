
#include "CloudsVHXAuthorization.h"
#include "CloudsGlobal.h"

string CloudsVHXAuthorization::userpwd = "";
string CloudsVHXAuthorization::clientsecret = "";
CloudsVHXAuthorization::CloudsVHXAuthorization(){
    if (userpwd.size() == 0) {
        // Load and decrypt the user password from file.
        ofBuffer buffer = ofBufferFromFile(GetCloudsDataPath() + "vhx/userpwd.txt");
        userpwd = buffer.getFirstLine();
    }
}

void CloudsVHXAuthorization::setup(){

    ssl.setup();
    ssl.setOpt(CURLOPT_CAINFO, ofToDataPath(GetCloudsDataPath() + "vhx/cacert.pem"));
    ssl.setURL("https://api.vhx.tv/oauth/codes");
//    ssl.setOpt(CURLOPT_USERPWD, userpwd);
//    ssl.setOpt(CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    
    ssl.addFormField("client_id", "6bc2ceb4437bd4e7d7ea9d1854d2495bac9ecc3ba29d6b566dbb0c2e524bfb07");
    //secret for this app, also keep out of repo
    ssl.addFormField("client_secret", clientsecret);
    
    
    ssl.perform();
    
    string response = ssl.getResponseBody();
    ofLogVerbose("CloudsVHXRequest::threadedFunction") << "Response:" << endl << response;
    cout << "RESPONSE " << response << endl;
}