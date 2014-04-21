//
//  TonicSample.h
//  CloudsVisualSystemEditor
//
//  Created by Render on 4/20/14.
//
//

#ifndef CloudsVisualSystemEditor_TonicSample_h
#define CloudsVisualSystemEditor_TonicSample_h
#include "ofxTonic.h"

class TonicSample{
public:
    TonicSample(){
//        soundFile = " ";
        playSample = false;
    }
    TonicSample(string _soundFile){
        soundFile = _soundFile;
    
    }
    string soundFile;
    bool playSample;
    Tonic::ControlTrigger soundTrigger;
    
};


#endif
