#pragma once

#include "ofxAudioUnitDSPNode.h"
#include <AudioToolbox/AudioToolbox.h>

// ofxAudioUnitRecorder can record all audio passing through it
// to a file

class ofxAudioUnitRecorder : public ofxAudioUnitDSPNode {
	
public:
	ofxAudioUnitRecorder();
	~ofxAudioUnitRecorder();
	
	// Note: only supported filetype at the moment is .m4a
	bool startRecording(const std::string &filePath);
	void stopRecording();
	
private:
	ExtAudioFileRef _recordFile;
};
