#pragma once

#include "ofxOpenSteer.h"
#include "Boid.h"

using namespace OpenSteer;
using namespace ofxOpenSteer;

// Extend the flocking boid adding obstacle avoidance behaviour
class ObstacleBoid: public Boid {
    public:
    
        // Origin and target will be set by the ObstacleAvoidance plugin
        Vec3* origin;
        Vec3* target;
    
        ObstacleGroup* obstacles;
        
        void reset(){
            // reset the vehicle
            Boid::reset ();
                    
            // Set to origin, and randomize slightly
            setPosition (*origin - RandomVectorInUnitRadiusSphere());
            
            // notify proximity database that our position has changed
            if(pt) pt->updateForNewPosition (position());
        };
        
        Vec3 getSteeringForce(const float elapsedTime){
            // reset if close enought to target
            if( Vec3::distance(position(), *target) < 2 ){
                reset();
                return Vec3();
            }
            
            // Inherit the flocking force
            Vec3 flock = Boid::getSteeringForce(elapsedTime);
            
            // Avoid the obstacle (if any)
            Vec3 avoidObstacle;
            if(obstacles) avoidObstacle = steerToAvoidObstacles (1.f, *obstacles);
            
            // seek for the target
            Vec3 seek = steerForSeek(*target);
            
            return seek + avoidObstacle + flock * 0.2; // reduce flocking force 
        }

};

class ObstacleAvoidance: public ofxOpenSteerPlugin {
	
public:
    Vec3 origin;
    Vec3 target;
    ObstacleGroup* obstacles;
	
	float radius;
    
    ProximityDatabase* pd;
    
    ObstacleAvoidance(){
        pd = NULL;  
    };
	
	string name(){ return "Obstacle Avoidance"; };
	

	void setup(){
		ofxOpenSteerPlugin::setup();
		
		ofBackground(255, 0, 255);
        
        // Origin and target points
        origin = Vec3(0,20,0);
        target = Vec3(0,-20,0);
		
		// Create the obstacle
        obstacles = new ObstacleGroup();
		radius = 10;		
		SphereObstacle* obstacle = new SphereObstacle(radius, Vec3::zero);
        obstacles->push_back(obstacle);
        
        // Create a proximity database with default settings
        pd = createProximityDatabase();
		
		for(unsigned int i=0; i<Boid::nBoids; i++){
			ObstacleBoid* v = new ObstacleBoid();
            v->pt = allocateProximityToken(pd, v);
            v->origin = &origin;
            v->target = &target;
			v->obstacles = obstacles;
            v->reset();
			addVehicle(v);
		}
	};
	
	void draw(){
		ofxOpenSteerPlugin::draw();
		ofSetColor(100, 100, 100, 100);
		ofSphere(ofPoint(), radius);
        
        ofSetColor(255, 255, 255, 100);
        ofSphere(toOf(origin), 2);
        ofSphere(toOf(target), 2);
	}
	
	void exit(){
		ofxOpenSteerPlugin::exit();
        
        // clear the obstacles
        if(obstacles){
            while (obstacles->size() > 0){
                const AbstractObstacle* o = obstacles->back();
                obstacles->pop_back();
                delete o;
            }
            delete obstacles;
        }
        obstacles = NULL;
        
        // Clear pd
        if(pd) delete pd;
        pd = NULL;
	}
};
