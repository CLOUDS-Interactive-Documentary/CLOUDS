//
//  gCell.cpp
//  Colony
//
//  Created by Omer Shapira on 10/11/13.
//
//

#include "gCell.h"

colonyCell::colonyCell(const ofPoint initialPosition) //As illegal default parameter
{
    if (!isInsideBoard(initialPosition)){
        position = ofPoint(ofRandom(ofGetWidth()),ofGetHeight());
    } else {
        position = ofPoint(initialPosition);
    }
    
    /* Default Params */
    //TODO Replace magic numbers
    acceleration = ofVec2f(0,0);
    velocity = ofVec2f(ofRandom(-5,5), ofRandom(-5,5));
    cellSize = 1;
    age = 0;
    nutrientLevel = 50;
    deathThreshold = .002;
    maxSpeed = ofRandom(1.5);
    maxForce = .4;
    maxSize = ofRandom(5, 20);
    lifespan = ofRandom(80, 600);
    fertile = ofRandomuf() > .8;
    dead = false;
    hasReplicated = false;
    fertilityAge = ofRandom(lifespan* 6./13., lifespan);
    dynamicFrictionCoeff = 0.1;
    amtTurbulence = .5;
    amtAlign = 2;
    amtSeparate = 200;
    amtCohere = .5;
    
}

//==========================================================================================

void colonyCell::update()
{
    // Dynamics
    acceleration = getUpdatedAcceleration();
    acceleration.limit(maxForce);
    
    velocity = acceleration + getInertialVelocity();
    //    velocity += acceleration;
    //    velocity += getInertialVelocity();
    velocity.limit(maxSpeed);
    doSetInertia();
    
    position += velocity;
    
    // Feed
    if (lastFeedValue > nutrientLevel && cellSize <= maxSize){ cellSize += (lastFeedValue/2500); }
    if (lastFeedValue < nutrientLevel){ cellSize -= .01; }
    
    // Age
    if (age > lifespan || hasReplicated){ cellSize -= .06;}
    if (cellSize <= deathThreshold){ dead = true; }
    age++;
}

//==========================================================================================

void colonyCell::draw()
{
    ofPushStyle();
    
    ofSetColor(255, 255, 255, ofMap(MIN(maxSize, cellSize), 0, maxSize, 60, 180));
    ofCircle(position.x, position.y, MIN(maxSize, cellSize)); //TODO: This is where you do art
    ofPopStyle();
}

//==========================================================================================

void colonyCell::doAddForce(const ofPoint& _force)
{
    forces += _force;
}

void colonyCell::doResetForces(){
    forces *= 0;
}

ofPoint colonyCell::getUpdatedAcceleration(){
    //    a = f/m ; remove previous iteration
    return forces/expf(logf(1 + cellSize)) - velocity;
}

void colonyCell::doSetInertia(){
    //    p = mv
    inertia = velocity * cellSize;
}

ofPoint colonyCell::getInertia(){return inertia;}

// Returns the speed increment from the inertia, minus friction
ofPoint colonyCell::getInertialVelocity(){
    // v = p/m
    ofPoint v = inertia / cellSize;
    v *= (1 - dynamicFrictionCoeff); //FIXME: YOUR MATH IS DEFINITELY WRONG AND THIS SHOUDLN'T BE HERE
    return v;
}

//==========================================================================================

void colonyCell::doScanAndFlock(neighbor_iterator& iter){
    
    ofPoint separate,align,cohere;
    float s = getSeparationDist();
    float a = getAlignmentDist();
    float ss = s * s;
    float aa = a * a;
    float neighborCount = 0;
    while (iter.hasNext()) {
        ofPoint diff = position - ((**iter).getPosition()); //direction from other to this
        float dd = diff.lengthSquared();
        if (dd > 0){
            if (dd < ss) { //Distance within separation range?
                separate += diff.normalized() * logf(1 + dd); //TODO: make the transition softer
            }
            if (dd < aa){ //Distance within flocking range?
                align  += (**iter).getVelocity() * (**iter).getSize();
                cohere -=  diff * (**iter).getSize(); //MINUS the diff.
            }}
        ++neighborCount;
        iter.increment();
    }
    cohere /= neighborCount;
    align /=  neighborCount;

    ofVec3f steer = (   separate.normalized()   * amtSeparate
                     +  cohere.normalized()     * amtCohere
                     +  align.normalized()      * amtAlign
                     );
    
    doAddForce(steer);
    
    // Gaussian for birth
    float f = neighborCount ;
    replicationChances = exp(-f*f/200); //FIXME: magic number
}

//==========================================================================================

void colonyCell::doFeedCellNoise(){
    lastFeedValue = ofNoise(position.x, position.y, position.z, ofGetElapsedTimef()*100)*250;
}

void colonyCell::doAddTurbulence(){
    float amplitude = ofNoise(-position.x/10,-position.y/10,-position.z/10, ofGetElapsedTimef()/1000); //FIXME: Magic number
    //oF works in degrees?!
    float theta = ofNoise(position.x/100,position.y/100,position.z/100, ofGetElapsedTimef()/1000) * 360; //FIXME: Magic number
    float rho = 0; //TODO: Change
    doAddForce(ofPoint(1,0,0).getRotated(0, 0, theta) * amplitude * amtTurbulence);
}

//==========================================================================================

void colonyCell::doWrapXY(){
    if (position.x > ofGetWidth() - 1) {position.x -= (ofGetWidth() - 1);}
    if (position.x < 1) {position.x += (ofGetWidth() - 1);}
    if (position.y > ofGetHeight() - 1) { position.y -= (ofGetHeight() - 1);}
    if (position.y < 1) { position.y += (ofGetHeight() - 1); }
}

//==========================================================================================

void colonyCell::doApplyBorders(float padding)
{
    if (position.x >= (ofGetWidth() - padding)) {
        if ((position + velocity).x > position.x){
            velocity *= ofPoint(-1,1,1);
        }}
    if (position.x <= padding) {
        if ((position + velocity).x < position.x){
            velocity *= ofPoint(-1,1,1);
        }}
    if (position.y >= (ofGetHeight() - padding)) {
        if ((position + velocity).y > position.y){
            velocity *= ofPoint(1,-1,1);
        }}
    if (position.y <=  padding) {
        if ((position + velocity).y < position.y){
            velocity *= ofPoint(1,-1,1);
        }}
}
//==========================================================================================

float colonyCell::getSeparationDist(){ return cellSize * 2;}
float colonyCell::getAlignmentDist(){ return 10*logf(1 + 800 / cellSize); }

//==========================================================================================

cellPtr colonyCell::doGetReplicated()
{
    hasReplicated = true;
    cellSize *= 0.6; //TODO: Remove magic number
    return cellPtr(new colonyCell(getPosition() + ofPoint(ofRandom(-1,1),ofRandom(-1,1))));
}

const ofPoint colonyCell::getVelocity() const { return velocity; }
const ofPoint colonyCell::getPosition() const { return position; }

float colonyCell::getSize(){ return cellSize; }
bool colonyCell::isInsideBoard(ofPoint p){ return (ofInRange(p.x, 0, ofGetWidth()) && ofInRange(p.y, 0, ofGetHeight())); }
bool colonyCell::isFertile(){ return fertile; }
bool colonyCell::isReadyToReplicate()
{
    return (lastFeedValue > nutrientLevel) && (age > fertilityAge) && (ofRandom(0.1) < replicationChances);
}
bool colonyCell::isDead(){ return dead; }
void colonyCell::doSetPosition(const ofPoint& p){ position = p; }
