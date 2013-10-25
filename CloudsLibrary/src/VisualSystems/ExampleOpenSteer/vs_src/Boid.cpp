//
//  Boid.cpp
//  ExampleOpenSteer
//
//  Created by Jeffrey Crouse on 10/25/13.
//
//

#include "Boid.h"

bool Boid::bDrawTail = true;
bool Boid::bDrawAnnotations = true;

Color Boid::fColor;

float Boid::fMaxSpeed = 9;
float Boid::fMaxForce = 27;
float Boid::fInitialPositionRadius = 20;

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
