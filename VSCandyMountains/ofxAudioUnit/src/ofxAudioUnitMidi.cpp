#include "ofxAudioUnitMidi.h"
#include "ofxAudioUnitUtils.h"

#pragma mark MIDI utilities

// ----------------------------------------------------------
vector<string> ofxAudioUnitMidi::getSourceNames()
// ----------------------------------------------------------
{
	vector<string> sourceNames;
	ItemCount sourceCount = MIDIGetNumberOfSources();
	for(int i = 0; i < sourceCount; i++)
	{
		MIDIEndpointRef source = MIDIGetSource(i);
		CFStringRef sourceName = NULL;
		MIDIObjectGetStringProperty(source, kMIDIPropertyName, &sourceName);
		char sourceNameCString[255];
		CFStringGetCString(sourceName, sourceNameCString, 255, kCFStringEncodingUTF8);
		sourceNames.push_back(sourceNameCString);
	}
	return sourceNames;
}

// ----------------------------------------------------------
void ofxAudioUnitMidi::printSourceNames()
// ----------------------------------------------------------
{
	vector<string> midiSources = ofxAudioUnitMidi::getSourceNames();
	for(int i = 0; i < midiSources.size(); i++)
	{
		cout << i << " : " << midiSources[i] << endl;
	}
}

#pragma mark - MIDI Receiver

// ----------------------------------------------------------
ofxAudioUnitMidiReceiver::ofxAudioUnitMidiReceiver(string clientName)
: _client(NULL), _endpoint(NULL), _port(NULL), _unit(NULL)
// ----------------------------------------------------------
{
	CFStringRef cName = CFStringCreateWithCString(kCFAllocatorDefault, clientName.c_str(), kCFStringEncodingUTF8);
	OFXAU_PRINT(MIDIClientCreate(cName, ofxAudioUnitMidiInputProc, this, &_client), 
				"creating MIDI client");
	CFRelease(cName);
}

// ----------------------------------------------------------
ofxAudioUnitMidiReceiver::ofxAudioUnitMidiReceiver(const ofxAudioUnitMidiReceiver &other)
: _client(other._client){}
// ----------------------------------------------------------

// ----------------------------------------------------------
ofxAudioUnitMidiReceiver& ofxAudioUnitMidiReceiver::operator=(const ofxAudioUnitMidiReceiver &other)
// ----------------------------------------------------------
{
	_client = other._client;
	return *this;
}

// ----------------------------------------------------------
ofxAudioUnitMidiReceiver::~ofxAudioUnitMidiReceiver()
// ----------------------------------------------------------
{
	MIDIPortDispose(_port);
	MIDIEndpointDispose(_endpoint);
}

// ----------------------------------------------------------
bool ofxAudioUnitMidiReceiver::createMidiDestination(std::string portName)
// ----------------------------------------------------------
{
	OSStatus success = noErr;
	CFStringRef pName = CFStringCreateWithCString(kCFAllocatorDefault, portName.c_str(), kCFStringEncodingUTF8);
	success = MIDIDestinationCreate(_client, pName, ofxAudioUnitMidiReadProc, &_unit, &_endpoint);
	CFRelease(pName);
	return (success == noErr);
}

// ----------------------------------------------------------
bool ofxAudioUnitMidiReceiver::connectToMidiSource(unsigned long midiSourceIndex)
// ----------------------------------------------------------
{
	if(!_port)
	{
		OSStatus s = MIDIInputPortCreate(_client, 
										 CFSTR("oF MIDI Input Port"), 
										 ofxAudioUnitMidiReadProc, 
										 &_unit, 
										 &_port);
		if(s != noErr) return false;
	}
	
	OFXAU_RET_BOOL(MIDIPortConnectSource(_port, MIDIGetSource(midiSourceIndex), NULL),
				   "connecting MIDI receiver to source");
}

// ----------------------------------------------------------
bool ofxAudioUnitMidiReceiver::connectToMidiSource(string midiSourceName)
// ----------------------------------------------------------
{
	vector<string> sources = ofxAudioUnitMidi::getSourceNames();
	bool foundSource = false;
	int sourceIndex;
	for(int i = 0; i < sources.size() && !foundSource; i++)
	{
		if(midiSourceName == sources.at(i))
		{
			foundSource = true;
			sourceIndex = i;
		}
	}
	
	if(foundSource) return connectToMidiSource(sourceIndex);
	else            return false;
}

// ----------------------------------------------------------
void ofxAudioUnitMidiReceiver::disconnectFromMidiSource(unsigned long midiSourceIndex)
// ----------------------------------------------------------
{
	OFXAU_PRINT(MIDIPortDisconnectSource(_port, MIDIGetSource(midiSourceIndex)), 
				"disconnecting from MIDI source");
}

// ----------------------------------------------------------
void ofxAudioUnitMidiReceiver::routeMidiTo(ofxAudioUnit &unitToRouteTo)
// ----------------------------------------------------------
{
	if((unitToRouteTo.getUnitRef()))
	{
		_unit = unitToRouteTo.getUnit();
	}
}

#pragma mark - Callbacks

// ----------------------------------------------------------
// TODO: Do things with MIDI Input notifications
void ofxAudioUnitMidiInputProc(const MIDINotification *message, void *refCon)
// ----------------------------------------------------------
{
}

// ----------------------------------------------------------
void ofxAudioUnitMidiReadProc(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon)
// ----------------------------------------------------------
{
	MIDIPacket * packet = (MIDIPacket *)pktlist->packet;
	AudioUnit unit = *(AudioUnit *)readProcRefCon;
	
	for(int i = 0; i < pktlist->numPackets; i++)
	{	
		MusicDeviceMIDIEvent(unit, packet->data[0], packet->data[1], packet->data[2], 0);
		packet = MIDIPacketNext(packet);
	}
}
