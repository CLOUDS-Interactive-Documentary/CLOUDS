//
//  CloudsMedia.h
//  CLOUDS
//
//  Created by Elias Zananiri on 2015-05-06.
//
//

#pragma once

#include "ofMain.h"
#ifdef VHX_MEDIA
    #include "CloudsVHXRequest.h"
#endif

class CloudsMedia
{
public:
    CloudsMedia();
    
    bool hasMediaAsset;

    string sourceVideoFilePath;
    
#ifdef VHX_MEDIA
    void fetchVhxSourceUrl();
    void vhxRequestComplete(CloudsVHXEventArgs& args);
    CloudsVHXRequest *vhxRequest;
    string vhxId;
    string vhxSourceVideoUrl;
    unsigned long long vhxTimestamp;
#endif
};
