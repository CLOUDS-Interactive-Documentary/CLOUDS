#pragma once

#include "ofxOpenSteer.h"
#include "Boid.h"

using namespace OpenSteer;
using namespace ofxOpenSteer;

// Extend the boid adding pursuit behaviour
class Predator: public Boid {
public:
    
    VehicleGroup* victims;
    
    Predator(){
        color = gRed;
        victims = NULL;
    };
    
    ~Predator(){
        victims = NULL;
    };
    
    void reset(){
        // reset the vehicle
        Boid::reset ();
        
        // Preadators on the left and right
        setPosition (Vec3( (ofRandom(1.f) < 0.5) ? -10:10,0,0) + RandomVectorInUnitRadiusSphere());
        
        // notify proximity database that our position has changed
        if(pt) pt->updateForNewPosition (position());
    };
    
    void draw(){
        drawBasic3dSphericalVehicle (*this, color);
        if(bDrawAnnotations) annotationVelocityAcceleration();
        if(bDrawTail) drawTrail();
    }
    
    Vec3 getSteeringForce(const float elapsedTime){
        
        // Inherit the flocking force
        Vec3 flock = Boid::getSteeringForce(elapsedTime);
        
        // If there are no victims, just flock normally
        if(!victims){
            return flock;
        }
        
        // Determine the closest victim
        float distance = numeric_limits<float>::infinity();
        AbstractVehicle* victim;
        
        for (VehicleIterator i = (*victims).begin(); i != (*victims).end(); i++) {
            float d = Vec3::distance(position(), (*i)->position());
            if(d < distance){
                distance = d;
                victim = (*i);
            }
        }
        
        // Pursuit force
        Vec3 pursuit = steerForPursuit(*victim, 1.f); // victim, prediction time
        
        return pursuit + flock * 0.2; // getting the victim is more important than flocking
    }
    
};

// Extend the boid adding evade behaviour
class Victim: public Boid {
public:
    
    VehicleGroup* predators;
    
    Victim(){
        color = gGreen;
        predators = NULL;
    };
    
    ~Victim(){
        predators = NULL;
    };
    
    void draw(){
        drawBasic3dSphericalVehicle (*this, color);
        if(bDrawAnnotations) annotationVelocityAcceleration();
        if(bDrawTail) drawTrail();
    }
    
    void reset(){
        // reset the vehicle
        Boid::reset();
        
        // Victims on the middle
        setPosition (RandomVectorInUnitRadiusSphere());
        
        // notify proximity database that our position has changed
        if(pt) pt->updateForNewPosition (position());
    };
    
    Vec3 getSteeringForce(const float elapsedTime){
        
        // Inherit the flocking force
        Vec3 flock = Boid::getSteeringForce(elapsedTime);
        
        // If there are no predators, just flock normally
        if(!predators){
            return flock;
        }
        
        // Determine the closest predator
        float distance = numeric_limits<float>::infinity();
        AbstractVehicle* predator;
        
        for(VehicleIterator i = (*predators).begin(); i != (*predators).end(); i++) {
            float d = Vec3::distance(position(), (*i)->position());
            if(d < distance){
                distance = d;
                predator = (*i);
            }
        }
        
        // Evade force
        Vec3 evade = steerForEvasion(*predator, 1.f); // victim, prediction time
        
        return evade + flock * 0.1; // scaping victim is more important than flocking
    }
    
};
class PursuitAndEvade: public ofxOpenSteerPlugin {
	
public:

	VehicleGroup victims;
    VehicleGroup predators;
    
    ProximityDatabase* pdPredators;
    ProximityDatabase* pdVictims;
    
    PursuitAndEvade(){
        pdPredators = NULL;  
        pdVictims = NULL;  
    };
	
	string name(){ return "Pursuit and Evade"; };
	
	void setup(){
		ofxOpenSteerPlugin::setup();
		
		ofBackground(255, 127, 0);
        
        // Create a proximity databases
        pdPredators = createProximityDatabase();
        pdVictims = createProximityDatabase();
        
        // Create the predators
		for(unsigned int i=0;i<5;i++){
			Predator* v = new Predator();
            v->pt = allocateProximityToken(pdPredators, v);
            v->reset();
			addVehicle(v);
            predators.push_back(v);
		}
        
        // Create the victims
		for(unsigned int i=0;i<30;i++){
			Victim* v = new Victim();
            v->pt = allocateProximityToken(pdVictims, v);
            v->reset();
			addVehicle(v);
            victims.push_back(v);
		}
        
        // Inform the predators about the victims
		for(VehicleIterator i = predators.begin(); i != predators.end(); i++) {
            ((Predator*)(*i))->victims = &victims;
        }
        
        // Inform the victims about the predators
		for(VehicleIterator i = victims.begin(); i != victims.end(); i++) {
            ((Victim*)(*i))->predators = &predators;
        }        
        

	};
	
	void exit(){
		ofxOpenSteerPlugin::exit();
		// clear predators
        while (predators.size() > 0){
            predators.pop_back();
        }
        
        // clear victims
        while (victims.size() > 0){
            victims.pop_back();
        }
        
        if(pdPredators) delete pdPredators;
        pdPredators = NULL;
        
        if(pdVictims) delete pdVictims;
        pdVictims = NULL;
	}
	
};
