//
//  lukeMusicFuncs.cpp
//  CloudsScoreDesigner
//
//  Created by R. Luke DuBois on 12/10/13.
//
//

#include <iostream>
#include "lukeFuncs.h"
#include "CloudsGlobal.h"

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

melodySolver::melodySolver(string c_type, lukePitchArray& c_p, lukeSimpleMelody& c_m)
{
    type = c_type;
    parray = c_p;
    marray = c_m;
    pick = 0;
    if(type=="markov") {
        pick = (int)ofRandom(0, parray.markov.size());
        curpitch = parray.mindex[pick];
    }
    if(type=="melody" || type=="static" || type=="simple") pick = 0;
}

int melodySolver::tick()
{
    int rval;
    
    if(type=="bucket") pick = (int)ofRandom(0, parray.notes.size());
    if(type=="markov") {
        pick = markov(pick, parray);
        curpitch = parray.mindex[pick];
    }
    if(type=="simple")
    {
        curpitch = marray.notes[pick];
    }
    else
    {
        curpitch = parray.notes[pick];
    }
    
    if(type=="melody") pick = (pick+1)%parray.notes.size();
    if(type=="simple") pick = (pick+1)%marray.notes.size();
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
    if(LUKEDEBUG) cout << "TEST MARKOV: " << p.notes.size() << endl;
    
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
    if(LUKEDEBUG)
    {
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
    
    
    
}

// return a markov result
int markov(int current, lukePitchArray& p)
{
    int pick = int(ofRandom(p.markov[current].size()));
    int next = p.markov[current][pick];
    return(next);
}
