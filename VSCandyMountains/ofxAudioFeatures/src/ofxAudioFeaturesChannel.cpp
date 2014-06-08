/*
 * Copyright Paul Reimer, 2012
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License.
 * To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc/3.0/
 * or send a letter to
 * Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include "ofxAudioFeaturesChannel.h"
#include <iostream>
#include <float.h>
#include <cmath>

#ifndef MIN
  #define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

//#include <aubio/onset/peakpicker.h>
#include "../libs/aubio/include/aubio/onset/peakpicker.h"

struct _aubio_onset_t
{
  aubio_pvoc_t * pv;            /**< phase vocoder */
  aubio_specdesc_t * od;        /**< spectral descriptor */
  aubio_peakpicker_t * pp;      /**< peak picker */
  cvec_t * fftgrain;            /**< phase vocoder output */
  fvec_t * desc;                /**< spectral description */
  smpl_t threshold;             /**< onset peak picking threshold */
  smpl_t silence;               /**< silence threhsold */
  uint_t minioi;                /**< minimum inter onset interval */
  uint_t delay;                 /**< constant delay, in samples, removed from detected onset times */
  uint_t samplerate;            /**< sampling rate of the input signal */
  uint_t hop_size;              /**< number of samples between two runs */
  
  uint_t total_frames;          /**< total number of frames processed since the beginning */
  uint_t last_onset;            /**< last detected onset location, in frames */
};

//--------------------------------------------------------------
ofxAudioFeaturesChannel::ofxAudioFeaturesChannel()
: calibrateMic(false)
, calibratedMic(false)
, usingOnsets(true)
, usingPitch(false)
, usingPhaseVocoderSpectrum(false)
, fftProcessor(NULL)
, onsetProcessor(NULL)
, pitchProcessor(NULL)
, currentHopBuffer(NULL)
, synthesizedOutputBuffer(NULL)
, fftComplexOutputBuffer(NULL)
, onsetOutputBuffer(NULL)
, pitchOutputBuffer(NULL)
//, transientSteadyStateSeparationProcessor(NULL)
//, transientOutputBuffer(NULL)
//, steadyStateOutputBuffer(NULL)
//, usingTransientSteadyStateSeparation(false)
//, usingPhaseVocoderSpectrum(true)
//, hopIdx(0)
, sampleRate(0.)
, spectrumSize(0)
, bufferSize(0)
, hopSize(0)
{}

//--------------------------------------------------------------
ofxAudioFeaturesChannel::~ofxAudioFeaturesChannel()
{
  destroy();
}

//--------------------------------------------------------------
void
ofxAudioFeaturesChannel::setup(size_t _bufferSize, size_t _hopSize, float _sampleRate)
{
  bufferSize    = _bufferSize;
  hopSize       = _hopSize;
  sampleRate    = _sampleRate;
  spectrumSize  = _bufferSize / 2;

  currentHopBuffer        = new_fvec(hopSize);
  synthesizedOutputBuffer = new_fvec(bufferSize);
  
  fftComplexOutputBuffer  = new_cvec(bufferSize);
//  fftInputBuffer          = new_fvec(bufferSize);
  fftProcessor            = new_aubio_fft(bufferSize);

  onsetOutputBuffer       = new_fvec(1);
  onsetProcessor          = new_aubio_onset("mkl", bufferSize, hopSize, sampleRate);

  pitchOutputBuffer       = new_fvec(1);
  pitchProcessor          = new_aubio_pitch("yin", bufferSize, hopSize, sampleRate);
  aubio_pitch_set_unit(pitchProcessor, "bin");

//  transientOutputBuffer   = new_cvec(bufferSize);
//  steadyStateOutputBuffer = new_cvec(bufferSize);
//  transientSteadyStateSeparationProcessor = new_aubio_tss(bufferSize, hopSize);

	inputBuffer.resize(bufferSize);
/*
  spectralFeatureProcessor["energy"]    = new_aubio_specdesc("energy",    hopSize);
  spectralFeatureProcessor["hfc"]       = new_aubio_specdesc("hfc",       hopSize);
//  spectralFeatureProcessor["complex"]   = new_aubio_specdesc("complex",   hopSize);
//  spectralFeatureProcessor["phase"]     = new_aubio_specdesc("phase",     hopSize);
//  spectralFeatureProcessor["specdiff"]  = new_aubio_specdesc("specdiff",  hopSize);
//  spectralFeatureProcessor["kl"]        = new_aubio_specdesc("kl",        hopSize);
//  spectralFeatureProcessor["mkl"]       = new_aubio_specdesc("mkl",       hopSize);
//  spectralFeatureProcessor["specflux"]  = new_aubio_specdesc("specflux",  hopSize);
  spectralFeatureProcessor["centroid"]  = new_aubio_specdesc("centroid",  hopSize);
  spectralFeatureProcessor["spread"]    = new_aubio_specdesc("spread",    hopSize);
  spectralFeatureProcessor["skewness"]  = new_aubio_specdesc("skewness",  hopSize);
  spectralFeatureProcessor["kurtosis"]  = new_aubio_specdesc("kurtosis",  hopSize);
  spectralFeatureProcessor["slope"]     = new_aubio_specdesc("slope",     hopSize);
  spectralFeatureProcessor["decrease"]  = new_aubio_specdesc("decrease",  hopSize);
  spectralFeatureProcessor["rolloff"]   = new_aubio_specdesc("rolloff",   hopSize);
*/
  for (size_t i=0; i<usingFeatures.size(); ++i)
  {
    const std::string& featureName(usingFeatures[i]);
    std::vector<char> featureNameCopy(featureName.c_str(),
                                      featureName.c_str() + featureName.size() + 1);
    spectralFeatureProcessor.insert(make_pair(featureName,
                                              new_aubio_specdesc(&featureNameCopy[0], hopSize)));
    spectralFeatureOutputBuffer.insert(make_pair(featureName, new_fvec(1)));
  }

  spectrum.resize(spectrumSize);
	phase.resize(spectrumSize);
	binsScale.resize(spectrumSize);
  
	calibrateMic = false;
}

//--------------------------------------------------------------
void
ofxAudioFeaturesChannel::destroy()
{
  if (currentHopBuffer)
  {
    del_fvec(currentHopBuffer);
    currentHopBuffer = NULL;
  }

  if (fftProcessor)
  {
    del_aubio_fft(fftProcessor);
    fftProcessor = NULL;
  }

  if (fftComplexOutputBuffer)
  {
    del_cvec(fftComplexOutputBuffer);
    fftComplexOutputBuffer = NULL;
  }

  if (synthesizedOutputBuffer)
  {
    del_fvec(synthesizedOutputBuffer);
    synthesizedOutputBuffer = NULL;
  }

  if (onsetProcessor)
  {
    del_aubio_onset(onsetProcessor);
    onsetProcessor = NULL;
  }

  if (onsetOutputBuffer)
  {
    del_fvec(onsetOutputBuffer);
    onsetOutputBuffer = NULL;
  }

  if (pitchProcessor)
  {
    del_aubio_pitch(pitchProcessor);
    pitchProcessor = NULL;
  }

  if (pitchOutputBuffer)
  {
    del_fvec(pitchOutputBuffer);
    pitchOutputBuffer = NULL;
  }
/*
  if (transientSteadyStateSeparationProcessor)
  {
    del_aubio_tss(transientSteadyStateSeparationProcessor);
    transientSteadyStateSeparationProcessor = NULL;
  }

  if (transientOutputBuffer)
  {
    del_cvec(transientOutputBuffer);
    transientOutputBuffer = NULL;
  }
  
  if (steadyStateOutputBuffer)
  {
    del_cvec(steadyStateOutputBuffer);
    steadyStateOutputBuffer = NULL;
  }
*/
  for (std::map<std::string, aubio_specdesc_t*>::iterator spectralFeatureProcessorIter = spectralFeatureProcessor.begin();
       spectralFeatureProcessorIter != spectralFeatureProcessor.end(); ++spectralFeatureProcessorIter)
    del_aubio_specdesc(spectralFeatureProcessorIter->second);
  spectralFeatureProcessor.clear();
  
  for (std::map<std::string, fvec_t*>::iterator spectralFeatureOutputBufferIter = spectralFeatureOutputBuffer.begin();
       spectralFeatureOutputBufferIter != spectralFeatureOutputBuffer.end(); ++spectralFeatureOutputBufferIter)
    del_fvec(spectralFeatureOutputBufferIter->second);
  spectralFeatureOutputBuffer.clear();
}

//--------------------------------------------------------------
void
ofxAudioFeaturesChannel::process(const float now/*,
                                 const SpectrumType spectrumType*/)
{
  // input new hop
	for (unsigned int i=0; i<hopSize; ++i)
  {
		currentHopBuffer->data[i] = inputBuffer[i];
//    fftInputBuffer->data[i + (hopIdx*hopSize)] =  inputBuffer[i];
  }

/*
  // process hop
  hopIdx++;
  if (!usingPhaseVocoderSpectrum)
  {
    if ((hopIdx*hopSize) >= bufferSize)
    {
      hopIdx = 0;
      aubio_fft_do(fftProcessor, fftInputBuffer, fftComplexOutputBuffer);
    }
  }
*/
  if (usingOnsets || usingPhaseVocoderSpectrum)
    aubio_onset_do(onsetProcessor, currentHopBuffer, onsetOutputBuffer);

  if (usingPitch)
    aubio_pitch_do(pitchProcessor, currentHopBuffer, pitchOutputBuffer);
/*
  if (usingTransientSteadyStateSeparation)
  {
    if (usingPhaseVocoderSpectrum)
      aubio_tss_do(transientSteadyStateSeparationProcessor, onsetProcessor->fftgrain, transientOutputBuffer, steadyStateOutputBuffer);
    else
      aubio_tss_do(transientSteadyStateSeparationProcessor, fftComplexOutputBuffer, transientOutputBuffer, steadyStateOutputBuffer);
  }
*/
  for (std::map<std::string, aubio_specdesc_t*>::iterator spectralFeatureProcessorIter = spectralFeatureProcessor.begin();
       spectralFeatureProcessorIter != spectralFeatureProcessor.end(); ++spectralFeatureProcessorIter)
  {
    // process all configured (at setup time) spectral features
    aubio_specdesc_do(spectralFeatureProcessorIter->second,
                      onsetProcessor->fftgrain, // should be pvoc output
                      spectralFeatureOutputBuffer[spectralFeatureProcessorIter->first]);

    spectralFeatures[spectralFeatureProcessorIter->first] = spectralFeatureOutputBuffer[spectralFeatureProcessorIter->first]->data[0];
  }

  // pitch extraction (per-hop)
  pitch = pitchOutputBuffer->data[0];

  // onset extraction (per-hop)
  bool onsetDetected = (onsetOutputBuffer->data[0] > FLT_EPSILON);
  if (onsetDetected)
  {
    onsets.push_back(std::make_pair(now, pitch));
  }
  else if (!onsets.empty())
  {
    onsets.rbegin()->second = pitch;
  }

  cvec_t* selectedSpectrum = onsetProcessor->fftgrain;
/*
  cvec_t* selectedSpectrum = NULL;
  switch (spectrumType)
  {
    case FFTSpectrum:
      selectedSpectrum = fftComplexOutputBuffer;
      break;
    case PhaseVocoderSpectrum:
      selectedSpectrum = onsetProcessor->fftgrain;
      break;
    case TransientSpectrum:
      selectedSpectrum = transientOutputBuffer;
      break;
    case SteadyStateSpectrum:
      selectedSpectrum = steadyStateOutputBuffer;
      break;
  }
*/
  if (selectedSpectrum != NULL)
  {
    for (unsigned int i=0; i<spectrum.size(); ++i)
    {
      spectrum[i] = selectedSpectrum->norm[i];
      phase[i] = selectedSpectrum->phas[i];
    }
  }

  // spectrum magnitude calibration
  if (calibrateMic)
  {
    for (unsigned int i=0; i<spectrum.size(); ++i)
    {
      if (binsScale[i] > FLT_MAX || binsScale[i] <= 0)
        binsScale[i] = 1.;
      else if (spectrum[i] > binsScale[i])
        binsScale[i] = spectrum[i];
    }

    calibratedMic = true;
  }
  else if (calibratedMic)
  {
    // scaling by calibrated values
    for (unsigned int i=0; i<spectrum.size(); ++i)
      if (binsScale[i] > 0)
        spectrum[i] /= binsScale[i];
  }
}

#define lerp(start, stop, amt)  (start + (stop-start) * amt)
#define clamp(value, min, max) (value < min ? min : value > max ? max : value)
//--------------------------------------------------------------
void
ofxAudioFeaturesChannel::updateSmoothedSpectrum(std::vector<float>& smoothedSpectrum,
                                                float attack,
                                                float decay)
{
  float adjustedAttack = powf(attack, (float)hopSize / (float)bufferSize);
  float adjustedDecay = powf(decay, (float)hopSize / (float)bufferSize);

  // spectrum smoothing
  for (unsigned int i=0; i<MIN(spectrum.size(), smoothedSpectrum.size()); ++i)
  {
    if (spectrum[i] > smoothedSpectrum[i])
      smoothedSpectrum[i] = lerp(spectrum[i], smoothedSpectrum[i], adjustedAttack);
    else if (spectrum[i] < smoothedSpectrum[i])
      smoothedSpectrum[i] = lerp(spectrum[i], smoothedSpectrum[i], adjustedDecay);
  }
}

//--------------------------------------------------------------
void
ofxAudioFeaturesChannel::updateSmoothedSpectrum(std::vector<float>& smoothedSpectrum,
                                                float attackOffset,
                                                float decayOffset,
                                                float attackMult,
                                                float decayMult)
{
  updateSmoothedSpectrum(smoothedSpectrum,
                         attackOffset,
                         decayOffset,
                         attackMult,
                         decayMult,
                         smoothedSpectrum,
                         smoothedSpectrum);
}

//--------------------------------------------------------------
void
ofxAudioFeaturesChannel::updateSmoothedSpectrum(std::vector<float>& smoothedSpectrum,
                                                float attackOffset,
                                                float decayOffset,
                                                float attackMult,
                                                float decayMult,
                                                const std::vector<float>& attackCoeffs,
                                                const std::vector<float>& decayCoeffs)
{
//  float adjustedAttackOffset = powf(attackOffset, (float)hopSize / (float)bufferSize);
//  float adjustedDecayOffset = powf(decayOffset, (float)hopSize / (float)bufferSize);

  // spectrum smoothing
  for (unsigned int i=0; i<MIN(spectrum.size(), smoothedSpectrum.size()); ++i)
  {
    float attack = clamp(attackOffset + attackMult*clamp(attackCoeffs[i], 0.0, 1.0), 0.0, 0.9999);
    float decay = clamp(decayOffset + decayMult*clamp(decayCoeffs[i], 0.0, 1.0), 0.0, 0.9999);

    if (spectrum[i] > smoothedSpectrum[i])
      smoothedSpectrum[i] = lerp(spectrum[i], smoothedSpectrum[i], attack);
    else if (spectrum[i] < smoothedSpectrum[i])
      smoothedSpectrum[i] = lerp(spectrum[i], smoothedSpectrum[i], decay);
  }
}
#undef lerp

//--------------------------------------------------------------
bool
ofxAudioFeaturesChannel::_compareSpectrumToReference(const std::vector<float>& spectrumReference)
{
  float maxValue = FLT_MIN;
  float maxValueBin = FLT_MAX;

  float maxValueReference = FLT_MIN;
  float maxValueBinReference = FLT_MAX;

  float sumReference = 0;
  float sum = 0;

  float total_error = 0;
  float maxError = FLT_MIN;
  int maxErrorBin = -1;
  size_t numIdenticalBins = 0;

  if (spectrum.size() != spectrumReference.size())
  {
    std::cout
    << "spectrum.size(), " << spectrum.size()
    << " != "
    << "spectrumReference.size()," << spectrumReference.size()
    << std::endl;
    return false;
  }
  
	for (unsigned int i=0; i<spectrumReference.size(); ++i)
  {
    float binVal = sqrtf(spectrum[i]);
    float binValReference = spectrumReference[i];

    if (binVal > 1000)
    {
      std::cout << "Wtf huge binval @ " << i << std::endl;
    }

    if (binVal != binVal)
    {
      std::cout << "NaNery in FFTW @ " << i << std::endl;
      continue;
    }
    if (binValReference != binValReference)
    {
      std::cout << "NaNery in Kiss @ " << i << std::endl;
      continue;
    }

    float error = std::abs(binValReference - binVal);
//    if (error < (0.0001))
    if (error == 0.0)
      numIdenticalBins++;

    sum += binVal;
    sumReference += binValReference;
    
    total_error += error;

    if (binValReference > maxValueReference)
    {
      maxValueReference = binValReference;
      maxValueBinReference = i;
    }
    
    if (binVal > maxValue)
    {
      maxValue = binVal;
      maxValueBin = i;
    }
    
    if (error > maxError)
    {
      maxError = error;
      maxErrorBin = i;
    }
  }
  
  float errorAmt = total_error / ((sum + sumReference)/2);
  if (numIdenticalBins != spectrum.size())
  {
    std::cout
    << "max error = " << maxError << " @ " << maxErrorBin
    << ", # correct = " << numIdenticalBins
    << ", ";

    std::cout
    << "total_error % = " << (errorAmt*100.0)
    << ", max kiss val = " << maxValueReference << " @ " << maxValueBinReference
    << ", max fftw val = " << maxValue << " @ " << maxValueBin
    << ", kiss_sum = " << sumReference
    << ", sum = " << sum
    << std::endl;
  }

  return (numIdenticalBins==spectrum.size());
}

//--------------------------------------------------------------
void
ofxAudioFeaturesChannel::resample(const std::vector<float>& spectrumFrom,
                                  std::vector<float>& spectrumTo,
                                  float scaleIncrementFactor)
{
  unsigned int bw = (spectrumFrom.size()/spectrumTo.size()) / scaleIncrementFactor;

  for (size_t b=0; b<spectrumTo.size(); ++b)
  {
    spectrumTo[b] = 0.0;
    unsigned int start = b*bw;
    
    for (size_t i=0; i<bw; ++i)
      spectrumTo[b] += std::abs(spectrumFrom[start+i]);
      
      spectrumTo[b] /= (float)bw;
  }
}
