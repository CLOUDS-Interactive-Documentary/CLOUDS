
#ifndef _FFTANALYZER
#define _FFTANALYZER

#ifndef M_PI
#define	M_PI		3.14159265358979323846  /* pi */
#endif


class FFTOctaveAnalyzer {
	
public:
	
	//FFT fft;  // a reference to the FFT instance
	
	float samplingRate; // sampling rate in Hz (needed to calculate frequency spans)
	int nSpectrum; // number of spectrum bins in the fft
	int nAverages; // number of averaging bins here
	int nAveragesPerOctave; // number of averages per octave as requested by user
	float spectrumFrequencySpan; // the "width" of an fft spectrum bin in Hz
	float firstOctaveFrequency; // the "top" of the first averaging bin here in Hz
	float averageFrequencyIncrement; // the root-of-two multiplier between averaging bin frequencies
	float * averages; // the actual averages
	float * peaks; // peaks of the averages, aka "maxAverages" in other implementations
	int * peakHoldTimes; // how long to hold THIS peak meter?  decay if == 0
	int peakHoldTime; // how long do we hold peaks? (in fft frames)
	float peakDecayRate; // how quickly the peaks decay:  0f=instantly .. 1f=not at all
	int * spe2avg; // the mapping between spectrum[] indices and averages[] indices
	// the fft's log equalizer() is no longer of any use (it would be nonsense to log scale
	// the spectrum values into log-sized average bins) so here's a quick-and-dirty linear
	// equalizer instead:
	float linearEQSlope; // the rate of linear eq
	float linearEQIntercept; // the base linear scaling used at the first averaging bin
	// the formula is:  spectrum[i] * (linearEQIntercept + i * linearEQSlope)
	// so.. note that clever use of it can also provide a "gain" control of sorts
	// (fe: set intercept to 2f and slope to 0f to double gain)
	
	void setup(float samplingRate, int nBandsInTheFFT, int nAveragesPerOctave);
	
	void calculate(float * fftData);
	
};


#endif	
