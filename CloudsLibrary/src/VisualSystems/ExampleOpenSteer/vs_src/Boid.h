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
    
    static OpenSteer::Color bodyColor;
    static OpenSteer::Color trailColor;
    static OpenSteer::Color tickColor;
    
    static float trailVertexCount;
    static float trailDuration;

    static float nBoids; //ugh -- has to be a float bcz of ofxUISlider
    
    deque<ofVec3f> history;
    ofVboMesh trailMesh;

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
        
        history.clear();
	};
    
    void update(float curTime, float elapsedTime){
        ofxOpenSteerVehicle::update(curTime, elapsedTime);
        if(pt) pt->updateForNewPosition (position());
        
        history.push_back(getPosition());
        while (history.size() > trailVertexCount) {
            history.pop_front();
        }
        
        trailMesh.clear();
        trailMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        
        if (history.size() > 0) {
            float trailRadius = 10;
            const ofVec3f back = history.back();
            float total = (float)(history.size());
            for (int i = history.size() - 1; i > 0; i--) {
                float pct = i / total;
                const ofVec3f& curr = history[i];
                const ofVec3f& last = history[i-1];
                
                ofVec3f perp0 = curr - last;
                ofVec3f perp1 = perp0.getCrossed(ofVec3f(0, 0, 1));
                ofVec3f perp2 = perp0.getCrossed(perp1);
                perp1 = perp0.getCrossed(perp2).getNormalized();
                float offWidth = (trailRadius * pct * 0.07f);
                float opacityScale = 0.95f * pct;
                if (pct > 0.8f) {
                    float temp = (1.0f - pct) / 0.2f;
                    float tempScale = sqrt(temp);
                    offWidth *= tempScale;
                    opacityScale *= tempScale;
                }
                
                ofVec3f off = perp1 * offWidth;
                ofColor color(trailColor.r() * 255, trailColor.g() * 255, trailColor.b() * 255, opacityScale * 255);
                
                trailMesh.addVertex(curr - off);
                trailMesh.addColor(color);
                trailMesh.addVertex(curr + off);
                trailMesh.addColor(color);
            }
        }
    };
	
    void draw(){
        drawBasic3dSphericalVehicle (*this, bodyColor);
        if(bDrawAnnotations) annotationVelocityAcceleration();
        if(bDrawTrail) {
            trailMesh.draw();
        }
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
		const Vec3 alignment  = steerForAlignment  (    alignmentRadius,
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