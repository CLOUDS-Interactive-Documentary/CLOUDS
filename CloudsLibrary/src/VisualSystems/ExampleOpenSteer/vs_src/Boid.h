// Base boid used by all the simulations with flocking behaviour

#pragma once

#include "ofxOpenSteer.h"

using namespace OpenSteer;
using namespace ofxOpenSteer;

class Boid: public ofxOpenSteerVehicle {
	
public:
    
    ProximityToken* pt;
    
    Boid(){
        pt = NULL;
    };
    
    ~Boid(){
        if(pt) delete pt;
        pt = NULL;
    };    
    
    void reset(){
		// reset the vehicle
		ofxOpenSteerVehicle::reset ();
		
		// steering force is clipped to this magnitude
		setMaxForce (27);
		
		// velocity is clipped to this magnitude
		setMaxSpeed (9);
		
		// initial slow speed
		setSpeed (maxSpeed() * 0.3f);
		
		// randomize initial orientation
		regenerateOrthonormalBasisUF (RandomUnitVector ());
		
		// randomize initial position
		setPosition (RandomVectorInUnitRadiusSphere() * 20);
		
		// notify proximity database that our position has changed
		if(pt) pt->updateForNewPosition (position());
	};
    
    void update(){
        ofxOpenSteerVehicle::update();
        if(pt) pt->updateForNewPosition (position());
    };
	
    void draw(){
        drawBasic3dSphericalVehicle (*this, color);
        annotationVelocityAcceleration ();
        drawTrail();
    }
    
    Vec3 getSteeringForce(const float elapsedTime){
        // if there is no proximity database, just wander
        if(!pt) return steerForWander(elapsedTime);
        
		const float separationRadius =  5.0f;
		const float separationAngle  = -0.707f;
		const float separationWeight =  12.0f;
		
		const float alignmentRadius = 7.5f;
		const float alignmentAngle  = 0.7f;
		const float alignmentWeight = 8.0f;
		
		const float cohesionRadius = 50.0f;
		const float cohesionAngle  = -0.15f;
		const float cohesionWeight = 8.0f;
		
		const float maxRadius = maxXXX (separationRadius,
										maxXXX (alignmentRadius,
												cohesionRadius));
		
		// find all flockmates within maxRadius using proximity database
		neighbors.clear();
		pt->findNeighbors (position(), maxRadius, neighbors);
		
		
		// determine each of the three component behaviors of flocking
		const Vec3 separation = steerForSeparation (separationRadius,
													separationAngle,
													neighbors);
		const Vec3 alignment  = steerForAlignment  (alignmentRadius,
													alignmentAngle,
													neighbors);
		const Vec3 cohesion   = steerForCohesion   (cohesionRadius,
													cohesionAngle,
													neighbors);
		
		// apply weights to components (save in variables for annotation)
		const Vec3 separationW = separation * separationWeight;
		const Vec3 alignmentW = alignment * alignmentWeight;
		const Vec3 cohesionW = cohesion * cohesionWeight;
		
		return separationW + alignmentW + cohesionW;	
	};
};