#pragma once

#include "ofxOpenSteer.h"
#include "Boid.h"


using namespace OpenSteer;
using namespace ofxOpenSteer;


class Flocking: public ofxOpenSteerPlugin {
    
public:
    
    ProximityDatabase* pd;

    Flocking(){
        pd = NULL;  
    };
    
    string name(){ return "Flocking"; };
    
    void setup(){
        ofxOpenSteerPlugin::setup();        
        ofBackground(255, 255, 0);
        
        // Create a proximity database with default settings
        pd = createProximityDatabase();
        
        for(unsigned int i=0; i<Boid::nBoids; i++){
            Boid* v = new Boid();
            v->pt = allocateProximityToken(pd, v);
            v->reset();
            addVehicle(v);
        }
    };
    
    void exit(){
        ofxOpenSteerPlugin::exit();
        if(pd) delete pd;
        pd = NULL;
    };
    
};