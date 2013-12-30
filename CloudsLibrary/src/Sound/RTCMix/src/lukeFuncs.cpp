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

void INITMIX()
{
    RTcmixParseScoreFile("cmixclear.sco");
}

// use the SPLITTER() instrument to configure effects path
void SETUPMIX(double outskip, double time, double amp, double dry, double verb, double echo, string inst, int auxbus, string ampcurve)
{
    char thebuf [256];
    int bx;
    
    // string format the auxiliary bus numbers
    int abl = auxbus*2 + 20;
    int abr = auxbus*2 + 21;
    string output = "aux " + ofToString(abl) + "-" + ofToString(abr) + " out";
    string input = "aux " + ofToString(abl) + "-" + ofToString(abr) + " in";
    // do the bus_config() calls
    
    // do the instrument bus_config()
    if(inst=="FILTERBANK") {
        bx = snprintf(thebuf, 256, "bus_config(\"%s\", \"aux 8-9 in\", \"%s\")", (char*)inst.c_str(), (char*)output.c_str());
    }
    else if(inst=="STEREO" || inst=="TRANS3")
    {
        bx = snprintf(thebuf, 256, "bus_config(\"%s\", \"in 0\", \"%s\")", (char*)inst.c_str(), (char*)output.c_str());
    }
    else
    {
        bx = snprintf(thebuf, 256, "bus_config(\"%s\", \"%s\")", (char*)inst.c_str(), (char*)output.c_str());
    }
    parse_score(thebuf, bx);
    
    // do the SPLITTER bus_config()
    bx = snprintf(thebuf, 256, "bus_config(\"SPLITTER\", \"%s\", \"aux 2-3 out\", \"aux 4-5 out\", \"aux 6-7 out\")", (char*)input.c_str());
    parse_score(thebuf, bx);

    // do the SPLITTER() calls
    bx = snprintf(thebuf, 256, "SPLITTER(%f, 0.0, %f, %f*%s*e_DECLICK, 0, %f, 0., %f, 0., %f, 0.)", outskip, time, amp, (char*)ampcurve.c_str(), dry, verb, echo);
    parse_score(thebuf, bx);
    bx = snprintf(thebuf, 256, "SPLITTER(%f, 0.0, %f, %f*%s*e_DECLICK, 1, 0., %f, %f, 0., 0., %f)", outskip, time, amp, (char*)ampcurve.c_str(), dry, verb, echo);
    parse_score(thebuf, bx);
    
}

// uses the SPLITTER() and MIX() and GVERB() instruments
void REVERB(double outskip, double time)
{
    char thebuf [256];
    int bx;
    
    // DRY MIX
    bx = snprintf(thebuf, 256, "MIX(%f, 0.0, %f, 1., 0, 1)", outskip, time);
    parse_score(thebuf, bx);
    
    // WET MIX
    bx = snprintf(thebuf, 256, "GVERB(%f, 0.0, %f, 1.0, 75., 8., 0.2, 1., -90., -15., -12., 3.0)", outskip, time);
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

// play an audio file from DISK
void STREAMSOUND(double outskip, string file, double dur, double amp)
{
    
    char thebuf [256];
    int bx;
    string p = GetCloudsDataPath() + "sound/trax/";
    ofDirectory sdir(p);
    
    string f = sdir.getAbsolutePath()+"/"+file;
    bx = snprintf(thebuf, 256, "rtinput(\"%s\")", (char*)f.c_str());
    parse_score(thebuf, bx);
    if(dur<0)
    {
        bx = snprintf(thebuf, 256, "STEREO(%f, 0., DUR(), %f*amp_declick, 0, 1)", outskip, amp);
    }
    else
    {
        bx = snprintf(thebuf, 256, "STEREO(%f, 0., %f, %f*amp_declick, 0, 1)", outskip, dur, amp);
    }
    parse_score(thebuf, bx);
}

// play an audio file from DISK with PFIELD UPDATING
void STREAMSOUND_DYNAMIC(double outskip, string file, double amp, string pvar, int updatenr)
{
    char thebuf [256];
    int bx;
    string p = GetCloudsDataPath() + "sound/trax/";
    ofDirectory sdir(p);
    
    string f = sdir.getAbsolutePath()+"/"+file;

    // establish handle
    bx = snprintf(thebuf, 256, "%s = makeconnection(\"pfbus\", %d, 1.0)", (char*)pvar.c_str(), updatenr);
    parse_score(thebuf, bx);
    cout << thebuf << endl;
    // load file
    bx = snprintf(thebuf, 256, "rtinput(\"%s\")", (char*)f.c_str());
    parse_score(thebuf, bx);
    cout << thebuf << endl;
    
    // start sound
    bx = snprintf(thebuf, 256, "STEREO(%f, 0., DUR(), %f*%s, 0, 1)", outskip, amp, (char*)pvar.c_str());
    parse_score(thebuf, bx);
    cout << thebuf << endl;
   
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
    
    if(LUKEDEBUG) printf("LOADED SOUND %s: file: %s  duration: %f\n", (char*)handle.c_str(),
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

// soundfile mixing with TRANS3
void STEREO3(double outskip, double inskip, double dur, double amp, double pan, string handle)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "rtinput(\"MMBUF\", \"%s\")", (char*)handle.c_str());
    parse_score(thebuf, bx);
    bx = snprintf(thebuf, 256, "TRANS3(%f, %f, %f, %f*amp_declick, 0., 0, %f)", outskip, inskip, dur, amp, pan);
    parse_score(thebuf, bx);
    
}


// loop a sound (transposition auto-corrected based on ideal length)
void SOUNDLOOP(double outskip, double inskip, double loopdur, double looplen, double amp, string handle)
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
    bx = snprintf(thebuf, 256, "TRANS3(%f, %f, %f, %f*amp_declick, %f, 0, 0)", outskip, inskip, looplen, amp, transp);
    parse_score(thebuf, bx);
    bx = snprintf(thebuf, 256, "TRANS3(%f, %f, %f, %f*amp_declick, %f, 1, 1)", outskip, inskip, looplen, amp, transp);
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

// granular synth
void GRANSYNTH(double outskip, double dur, double amp, double freq, double freq_jitter, double grate, double grate_var, double gdur_min, double gdur_max, double gamp_min, double gamp_max, double gpan_min, double gpan_max, string waveform, string ampenvelope, string transphandle)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "GRANSYNTH(%f, %f, %f*amp_declick, %s, %s, %f, %f, %f, %f, %f, %f, %f, %s, %f, %f, %f, %f)", outskip, dur, amp*MAXAMP, (char*)waveform.c_str(), (char*)ampenvelope.c_str(), grate, grate_var, gdur_min, gdur_max, gamp_min, gamp_max, octcps(freq), (char*)transphandle.c_str(), freq_jitter, ofRandom(0, 1000.), gpan_min, gpan_max);
    parse_score(thebuf, bx);
    
}

// granular synth, overloaded
void GRANSYNTH(double outskip, double dur, double amp, double freq, double freq_jitter, double grate, double grate_var, double gdur_min, double gdur_max, double gamp_min, double gamp_max, double gpan_min, double gpan_max, string waveform, string ampenvelope, string transphandle, string pitchhandle, string ratehandle, string durhandle)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "GRANSYNTH(%f, %f, %f*amp_declick, %s, %s, %f*%s, %f*%s, %f*%s, %f*%s, %f, %f, %f, %s, %f*%s, %f, %f, %f)", outskip, dur, amp*MAXAMP, (char*)waveform.c_str(), (char*)ampenvelope.c_str(), grate, (char*)ratehandle.c_str(), grate_var, (char*)ratehandle.c_str(), gdur_min, (char*)durhandle.c_str(), gdur_max, (char*)durhandle.c_str(), gamp_min, gamp_max, octcps(freq), (char*)transphandle.c_str(), freq_jitter, (char*)pitchhandle.c_str(), ofRandom(RAND_MAX), gpan_min, gpan_max);
    parse_score(thebuf, bx);
}

// waveshaper
void WAVESHAPE(double outskip, double dur, double amp, double freq, double pan, string waveform, string ampenvelope, string xferfunc, string controlenv)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "WAVESHAPE(%f, %f, %f, 0., 1., %f*%s, %f, %s, %s, %s)", outskip, dur, freq, amp*MAXAMP, (char*)ampenvelope.c_str(), pan, (char*)waveform.c_str(), (char*)xferfunc.c_str(), (char*)controlenv.c_str());
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

// patch synthesizer
void PATCHSYNTH(string inst, string output)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "bus_config(\"%s\", \"%s\")", (char*)inst.c_str(), (char*)output.c_str());
    parse_score(thebuf, bx);
}

// patch effect
void PATCHFX(string inst, string input, string output)
{
    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "bus_config(\"%s\", \"%s\", \"%s\")", (char*)inst.c_str(), (char*)input.c_str(), (char*)output.c_str());
    parse_score(thebuf, bx);
}

// p-field updating
void PFIELD_SCHED(float outskip, float duration, int nr, string action)
{
    char thebuf [256];
    int bx;
    // BUG - the de-queue flag doesn't work as advertised; brad is investigating
    bx = snprintf(thebuf, 256, "PFSCHED(%f, %f, %d, %s, 1)", outskip, duration, nr, (char*)action.c_str());
    parse_score(thebuf, bx);
    cout << thebuf << endl;
}

