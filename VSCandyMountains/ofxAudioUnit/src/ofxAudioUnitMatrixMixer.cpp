#include "ofxAudioUnitMatrixMixer.h"
#include "ofxAudioUnitUtils.h"

AudioComponentDescription matrixMixerDesc = {
	kAudioUnitType_Mixer,
	kAudioUnitSubType_MatrixMixer,
	kAudioUnitManufacturer_Apple
};

ofxAudioUnitMatrixMixer::ofxAudioUnitMatrixMixer(UInt32 inputBusCount, UInt32 outputBusCount, AudioStreamBasicDescription * inASBD, AudioStreamBasicDescription * outASBD)
{
	initWithLayout(inputBusCount, outputBusCount, inASBD, outASBD);
}

ofxAudioUnitMatrixMixer::ofxAudioUnitMatrixMixer(const ofxAudioUnitMatrixMixer &orig)
{	
	vector<UInt32> origBusCount = orig.getBusCount();
	vector<AudioStreamBasicDescription> origASBDs = orig.getASBDs();
	initWithLayout(origBusCount[0], origBusCount[1], &origASBDs[0], &origASBDs[1]);
}

ofxAudioUnitMatrixMixer& ofxAudioUnitMatrixMixer::operator=(const ofxAudioUnitMatrixMixer &orig)
{
	vector<UInt32> origBusCount = orig.getBusCount();
	vector<AudioStreamBasicDescription> origASBDs = orig.getASBDs();
	initWithLayout(origBusCount[0], origBusCount[1], &origASBDs[0], &origASBDs[1]);
}

void ofxAudioUnitMatrixMixer::initWithLayout(UInt32 inputBusCount, UInt32 outputBusCount, AudioStreamBasicDescription * inASBD, AudioStreamBasicDescription * outASBD)
{
	_desc = matrixMixerDesc;
	_unit = allocUnit(_desc);
	
	OFXAU_PRINT(AudioUnitSetProperty(*_unit,
									 kAudioUnitProperty_ElementCount,
									 kAudioUnitScope_Input,
									 0,
									 &inputBusCount,
									 sizeof(inputBusCount)),
				"setting number of input busses");
	
	OFXAU_PRINT(AudioUnitSetProperty(*_unit,
									 kAudioUnitProperty_ElementCount,
									 kAudioUnitScope_Output,
									 0,
									 &outputBusCount,
									 sizeof(outputBusCount)),
				"setting number of output busses");
	
	if(inASBD) {
		for(int i = 0; i < inputBusCount; i++) {
			OFXAU_PRINT(AudioUnitSetProperty(*_unit,
											 kAudioUnitProperty_StreamFormat,
											 kAudioUnitScope_Input,
											 i,
											 inASBD,
											 sizeof(AudioStreamBasicDescription)),
						"setting matrix mixer input ASBD");
		}
	}
	
	if(outASBD) {
		for(int i = 0; i < outputBusCount; i++) {
			OFXAU_PRINT(AudioUnitSetProperty(*_unit,
											 kAudioUnitProperty_StreamFormat,
											 kAudioUnitScope_Output,
											 i,
											 outASBD,
											 sizeof(AudioStreamBasicDescription)),
						"setting matrix mixer output ASBD");
		}
	}
	
	OFXAU_PRINT(AudioUnitInitialize(*_unit), "initializing matrix mixer unit");
}

ofxAudioUnitMatrixMixer::~ofxAudioUnitMatrixMixer()
{
	
}

void ofxAudioUnitMatrixMixer::setChannelLayout(UInt32 inputBusCount, UInt32 outputBusCount, AudioStreamBasicDescription * inASBD, AudioStreamBasicDescription * outASBD)
{
	initWithLayout(inputBusCount, outputBusCount, inASBD, outASBD);
}

#pragma mark - Util

std::vector<UInt32> ofxAudioUnitMatrixMixer::getBusCount() const
{
	std::vector<UInt32> busCount;
	UInt32 inputBusCount, outputBusCount, inputSize, outputSize;
	inputBusCount = outputBusCount = 0;
	inputSize = outputSize = sizeof(UInt32);
	
	OFXAU_PRINT(AudioUnitGetProperty(*_unit,
									 kAudioUnitProperty_ElementCount,
									 kAudioUnitScope_Input,
									 0,
									 &inputBusCount,
									 &inputSize),
				"getting matrix mixer input bus count");
	
	OFXAU_PRINT(AudioUnitGetProperty(*_unit,
									 kAudioUnitProperty_ElementCount,
									 kAudioUnitScope_Output,
									 0,
									 &outputBusCount,
									 &outputSize),
				"getting matrix mixer output bus count");
	
	busCount.push_back(inputBusCount);
	busCount.push_back(outputBusCount);
	return busCount;
}

std::vector<AudioStreamBasicDescription> ofxAudioUnitMatrixMixer::getASBDs() const
{
	std::vector<AudioStreamBasicDescription> ASBDs;
	AudioStreamBasicDescription inputASBD, outputASBD;
	UInt32 inputSize, outputSize;
	inputSize = outputSize = sizeof(UInt32);
	
	OFXAU_PRINT(AudioUnitGetProperty(*_unit,
									 kAudioUnitProperty_StreamFormat,
									 kAudioUnitScope_Input,
									 0,
									 &inputASBD,
									 &inputSize),
				"getting matrix mixer input ASBD");
	
	OFXAU_PRINT(AudioUnitGetProperty(*_unit,
									 kAudioUnitProperty_StreamFormat,
									 kAudioUnitScope_Output,
									 0,
									 &outputASBD,
									 &outputSize),
				"getting matrix mixer output ASBD");
	
	ASBDs.push_back(inputASBD);
	ASBDs.push_back(outputASBD);
	return ASBDs;
}
