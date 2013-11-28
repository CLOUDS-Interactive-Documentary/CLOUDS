
#include "FFTOctaveAnalyzer.h"	


void FFTOctaveAnalyzer::setup(float samplingRate, int nBandsInTheFFT, int nAveragesPerOctave){

    samplingRate = samplingRate;
    nSpectrum = nBandsInTheFFT;
    spectrumFrequencySpan = (samplingRate / 2.0f) / (float)(nSpectrum);
    nAverages = nBandsInTheFFT;
    // fe:  2f for octave bands, sqrt(2) for half-octave bands, cuberoot(2) for third-octave bands, etc
    if (nAveragesPerOctave==0) // um, wtf?
		nAveragesPerOctave = 1;
    nAveragesPerOctave = nAveragesPerOctave;
    averageFrequencyIncrement = pow(2.0f, 1.0f/(float)(nAveragesPerOctave));
    // this isn't currently configurable (used once here then no effect), but here's some reasoning:
    // 43 is a good value if you want to approximate "computer" octaves: 44100/2/2/2/2/2/2/2/2/2/2
    // 55 is a good value if you'd rather approximate A-440 octaves: 440/2/2/2
    // 65 is a good value if you'd rather approximate "middle-C" octaves:  ~262/2/2
    // you could easily double it if you felt the lowest band was just rumble noise (as it probably is)
    // but don't go much smaller unless you have a huge fft window size (see below for more why)
    // keep in mind, if you change it, that the number of actual bands may change +/-1, and
    // for some values, the last averaging band may not be very useful (may extend above nyquist)
    firstOctaveFrequency = 55.0f;
    // for each spectrum[] bin, calculate the mapping into the appropriate average[] bin.
    // this gives us roughly log-sized averaging bins, subject to how "fine" the spectrum bins are.
    // with more spectrum bins, you can better map into the averaging bins (especially at low
    // frequencies) or use more averaging bins per octave.  with an fft window size of 2048,
    // sampling rate of 44100, and first octave around 55, that's about enough to do half-octave
    // analysis.  if you don't have enough spectrum bins to map adequately into averaging bins
    // at the requested number per octave then you'll end up with "empty" averaging bins, where
    // there is no spectrum available to map into it.  (so... if you have "nonreactive" averages,
    // either increase fft buffer size, or decrease number of averages per octave, etc)
    spe2avg = new int[nSpectrum];
    int avgidx = 0;
    float averageFreq = firstOctaveFrequency; // the "top" of the first averaging bin
    // we're looking for the "top" of the first spectrum bin, and i'm just sort of
    // guessing that this is where it is (or possibly spectrumFrequencySpan/2?)
    // ... either way it's probably close enough for these purposes
    float spectrumFreq = spectrumFrequencySpan;
    for (int speidx=0; speidx < nSpectrum; speidx++) {
		while (spectrumFreq > averageFreq) {
			avgidx++;
			averageFreq *= averageFrequencyIncrement;
		}
		spe2avg[speidx] = avgidx;
		spectrumFreq += spectrumFrequencySpan;
    }
    nAverages = avgidx;
    averages = new float[nAverages];
    peaks = new float[nAverages];
    peakHoldTimes = new int[nAverages];
    peakHoldTime = 0; // arbitrary
    peakDecayRate = 0.9f; // arbitrary
    linearEQIntercept = 1.0f; // unity -- no eq by default
    linearEQSlope = 0.0f; // unity -- no eq by default
}

void FFTOctaveAnalyzer::calculate(float * fftData){
	
	int last_avgidx = 0; // tracks when we've crossed into a new averaging bin, so store current average
    float sum = 0.0f; // running total of spectrum data
    int count = 0; // count of spectrums accumulated (for averaging)
    for (int speidx=0; speidx < nSpectrum; speidx++) {
		count++;
		sum += fftData[speidx] * (linearEQIntercept + (float)(speidx) * linearEQSlope);
		int avgidx = spe2avg[speidx];
		if (avgidx != last_avgidx) {
			
			for (int j = last_avgidx; j < avgidx; j++){
				averages[j] = sum / (float)(count);
			}
			count = 0;
			sum = 0.0f;
		}
		last_avgidx = avgidx;
    }
    // the last average was probably not calculated...
    if ((count > 0) && (last_avgidx < nAverages)){
		averages[last_avgidx] = sum / (float)(count);
	}
	
    // update the peaks separately
    for (int i=0; i < nAverages; i++) {
		if (averages[i] >= peaks[i]) {
			// save new peak level, also reset the hold timer
			peaks[i] = averages[i];
			peakHoldTimes[i] = peakHoldTime;
		} else {
			// current average does not exceed peak, so hold or decay the peak
			if (peakHoldTimes[i] > 0) {
				peakHoldTimes[i]--;
			} else {
				peaks[i] *= peakDecayRate;
			}
		}
    }
}
