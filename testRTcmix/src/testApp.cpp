#include "testApp.h"
#include <globals.h>
#include <RTcmix.h>

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(0,0,0);
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofSetBackgroundAuto(true);
    
    // RTCMIX path crap...
    libpath = ofToDataPath("shlib/", true);
    string wt = libpath + "libWAVETABLE.so"; // bullshit in C++
    const char * wtc = wt.c_str(); // bullshit in C
    string rv = libpath + "libREVERBIT.so"; // more bullshit in C++
    const char * rvc = rv.c_str(); // more bullshit in C
    string mb = libpath + "libMMODALBAR.so"; // yet more bullshit in C++
    const char * mbc = mb.c_str(); // yet more bullshit in C
                           
    // start CMIX engine
    rrr = new RTcmix(44100.0, 2);
    sleep(1); // might not be necessary
    rrr->printOff(); // kill console printing of CMIX commands
    
    // load dynamic libraries
    rrr->cmd("load", 1, wtc);
    rrr->cmd("load", 1, rvc);
    rrr->cmd("load", 1, mbc);
    
    // set up function generators
    rrr->cmd("makegen", 9, 1.0, 24.0, 1000.0, 0.0, 0.0, 0.3, 1.0, 3.0, 0.0);
    rrr->cmd("makegen", 7, 2.0, 10.0, 1000.0, 1.0, 0.3, 0.2, 0.05);
    
    // set up bus architecture
    rrr->cmd("bus_config", 2, "WAVETABLE", "aux 0 out");
    rrr->cmd("bus_config", 3, "REVERBIT", "aux 0 in", "out 0-1");
    rrr->cmd("bus_config", 2, "MMODALBAR", "aux 0 out");
    
    // initial reverb + drone
    rrr->cmd("WAVETABLE", 5, 0.0, 10.0, MAXAMP/40., mtof(34, 440.), 0.5);
    rrr->cmd("WAVETABLE", 5, 0.0, 10.0, MAXAMP/40., mtof(41, 440.), 0.5);    
    rrr->cmd("REVERBIT", 8, 0.0, 0.0, 10.0, 1.0, 2.0, 0.7, 0.16, 800.0);

    elapsed = 0.;
    fpos = 0.;

    // setup video
    vidPlayer.loadMovie("foo.mov");
    //vidPlayer.setVolume(0);
	vidPlayer.play();
    viddur = vidPlayer.getDuration();

    // LOAD DATA FOR FILM
    ofBuffer buffer = ofBufferFromFile("foo.txt"); // reading into the buffer
    string thecrap;
    vector <string> bits;
    while(buffer.isLastLine()==0)
    {
        thecrap = buffer.getNextLine(); // let's see what it says
        bits = ofSplitString(thecrap, " ");
        foo_t.push_back(ofToFloat(bits[0])/1000.);
        foo_f1.push_back(ftom(ofToFloat(bits[1]),440.));
        foo_a1.push_back(ofToFloat(bits[2]));
        foo_f2.push_back(ftom(ofToFloat(bits[3]),440.));
        foo_a2.push_back(ofToFloat(bits[4]));
        foo_f3.push_back(ftom(ofToFloat(bits[5]),440.));
        foo_a3.push_back(ofToFloat(bits[6]));
    }
}

//--------------------------------------------------------------
void testApp::update(){
    vidPlayer.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    
    
    // IMAGE
    ofSetColor(255,255,255,255);
    vidPlayer.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    
    ofSetColor(64,128,255,100);

    
    //
    // SOUND BLOCK
    //
    
    if(DEBUG) std::cout << ofGetElapsedTimef() << std::endl;

    //
    // REFRESH DRONE EVERY 10 SECONDS
    //
    
    if(ofGetElapsedTimef()-elapsed>5.)
    {
        elapsed = ofGetElapsedTimef();
        int p = int(ofRandom(30,40));
        p = scale(p);
        // set up function generators
        rrr->cmd("makegen", 9, 1.0, 24.0, 1000.0, 0.0, 0.0, 0.3, 1.0, 3.0, 0.0);
        rrr->cmd("makegen", 7, 2.0, 10.0, 1000.0, 1.0, 0.3, 0.2, 0.05);
        rrr->cmd("WAVETABLE", 5, 0.0, 10.0, MAXAMP/60., mtof(p, 440.), 0.5);
        rrr->cmd("WAVETABLE", 5, 0.0, 10.0, MAXAMP/60., mtof(p+7, 440.), 0.5);
        rrr->cmd("WAVETABLE", 5, 0.0, 10.0, MAXAMP/60., mtof(p, 440.)*0.99, 0.5);
        rrr->cmd("WAVETABLE", 5, 0.0, 10.0, MAXAMP/60., mtof(p+7, 440.)*0.99, 0.5);

        
    }
    
    //
    // SEQUENCE CHORDS BASED ON FILE
    //
    
    double v = vidPlayer.getPosition();
    double dur = 1.0;
    int i;
    float offset = 0.0;
    for(i = 0;i<foo_t.size();i++)
    {
        if(v+offset>=foo_t[i]&&v+offset<foo_t[(i+1)%foo_t.size()]) break;
    }
    
    if(i!=fpos)
    {
        // relaunch REVERB each movie loop
        if(fpos==0)
        {
            rrr->cmd("REVERBIT", 8, 0.0, 0.0, viddur, 1.0, 4.0, 0.7, 0.16, 10000.0);
            
            for(float st = 0;st<viddur;st=st+0.25)
            {
                rrr->cmd("MMODALBAR", 7, st, 1.0, ofRandom(0.2,1)*10000., mtof(36+int(ofRandom(0,3))*7, 440.), ofRandom(0,1), ofRandom(0,1), 0);
            }

        
        }
 
        fpos = i;
        // PLAY NOTES
        rrr->cmd("makegen", 9, 1.0, 24.0, 1000.0, 0.0, 0.0, 0.01, 1.0, 3.0, 0.0);
        rrr->cmd("makegen", 9, 2.0, 10.0, 1000.0, 1.0, 0.7, 0.3, 0.3, 0.1, 0.1);
        if(foo_f1[fpos]>0.) rrr->cmd("WAVETABLE", 5, 0.0, dur, foo_a1[fpos]*MAXAMP/5., 2*mtof(scale(foo_f1[fpos]), 440.), 0.5);
        if(foo_f2[fpos]>0.) rrr->cmd("WAVETABLE", 5, 0.0, dur, foo_a2[fpos]*MAXAMP/5., 2*mtof(scale(foo_f2[fpos]), 440.), 0.);
        if(foo_f3[fpos]>0.) rrr->cmd("WAVETABLE", 5, 0.0, dur, foo_a3[fpos]*MAXAMP/5., 2*mtof(scale(foo_f3[fpos]), 440.), 1.);
        
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
    
}

// LUKE FUNCTIONS

double testApp::ftom(double f, double tuning)
{
    // midi -> frequency conversion
    if(f>0) return(69. + (1./.057762265) * log(f/tuning));
    else return(-999.);
}


double testApp::mtof(double f, double tuning)
{
    // midi -> frequency conversion
	return (tuning * exp(.057762265 * (f - 69.)));
}

int testApp::scale(int p)
{
    // C minor
    int s[12] = {0, 0, 2, 3, 3, 5, 5, 7, 8, 8, 10, 10};
    int oct = p/12;
    int pc = p%12;
    return(oct*12 + s[pc]);
}
