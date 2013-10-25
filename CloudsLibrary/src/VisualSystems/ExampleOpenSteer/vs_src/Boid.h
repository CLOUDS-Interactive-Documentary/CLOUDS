//
//  Boid.h
//  ExampleOpenSteer
//
//  Created by Jeffrey Crouse on 10/25/13.
//
//

#pragma once


#include "ofxOpenSteer.h"

using namespace OpenSteer;
using namespace ofxOpenSteer;

class Boid: public ofxOpenSteerVehicle {
	
public:
    
    ProximityToken* pt;
    static bool bDrawTail;
    static bool bDrawAnnotations;
    
    static float fMaxSpeed;
    static float fMaxForce;
    static float fInitialPositionRadius;
    
    static float radius;
    
    static float separationRadius;
    static float separationAngle;
    static float separationWeight;
    
    static float alignmentRadius;
    static float alignmentAngle;
    static float alignmentWeight;
    
    static float cohesionRadius;
    static float cohesionAngle;
    static float cohesionWeight;
    
    static Color fColor;
    
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
		setMaxForce (Boid::fMaxForce);
		
		// velocity is clipped to this magnitude
		setMaxSpeed (Boid::fMaxSpeed);
		
        setRadius(Boid::radius);
        
		// initial slow speed
		setSpeed (maxSpeed() * 0.3f);
		
		// randomize initial orientation
		regenerateOrthonormalBasisUF (RandomUnitVector ());
		
		// randomize initial position
		setPosition (RandomVectorInUnitRadiusSphere() * Boid::fInitialPositionRadius);
		
		// notify proximity database that our position has changed
		if(pt) pt->updateForNewPosition (position());
	};
    
    void update(){
        ofxOpenSteerVehicle::update();
        if(pt) pt->updateForNewPosition (position());
    };
	
    void draw(){
        drawBasic3dSphericalVehicle (*this, fColor);
        if(bDrawAnnotations) annotationVelocityAcceleration();
        if(bDrawTail) drawTrail();
    }
    
    Vec3 getSteeringForce(const float elapsedTime){
        // if there is no proximity database, just wander
        if(!pt) return steerForWander(elapsedTime);
        

		
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