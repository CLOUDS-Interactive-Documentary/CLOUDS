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
    static bool bDrawTrail;
    static bool bDrawAnnotations;
    
    static float fMaxSpeed;
    static float fMaxForce;
    static float fInitialPositionRadius;
    static float fMaximumRadius;
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
    
    static Color bodyColor;
    static Color trailColor;
    static Color tickColor;
    
    static float trailVertexCount;
    static float trailDuration;

    static float nBoids; //ugh -- has to be a float bcz of ofxUISlider

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

		setTrailParameters(Boid::trailDuration, Boid::trailVertexCount);
	};
    
    void update(){
        ofxOpenSteerVehicle::update();
        if(pt) pt->updateForNewPosition (position());
    };
	
    void draw(){
        drawBasic3dSphericalVehicle (*this, bodyColor);
        if(bDrawAnnotations) annotationVelocityAcceleration();
        if(bDrawTrail) drawTrail(trailColor, tickColor);
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
		
		const ofVec3f position = getPosition();

        Vec3 force = separationW + alignmentW + cohesionW;
        
        if(position.distance(ofVec3f(0,0,0)) > Boid::fMaximumRadius) {
            force += steerForSeek(Vec3(0,0,0));
		}
		return force;
	};
};