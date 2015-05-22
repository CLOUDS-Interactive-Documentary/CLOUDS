//
//  CloudsMedia.cpp
//  CLOUDS
//
//  Created by Elias Zananiri on 2015-05-06.
//
//

#include "CloudsMedia.h"

CloudsMedia::CloudsMedia(){
    hasMediaAsset = false;

    sourceVideoFilePath = "";
    
#ifdef VHX_MEDIA
    vhxRequest = NULL;
    vhxId = "";
    vhxSourceVideoUrl = "";
    vhxTimestamp = 0;
#endif
}

#ifdef VHX_MEDIA
void CloudsMedia::fetchVhxSourceUrl(){
    if (vhxId.empty()) {
        // No ID :(
        ofLogError("CloudsClip::fetchVhxSourceUrl") << "VHX ID not set! This shouldn't happen!";
        hasMediaAsset = false;
        return;
    }
    
    // If the VHX url is already set and is recent...
    if (vhxSourceVideoUrl.size() && (ofGetElapsedTimeMillis() - vhxTimestamp) < CloudsVHXUrlTimeLimit) {
        // ...just re-use it.
        //pass along
        CloudsVHXEventArgs args;
        args.success = true;
        args.result = vhxSourceVideoUrl;
        ofNotifyEvent(completeEvent, args, this);

        hasMediaAsset = true;
        return;
    }
    
    if (vhxRequest == NULL) {
        vhxRequest = new CloudsVHXRequest();
        ofAddListener(vhxRequest->completeEvent, this, &CloudsMedia::vhxRequestComplete);
    }
    
    vhxSourceVideoUrl = "";
    vhxRequest->fetchSourceUrl(vhxId);
}

void CloudsMedia::vhxRequestComplete(CloudsVHXEventArgs& args){
    if (args.success) {
        hasMediaAsset = true;
        vhxSourceVideoUrl = args.result;
        vhxTimestamp = ofGetElapsedTimeMillis();
        ofLogVerbose("CloudsClip::vhxRequestComplete") << "Got source video URL " << vhxSourceVideoUrl;
        
        //pass along
        ofNotifyEvent(completeEvent, args, this);
    }
    else{
        ofLogError("CloudsClip::vhxRequestComplete") << "Error returned from vhx request";
    }
    
    if (vhxRequest) {
        ofRemoveListener(vhxRequest->completeEvent, this, &CloudsMedia::vhxRequestComplete);
        delete vhxRequest;
        vhxRequest = NULL;
    }
}
#endif
