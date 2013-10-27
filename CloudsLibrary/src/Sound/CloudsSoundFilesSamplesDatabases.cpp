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
    cout << "==================" << endl;
    cout << "LOADING SOUND DATA" << endl;
    cout << "==================" << endl;
    
    registerOrchs();
    cout << "ORCHESTRA:" << endl;
    for(int i = 0;i<orchestra.size();i++)
    {
        cout << orchestra[i] << endl;
    }
    
    loadrhythms("rhythms.txt", rhythms);
    cout << "RHYTHMS:" << endl;
    for(int i = 0;i<rhythms.size();i++)
    {
        for(int j = 0;j<rhythms[i].beats.size();j++)
        {
            cout << rhythms[i].beats[j] << " ";
        }
        cout << endl;
    }
    
    loadpitches("pitches.txt", pitches);
    cout << "PITCHES:" << endl;
    for(int i = 0;i<pitches.size();i++)
    {
        for(int j = 0;j<pitches[i].notes.size();j++)
        {
            cout << pitches[i].notes[j] << " ";
        }
        cout << endl;
    }
    
    loadpresets("presets.txt", presets);
    cout << "PRESETS:" << endl;
    for(int i = 0;i<presets.size();i++)
    {
        for(int j = 0;j<presets[i].instruments.size();j++)
        {
            cout << presets[i].instruments[j] << " ";
        }
        cout << presets[i].harmony << " ";
        cout << presets[i].rhythm << " ";
        cout << presets[i].tempo << " ";
        cout << endl;
    }
    
    cout << "====" << endl;
    cout << "DONE" << endl;
    cout << "====" << endl;
    
}

void CloudsSound::loadRTcmixSamples()
{
    cout << "==============" << endl;
    cout << "LOADING SOUNDS" << endl;
    cout << "==============" << endl;
    string spath = getDataPath() + "sound/samps/";
    LOADSOUND(spath + "BD.aif", "BD");
    LOADSOUND(spath + "SD.aif", "SD");
    LOADSOUND(spath + "CH.aif", "CH");
    LOADSOUND(spath + "OH.aif", "OH");
    
    LOADSOUND(spath + "BD2.aif", "BD2");
    LOADSOUND(spath + "VD1.aif", "VD1");
    LOADSOUND(spath + "VD2.aif", "VD2");
    LOADSOUND(spath + "VD3.aif", "VD3");
    LOADSOUND(spath + "VD4.aif", "VD4");
    tl1 = LOADSOUND(spath + "testloop1.aif", "testloop1");
    tl2 = LOADSOUND(spath + "testloop2.aif", "testloop2");
    tl3 = LOADSOUND(spath + "testloop3.aif", "testloop3");
    
    //some path, may be absolute or relative to bin/data
    spath = spath+"loops/";
    ofDirectory dir(spath);
    //only show sound files
    dir.allowExt("wav");
    dir.allowExt("aif");
    //populate the directory object
    dir.listDir();
    
    //go through and print out all the paths
    for(int i = 0; i < dir.numFiles(); i++){
        lukeSample foo;
        foo.filename = dir.getPath(i);
        foo.handle = dir.getName(i);
        foo.length = LOADSOUND(foo.filename, foo.handle);
        foo.bank = ofSplitString(foo.handle, "_")[0];
        string len = ofSplitString(foo.handle, "_")[2];
        len = ofSplitString(len, ".")[0];
        foo.numbeats = ofToInt(len);
        //cout << foo.handle << " " << foo.bank << " " << foo.numbeats << " " << foo.length << endl;
        looperSamples.push_back(foo);
    }
    
    cout << "====" << endl;
    cout << "DONE" << endl;
    cout << "====" << endl;
    
    
    
}


void CloudsSound::registerOrchs()
{
    orchestra.clear();
    orchestra.push_back("slowwaves");
    orchestra.push_back("modalbeats");
    orchestra.push_back("helmholtz");
    orchestra.push_back("meshbeats");
    orchestra.push_back("filternoise");
    orchestra.push_back("lowwavepulse");
    orchestra.push_back("slowwaveshi");
    orchestra.push_back("slowmeshbeats");
    orchestra.push_back("slowwave");
    orchestra.push_back("strumsine");
    orchestra.push_back("waveguide");
    orchestra.push_back("waveguidebeats");
    orchestra.push_back("phatbeatz");
    orchestra.push_back("vermontbeatz");
    orchestra.push_back("waveshipatterned");
    orchestra.push_back("testloop3");
    orchestra.push_back("kissmyarp");
    orchestra.push_back("testloop1");
    orchestra.push_back("reichomatic");
    orchestra.push_back("glassomatic");
    orchestra.push_back("kissmyarpfast");
    orchestra.push_back("kissmyarpsynch");
}





