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
    acceleration = ofVec2f(ofRandom(-4,4), ofRandom(-4,4)); //ofVec2f(0,0);
    velocity = ofVec2f(ofRandom(-3,3), ofRandom(-3,3));
    cellSize = 1;
    age = 0;
    nutrientLevel = 50;
    deathThreshold = .002;
    maxSpeed = .4; //TODO: Tweak
    maxForce = .4; //TODO: Tweak
    maxSize = ofRandom(5, 15);
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
    velocity += acceleration;
    velocity.limit(maxSpeed);
    position += velocity;
    
    //housekeeping
    acceleration *= 0;
    if (lastFeedValue > nutrientLevel && cellSize <= maxSize){ cellSize += (lastFeedValue/2500.0); }
    if (lastFeedValue < nutrientLevel){ cellSize -= .01; }
    if (age > lifespan || hasReplicated){ cellSize -= .03;}
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
    //TODO: Add mass
    acceleration += _force;
}

//==========================================================================================

void colonyCell::doScanAndFlock(neighbor_iterator& n_iter){
    
    ofPoint separate,align,cohere;
    float s = getSeparationDist();
    float ss = s * s;
    float a = getAlignmentDist();
    float aa = a  * a;
    float count = 0;
    while (n_iter.hasNext()) {
        ofPoint diff = position - ((**n_iter).getPosition());
        float dd = diff.lengthSquared();
        if ((dd > 0) && (dd < ss)) {
            separate += diff/dd; //TODO: make the transition softer
        }
        if ((dd > 0) && (dd < aa)){
            align  += n_iter->get()->getVelocity();
            cohere += n_iter->get()->getPosition();
        }
        ++count;
        n_iter.increment();
    }
    cohere /= count;
    align /=  count;
    //TODO: Remove magic numbers
    ofVec3f steer = (   separate.normalized()   * 120
                     +  cohere.normalized()     * .5
                     +  align.normalized()      * 1.15
                     ) * maxSpeed - velocity;  //TODO: Why - velocity? ;
    steer.limit(maxSpeed);
    
    doApplyForce(steer);
    // Gaussian for birth
    float f = count ; //FIXME: magic number
    replicationChances = exp(-f*f/200);
}

//==========================================================================================

void colonyCell::doFeedCellWidth(ofPixels &_pixels){
    int safeX = ofClamp(position.x,0,ofGetWidth()-1);
	int safeY = ofClamp(position.y,0,ofGetHeight()-1);
    lastFeedValue = _pixels.getColor(safeX, safeY).getBrightness();
}

//==========================================================================================

void colonyCell::doFeedCellNoise(){
    lastFeedValue = ofNoise(position.x, position.y, position.z)*150;
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

float colonyCell::getSeparationDist(){ return cellSize * 3;}
float colonyCell::getAlignmentDist(){ return 200 / cellSize; }

//==========================================================================================

cellPtr colonyCell::doGetReplicated()
{
    hasReplicated = true;
    cellSize *= 0.5; //TODO: Remove magic number
    return cellPtr(new colonyCell(getPosition()));
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
