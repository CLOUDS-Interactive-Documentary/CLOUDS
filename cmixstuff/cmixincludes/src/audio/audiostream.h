// audiostream.h
//
// Template inlines and classes used to generate the audio format conversion
// routines for AudioDeviceImpl.cpp
//

#include <stdint.h>  // for int32_t
#include "sndlib.h"

enum Endian { Little, Big };

inline unsigned short swapit(unsigned short us) { return (((us >> 8) & 0xff) | ((us & 0xff) << 8)); }

inline short swapit(short s) { return swapit((unsigned short) s); }

inline unsigned swapit(unsigned ul) {
    return (ul >> 24) | ((ul >> 8) & 0xff00) | ((ul << 8) & 0xff0000) | (ul << 24);
}

inline int swapit(int x) { return swapit((unsigned)x); }

inline float swapit(float uf) {
    union { unsigned l; float f; } u;
    u.f = uf;
    u.l = swapit(u.l);
    return (u.f);
}

inline int64_t swapit(int64_t x) {
	return (((int64_t)swapit((unsigned)(x>>32))) << 32) | (int64_t)swapit((unsigned)(x&0xffffffff));
}

template<class SampleType>
inline SampleType swap(bool doSwap, SampleType value);

template<>
inline short swap<short>(bool doSwap, short value) { return doSwap ? swapit(value) : value; }
 
template<>
inline int32_t swap<int32_t>(bool doSwap, int32_t value) { return doSwap ? swapit(value) : value; }
 
template<>
inline float swap<float>(bool doSwap, float value) { return doSwap ? swapit(value) : value; }

// deNormalize template and specializations.
 
template< class InputSampleType, class OutputSampleType >
inline OutputSampleType deNormalize(bool doIt, const InputSampleType &value);
 
template<>
inline short deNormalize<short, short>(bool doIt, const short &value) { return value; }

template<>
inline float deNormalize<short, float>(bool doIt, const short &value) {
	return (float) value;
}

template<>
inline short deNormalize<float, short>(bool doIt, const float &value) {
	return (short) value;
}

const static float kIntNormalizer = 1/65536.0f;

template<>
inline float deNormalize<int32_t, float>(bool doIt, const int32_t &value) {
	return value * kIntNormalizer;
}

template<>
inline int32_t deNormalize<float, int32_t>(bool doIt, const float &value) {
	return int32_t(value * 65536.0f);
}

template<>
inline float deNormalize<float, float>(bool doIt, const float &value) {
	return doIt ? value * 32767.0f : value;
}

// normalize template and specializations.

const static float kNormalizer = 1/32768.0f;

template< class SampleType >
inline SampleType normalize(bool doIt, const SampleType &value);
 
template<>
inline short normalize<short>(bool doIt, const short &value) { return value; }
 
template<>
inline int32_t normalize<int32_t>(bool doIt, const int32_t &value) {
	return value;
}
 
template<>
inline float normalize<float>(bool doIt, const float &value) { return doIt ? value * kNormalizer : value; }

#if MUS_LITTLE_ENDIAN
static const Endian kMachineEndian = Little;
#else
static const Endian kMachineEndian = Big;
#endif

template < class SampleType, Endian theEndian, bool isNormalized = false >
class InterleavedStream {
public:
	static const Endian endian = theEndian;
	static const bool normalized = isNormalized;
	typedef SampleType StreamType;
	typedef StreamType ChannelType;
	static int channelIncrement(int chans) { return chans; }
	static ChannelType *innerFromOuter(StreamType *streamData, int ch) { return &streamData[ch]; }
};

template < class SampleType, Endian theEndian, bool isNormalized = false >
class NonInterleavedStream {
public:
	static const Endian endian = theEndian;
	static const bool normalized = isNormalized;
	typedef SampleType *StreamType;
	typedef SampleType ChannelType;
	static int channelIncrement(int chans) { return 1; }
	static ChannelType *innerFromOuter(StreamType *streamData, int ch) { return streamData[ch]; }
};

