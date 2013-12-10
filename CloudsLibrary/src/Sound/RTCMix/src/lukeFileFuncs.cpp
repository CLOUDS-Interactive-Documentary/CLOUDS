//
//  lukeFileFuncs.cpp
//  CloudsScoreDesigner
//
//  Created by R. Luke DuBois on 12/10/13.
//
//

#include <iostream>
#include "lukeFuncs.h"
#include "CloudsGlobal.h"

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

