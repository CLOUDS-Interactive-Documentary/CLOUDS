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



// --- music functions ---

// load rhythm array
void loadrhythms(string f, vector<lukeRhythm>& r)
{
    string sline;
    ofFile rfile (GetCloudsDataPath()+"sound/"+f);
    if(!rfile.exists())
    {
        ofLogError("no data file!");
    }
    ofBuffer rbuf(rfile);
    r.clear();
    while(!rbuf.isLastLine())
    {
        sline=rbuf.getNextLine();
        lukeRhythm foo;
        vector<string> temp = ofSplitString(sline, " ");
        for(int i = 0;i<temp.size();i++)
        {
            foo.beats.push_back(ofToFloat(temp[i]));
        }
        r.push_back(foo);
    }
}

// load pitch array
void loadpitches(string f, vector<lukePitchArray>& p)
{
    string sline;
    ofFile pfile (GetCloudsDataPath()+"sound/"+f);
    if(!pfile.exists())
    {
        ofLogError("no data file!");
    }
    ofBuffer pbuf(pfile);
    p.clear();
    while(!pbuf.isLastLine())
    {
        sline=pbuf.getNextLine();
        lukePitchArray foo;
        vector<string> temp = ofSplitString(sline, " ");
        foo.basenote = ofToInt(temp[0]);
        foo.scale = ofToInt(temp[1]);
        for(int i = 2;i<temp.size();i++)
        {
            foo.notes.push_back(ofToInt(temp[i]));
        }
        p.push_back(foo);
    }
}

// load presets (NEW)
void loadpresets_xml(string f, vector<lukePreset>& p)
{
    ofxXmlSettings thestuff;

    cout << "=============" << endl;
    cout << "SCORE PRESETS:" << endl;
    cout << "=============" << endl;

    
    if(thestuff.loadFile(GetCloudsDataPath()+"sound/"+f))
    {
        thestuff.pushTag("pattrstorage");
        int numpresets = thestuff.getNumTags("slot");
        for(int i = 0;i<numpresets;i++)
        {
            lukePreset foo;
            foo.instruments.resize(5);
            foo.arg_a.resize(5);
            foo.arg_b.resize(5);
            foo.dichomin.resize(8);
            foo.dichomax.resize(8);
            thestuff.pushTag("slot", i);
            int numfields = thestuff.getNumTags("pattr");
            for(int j = 0;j<numfields;j++)
            {
                string pat = thestuff.getAttribute("pattr", "name", "foo", j);
                if(pat=="pname") {
                    foo.name = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="harmony") {
                    foo.harmony = thestuff.getAttribute("pattr", "value", 0, j);
                }
                else if(pat=="rhythm") {
                    foo.rhythm = thestuff.getAttribute("pattr", "value", 0, j);
                }
                else if(pat=="tempo") {
                    foo.tempo = thestuff.getAttribute("pattr", "value", 0, j);
                }
                else if(pat=="orch1") {
                    foo.instruments[0] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="orch2") {
                    foo.instruments[1] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="orch3") {
                    foo.instruments[2] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="orch4") {
                    foo.instruments[3] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="orch5") {
                    foo.instruments[4] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="arg_a1") {
                    foo.arg_a[0] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="arg_a2") {
                    foo.arg_a[1] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="arg_a3") {
                    foo.arg_a[2] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="arg_a4") {
                    foo.arg_a[3] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="arg_a5") {
                    foo.arg_a[4] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="arg_b1") {
                    foo.arg_b[0] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="arg_b2") {
                    foo.arg_b[1] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="arg_b3") {
                    foo.arg_b[2] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="arg_b4") {
                    foo.arg_b[3] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="arg_b5") {
                    foo.arg_b[4] = thestuff.getAttribute("pattr", "value", "foo", j);
                }
                else if(pat=="d1") {
                    string d = thestuff.getAttribute("pattr", "value", "foo", j);
                    foo.dichomin[0] = ofToInt(ofSplitString(d, " ")[0])-5;
                    foo.dichomax[0] = ofToInt(ofSplitString(d, " ")[1])-5;
                }
                else if(pat=="d2") {
                    string d = thestuff.getAttribute("pattr", "value", "foo", j);
                    foo.dichomin[1] = ofToInt(ofSplitString(d, " ")[0])-5;
                    foo.dichomax[1] = ofToInt(ofSplitString(d, " ")[1])-5;
                }
                else if(pat=="d3") {
                    string d = thestuff.getAttribute("pattr", "value", "foo", j);
                    foo.dichomin[2] = ofToInt(ofSplitString(d, " ")[0])-5;
                    foo.dichomax[2] = ofToInt(ofSplitString(d, " ")[1])-5;
                }
                else if(pat=="d4") {
                    string d = thestuff.getAttribute("pattr", "value", "foo", j);
                    foo.dichomin[3] = ofToInt(ofSplitString(d, " ")[0])-5;
                    foo.dichomax[3] = ofToInt(ofSplitString(d, " ")[1])-5;
                }
                else if(pat=="d5") {
                    string d = thestuff.getAttribute("pattr", "value", "foo", j);
                    foo.dichomin[4] = ofToInt(ofSplitString(d, " ")[0])-5;
                    foo.dichomax[4] = ofToInt(ofSplitString(d, " ")[1])-5;
                }
                else if(pat=="d6") {
                    string d = thestuff.getAttribute("pattr", "value", "foo", j);
                    foo.dichomin[5] = ofToInt(ofSplitString(d, " ")[0])-5;
                    foo.dichomax[5] = ofToInt(ofSplitString(d, " ")[1])-5;
                }
                else if(pat=="d7") {
                    string d = thestuff.getAttribute("pattr", "value", "foo", j);
                    foo.dichomin[6] = ofToInt(ofSplitString(d, " ")[0])-5;
                    foo.dichomax[6] = ofToInt(ofSplitString(d, " ")[1])-5;
                }
                else if(pat=="d8") {
                    string d = thestuff.getAttribute("pattr", "value", "foo", j);
                    foo.dichomin[7] = ofToInt(ofSplitString(d, " ")[0])-5;
                    foo.dichomax[7] = ofToInt(ofSplitString(d, " ")[1])-5;
                }
            }
            
            thestuff.popTag();
            // strip blank instruments
            for(int j =foo.instruments.size()-1;j>=0;j--)
            {
                if(foo.instruments[j]=="off")
                {
                    foo.instruments.erase(foo.instruments.begin()+j);
                    foo.arg_a.erase(foo.arg_a.begin()+j);
                    foo.arg_b.erase(foo.arg_b.begin()+j);
                }
            }
            cout << "PRESET XML " << i << ": " << foo.name << endl;
            for(int j =0;j<foo.instruments.size();j++)
            {
                cout << "   orchestration: " << foo.instruments[j] << endl;
                cout << "      arg_a: " << foo.arg_a[j] << endl;
                cout << "      arg_b: " << foo.arg_b[j] << endl;
            }
            cout << "   harmony: " << foo.harmony << endl;
            cout << "   rhythm: " << foo.rhythm << endl;
            cout << "   tempo: " << foo.tempo << endl;
            cout << "   dichotomies:" << endl;
            cout << "      art vs. tech: " << foo.dichomin[0] << " to " << foo.dichomax[0] << endl;
            cout << "      emotional vs. logical: " << foo.dichomin[1] << " to " << foo.dichomax[1] << endl;
            cout << "      breakthrough vs. obstacle: " << foo.dichomin[2] << " to " << foo.dichomax[2] << endl;
            cout << "      inspiring vs. discouraging: " << foo.dichomin[3] << " to " << foo.dichomax[3] << endl;
            cout << "      fun vs. serious: " << foo.dichomin[4] << " to " << foo.dichomax[4] << endl;
            cout << "      sincere vs. ironic: " << foo.dichomin[5] << " to " << foo.dichomax[5] << endl;
            cout << "      mindblowing vs. mundane: " << foo.dichomin[6] << " to " << foo.dichomax[6] << endl;
            cout << "      rational vs. surreal: " << foo.dichomin[7] << " to " << foo.dichomax[7] << endl;

            p.push_back(foo); // add to preset list
        }
        thestuff.popTag();
    }
    
    cout << "====" << endl;
    cout << "DONE" << endl;
    cout << "====" << endl;

    

}

// load preset file - TEXT MODE, DEPRECATED
void loadpresets(string f, vector<lukePreset>& p)
{
    string sline;
    ofFile pfile (GetCloudsDataPath()+"sound/"+f);
    if(!pfile.exists())
    {
        ofLogError("no data file!");
    }
    ofBuffer pbuf(pfile);
    p.clear();
    while(!pbuf.isLastLine())
    {
        sline=pbuf.getNextLine();
        lukePreset foo;
        vector<string> temp = ofSplitString(sline, " ");
        for(int i = 0;i<temp.size()-4;i++)
        {
            foo.instruments.push_back(temp[i]);
        }
        foo.harmony = ofToInt(temp[temp.size()-4])-1;
        foo.rhythm = ofToInt(temp[temp.size()-3])-1;
        foo.tempo = ofToFloat(temp[temp.size()-2]);
        foo.bank = temp[temp.size()-1];
        p.push_back(foo);
    }
}

// ====================
// MISC MUSIC FUNCTIONS
// ====================


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

// midi-to-string
string ptos(int p)
{
    string s = "";
    int pc = p%12;
    if(pc==0) s = "C";
    else if(pc==1) s = "C#";
    else if(pc==2) s = "D";
    else if(pc==3) s = "D#";
    else if(pc==4) s = "E";
    else if(pc==5) s = "F";
    else if(pc==6) s = "F#";
    else if(pc==7) s = "G";
    else if(pc==8) s = "G#";
    else if(pc==9) s = "A";
    else if(pc==10) s = "A#";
    else if(pc==11) s = "B";
    
    return(s);
}

// quantize to a specific scale register
int scale(int p, int o)
{
    // minor scale
    int basescale[12] = {0, 0, 2, 3, 3, 5, 5, 7, 8, 8, 10, 10};
    int s[12];
    if(o==-1)
    {
        for(int i = 0;i<12;i++)
        {
            s[i] = i;
        }
    }
    else
    {
        for(int i = 0;i<12;i++)
        {
            s[i] = (basescale[(i-o+12)%12]+o)%12;
        }
    }
    
    int oct = p/12;
    int pc = p%12;
    return(oct*12 + s[pc]);
}

//
// MELODY SOLVERS
//

melodySolver::melodySolver(string c_type, lukePitchArray& c_p)
{
    type = c_type;
    parray = c_p;
    pick = 0;
    if(type=="markov") {
        pick = (int)ofRandom(0, parray.markov.size());
        curpitch = parray.mindex[pick];
    }
    if(type=="melody" || type=="static") pick = 0;
}

int melodySolver::tick()
{
    int rval;
    
    if(type=="bucket") pick = (int)ofRandom(0, parray.notes.size());
    if(type=="markov") {
        pick = markov(pick, parray);
        curpitch = parray.mindex[pick];
    }
    else
    {
        curpitch = parray.notes[pick];
    }
        
    if(type=="melody") pick = (pick+1)%parray.notes.size();
    
    rval = scale(curpitch+parray.basenote, parray.scale);
    return(rval);

}

//
// SEQUENCER
//

cloudsSequencer::cloudsSequencer(string f, vector<lukeNote>& n)
{
    string sline;
    ofFile seqfile (GetCloudsDataPath()+"sound/seqs/" + f);
    if(!seqfile.exists())
    {
        ofLogError("can't find sequence!");
    }
    ofBuffer seqbuf(seqfile);
    while(!seqbuf.isLastLine())
    {
        sline = seqbuf.getNextLine();
        vector<string> temp = ofSplitString(sline, " ");
        lukeNote foo;
        foo.starttime = ofToFloat(temp[0])/1000.;
        foo.pitch = ofToInt(temp[1]);
        foo.velo = (ofToFloat(temp[2])/128.);
        foo.dur = ofToFloat(temp[3])/1000.;
        //cout << foo.starttime << ": " << foo.pitch << " " << foo.velo << " " << foo.dur << endl;
        n.push_back(foo);
    }
}

//
// RHYTHM SOLVER
//

rhythmSolver::rhythmSolver(string c_type, string c_arg_b, lukeRhythm& c_r)
{
    type = c_type;
    arg_b = c_arg_b;
    rarray = c_r;
    if(c_type=="lsys") {
        lsys = new lindenSequencer(arg_b);
    }
    else lsys = new lindenSequencer();
    ptr = 0;
}

bool rhythmSolver::tick()
{
    bool rval;
    if(type=="lsys") rval = lsys->tick();
    else if(type=="pattern") rval = (ofRandom(rarray.beats[ptr]))>0.5;
    else if(type=="straight") rval = true;
    
    ptr = (ptr+1) % rarray.beats.size();
    
    return(rval);
}

//
// LINDENMAYER
//

lindenSequencer::lindenSequencer(string f)
{
    string sline;
    thestring = " ";
    ptr = 0;
    match = '*';
    
    string axiom;
    int generations;
    vector< vector<string> > rules;
    
    // load
    ofFile seqfile (GetCloudsDataPath()+"sound/lsys/" + f);
    if(!seqfile.exists())
    {
        ofLogError("can't find sequence!");
    }
    ofBuffer seqbuf(seqfile);
    // setup line
    sline = seqbuf.getNextLine();
    vector<string> sups = ofSplitString(sline, " ");
    axiom = sups[0];
    match = sups[1][0];
    generations = ofToInt(sups[2]);
    // rules
    while(!seqbuf.isLastLine())
    {
        sline = seqbuf.getNextLine();
        vector<string> temp = ofSplitString(sline, " ");
        rules.push_back(temp);
    }
    
    if(LUKEDEBUG) {
        cout << "lsys axiom: " << axiom << ", matching on: " << match << " for " << generations << " generations." << endl;
        for(int i = 0;i<rules.size();i++)
        {
            cout << "lsys: " << rules[i][0] + ": " << rules[i][1] << endl;
        }
    }
    
    // solve
    string instring, outstring;
    int i, j, k;
    int ismatching = 0;
    
    instring = axiom;
    for (int i=0;i<generations;i++) // 1 - loop through the generations
    {
        outstring = "";
        for (int j=0;j<instring.length();j++) // 2 - loop through the input string
        {
            ismatching = 0;
            for (int k=0;k<rules.size();k++) // 3 - loop through the rules
            {
                if (instring[j]==rules[k][0][0]) ismatching++;
                if (ismatching>0) {
                    outstring=outstring+rules[k][1];
                    k = rules.size();
                }
            }
            if (ismatching==0)
            {
                outstring=outstring+instring[j];
            }
        }
        instring = outstring; // we know we're doing this at the end
        if(LUKEDEBUG) cout << i << ": " << outstring << endl;
    }

    thestring = outstring;
}

lindenSequencer::lindenSequencer()
{
    // blank
    thestring = " ";
    ptr = 0;
}

int lindenSequencer::tick()
{
    bool rval = false;
    if(thestring[ptr]==match) rval = true;
    ptr = (ptr+1) % thestring.length();
    return(rval);
}

//
// MARKOV SHIT
//

// precompute markov chain for pitch array
void precomputemarkov(lukePitchArray& p)
{
    cout << "TEST MARKOV: " << p.notes.size() << endl;
    
    // step one - analyze
    int tabsize = 0;
    bool pass;
    for(int i =0;i<128;i++)
    {
        p.mindex[i]=0;
    }
    int indexed_sequence[p.notes.size()];
    for(int i=0;i<p.notes.size();i++){
        pass = false;
        for(int j=0;j<tabsize;j++)
        {
            if(p.mindex[j]==p.notes[i])
            {
                pass = true;
                indexed_sequence[i] = j; // get us out of here, we've seen this note before
            }
        }
        if(!pass) {
            p.mindex[tabsize]=p.notes[i];
            indexed_sequence[i]=tabsize;
            tabsize++;
        }
    }

    // step two: build our markov table
    int current, next;
    p.markov.resize(tabsize); // make us a probability table
    for(int i = 0; i< p.notes.size();i++)
    {
        current = indexed_sequence[i];
        next = indexed_sequence[(i+1)%p.notes.size()];
        
        p.markov[current].push_back(next);
    }
    
    
    // DEBUG
    cout << "markov table:" << endl;
    for(int i=0;i<p.markov.size();i++)
    {
        cout << "  " << i << ": ";
        for(int j = 0;j<p.markov[i].size();j++)
        {
            cout << p.markov[i][j] << " ";
        }
        cout << endl;
     
    }

    
    
}

// return a markov result
int markov(int current, lukePitchArray& p)
{
    int pick = int(ofRandom(p.markov[current].size()));
    int next = p.markov[current][pick];
    return(next);
}
