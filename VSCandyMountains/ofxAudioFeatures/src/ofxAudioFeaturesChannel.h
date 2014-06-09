/*
 * Copyright Paul Reimer, 2012
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License.
 * To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc/3.0/
 * or send a letter to
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#pragma once

//#include <aubio/aubio.h>
#include "../libs/aubio/include/aubio/aubio.h"


#include <vector>
#include <deque>
#include <map>
#include <string>

class ofxAudioFeaturesChannel
{
public:
  ofxAudioFeaturesChannel();
  ~ofxAudioFeaturesChannel();

  void setup(size_t _bufferSize, size_t _hopSize, float _sampleRate);
  void destroy();
  void process(const float now);
  void updateSmoothedSpectrum(std::vector<float>& smoothedSpectrum,
                              float attack,
                              float decay);
  void updateSmoothedSpectrum(std::vector<float>& smoothedSpectrum,
                              float attackOffset,
                              float decayOffset,
                              float attackMult,
                              float decayMult);
  void updateSmoothedSpectrum(std::vector<float>& smoothedSpectrum,
                              float attackOffset,
                              float decayOffset,
                              float attackMult,
                              float decayMult,
                              const std::vector<float>& attackCoeffs,
                              const std::vector<float>& decayCoeffs);

  void resample(const std::vector<float>& spectrumFrom,
                std::vector<float>& spectrumTo,
                float scaleIncrementFactor=4.0);

  bool _compareSpectrumToReference(const std::vector<float>& spectrumReference);

  std::vector<float> inputBuffer;
  std::vector<float> spectrum;
  std::vector<float> phase;

  bool calibrateMic;
  bool calibratedMic;

  std::deque<std::pair<float, float> > onsets;
  float pitch;

  std::map<std::string, float> spectralFeatures;
  std::vector<std::string> usingFeatures;
  bool usingOnsets;
  bool usingPitch;
  bool usingPhaseVocoderSpectrum;

  aubio_fft_t* fftProcessor;
  aubio_onset_t* onsetProcessor;
  aubio_pitch_t* pitchProcessor;
  std::map<std::string, aubio_specdesc_t*> spectralFeatureProcessor;

protected:
  fvec_t* currentHopBuffer;
  fvec_t* synthesizedOutputBuffer;

  cvec_t* fftComplexOutputBuffer;
  fvec_t* onsetOutputBuffer;
  fvec_t* pitchOutputBuffer;

  std::map<std::string, fvec_t*> spectralFeatureOutputBuffer;

private:
  std::vector<float> binsScale;

  float sampleRate;
  size_t spectrumSize;
  size_t bufferSize;
  size_t hopSize;
};
