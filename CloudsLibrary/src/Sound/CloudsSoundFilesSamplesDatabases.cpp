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
    
    //loadpresets("presets.txt", presets);
    loadpresets_xml("soundpresets.xml", presets);
    
}

void CloudsSound::loadRTcmixSamples()
{
    cout << "==============" << endl;
    cout << "LOADING SOUNDS" << endl;
    cout << "==============" << endl;
    string spath = GetCloudsDataPath() + "sound/samps/";
    ofDirectory sdir(spath);
    LOADSOUND(sdir.getAbsolutePath() + "/" + "BD.aif", "BD");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "SD.aif", "SD");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "CH.aif", "CH");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "OH.aif", "OH");
    
    LOADSOUND(sdir.getAbsolutePath() + "/" + "BD2.aif", "BD2");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "VD1.aif", "VD1");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "VD2.aif", "VD2");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "VD3.aif", "VD3");
    LOADSOUND(sdir.getAbsolutePath() + "/" + "VD4.aif", "VD4");
    tl1 = LOADSOUND(sdir.getAbsolutePath() + "/" + "testloop1.aif", "testloop1");
    tl2 = LOADSOUND(sdir.getAbsolutePath() + "/" + "testloop2.aif", "testloop2");
    tl3 = LOADSOUND(sdir.getAbsolutePath() + "/" + "testloop3.aif", "testloop3");
    
    //some path, may be absolute or relative to bin/data
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





