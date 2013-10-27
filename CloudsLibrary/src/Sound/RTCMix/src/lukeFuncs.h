//
//  lukeFuncs.h
//  RTCMIX proof of concept
//
//  Created by Roger DuBois on 5/28/13.
//  Copyright (c) 2013 Polytechnic Institute of NYU. All rights reserved.
//

#ifndef testRTcmix3_lukeFuncs_h
#define testRTcmix3_lukeFuncs_h

#include "ofMain.h"

#define MAXAMP 32768.0 // maximum amp for oscillators (-1., 1) = 2^15
#define DEBUG false

// BGG rtcmix stuff
extern "C" {
    int rtcmixmain();
    int maxmsp_rtsetparams(float sr, int nchans, int vecsize, float *mm_inbuf, float *mm_outbuf);
    void pullTraverse(short *inbuf, short *outbuf);
    int parse_score(char *thebuf, int buflen);
    int check_bang();
    void flush_sched();
    extern char *get_print();
    extern void reset_print();
    void OF_buffer_load_set(char *filename, char *bufname, float insk, float dur);
    int mm_buf_getframes(char *bufname);
    int mm_buf_getchans(char *bufname);
    float maxmsp_vals[1024];
    int vals_ready;
}

// luke's comp structures
struct lukeRhythm {
    vector<float> beats;
};

struct lukePitchArray {
    vector<int> notes;
    int basenote;
    int scale;
};

struct lukePreset {
    vector<string> instruments;
    int harmony;
    int rhythm;
    float tempo;
    string bank;
};

struct lukeSample {
    string filename;
    string handle;
    string bank;
    float length;
    float numbeats;
};

// luke's music functions
double mtof(double f, double tuning);
double mtof(double f);
double ftom(double f, double tuning);
string ptos(int p);
int scale(int p, int o);
void loadrhythms(string f, vector<lukeRhythm>& r);
void loadpitches(string f, vector<lukePitchArray>& p);
void loadpresets(string f, vector<lukePreset>& p);

// luke's audio functions
void RTcmixParseScoreFile(string f);
void WAVETABLE(double outskip, double dur, double amp, double freq, double pan, string waveform, string ampenvelope);
void MMODALBAR(double outskip, double dur, double amp, double freq, double hardness, double pos, int instrument);
void STRUM(double outskip, double dur, double amp, double freq, double squish, double decay, double pan);
void MBLOWBOTL(double outskip, double dur, double amp, double freq, double noiseamp, double maxpressure, double pan, string pressureenv, string ampenvelope);
void MMESH2D(double outskip, double dur, double amp, int nxpoints, int nypoints, double xpos, double ypos, double decay, double strike, double pan);
void MBANDEDWG(double outskip, double dur, double amp, double freq, double strikepos, int pluckflag, double maxvel, int preset, double bowpressure, double resonance, double integration, double pan, string velocityenvelope);
void FNOISE3(double outskip, double dur, double amp, double ringdown, double pan, double f1, double f2, double f3, double Q, string ampenvelope);

void REVERB(double time);
float LOADSOUND(string file, string handle);
void STEREO(double outskip, double inskip, double dur, double amp, double pan, string handle);
void SOUNDLOOP(double outskip, double loopdur, double looplen, double amp, string handle);
void SOUNDLOOPMONO(double outskip, double loopdur, double looplen, double amp, string handle, double pan);
void PANECHO(double outskip, double inskip, double dur, double amp, double leftdelay, double rightdelay, double feedback, double ringdown);
void SCHEDULEBANG(double time);

#endif
