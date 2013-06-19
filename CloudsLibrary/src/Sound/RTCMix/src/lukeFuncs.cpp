//
//  lukeFuncs.cpp
//  RTCMIX proof of concept
//
//  Created by Roger DuBois on 5/28/13.
//  Copyright (c) 2013 Polytechnic Institute of NYU. All rights reserved.
//

#include <iostream>
#include "lukeFuncs.h"

// LUKE FUNCTIONS

void RTcmixInit()
{
    // luke's initial rtcmix score sent for parsing
    string thescore = "srand() \
    theamp = maketable(\"line\", 1000, 0.0, 0.0, 0.01, 1.0, 3.0, 0.0) \
    thewave = maketable(\"wave\", 1000, 1.0, 0.7, 0.3, 0.3, 0.1, 0.1) \
    themellowamp = maketable(\"line\", 1000, 0.0, 0.0, 1.0, 1.0, 2.0, 0.0) \
    themellowwave = maketable(\"wave\", 1000, 1.0, 0.3, 0.2, 0.1, 0.05) \
    thestereoamp = maketable(\"line\", 1000, 0.0, 0.0, 50.0, 1.0, 200.0, 1., 250.0, 0.0) \
    bus_config(\"WAVETABLE\", \"aux 0-1 out\") \
    bus_config(\"STRUM2\", \"aux 0-1 out\") \
    bus_config(\"STEREO\", \"in 0\", \"aux 0-1 out\") \
    bus_config(\"MMODALBAR\", \"aux 0-1 out\") \
    bus_config(\"GVERB\", \"aux 0-1 in\", \"out 0-1\") \
    bus_config(\"PANECHO\", \"in 0\", \"out 0-1\") \
    ";

    parse_score((char*)thescore.c_str(), thescore.length());
    

}

// uses the GVERB() instrument
void REVERB(double time)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "GVERB(0.0, 0.0, %f, 1.0, 80., 10., 0.5, 0.5, 0., -12.,-12.,5)", time);
    parse_score(thebuf, bx);
}

// schedules a bang flag to be set for a callback
void SCHEDULEBANG(double time)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "MAXBANG(%f)", time);
    parse_score(thebuf, bx);
}

// sets up rtinput() for a signal processing routine that requires an audio file
void INPUTSOUND(string file)
{
    string fullfile = ofToDataPath(file);
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "rtinput(\"%s\")", (char*)fullfile.c_str());
    parse_score(thebuf, bx);
    
}

// basic soundfile mixing interface
void STEREO(double outskip, double inskip, double dur, double amp, double pan)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "STEREO(%f, %f*DUR(), %f, %f*thestereoamp, %f)", outskip, inskip, dur, amp, pan);
    parse_score(thebuf, bx);
    
}

void PANECHO(double outskip, double inskip, double dur, double amp, double leftdelay, double rightdelay, double feedback, double ringdown)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "PANECHO(%f, %f*DUR(), %f, %f*thestereoamp, %f, %f, %f, %f)", outskip, inskip, dur, amp, leftdelay, rightdelay, feedback, ringdown);
    parse_score(thebuf, bx);
    
}


// basic wavetable interface
void WAVETABLE(double outskip, double dur, double amp, double freq, double pan, string waveform, string ampenvelope)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "WAVETABLE(%f, %f, %f*%s, %f, %f, %s)", outskip, dur, amp*MAXAMP, (char*)ampenvelope.c_str(), freq, pan, (char*)waveform.c_str());
    parse_score(thebuf, bx);
}

// modal bars from the STK
void MMODALBAR(double outskip, double dur, double amp, double freq, double hardness, double pos, int instrument)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "MMODALBAR(%f, %f, %f, %f, %f, %f, %i)", outskip, dur, amp*MAXAMP, freq, hardness, pos, instrument);
    parse_score(thebuf, bx);
}

// karplus-strong algorithm
void STRUM(double outskip, double dur, double amp, double freq, double squish, double decay, double pan)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "STRUM2(%f, %f, %f, %f, %f, %f, %f)", outskip, dur, amp*MAXAMP, freq, squish, decay, pan);
    parse_score(thebuf, bx);
    
}


// --- music functions ---


// frequency-to-midi
double ftom(double f, double tuning)
{
    // midi -> frequency conversion
    if(f>0) return(69. + (1./.057762265) * log(f/tuning));
    else return(-999.);
}

// midi-to-frequency
double mtof(double f, double tuning)
{
    // midi -> frequency conversion
	return (tuning * exp(.057762265 * (f - 69.)));
}

// ditto, overloaded to assume A440
double mtof(double f)
{
    // midi -> frequency conversion
	return (440. * exp(.057762265 * (f - 69.)));
}

// quantize to a specific scale register
int scale(int p, int o)
{
    // minor scale
    int s[12] = {0, 0, 2, 3, 3, 5, 5, 7, 8, 8, 10, 10};
    int oct = p/12;
    int pc = p%12;
    return(oct*12 + s[(pc+o)%12]-s[o]);
}
