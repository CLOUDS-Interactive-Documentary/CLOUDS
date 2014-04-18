#include "ofxAudioUnitFftNode.h"
#include <math.h>

// these values taken from the Simple Spectrum Analyser project here
// https://github.com/fredguile/SimpleSpectrumAnalyzer
const float DB_CORRECTION_HAMMING  = 1.0;
const float DB_CORRECTION_HANNING  = -3.2;
const float DB_CORRECTION_BLACKMAN = 2.37;

ofxAudioUnitFftNode::ofxAudioUnitFftNode(unsigned int fftBufferSize, Settings settings)
: _currentMaxLog2N(0)
, _outputSettings(settings)
, _fftSetup(NULL)
, _fftData((COMPLEX_SPLIT){NULL, NULL})
, _window(NULL)
{
	setFftBufferSize(fftBufferSize);
}

ofxAudioUnitFftNode::ofxAudioUnitFftNode(const ofxAudioUnitFftNode &orig)
: _currentMaxLog2N(0)
, _outputSettings(orig._outputSettings)
, _fftSetup(NULL)
, _fftData((COMPLEX_SPLIT){NULL, NULL})
, _window(NULL)
{
	setFftBufferSize(orig._N);
}

ofxAudioUnitFftNode& ofxAudioUnitFftNode::operator=(const ofxAudioUnitFftNode &orig)
{
	setFftBufferSize(orig._N);
	setSettings(orig._outputSettings);
	return *this;
}

ofxAudioUnitFftNode::~ofxAudioUnitFftNode()
{
	freeBuffers();
}

void ofxAudioUnitFftNode::freeBuffers()
{
	if(_fftSetup)      vDSP_destroy_fftsetup(_fftSetup);
	if(_fftData.realp) free(_fftData.realp);
	if(_fftData.imagp) free(_fftData.imagp);
	if(_window)        free(_window);
}

void generateWindow(ofxAudioUnitWindowType windowType, float * window, size_t size)
{
	switch (windowType) {
		case OFXAU_WINDOW_HAMMING:
			vDSP_hamm_window(window, size, 0);
			break;
		case OFXAU_WINDOW_HANNING:
			vDSP_hann_window(window, size, 0);
			break;
		case OFXAU_WINDOW_BLACKMAN:
			vDSP_blkman_window(window, size, 0);
			break;
	}
}

void ofxAudioUnitFftNode::setFftBufferSize(unsigned int bufferSize)
{
	_log2N = (unsigned int) ceilf(log2f(bufferSize));
	_N = 1 << _log2N;
	
	// if the new buffer size is bigger than what we've allocated for,
	// free everything and allocate anew (otherwise re-use)
	if(_log2N > _currentMaxLog2N) {
		freeBuffers();
		_fftData.realp = (float *)calloc(_N / 2, sizeof(float));
		_fftData.imagp = (float *)calloc(_N / 2, sizeof(float));
		_window = (float *)calloc(_N, sizeof(float));
		_fftSetup = vDSP_create_fftsetup(_log2N, kFFTRadix2);
		_currentMaxLog2N = _log2N;
	}

	generateWindow(_outputSettings.window, _window, _N);
	setBufferSize(_N);
}

void ofxAudioUnitFftNode::setWindowType(ofxAudioUnitWindowType windowType)
{
	_outputSettings.window = windowType;
	generateWindow(_outputSettings.window, _window, _N);
}

void ofxAudioUnitFftNode::setScale(ofxAudioUnitScaleType scaleType)
{
	_outputSettings.scale = scaleType;
}

void ofxAudioUnitFftNode::setNormalizeInput(bool normalizeInput)
{
	_outputSettings.normalizeInput = normalizeInput;
}

void ofxAudioUnitFftNode::setNormalizeOutput(bool normalizeOutput)
{
	_outputSettings.normalizeOutput = normalizeOutput;
}

void ofxAudioUnitFftNode::setClampMinToZero(bool clampToZero)
{
	_outputSettings.clampMinToZero = clampToZero;
}

void ofxAudioUnitFftNode::setSettings(const ofxAudioUnitFftNode::Settings &settings)
{
	_outputSettings = settings;
	generateWindow(_outputSettings.window, _window, _N);
}

static void PerformFFT(float * input, float * window, COMPLEX_SPLIT &fftData, FFTSetup &setup, size_t N)
{	
	// windowing
	vDSP_vmul(input, 1, window, 1, input, 1, N);
	
	// FFT
	vDSP_ctoz((COMPLEX *) input, 2, &fftData, 1, N/2);
	vDSP_fft_zrip(setup, &fftData, 1, (size_t)ceilf(log2f(N)), kFFTDirection_Forward);
	
	// zero-ing out Nyquist freq
	fftData.imagp[0] = 0.0f;
}

bool ofxAudioUnitFftNode::getAmplitude(vector<float> &outAmplitude)
{
	getSamplesFromChannel(_sampleBuffer, 0);
	
	// return empty if we don't have enough samples yet
	if(_sampleBuffer.size() < _N) {
		outAmplitude.clear();
		return false;
	}
	
	// normalize input waveform
	if(_outputSettings.normalizeInput) {
		float timeDomainMax;
		vDSP_maxv(&_sampleBuffer[0], 1, &timeDomainMax, _N);
		vDSP_vsdiv(&_sampleBuffer[0], 1, &timeDomainMax, &_sampleBuffer[0], 1, _N);
	}
	
	PerformFFT(&_sampleBuffer[0], _window, _fftData, _fftSetup, _N);
	
	// get amplitude
	vDSP_zvmags(&_fftData, 1, _fftData.realp, 1, _N/2);
	
	// normalize magnitudes
	float two = 2.0;
	vDSP_vsdiv(_fftData.realp, 1, &two, _fftData.realp, 1, _N/2);

	// scale output according to requested settings
	if(_outputSettings.scale == OFXAU_SCALE_LOG10) {
		for(int i = 0; i < (_N / 2); i++) {
			_fftData.realp[i] = log10f(_fftData.realp[i] + 1);
		}
	} else if(_outputSettings.scale == OFXAU_SCALE_DECIBEL) {
		float ref = 1.0;
		vDSP_vdbcon(_fftData.realp, 1, &ref, _fftData.realp, 1, _N / 2, 1);
		
		float dbCorrectionFactor = 0;
		switch (_outputSettings.window) {
			case OFXAU_WINDOW_HAMMING:
				dbCorrectionFactor = DB_CORRECTION_HAMMING;
				break;
			case OFXAU_WINDOW_HANNING:
				dbCorrectionFactor = DB_CORRECTION_HAMMING;
				break;
			case OFXAU_WINDOW_BLACKMAN:
				dbCorrectionFactor = DB_CORRECTION_HAMMING;
				break;
		}
		
		vDSP_vsadd(_fftData.realp, 1, &dbCorrectionFactor, _fftData.realp, 1, _N / 2);
	}
	
	// restrict minimum to 0
	if(_outputSettings.clampMinToZero) {
		float min = 0.0;
		float max = INFINITY;
		vDSP_vclip(_fftData.realp, 1, &min, &max, _fftData.realp, 1, _N / 2);
	}
	
	// normalize output between 0 and 1
	if(_outputSettings.normalizeOutput) {
		float max;
		vDSP_maxv(_fftData.realp, 1, &max, _N / 2);
		if(max > 0) {
			vDSP_vsdiv(_fftData.realp, 1, &max, _fftData.realp, 1, _N / 2);
		}
	}
	
	outAmplitude.assign(_fftData.realp, _fftData.realp + _N/2);
	return true;
}

bool ofxAudioUnitFftNode::getPhase(std::vector<float> &outPhase)
{
	getSamplesFromChannel(_sampleBuffer, 0);
	
	if(_sampleBuffer.size() < _N) {
		outPhase.clear();
		return false;
	}
	
	PerformFFT(&_sampleBuffer[0], _window, _fftData, _fftSetup, _N);
	
	vDSP_zvphas(&_fftData, 1, &_sampleBuffer[0], 1, _N / 2);
	
	outPhase.assign(_sampleBuffer.begin(), _sampleBuffer.begin() + (_N / 2));
	return true;
}
