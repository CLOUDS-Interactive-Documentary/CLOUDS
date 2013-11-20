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
    acceleration = ofVec2f(ofRandom(-5,5), ofRandom(-5,5)); //ofVec2f(0,0);
    velocity = ofVec2f(ofRandom(-5,5), ofRandom(-5,5));
    cellSize = 1;
    age = 0;
    nutrientLevel = 50;
    deathThreshold = .002;
    maxSpeed = ofRandom(2.5);//.4; //TODO: Tweak
    maxForce = .5; //TODO: Tweak
    maxSize = ofRandom(5, 25);
    lifespan = ofRandom(80, 600); //TODO: Tweak
    fertile = ofRandomuf() > .8;
    dead = false;
    hasReplicated = false;
    fertilityAge = ofRandom(lifespan* 6./13., lifespan);
    
}

//==========================================================================================

void colonyCell::update()
{
    //forces
    acceleration.limit(maxForce);
    velocity += acceleration;
    velocity.limit(maxSpeed);
    position += velocity; //FIXME: This is happening: - ofPoint(0.5,0.5);
    
    //housekeeping
    acceleration *= 0.3; //TODO: change to something
    if (lastFeedValue > nutrientLevel && cellSize <= maxSize){ cellSize += (lastFeedValue/2500); }
    if (lastFeedValue < nutrientLevel){ cellSize -= .01; }
    if (age > lifespan || hasReplicated){ cellSize -= .06;}
    if (cellSize <= deathThreshold){ dead = true; }
    age++;
}

//==========================================================================================

void colonyCell::draw()
{
    ofPushStyle();

    ofSetColor(255, 255, 255, ofMap(MIN(maxSize, cellSize), 0, maxSize, 60, 180));
//    ofSphere(position.x, position.y, 500.*(cos(position.x/500.0) + sin(position.y/500.0)), MIN(maxSize, cellSize));
    ofCircle(position.x, position.y, MIN(maxSize, cellSize)); //TODO: This is where you do art
    ofPopStyle();
}

//==========================================================================================

void colonyCell::doApplyForce(const ofPoint& _force)
{
//    a = f/m
    acceleration += _force/cellSize;
}

//==========================================================================================

void colonyCell::doScanAndFlock(neighbor_iterator& iter){
    
    ofPoint separate,align,cohere;
    align *= 0;
    cohere *= 0;
    float s = getSeparationDist();
    float ss = s * s;
    float a = getAlignmentDist();
    float aa = a  * a;
    float count = 0;
    while (iter.hasNext()) {
        ofPoint diff = position - ((**iter).getPosition());
        float dd = diff.lengthSquared();
        if ((dd > 0) && (dd < ss)) {
            separate += diff.normalized()*logf(dd); //TODO: make the transition softer
        }
        if ((dd > 0) && (dd < aa)){
            align  += (**iter).getVelocity();
            cohere += (**iter).getPosition() * (**iter).getSize();
        }
        ++count;
        iter.increment();
    }
    cohere /= count;
    align /=  count;
    //TODO: Remove magic numbers
    ofVec3f steer = (   separate.normalized()   * 30
                     +  cohere.normalized()     * 2.5
                     +  align.normalized()      * .15
                     );//  - velocity;  //TODO: Why - velocity? ;
    
    doApplyForce(steer);
    // Gaussian for birth
    float f = count ; //FIXME: magic number
    replicationChances = exp(-f*f/200);
}

//==========================================================================================

void colonyCell::doFeedCellNoise(){
    lastFeedValue = ofNoise(position.x, position.y, position.z, ofGetElapsedTimef()*100)*250;
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

float colonyCell::getSeparationDist(){ return cellSize * 4;}
float colonyCell::getAlignmentDist(){ return 10*logf(1 + 800 / cellSize); }

//==========================================================================================

cellPtr colonyCell::doGetReplicated()
{
    hasReplicated = true;
    cellSize *= 0.5; //TODO: Remove magic number
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
