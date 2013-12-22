//
//  CloudsSoundFilesSamplesDatabases.cpp
//  CLOUDS
//
//  Created by R. Luke DuBois on 10/27/13.
//  Moved some shit into another file.
//
//

#include "CloudsSound.h"


void CloudsSound::loadRTcmixFiles()
{
    if(LUKEDEBUG)
    {
        cout << "==================" << endl;
        cout << "LOADING SOUND DATA" << endl;
        cout << "==================" << endl;
    }
    
    loadrhythms("rhythms.txt", rhythms);
    
    if(LUKEDEBUG)
    {
        cout << "RHYTHMS:" << endl;
        for(int i = 0;i<rhythms.size();i++)
        {
            for(int j = 0;j<rhythms[i].beats.size();j++)
            {
                cout << rhythms[i].beats[j] << " ";
            }
            cout << endl;
        }
    }
    
    loadpitches("pitches.txt", pitches);
    if(LUKEDEBUG)
    {
        cout << "PITCHES:" << endl;
        for(int i = 0;i<pitches.size();i++)
        {
            for(int j = 0;j<pitches[i].notes.size();j++)
            {
                cout << pitches[i].notes[j] << " ";
            }
            cout << endl;
        }
    }

    loadsimplemelodies("simplemelodies.txt", simplemelodies);
    if(LUKEDEBUG)
    {
        cout << "SIMPLE MELODIES:" << endl;
        for(int i = 0;i<simplemelodies.size();i++)
        {
            for(int j = 0;j<simplemelodies[i].notes.size();j++)
            {
                cout << simplemelodies[i].notes[j] << " ";
            }
            cout << endl;
        }
    }

    //loadpresets("presets.txt", presets);
    loadpresets_xml("soundpresets.xml", presets);
    
}

void CloudsSound::loadRTcmixSamples()
{
    if(LUKEDEBUG)
    {
        cout << "==============" << endl;
        cout << "LOADING SOUNDS" << endl;
        cout << "==============" << endl;
    }
    
    string spath = GetCloudsDataPath() + "sound/samps/";
    ofDirectory sdir(spath);
    
    // load individual base hits
    LOADSOUND(sdir.getAbsolutePath() + "/" + "BD.aif", "BD");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "SD.aif", "SD");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "CH.aif", "CH");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "OH.aif", "OH");
    
    LOADSOUND(sdir.getAbsolutePath() + "/" + "BD2.aif", "BD2");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "VD1.aif", "VD1");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "VD2.aif", "VD2");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "VD3.aif", "VD3");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "VD4.aif", "VD4");
    
    //load loops
    spath = spath+"loops/";
    ofDirectory ldir(spath);
    //only show sound files
    ldir.allowExt("wav");
    ldir.allowExt("aif");
    //populate the directory object
    ldir.listDir();
    
    //go through and print out all the paths
    for(int i = 0; i < ldir.numFiles(); i++){
        lukeSample foo;
        foo.filename = ldir.getAbsolutePath() + "/" + ldir.getName(i);
        foo.handle = ldir.getName(i);
        foo.length = LOADSOUND(foo.filename, foo.handle);
        foo.bank = ofSplitString(foo.handle, "_")[0];
        string len = ofSplitString(foo.handle, "_")[2];
        len = ofSplitString(len, ".")[0];
        foo.numbeats = ofToInt(len);
        looperSamples.push_back(foo);
    }
    
    //load pattern data
    ofDirectory ddir(spath);
    //only show sound files
    ddir.allowExt("txt");
    //populate the directory object
    ddir.listDir();
    for(int i = 0;i < ddir.numFiles(); i++)
    {
        string sline;
        ofFile dfile (ddir.getAbsolutePath() + "/" + ddir.getName(i));
        if(!dfile.exists())
        {
            ofLogError("fucked");
        }
        ofBuffer dbuf(dfile);
        while(!dbuf.isLastLine())
        {
            sline = dbuf.getNextLine();
            vector <string> d = ofSplitString(sline, " ");
            int pick = -1;
            for(int j=0;j<looperSamples.size();j++)
            {
                if(d[0]==looperSamples[j].handle) pick = j;
            }
            if(pick>-1) {
                for(int j = 1; j<d.size();j++)
                {
                    looperSamples[pick].pattern.push_back(d[j]);
                }
                
                if(LUKEDEBUG) {
                    cout << looperSamples[pick].handle << ": ";
                    for(int j = 0;j<looperSamples[pick].pattern.size();j++)
                    {
                        cout << looperSamples[pick].pattern[j] << " ";
                    }
                    cout << endl;
                }
            }
        }
    }
    
    if(LUKEDEBUG)
    {
        cout << "============" << endl;
        cout << "SAMPLES DONE" << endl;
        cout << "============" << endl;
    }
    
    
    
}
