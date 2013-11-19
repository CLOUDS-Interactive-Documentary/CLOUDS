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
    velocity = ofVec2f(ofRandom(-1,1), ofRandom(-1,1));
    cellSize = 1;
    age = 0;
    nutrientLevel = 50;
    deathThreshold = .002;
    maxSpeed = 0.8; //TODO: Tweak
    maxForce = 0.8; //TODO: Tweak
    maxSize = ofRandom(5, 15);
    lifespan = ofRandom(60, 120); //TODO: Tweak
    fertile = ofRandomuf() > .3;
    dead = false;
    hasReplicated = false;
    fertilityAge = ofRandom(lifespan* 11./12., lifespan);
    
    //Private stuff
    separationDist = 20;
    alignmentDist = 50.0;
}

void colonyCell::update()
{
    //forces
    velocity += acceleration;
    velocity.limit(maxSpeed);
    position += velocity;
    
    //housekeeping
    acceleration *= 0;
    if (lastFeedValue > nutrientLevel && cellSize <= maxSize){ cellSize += (lastFeedValue/5000.0); }
    if (lastFeedValue < nutrientLevel){ cellSize -= .01; }
    if (age > lifespan || hasReplicated){ cellSize -= .03;}
    if (cellSize <= deathThreshold){ dead = true; }
    age++;
}

void colonyCell::draw()
{
    ofPushStyle();

    ofSetColor(255, 255, 255, 80);
//    ofSphere(position.x, position.y, 500.*(cos(position.x/500.0) + sin(position.y/500.0)), MIN(maxSize, cellSize));
    ofCircle(position.x, position.y, MIN(maxSize, cellSize)); //TODO: This is where you do art
    ofPopStyle();
}

void colonyCell::doApplyForce(const ofPoint& _force)
{
    //TODO: Add mass
    acceleration += _force;
}

void colonyCell::doScanAndFlock(neighbor_iterator& n_iter){
    
    ofPoint separate,align,cohere;
    float ss = separationDist * separationDist;
    float aa = alignmentDist  * alignmentDist;
    float count = 0;
    while (n_iter.hasNext()) {
        ofPoint diff = position - ((**n_iter).getPosition());
        float dd = diff.lengthSquared();
        if ((dd > 0) && (dd < ss)) {
            separate += diff/dd;
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
    ofVec3f steer = (   separate.normalized()   * 75
                     +  cohere.normalized()     * 5
                     +  align.normalized()      * 5
                     ) * maxSpeed;  //TODO: Why - velocity? //- velocity;
    steer.limit(maxSpeed);
    
    doApplyForce(steer);
    // Gaussian for birth
    float f = count - 1; //FIXME: magic number
    replicationChances = exp(-f*f);
}

void colonyCell::doFeedCellWidth(ofPixels &_pixels){
    int safeX = ofClamp(position.x,0,ofGetWidth()-1);
	int safeY = ofClamp(position.y,0,ofGetHeight()-1);
    lastFeedValue = _pixels.getColor(safeX, safeY).getBrightness();
}


void colonyCell::doApplyBorders()
{
    //FIXME: Oh oh FIXME, FIXME if you think that
    //       you've leared any math before
    
    //TODO: Remove magic numbers
    
    if (position.x >= ofGetWidth() - 30) {
        float diff = position.x - ofGetWidth();
        velocity.x += diff;
    }
    
    if (position.x <= 30) {
        float diff = position.x - 0;
        velocity.x -= diff;
    }
    
    if (position.y >= ofGetHeight() - 30) {
        float diff = position.y - ofGetHeight();
        velocity.y += diff;
    }
    
    if (position.y <=  30) {
        float diff = position.y - 0;
        velocity.y -= diff;
    }

}
const ofPoint colonyCell::getVelocity() const { return velocity; }
const ofPoint colonyCell::getPosition() const { return position; }
float colonyCell::getSize(){ return cellSize; }


cellPtr colonyCell::doGetReplicated()
{
    hasReplicated = true;
    cellSize *= 0.2; //TODO: Check if this really need to shrink to 0.5
    return cellPtr(new colonyCell(getPosition()));
}



bool colonyCell::isInsideBoard(ofPoint p){ return (ofInRange(p.x, 0, ofGetWidth()) && ofInRange(p.y, 0, ofGetHeight())); }
bool colonyCell::isFertile(){ return fertile; }
bool colonyCell::isReadyToReplicate()
{
    return (lastFeedValue > nutrientLevel) && (age > fertilityAge) && (ofRandom(1.0) < replicationChances);
}
bool colonyCell::isDead(){ return dead; }
void colonyCell::doSetPosition(const ofPoint& p){ position = p; }
