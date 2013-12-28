//
//  CloudsSoundFuncs.cpp
//  CLOUDS
//
//  Created by R. Luke DuBois on 10/27/13.
//  Moved some shit into another file.
//
//

#include "CloudsSound.h"

void CloudsSound::startMusicFX(float outskip, float musicdur)
{
    // blow out routing table
    INITMIX();
    // REVERB
    REVERB(outskip, musicdur+7.0); // gimme some reverb
    // schedule end event at time + ring-down
    // SCHEDULEBANG(outskip+musicdur+7.0);
}

void CloudsSound::startMusic(float outskip, string mo, string arg_a, string arg_b, int mh, int mr, float musicdur, float bpm, float m_amp, float m_rev, int instnum)
{
    
    float t, beatoffset;
    float i, j;
    lukeSimpleMelody mel;
    
    // trap array shit...
    if(mr>rhythms.size()-1) mr=rhythms.size()-1;
    if(mr<0) mr=0;
    if(mh>pitches.size()-1) mh=pitches.size()-1;
    if(mh<0) mh=0;
    if(musicdur<0) musicdur=0;
    if(bpm<10) bpm=10;
    if(bpm>500) bpm=500;
	
    // some timing shit...
    t = ofGetElapsedTimef();
    float tempo = (15./bpm);
    float seqrate = 120./bpm;
    int bcount = 0;
    beatoffset = tempo-fmod(t,tempo); // use for accurate ahead-of-time quantization for rhythmic triggering
	
    //flush_sched(); // kill previous music
    
    outskip = outskip + beatoffset; // fix beat offset to get things in time
    
    if(LUKEDEBUG) cout << "   start: " << outskip << " " << "s, dur: " << musicdur << " " << "s, orch: " << mo << ", harmony: " << mh << ", rhythm: " << mr << ", amp: " << m_amp << ", rev: " << m_rev << endl;
    
    //
    // =========================
    // BEGIN ORCHESTRATION BLOCK
    // =========================
    //
    
    //
    // MELODIC INSTRUMENTS
    //
    
    // META
    if(arg_a=="simple")
    {
        mel = simplemelodies[ofToInt(arg_b)];
    }
    
    // SLOWWAVES
    if (mo=="slowwaves")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
        if(arg_a=="sequencer")
        {
            vector<lukeNote> n;
            cloudsSequencer(arg_b, n);
            for(int i = 0;i<n.size();i++)
            {
                if(n[i].starttime < musicdur)
                {
                    int pitch = scale(n[i].pitch, pitches[mh].scale);
                    WAVETABLE(outskip+n[i].starttime*seqrate, n[i].dur, n[i].velo*0.1, mtof(pitch), ofRandom(0.,1.), "wf_slowwaves", "amp_triangle");
                    WAVETABLE(outskip+n[i].starttime*seqrate, n[i].dur, n[i].velo*0.1, mtof(pitch)*0.99, ofRandom(0.,1.), "wf_slowwaves", "amp_triangle");
                }
            }
            
        }
        else
        {
            melodySolver m(arg_a, pitches[mh], mel);
            int curpitch;
            float freq;
            
            for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(4, 16)))
            {
                curpitch = m.tick();
                freq = mtof(curpitch);
                
                WAVETABLE(outskip+i, ofRandom(3., 10.), 0.025, freq, ofRandom(0.,1.), "wf_slowwaves", "amp_triangle");
                WAVETABLE(outskip+i, ofRandom(3., 10.), 0.025, freq*0.99, ofRandom(0.,1.), "wf_slowwaves", "amp_triangle");
            }
        }
        
    }
    
    // HARDBUZZ
    if (mo=="hardbuzz")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
        if(arg_a=="sequencer")
        {
            vector<lukeNote> n;
            cloudsSequencer(arg_b, n);
            for(int i = 0;i<n.size();i++)
            {
                if(n[i].starttime < musicdur)
                {
                    int pitch = scale(n[i].pitch, pitches[mh].scale);
                    WAVETABLE(outskip+n[i].starttime*seqrate, n[i].dur, n[i].velo*0.1, mtof(pitch), ofRandom(0.,1.), "vel_strike", "amp_triangle");
                    WAVETABLE(outskip+n[i].starttime*seqrate, n[i].dur, n[i].velo*0.1, mtof(pitch)*0.99, ofRandom(0.,1.), "vel_strike", "amp_triangle");
                }
            }
            
        }
        else
        {
            melodySolver m(arg_a, pitches[mh], mel);
            int curpitch;
            float freq;
            
            for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(4, 16)))
            {
                curpitch = m.tick();
                freq = mtof(curpitch);
                
                WAVETABLE(outskip+i, ofRandom(3., 10.), 0.025, freq, ofRandom(0.,1.), "vel_strike", "amp_triangle");
                WAVETABLE(outskip+i, ofRandom(3., 10.), 0.025, freq*0.99, ofRandom(0.,1.), "vel_strike", "amp_triangle");
            }
        }
        
    }

    // SLOWWAVESHI
    if (mo=="slowwaveshi")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;

        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch+12.);
            
            float d0 = ofRandom(2., 6.);
            float of1 = d0*ofRandom(0.3, 0.7);
            float d1 = d0+of1;
            float of2 = d1*ofRandom(0.3, 0.7);
            float d2 = d1+of1+of2;
            WAVETABLE(outskip+i, d0, 0.02, freq, ofRandom(0.,1.), "wf_slowwaveshi", "amp_triangle");
            WAVETABLE(outskip+i+of1, d1, 0.02, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_slowwaveshi", "amp_triangle");
            WAVETABLE(outskip+i+of2, d2, 0.02, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_slowwaveshi", "amp_triangle");
        }
        
    }
    
    // WAVESHIPATTERNED
    if (mo=="waveshipatterned")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;

        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                curpitch = m.tick();
                freq = mtof(curpitch+12.);

                float d0 = ofRandom(0.1, 0.5);
                float of1 = d0*ofRandom(0.3, 0.7);
                float d1 = d0+of1;
                float of2 = d1*ofRandom(0.3, 0.7);
                float d2 = d1+of1+of2;
                float amp = 0.02*rhythms[mr].beats[bcount];
                WAVETABLE(outskip+i, d0, amp, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                WAVETABLE(outskip+i+of1, d1, amp, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                WAVETABLE(outskip+i+of2, d2, amp, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }

    // LOWWAVEPULSE
    if (mo=="lowwavepulse")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        for(i = 0;i<musicdur;i+=tempo)
        {
            float p = 0.7;
            if(rhythms[mr].beats[bcount]>0.) {
                curpitch = m.tick();
                freq = mtof(curpitch)*0.5;
                
                float t_amp = rhythms[mr].beats[bcount]*0.05;
                
                WAVETABLE(outskip+i, ofRandom(0.1, 0.3), t_amp, freq, p, "wf_organ", "amp_sharpadsr");
                WAVETABLE(outskip+i, ofRandom(0.1, 0.3), t_amp, freq*1.5, 1.-p, "wf_organ", "amp_sharpadsr");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
            p = 1.0-p;
        }
        
    }
  
    // KISS MY ARP
    if (mo=="kissmyarp")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
        if(arg_a=="sequencer")
        {
            vector<lukeNote> n;
            cloudsSequencer(arg_b, n);
            for(int i = 0;i<n.size();i++)
            {
                if(n[i].starttime < musicdur)
                {
                    int pitch = scale(n[i].pitch, pitches[mh].scale);
                    WAVETABLE(outskip+n[i].starttime*seqrate, n[i].dur, n[i].velo*0.1, mtof(pitch), ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                    WAVETABLE(outskip+n[i].starttime*seqrate, n[i].dur*3., n[i].velo*0.05, mtof(pitch), ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                }
            }
            
        }
        else
        {
            SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
            melodySolver m(arg_a, pitches[mh], mel);
            int curpitch;
            float freq;

            for(i = 0;i<musicdur;i+=tempo*2)
            {
                int oct = ofRandom(0., 1.)*12;
                curpitch = m.tick();
                int pitch = curpitch % 12;
                pitch+=pitches[mh].basenote;
                pitch+=oct;
                pitch = scale(pitch, pitches[mh].scale);
                freq = mtof(pitch);
                WAVETABLE(outskip+i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                WAVETABLE(outskip+i+tempo*6, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            }
        }
    }
    
    // KISS MY SINE
    if (mo=="kissmysine")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
        if(arg_a=="sequencer")
        {
            vector<lukeNote> n;
            cloudsSequencer(arg_b, n);
            for(int i = 0;i<n.size();i++)
            {
                if(n[i].starttime < musicdur)
                {
                    int pitch = scale(n[i].pitch, pitches[mh].scale);
                    WAVETABLE(outskip+n[i].starttime*seqrate, n[i].dur, n[i].velo*0.1, mtof(pitch), ofRandom(0.,1.), "wf_puresine", "amp_sharpadsr");
                    WAVETABLE(outskip+n[i].starttime*seqrate, n[i].dur*3., n[i].velo*0.05, mtof(pitch), ofRandom(0.,1.), "wf_puresine", "amp_sharpadsr");
                }
            }
            
        }
        else
        {
            SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
            melodySolver m(arg_a, pitches[mh], mel);
            int curpitch;
            float freq;
            
            for(i = 0;i<musicdur;i+=tempo*2)
            {
                int oct = ofRandom(0., 1.)*12;
                curpitch = m.tick();
                int pitch = curpitch % 12;
                pitch+=pitches[mh].basenote;
                pitch+=oct;
                pitch = scale(pitch, pitches[mh].scale);
                freq = mtof(pitch);
                WAVETABLE(outskip+i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                WAVETABLE(outskip+i+tempo*6, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            }
        }
    }
    
    // KISS MY ARP SYNCH
    if (mo=="kissmyarpsynch")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        for(i = 0;i<musicdur;i+=tempo*2)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                int oct = ofRandom(0., 1.)*12;
                curpitch = m.tick();
                int pitch = curpitch % 12;
                pitch+=pitches[mh].basenote;
                pitch+=oct;
                pitch = scale(pitch, pitches[mh].scale);
                freq = mtof(pitch);
                WAVETABLE(outskip+i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                WAVETABLE(outskip+i+tempo*6, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }
    
    // KISS MY SINE SYNCH
    if (mo=="kissmysinesynch")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        for(i = 0;i<musicdur;i+=tempo*2)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                int oct = ofRandom(0., 1.)*12;
                curpitch = m.tick();
                int pitch = curpitch % 12;
                pitch+=pitches[mh].basenote;
                pitch+=oct;
                pitch = scale(pitch, pitches[mh].scale);
                freq = mtof(pitch);
                WAVETABLE(outskip+i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_puresine", "amp_sharpadsr");
                WAVETABLE(outskip+i+tempo*6, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_puresine", "amp_sharpadsr");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }
    
    // KISS MY ARP FAST
    if (mo=="kissmyarpfast")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        for(i = 0;i<musicdur;i+=tempo)
        {
            int oct = ofRandom(0., 2.)*12;
            curpitch = m.tick();
            int pitch = curpitch % 12;
            pitch+=pitches[mh].basenote;
            pitch+=oct;
            pitch = scale(pitch, pitches[mh].scale);
            freq = mtof(pitch);
            WAVETABLE(outskip+i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            WAVETABLE(outskip+i+tempo*3, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
        }
    }
    
    // KISS MY SINE FAST
    if (mo=="kissmysinefast")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVETABLE", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        for(i = 0;i<musicdur;i+=tempo)
        {
            int oct = ofRandom(0., 2.)*12;
            curpitch = m.tick();
            int pitch = curpitch % 12;
            pitch+=pitches[mh].basenote;
            pitch+=oct;
            pitch = scale(pitch, pitches[mh].scale);
            freq = mtof(pitch);
            WAVETABLE(outskip+i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_puresine", "amp_sharpadsr");
            WAVETABLE(outskip+i+tempo*3, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_puresine", "amp_sharpadsr");
        }
    }

    // WISH
    if (mo=="wish")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVESHAPE", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                curpitch = m.tick();
                freq = mtof(curpitch+12.);
                
                float d0 = ofRandom(0.1, 0.5);
                float of1 = d0*ofRandom(0.3, 0.7);
                float d1 = d0+of1;
                float of2 = d1*ofRandom(0.3, 0.7);
                float d2 = d1+of1+of2;
                float amp = 0.02*rhythms[mr].beats[bcount];

                WAVESHAPE(outskip+i, d0, amp, freq, ofRandom(0., 1.), "wf_puresine", "amp_sharpadsr", "ws_xfer", "ramp_01");
                WAVESHAPE(outskip+i+of1, d1, amp, freq*ofRandom(0.99, 1.01), ofRandom(0., 1.), "wf_puresine", "amp_sharpadsr", "ws_xfer", "ramp_01");
                WAVESHAPE(outskip+i+of2, d2, amp, freq*ofRandom(0.99, 1.01), ofRandom(0., 1.), "wf_puresine", "amp_sharpadsr", "ws_xfer", "ramp_01");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }

    // SLOWLOWWISH
    if (mo=="slowlowwish")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "WAVESHAPE", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(4, 16)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch-12.);
            
            float d0 = tempo*floor(ofRandom(8, 16));
            float of1 = d0*ofRandom(0.3, 0.7);
            float d1 = d0+of1;
            float of2 = d1*ofRandom(0.3, 0.7);
            float d2 = d1+of1+of2;
            float amp = 0.02;
            
            WAVESHAPE(outskip+i, d0, amp, freq, ofRandom(0., 1.), "wf_puresine", "amp_triangle", "ws_xfer", "ramp_010");
            WAVESHAPE(outskip+i+of1, d1, amp, freq*2., ofRandom(0., 1.), "wf_puresine", "amp_triangle", "ws_xfer", "ramp_10");
            WAVESHAPE(outskip+i+of2, d2, amp, freq*0.5, ofRandom(0., 1.), "wf_puresine", "amp_triangle", "ws_xfer", "ramp_01");
        }
    }
    
    // GRANSYNTH1
    if (mo=="granular_sparkles")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "GRANSYNTH", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;

        float freq_jitter = 1.;
        float grate = 0.1;
        float grate_var = 0.01;
        float gdur_min = 0.04;
        float gdur_max = 0.25;
        float gamp_min = 0.05;
        float gamp_max = 0.4;

        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch+12.);
            
            float d = tempo*ofRandom(8., 32.);
            GRANSYNTH(outskip+i, d, 0.1, freq, freq_jitter, grate, grate_var, gdur_min, gdur_max, gamp_min, gamp_max, 0., 1., "wf_puresine", "hanning", "transplist1");
        }
    }

    // GRANSYNTH2
    if (mo=="granular_rain")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "GRANSYNTH", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        float freq_jitter = 1;
        float grate = 0.01;
        float grate_var = 0.005;
        float gdur_min = 0.004;
        float gdur_max = 0.025;
        float gamp_min = 0.05;
        float gamp_max = 0.4;
        
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 16)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch);
            
            float d = tempo*ofRandom(12., 36.);
            GRANSYNTH(outskip+i, d, 0.1, freq, freq_jitter, grate, grate_var, gdur_min, gdur_max, gamp_min, gamp_max, 0., 1., "wf_slowwaveshi", "hanning", "transplist2");
        }
    }

    // GRANSYNTH3
    if (mo=="granular_shimmer")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "GRANSYNTH", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        float freq_jitter = 1.;
        float grate = 0.25;
        float grate_var = 0.05;
        float gdur_min = 0.04;
        float gdur_max = 0.25;
        float gamp_min = 0.05;
        float gamp_max = 0.4;

        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch+12.);
            
            float d = tempo*ofRandom(8., 24.);
            GRANSYNTH(outskip+i, d, 0.1, freq, freq_jitter, grate, grate_var, gdur_min, gdur_max, gamp_min, gamp_max, 0., 1., "wf_organ", "hanning", "transplist2");
        }
    }

    // GRANSYNTH4
    if (mo=="granular_whales")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "GRANSYNTH", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        float freq_jitter = 1.;
        float grate = 0.05;
        float grate_var = 0.01;
        float gdur_min = 0.1;
        float gdur_max = 0.25;
        float gamp_min = 0.05;
        float gamp_max = 0.4;

        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch);
            
            float d = tempo*ofRandom(12., 36.);
            GRANSYNTH(outskip+i, d, 0.1, freq, freq_jitter, grate, grate_var, gdur_min, gdur_max, gamp_min, gamp_max, 0., 1., "wf_puresine", "hanning", "transplist3");
        }
    }

    // GRANSYNTH5
    if (mo=="granular_pad")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "GRANSYNTH", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        float freq_jitter = 0.5;
        float grate = 0.1;
        float grate_var = 0.1;
        float gdur_min = 0.7;
        float gdur_max = 2.;
        float gamp_min = 0.05;
        float gamp_max = 0.4;
        
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch);
            
            float d = tempo*ofRandom(12., 36.);
            GRANSYNTH(outskip+i, d, 0.1, freq, freq_jitter, grate, grate_var, gdur_min, gdur_max, gamp_min, gamp_max, 0., 1., "wf_puresine", "hamming", "transplist1");
        }
    }
    
    // GRANSYNTH6
    if (mo=="granular_sparse")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "GRANSYNTH", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        float freq_jitter = 1;
        float grate = 0.1;
        float grate_var = 0.01;
        float gdur_min = 0.04;
        float gdur_max = 0.25;
        float gamp_min = 0.05;
        float gamp_max = 0.4;
        
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch);
            
            float d = tempo*ofRandom(4., 8.);
            GRANSYNTH(outskip+i, d, 0.1, freq, freq_jitter, grate, grate_var, gdur_min, gdur_max, gamp_min, gamp_max, 0., 1., "wf_puresine", "hamming", "transplist1");
        }
    }

    // GRANSYNTH7
    if (mo=="granular_buzz")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "GRANSYNTH", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        float freq_jitter = 1.;
        float grate = 0.25;
        float grate_var = 0.05;
        float gdur_min = 0.04;
        float gdur_max = 0.25;
        float gamp_min = 0.05;
        float gamp_max = 0.4;
        string pitchhandle = "ramp_01";
        string ratehandle = "ramp_12";
        string durhandle = "ramp_121";
        
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(4, 16)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch);
            
            float d = tempo*ofRandom(16., 48.);
            GRANSYNTH(outskip+i, d, 0.1, freq, freq_jitter, grate, grate_var, gdur_min, gdur_max, gamp_min, gamp_max, 0., 1., "vel_strike", "hanning", "transplist2",pitchhandle, ratehandle, durhandle);
        }
    }

    // GRANSYNTH8
    if (mo=="granular_swish")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "GRANSYNTH", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        float freq_jitter = 1.;
        float grate = 0.1;
        float grate_var = 0.05;
        float gdur_min = 0.01;
        float gdur_max = 0.2;
        float gamp_min = 0.05;
        float gamp_max = 0.4;
        string pitchhandle = "ramp_12";
        string ratehandle = "ramp_21";
        string durhandle = "ramp_12";
        
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(4, 16)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch);
            
            float d = tempo*ofRandom(16., 48.);
            GRANSYNTH(outskip+i, d, 0.1, freq, freq_jitter, grate, grate_var, gdur_min, gdur_max, gamp_min, gamp_max, 0., 1., "wf_puresine", "hanning", "transplist1",pitchhandle, ratehandle, durhandle);
        }
    }
    
    
    
    // PHYSICAL MODELS (MELODIC)
    
    // WAVEGUIDE
    if (mo=="waveguide")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "MBANDEDWG", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        int preset = 3;
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(6, 12)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch);
            MBANDEDWG(outskip+i, 2., ofRandom(0.05, 0.25), freq, ofRandom(0.,1.), ofRandom(0.,1.)>0.5, ofRandom(0.7, 1.0), preset, ofRandom(0.8, 1.), 0.99, 0., ofRandom(0.,1.), "vel_strike");
        }
    }
    
    // WAVEGUIDE2
    if (mo=="waveguide2")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "MBANDEDWG", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        int preset = 2;
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(6, 12)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch);
            MBANDEDWG(outskip+i, 2., ofRandom(0.05, 0.25), freq, ofRandom(0.,1.), ofRandom(0.,1.)>0.5, ofRandom(0.7, 1.0), preset, ofRandom(0.8, 1.), 0.99, 0., ofRandom(0.,1.), "vel_strike");
        }
    }

    // WAVEGUIDE
    if (mo=="waveguide3")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "MBANDEDWG", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        int preset = 1;
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(6, 12)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch);
            MBANDEDWG(outskip+i, 2., ofRandom(0.05, 0.25), freq, ofRandom(0.,1.), ofRandom(0.,1.)>0.5, ofRandom(0.7, 1.0), preset, ofRandom(0.8, 1.), 0.99, 0., ofRandom(0.,1.), "vel_strike");
        }
    }

    // WAVEGUIDE
    if (mo=="waveguide4")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "MBANDEDWG", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        int preset = 0;
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(6, 12)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch);
            MBANDEDWG(outskip+i, 2., ofRandom(0.05, 0.25), freq, ofRandom(0.,1.), ofRandom(0.,1.)>0.5, ofRandom(0.7, 1.0), preset, ofRandom(0.8, 1.), 0.99, 0., ofRandom(0.,1.), "vel_strike");
        }
    }

    // HELMHOLTZ
    if (mo=="helmholtz")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "MBLOWBOTL", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(4, 16)))
        {
            curpitch = m.tick();
            freq = mtof(curpitch)*2.;
            MBLOWBOTL(outskip+i, ofRandom(1., 3.), 0.25, freq, ofRandom(0.05, 0.2), ofRandom(0.5, 0.9), ofRandom(0.,1.), "amp_sharp", "amp_triangle");
            MBLOWBOTL(outskip+i+tempo*floor(ofRandom(0, 4)), ofRandom(1., 3.), 0.25, freq*1.5, ofRandom(0.01, 0.08), ofRandom(0.5, 0.9), ofRandom(0.,1.), "amp_sharp", "amp_triangle");
        }
        
    }
    
    // FILTERNOISE
    if (mo=="filternoise")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0, "FILTERBANK", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            curpitch = m.tick();
            float freq = mtof(curpitch)*2.;
            FNOISE3(outskip+i, ofRandom(1., 3.), 0.25, 1.0, ofRandom(0.,1.), freq, freq*2.0, freq*3.0, 90., "amp_triangle");
        }
     }
    
    // STRUMECHO
    if (mo=="strumecho")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0.5, "STRUM2", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        for(i = 0;i<musicdur;i+=tempo*4.0)
        {
            curpitch = m.tick();
            float freq = mtof(curpitch);
            STRUM(outskip+i, 1., 0.5*m_amp, freq, ofRandom(1.0, 5.0), ofRandom(1.,5.), ofRandom(1.0));
        }
        // repatch
        PANECHO(outskip, 0, musicdur+5.0, 1., tempo*3., tempo*4., 0.5, 2.);
    }

    // STRUMSINE
    if (mo=="strumsine")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0.5, "WAVETABLE", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            curpitch = m.tick();
            float pitch = curpitch;
            WAVETABLE(outskip+i, 2., 0.025, mtof(pitch), ofRandom(1.0), "wf_organ", "amp_sharpadsr");
            for(j=0;j<tempo;j+=(tempo/floor(ofRandom(4,8))))
            {
                WAVETABLE(outskip+i+j, 1., 0.05, mtof(pitch), ofRandom(1.0), "wf_puresine", "amp_sharphold");
                int tr = ofRandom(0, 5);
                if(tr==0) pitch+=7;
            }
        }
        PANECHO(outskip, 0., musicdur+5., 1., tempo*4., tempo*6., 0.7, 7.);
        
    }
    
    // OTHERS
    
    //
    // PITCHED RHYTHM
    //
    
    // MODALBEATS
    if (mo=="modalbeats")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0., "MMODALBAR", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        float amp;
        for(i = 0;i<musicdur;i+=tempo)
        {
            amp = ofRandom(rhythms[mr].beats[bcount]);
            if(amp>0.5) {
                curpitch = m.tick();
                freq = mtof(curpitch);
                
                MMODALBAR(outskip+i, 1., amp*0.2, freq, ofRandom(0.1,0.9), ofRandom(0.,1.), int(ofRandom(8))    );
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
        
    }
    
    // WAVEGUIDEBEATS
    if (mo=="waveguidebeats")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0., "MBANDEDWG", instnum);
        melodySolver m(arg_a, pitches[mh], mel);
        int curpitch;
        float freq;
        float amp;
        int preset = 3;
        for(i = 0;i<musicdur;i+=tempo)
        {
            amp = ofRandom(rhythms[mr].beats[bcount]);
            if(amp>0.5) {
                curpitch = m.tick();
                freq = mtof(curpitch);
                MBANDEDWG(outskip+i, ofRandom(0.05, 0.5), amp*0.15*m_amp, freq, ofRandom(0.,1.), ofRandom(0.,1.)>0.5, ofRandom(0.7, 1.0), preset, ofRandom(0.8, 1.), 0.99, 0., ofRandom(0.,1.), "vel_strike");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
        
    }

    //
    // UNPITCHED RHYTHM
    //
    
    // MESHBEATS
    if (mo=="meshbeats")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0., "MMESH2D", instnum);
        rhythmSolver r(arg_a, arg_b, rhythms[mr]);
        for(i=0;i<musicdur;i+=tempo)
        {
            if(r.tick())
            {
                float t_amp = 0.38;
                int nx = ofRandom(2,12);
                int ny = ofRandom(2,12);
                MMESH2D(outskip+i, 1., t_amp*0.5*m_amp, nx, ny, ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.));
            }
        }
        
    }
    
    // SLOWMESHBEATS
    if (mo=="slowmeshbeats")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0., "MMESH2D", instnum);
        rhythmSolver r(arg_a, arg_b, rhythms[mr]);
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            if(r.tick())
            {
                float t_amp = 0.38;
                int nx = ofRandom(2,12);
                int ny = ofRandom(2,12);
                MMESH2D(outskip+i, 1., t_amp*0.5*m_amp, nx, ny, ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.));
            }
        }
        
    }

    // VERMONTBEATZ
    if (mo=="vermontbeatz")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0., "STEREO", instnum);
        rhythmSolver r(arg_a, arg_b, rhythms[mr]);
        int pick;
        for(i = 0;i<musicdur;i+=tempo)
        {
            if(r.tick())
            {

                pick = ofRandom(0,5);
                if(pick==1) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "BD1");
                else if(pick==2) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "BD2");
                else if(pick==3) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "BD3");
                else if(pick==4) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "BD4");
            }
        }
    }

    // SLOWVERMONTBEATZ
    if (mo=="slowvermontbeatz")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0., "STEREO", instnum);
        rhythmSolver r(arg_a, arg_b, rhythms[mr]);
        int pick;
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            if(r.tick())
            {
                
                pick = ofRandom(0,5);
                if(pick==1) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "BD1");
                else if(pick==2) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "BD2");
                else if(pick==3) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "BD3");
                else if(pick==4) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "BD4");
            }
        }
    }

    // PHATBEATZ
    if (mo=="phatbeatz")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0., "STEREO", instnum);
        int pick;
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                
                float t_amp = rhythms[mr].beats[bcount]*ofRandom(0.1, 0.2);
                pick = (int)ofRandom(0, 3);
                if(pick<2) STEREO(outskip+i, 0., 0.5, t_amp*m_amp, 0.5, "BD1"); else STEREO(outskip+i, 0., 0.5, t_amp, 0.5, "SD");
            }
            pick = ofRandom(0,4);
            if(pick<2) STEREO(outskip+i, 0., 0.5, ofRandom(0.05, 0.2)*m_amp, 0.5, "CH");
            else if(pick==2) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2)*m_amp, 0.5, "OH");
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }
    
    //
    // BEATS (SAMPLERS)
    //
    
    // AVIBEATS
    if (mo=="avibeats")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0., "TRANS3", instnum);
        cout << "Sample number: " << looperSamples.size() << endl;
        for(i = 0;i<looperSamples.size();i++)
        {
            if(looperSamples[i].bank==arg_a)
            {
                cout << "playing: " << looperSamples[i].handle << endl;
                int pptr = 0; // pattern pointer
                for(j = 0;j<musicdur;j+=tempo*looperSamples[i].numbeats*2.0)
                {
                    if(looperSamples[i].pattern.size()>0) // trap for missing patterns
                    {
                        if(looperSamples[i].pattern[pptr]=="A") { // first half
                            SOUNDLOOP(outskip+j, 0, looperSamples[i].length/2.0, tempo*looperSamples[i].numbeats*2, 0.25*m_amp, looperSamples[i].handle);
                        }
                        else if(looperSamples[i].pattern[pptr]=="B") { // second half
                            SOUNDLOOP(outskip+j, looperSamples[i].length/2.0, looperSamples[i].length/2.0, tempo*looperSamples[i].numbeats*2.0, 0.25*m_amp, looperSamples[i].handle);
                        }
                        pptr = (pptr+1) % looperSamples[i].pattern.size();
                    }
                }
            }
        }
    }

    //
    // UNUSED
    //
    
    // REICHOMATIC
    if (mo=="reichomatic")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0., "TRANS3", instnum);
        cout << "Sample number: " << looperSamples.size() << endl;
        for(i = 0;i<looperSamples.size();i++)
        {
            if(looperSamples[i].bank==arg_a)
            {
                cout << "Sample: " << looperSamples[i].handle << endl;
                for(j = 0;j<musicdur;j+=tempo*looperSamples[i].numbeats*4)
                {
                    int p = ofRandom(0, 3);
                    if(p<1)
                    {
                        cout << "Playing: " << looperSamples[i].handle << " of length " << looperSamples[i].length << " at " << j << endl;
                        SOUNDLOOPMONO(outskip+j, looperSamples[i].length, tempo*looperSamples[i].numbeats*4, 0.25*m_amp, looperSamples[i].handle, ofRandom(0.,1));
                    }
                }
            }
        }
    }
    
    // GLASSOMATIC
    if (mo=="glassomatic")
    {
        SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0., "TRANS3", instnum);
        cout << "Sample number: " << looperSamples.size() << endl;
        for(i = 0;i<looperSamples.size();i++)
        {
            if(looperSamples[i].bank==arg_b)
            {
                for(j = 0;j<musicdur;j+=tempo*looperSamples[i].numbeats*4)
                {
                    SOUNDLOOPMONO(outskip+j, looperSamples[i].length, tempo*looperSamples[i].numbeats*4, 0.25*m_amp, looperSamples[i].handle, (float)i/looperSamples.size()-1);
                }
            }
        }
    }
    
    // UTILITY
    if (mo=="simplesound")
    {
        //SETUPMIX(outskip, musicdur, m_amp, 1.0-m_rev, m_rev, 0., "STEREO", instnum);
        STREAMSOUND(arg_b, musicdur, 1.*m_amp, backupsound);
    }

    //
    // =======================
    // END ORCHESTRATION BLOCK
    // =======================
    //
    
    rtc_playing = true;
        
}

void CloudsSound::stopMusic()
{
    flush_sched();
/*    char thebuf [256];
    int bx;
    bx = snprintf(thebuf, 256, "exit()");
    parse_score(thebuf, bx);
 */
    rtc_playing = false;
    
    backupsound.stop();
    backupsound.unloadSound();
}
