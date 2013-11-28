//
//  Boid.cpp
//  ExampleOpenSteer
//
//  Created by Jeffrey Crouse on 10/25/13.
//
//

#include "Boid.h"

BoidTrailType Boid::trailType = BOID_TRAIL_SOLID_LINE;
float Boid::trailRibbonSize = 10;
float Boid::trailColorMix = 0.5f;
bool Boid::bDrawAnnotations = true;

Color Boid::bodyColor;
Color Boid::trailColor;
Color Boid::trailColor2;
Color Boid::tickColor;

float Boid::fMaxSpeed = 9;
float Boid::fMaxForce = 27;
float Boid::fInitialPositionRadius = 20;
float Boid::fMaximumRadius = 200;
float Boid::radius = 1;

float Boid::separationRadius =  5.0f;
float Boid::separationAngle  = -0.707f;
float Boid::separationWeight =  12.0f;

float Boid::alignmentRadius = 7.5f;
float Boid::alignmentAngle  = 0.7f;
float Boid::alignmentWeight = 8.0f;

float Boid::cohesionRadius = 50.0f;
float Boid::cohesionAngle  = -0.15f;
float Boid::cohesionWeight = 8.0f;

float Boid::trailVertexCount = 100;
float Boid::trailDuration = 5;

float Boid::nBoids = 100;