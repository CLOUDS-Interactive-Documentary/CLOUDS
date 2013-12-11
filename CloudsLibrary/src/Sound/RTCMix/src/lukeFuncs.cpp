//
//  lukeFuncs.cpp
//  RTCMIX proof of concept
//
//  Created by Roger DuBois on 5/28/13.
//  Copyright (c) 2013 Polytechnic Institute of NYU. All rights reserved.
//

#include <iostream>
#include "lukeFuncs.h"
#include "CloudsGlobal.h"

// LUKE FUNCTIONS

// load an RTcmix text score (.sco) and parse
void RTcmixParseScoreFile(string f)
{
    string thescore = "";
    
    ofFile sco (GetCloudsDataPath()+"sound/"+f);
    if(!sco.exists())
    {
        ofLogError("no score file!");
    }
    ofBuffer scobuf(sco);
    while(!scobuf.isLastLine())
    {
        thescore+=scobuf.getNextLine()+"\n";
    }
    parse_score((char*)thescore.c_str(), thescore.length());
}

// uses the SPLITTER() and MIX() and GVERB() instruments
void REVERB(double outskip, double time)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "SPLITTER(%f, 0.0, %f, 1., 0, 1., 0., 1., 0., 1., 0.)", outskip, time);
    parse_score(thebuf, bx);
    bx = snprintf(thebuf, 256, "SPLITTER(%f, 0.0, %f, 1., 1, 0., 1., 1., 0., 0., 1.)", outskip, time);
    parse_score(thebuf, bx);
    bx = snprintf(thebuf, 256, "MIX(%f, 0.0, %f, 1., 0, 1)", outskip, time);
    parse_score(thebuf, bx);
    //bx = snprintf(thebuf, 256, "GVERB(%f, 0.0, %f, 1.0, 50., 8., 0.5, 0.1, -90., -9., -9., 3.0)", outskip, time);
    //bx = snprintf(thebuf, 256, "GVERB(%f, 0.0, %f, 1.0, 150., 8., 0.5, 1.0, -90., -18., -9., 3.0)", outskip, time);
    bx = snprintf(thebuf, 256, "GVERB(%f, 0.0, %f, 1.0, 50., 4., 0.9, 0.2, -90., -24., -18., 3.0)", outskip, time);
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

// loads an audio file into RAM as a buffer handle
float LOADSOUND(string file, string handle)
{
    ofVideoPlayer playa;
    playa.loadMovie(file);
    float duration = playa.getDuration();
    playa.close();
    
    OF_buffer_load_set((char*)file.c_str(), (char*)handle.c_str(), 0., 30.);
    // you can now use the buffer name (bname) in rtinput("MMBUF", "buffername")
    
    printf("LOADED SOUND %s: file: %s  duration: %f\n", (char*)handle.c_str(),
           (char*)file.c_str(), duration);
    return(duration);
}

// basic soundfile mixing interface
void STEREO(double outskip, double inskip, double dur, double amp, double pan, string handle)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "rtinput(\"MMBUF\", \"%s\")", (char*)handle.c_str());
    parse_score(thebuf, bx);
    bx = snprintf(thebuf, 256, "STEREO(%f, %f*DUR(), %f, %f*amp_declick, %f)", outskip, inskip, dur, amp, pan);
    parse_score(thebuf, bx);
    
}

// loop a sound (transposition auto-corrected based on ideal length)
void SOUNDLOOP(double outskip, double loopdur, double looplen, double amp, string handle)
{
    float incr = loopdur/looplen;
    float freq = mtof(60);
    freq = freq*incr;
    double newp = ftom(freq, 440.);
    double tp = newp-60.;
    int oct = tp/12;
    double pc = (fmod(tp,12.))*0.01;
    double transp = oct+pc;
    
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "rtinput(\"MMBUF\", \"%s\")", (char*)handle.c_str());
    parse_score(thebuf, bx);
    bx = snprintf(thebuf, 256, "TRANS3(%f, 0., %f, %f*amp_declick, %f, 0, 0)", outskip, looplen, amp, transp);
    parse_score(thebuf, bx);
    bx = snprintf(thebuf, 256, "TRANS3(%f, 0., %f, %f*amp_declick, %f, 1, 1)", outskip, looplen, amp, transp);
    parse_score(thebuf, bx);
    
}

// loop a sound in, well, mono
void SOUNDLOOPMONO(double outskip, double loopdur, double looplen, double amp, string handle, double pan)
{
    float incr = loopdur/looplen;
    float freq = mtof(60);
    freq = freq*incr;
    double newp = ftom(freq, 440.);
    double tp = newp-60.;
    int oct = tp/12;
    double pc = (fmod(tp,12.))*0.01;
    double transp = oct+pc;
    
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "rtinput(\"MMBUF\", \"%s\")", (char*)handle.c_str());
    parse_score(thebuf, bx);
    bx = snprintf(thebuf, 256, "TRANS3(%f, 0., %f, %f*amp_declick, %f, 0, %f)", outskip, looplen, amp, transp, pan);
    parse_score(thebuf, bx);
    
}


void PANECHO(double outskip, double inskip, double dur, double amp, double leftdelay, double rightdelay, double feedback, double ringdown)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "PANECHO(%f, %f, %f, %f, %f, %f, %f, %f, 0)", outskip, inskip, dur, amp, leftdelay, rightdelay, feedback, ringdown);
    parse_score(thebuf, bx);
    bx = snprintf(thebuf, 256, "PANECHO(%f, %f, %f, %f, %f, %f, %f, %f, 1)", outskip, inskip, dur, amp, rightdelay, leftdelay, feedback, ringdown);
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

// helmholtz resonator
void MBLOWBOTL(double outskip, double dur, double amp, double freq, double noiseamp, double maxpressure, double pan, string pressureenv, string ampenvelope)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "MBLOWBOTL(%f, %f, %f*%s, %f, %f, %f, %f, %s)", outskip, dur, amp*MAXAMP, (char*)ampenvelope.c_str(), freq, noiseamp, maxpressure, pan, (char*)pressureenv.c_str());
    parse_score(thebuf, bx);
}

// 2D mesh from the STK
void MMESH2D(double outskip, double dur, double amp, int nxpoints, int nypoints, double xpos, double ypos, double decay, double strike, double pan)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "MMESH2D(%f, %f, %f, %i, %i, %f, %f, %f, %f, %f)", outskip, dur, amp*MAXAMP, nxpoints, nypoints, xpos, ypos, decay, strike, pan);
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

// three-pitch filtered noise
void FNOISE3(double outskip, double dur, double amp, double ringdown, double pan, double f1, double f2, double f3, double Q, string ampenvelope)
{
    double bw1 = 1.0/Q;
    double bw2 = 1.0/Q;
    double bw3 = 1.0/Q;
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "NOISE(%f, %f, %f*%s, 1)", outskip, dur, MAXAMP*amp, (char*)ampenvelope.c_str());
    parse_score(thebuf, bx);
    bx = snprintf(thebuf, 256, "FILTERBANK(%f, 0, %f, %f, %f, 0, %f, %f, %f, 1., %f, %f, 1., %f, %f, 1.)", outskip, dur, 0.1, ringdown, pan, f1, bw1, f2, bw2, f3, bw3);
    parse_score(thebuf, bx);
    
}

// banded waveguide
void MBANDEDWG(double outskip, double dur, double amp, double freq, double strikepos, int pluckflag, double maxvel, int preset, double bowpressure, double resonance, double integration, double pan, string velocityenvelope)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "MBANDEDWG(%f, %f, %f, %f, %f, %i, %f, %i, %f, %f, %f, %f, %s)", outskip, dur, amp*MAXAMP, freq, strikepos, pluckflag, maxvel, preset, bowpressure, resonance, integration, pan, (char*)velocityenvelope.c_str());
    parse_score(thebuf, bx);
}

