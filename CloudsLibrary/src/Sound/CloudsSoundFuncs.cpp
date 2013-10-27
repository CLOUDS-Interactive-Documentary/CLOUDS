//
//  CloudsSoundFuncs.cpp
//  CLOUDS
//
//  Created by R. Luke DuBois on 10/27/13.
//  Moved some shit into another file.
//
//

#include "CloudsSound.h"

void CloudsSound::startMusic(float outskip, vector<string> mo, int mh, int mr, float musicdur, float bpm, string samplebank)
{
    
    float t, beatoffset;
    float i, j;
	
    // some timing shit...
    t = ofGetElapsedTimef();
    float tempo = (15./bpm);
    int bcount = 0;
    beatoffset = tempo-fmod(t,tempo); // use for accurate ahead-of-time quantization for rhythmic triggering
	
    flush_sched(); // kill previous music
    
    vector<string> ilist = mo; // list of instruments
    
    
    cout << "===============" << endl;
    cout << "MAKING MUSIC!!!" << endl;
    cout << musicdur << " " << "seconds" << endl;
    cout << "harmony: " << mh << ", rhythm: " << mr << endl;
    cout << "orchestration:" << endl;
    for(int i = 0;i<ilist.size();i++)
    {
        cout << ilist[i] << endl;
    }
    cout << "===============" << endl;
    
    // REVERB
    REVERB(outskip, musicdur+7.0); // gimme some reverb
    
    //
    // =========================
    // BEGIN ORCHESTRATION BLOCK
    // =========================
    //
    
    // MODALBEATS
    if (find(ilist.begin(), ilist.end(), "modalbeats") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                
                float t_amp = (1.0-fabs((i/musicdur)-0.5)*2.)*rhythms[mr].beats[bcount];
                float pick = (int)ofRandom(0, pitches[mh].notes.size());
                float t_freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale));
                
                MMODALBAR(outskip+i, 1., t_amp*0.2, t_freq, ofRandom(0.1,0.9), ofRandom(0.,1.), int(ofRandom(8))    );
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
        
    }
    
    // WAVEGUIDEBEATS
    if (find(ilist.begin(), ilist.end(), "waveguidebeats") != ilist.end())
    {
        int preset = 3;
        for(i = 0;i<musicdur;i+=tempo)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                
                float t_amp = rhythms[mr].beats[bcount];
                float pick = (int)ofRandom(0, pitches[mh].notes.size());
                float t_freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale));
                
                MBANDEDWG(outskip+i, ofRandom(0.05, 0.5), t_amp*ofRandom(0.05, 0.15), t_freq, ofRandom(0.,1.), ofRandom(0.,1.)>0.5, ofRandom(0.7, 1.0), preset, ofRandom(0.8, 1.), 0.99, 0., ofRandom(0.,1.), "vel_strike");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
        
    }
    
    // LOWWAVEPULSE
    if (find(ilist.begin(), ilist.end(), "lowwavepulse") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo)
        {
            float p = 0.7;
            if(rhythms[mr].beats[bcount]>0.) {
                
                float t_amp = rhythms[mr].beats[bcount];
                float pick = (int)ofRandom(0, pitches[mh].notes.size());
                float t_freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale))*0.5;
                
                WAVETABLE(outskip+i, ofRandom(0.1, 0.3), 0.05, t_freq, p, "wf_organ", "amp_sharpadsr");
                WAVETABLE(outskip+i, ofRandom(0.1, 0.3), 0.05, t_freq*1.5, 1.-p, "wf_organ", "amp_sharpadsr");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
            p = 1.0-p;
        }
        
    }
    
    // MESHBEATS
    if (find(ilist.begin(), ilist.end(), "meshbeats") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo)
        {
            float t_amp = (1.0-fabs((i/musicdur)-0.5)*2.)*0.38;
            int nx = ofRandom(2,12);
            int ny = ofRandom(2,12);
            MMESH2D(outskip+i, 1., t_amp*0.5, nx, ny, ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.));
        }
        
    }
    
    // SLOWMESHBEATS
    if (find(ilist.begin(), ilist.end(), "slowmeshbeats") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            float t_amp = (1.0-fabs((i/musicdur)-0.5)*2.)*0.38;
            int nx = ofRandom(2,12);
            int ny = ofRandom(2,12);
            MMESH2D(outskip+i, 1., t_amp*0.5, nx, ny, ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.), ofRandom(0.,1.));
        }
        
    }
    
    // SLOWWAVES
    if (find(ilist.begin(), ilist.end(), "slowwaves") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(4, 16)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale));
            WAVETABLE(outskip+i, ofRandom(3., 10.), 0.025, freq, ofRandom(0.,1.), "wf_slowwaves", "amp_triangle");
            WAVETABLE(outskip+i, ofRandom(3., 10.), 0.025, freq*0.99, ofRandom(0.,1.), "wf_slowwaves", "amp_triangle");
        }
        
    }
    
    // SLOWWAVESHI
    if (find(ilist.begin(), ilist.end(), "slowwaveshi") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float d0 = ofRandom(2., 6.);
            float of1 = d0*ofRandom(0.3, 0.7);
            float d1 = d0+of1;
            float of2 = d1*ofRandom(0.3, 0.7);
            float d2 = d1+of1+of2;
            float freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote+12., pitches[mh].scale));
            WAVETABLE(outskip+i, d0, 0.02, freq, ofRandom(0.,1.), "wf_slowwaveshi", "amp_triangle");
            WAVETABLE(outskip+i+of1, d1, 0.02, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_slowwaveshi", "amp_triangle");
            WAVETABLE(outskip+i+of2, d2, 0.02, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_slowwaveshi", "amp_triangle");
        }
        
    }
    
    // WAVESHIPATTERNED
    if (find(ilist.begin(), ilist.end(), "waveshipatterned") != ilist.end())
    {
        int pick = 0;
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                float d0 = ofRandom(0.1, 0.5);
                float of1 = d0*ofRandom(0.3, 0.7);
                float d1 = d0+of1;
                float of2 = d1*ofRandom(0.3, 0.7);
                float d2 = d1+of1+of2;
                float freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote+12., pitches[mh].scale));
                WAVETABLE(outskip+i, d0, 0.02, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                WAVETABLE(outskip+i+of1, d1, 0.02, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                WAVETABLE(outskip+i+of2, d2, 0.02, freq*ofRandom(0.99, 1.01), ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                pick = (pick+1) % pitches[mh].notes.size();
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }
    
    
    // KISS MY ARP
    if (find(ilist.begin(), ilist.end(), "kissmyarp") != ilist.end())
    {
        int pick = 0;
        for(i = 0;i<musicdur;i+=tempo*2)
        {
            int oct = ofRandom(0., 1.)*12;
            int pitch = pitches[mh].notes[pick] % 12;
            pitch+=pitches[mh].basenote;
            pitch+=oct;
            pitch = scale(pitch, pitches[mh].scale);
            // cout << "doing pitch: " << ptos(pitch) << endl;
            float freq = mtof(pitch);
            WAVETABLE(outskip+i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            WAVETABLE(outskip+i+tempo*6, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            pick = (pick+1) % pitches[mh].notes.size();
        }
    }
    
    // KISS MY ARP SYNCH
    if (find(ilist.begin(), ilist.end(), "kissmyarpsynch") != ilist.end())
    {
        int pick = 0;
        for(i = 0;i<musicdur;i+=tempo*2)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                int oct = ofRandom(0., 1.)*12;
                int pitch = pitches[mh].notes[pick] % 12;
                pitch+=pitches[mh].basenote;
                pitch+=oct;
                pitch = scale(pitch, pitches[mh].scale);
                // cout << "doing pitch: " << ptos(pitch) << endl;
                float freq = mtof(pitch);
                WAVETABLE(outskip+i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                WAVETABLE(outskip+i+tempo*6, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
                pick = (pick+1) % pitches[mh].notes.size();
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }
    
    // KISS MY ARP FAST
    if (find(ilist.begin(), ilist.end(), "kissmyarpfast") != ilist.end())
    {
        int pick = 0;
        for(i = 0;i<musicdur;i+=tempo)
        {
            int oct = ofRandom(0., 2.)*12;
            int pitch = pitches[mh].notes[pick] % 12;
            pitch+=pitches[mh].basenote;
            pitch+=oct;
            pitch = scale(pitch, pitches[mh].scale);
            // cout << "doing pitch: " << ptos(pitch) << endl;
            float freq = mtof(pitch);
            WAVETABLE(outskip+i, tempo*1.5, 0.05, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            WAVETABLE(outskip+i+tempo*3, tempo*1.5, 0.025, freq, ofRandom(0.,1.), "wf_waveshi", "amp_sharphold");
            pick = (pick+1) % pitches[mh].notes.size();
        }
    }
    
    // HELMHOLTZ
    if (find(ilist.begin(), ilist.end(), "helmholtz") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(4, 16)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale))*2.;
            MBLOWBOTL(outskip+i, ofRandom(1., 3.), 0.25, freq, ofRandom(0.05, 0.2), ofRandom(0.5, 0.9), ofRandom(0.,1.), "amp_sharp", "amp_triangle");
            MBLOWBOTL(outskip+i+tempo*floor(ofRandom(0, 4)), ofRandom(1., 3.), 0.25, freq*1.5, ofRandom(0.01, 0.08), ofRandom(0.5, 0.9), ofRandom(0.,1.), "amp_sharp", "amp_triangle");
        }
        
    }
    
    // FILTERNOISE
    if (find(ilist.begin(), ilist.end(), "filternoise") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float freq = mtof(scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale))*2.;
            FNOISE3(outskip+i, ofRandom(1., 3.), 0.25, 1.0, ofRandom(0.,1.), freq, freq*2.0, freq*3.0, 90., "amp_triangle");
        }
        
    }
    
    // STRUMSINE
    if (find(ilist.begin(), ilist.end(), "strumsine") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(8, 32)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float pitch = scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale);
            WAVETABLE(outskip+i, 2., 0.025, mtof(pitch), ofRandom(1.0), "wf_organ", "amp_sharpadsr");
            for(j=0;j<tempo;j+=(tempo/floor(ofRandom(4,8))))
            {
                
                STRUM(outskip+i+j, 1.0, 0.05, mtof(pitch), ofRandom(1.0, 5.0), ofRandom(1.0, 5.0), ofRandom(1.0));
                int tr = ofRandom(0, 5);
                if(tr==0) pitch+=7;
            }
        }
        PANECHO(outskip, 0., musicdur+5., 1., tempo*4., tempo*6., 0.7, 7.);
        
    }
    
    // WAVEGUIDE
    if (find(ilist.begin(), ilist.end(), "waveguide") != ilist.end())
    {
        int preset = 3;
        for(i = 0;i<musicdur;i+=tempo*floor(ofRandom(6, 24)))
        {
            int pick = (int)ofRandom(0, pitches[mh].notes.size());
            float pitch = scale(pitches[mh].notes[pick]+pitches[mh].basenote, pitches[mh].scale);
            MBANDEDWG(outskip+i, ofRandom(7., 15.0), ofRandom(0.05, 0.15), mtof(pitch), ofRandom(0.,1.), ofRandom(0.,1.)>0.5, ofRandom(0.7, 1.0), preset, ofRandom(0.8, 1.), 0.99, 0., ofRandom(0.,1.), "vel_strike");
            
        }
    }
    
    // PHATBEATZ
    if (find(ilist.begin(), ilist.end(), "phatbeatz") != ilist.end())
    {
        int pick;
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            if(rhythms[mr].beats[bcount]>0.) {
                
                float t_amp = rhythms[mr].beats[bcount]*ofRandom(0.1, 0.2);
                pick = (int)ofRandom(0, 3);
                if(pick<2) STEREO(outskip+i, 0., 0.5, t_amp, 0.5, "BD"); else STEREO(outskip+i, 0., 0.5, t_amp, 0.5, "SD");
            }
            pick = ofRandom(0,4);
            if(pick<2) STEREO(outskip+i, 0., 0.5, ofRandom(0.05, 0.2), 0.5, "CH");
            else if(pick==2) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "OH");
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }
    
    // VERMONTBEATZ
    if (find(ilist.begin(), ilist.end(), "vermontbeatz") != ilist.end())
    {
        int pick;
        for(i = 0;i<musicdur;i+=tempo*2.)
        {
            if(rhythms[mr].beats[bcount]>0.) { // this is a note!
                float t_amp = rhythms[mr].beats[bcount]*ofRandom(0.1, 0.2);
                STEREO(outskip+i, 0., 0.5, t_amp, 0.5, "BD2");
            }
            else
            {
                pick = ofRandom(0,5);
                if(pick==1) STEREO(outskip+i, 0., 0.5, ofRandom(0.05, 0.2), 0.5, "VD1");
                else if(pick==2) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "VD2");
                else if(pick==3) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "VD3");
                else if(pick==4) STEREO(outskip+i, 0., 0.2, ofRandom(0.05, 0.2), 0.5, "VD4");
            }
            bcount = (bcount+1)%rhythms[mr].beats.size();
        }
    }
    
    // TESTLOOP1
    if (find(ilist.begin(), ilist.end(), "testloop1") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*64.)
        {
            SOUNDLOOP(outskip+i, tl1, tempo*64., 0.25, "testloop1");
        }
    }
    
    // TESTLOOP2
    if (find(ilist.begin(), ilist.end(), "testloop2") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*64.)
        {
            SOUNDLOOP(outskip+i, tl2, tempo*64., 0.25, "testloop2");
        }
    }
    
    // TESTLOOP3
    if (find(ilist.begin(), ilist.end(), "testloop3") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*64.)
        {
            SOUNDLOOP(outskip+i, tl3, tempo*64., 0.25, "testloop3");
        }
    }
    
    // BASSLOOP1
    if (find(ilist.begin(), ilist.end(), "bassloop1") != ilist.end())
    {
        for(i = 0;i<musicdur;i+=tempo*64)
        {
            SOUNDLOOP(outskip+i, bl1, tempo*64., 0.25, "bassloop1");
        }
    }
    
    // REICHOMATIC
    if (find(ilist.begin(), ilist.end(), "reichomatic") != ilist.end())
    {
        cout << "Sample number: " << looperSamples.size() << endl;
        for(i = 0;i<looperSamples.size();i++)
        {
            if(looperSamples[i].bank==samplebank)
            {
                cout << "Sample: " << looperSamples[i].handle << endl;
                for(j = 0;j<musicdur;j+=tempo*looperSamples[i].numbeats*4)
                {
                    int p = ofRandom(0, 3);
                    if(p<1)
                    {
                        cout << "Playing: " << looperSamples[i].handle << " of length " << looperSamples[i].length << " at " << j << endl;
                        SOUNDLOOPMONO(outskip+j, looperSamples[i].length, tempo*looperSamples[i].numbeats*4, 0.25, looperSamples[i].handle, ofRandom(0.,1));
                    }
                }
            }
        }
    }
    
    // GLASSOMATIC
    if (find(ilist.begin(), ilist.end(), "glassomatic") != ilist.end())
    {
        cout << "Sample number: " << looperSamples.size() << endl;
        for(i = 0;i<looperSamples.size();i++)
        {
            if(looperSamples[i].bank==samplebank)
            {
                for(j = 0;j<musicdur;j+=tempo*looperSamples[i].numbeats*4)
                {
                    SOUNDLOOPMONO(outskip+j, looperSamples[i].length, tempo*looperSamples[i].numbeats*4, 0.25, looperSamples[i].handle, (float)i/looperSamples.size()-1);
                }
            }
        }
    }
    
    //
    // =======================
    // END ORCHESTRATION BLOCK
    // =======================
    //
    
    // schedule end event at time + ring-down
    SCHEDULEBANG(outskip+musicdur+7.0);
    
}

void CloudsSound::stopMusic()
{
    flush_sched();
}
