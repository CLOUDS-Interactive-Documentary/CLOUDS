// audio_devices.h

#ifdef __cplusplus
// From audio_dev_creator.cpp.  Only used by C++ sources.
AudioDevice *createAudioDevice(const char *inputDesc, const char *outputDesc, 
							   bool recording, bool playing);
extern "C" {
#endif

AudioDevice *
create_audio_devices(int record, int play, 
					 int chans, float sr, 
					 int *buffersize, int numBuffers);

AudioDevice *
create_audio_file_device(AudioDevice *inDevice,
						 const char *outfilename,
						 int header_type,
						 int sample_format,
						 int chans,
						 float srate,
						 int normalize_output_floats,
						 int check_peaks);

#ifdef __cplusplus
}
#endif
