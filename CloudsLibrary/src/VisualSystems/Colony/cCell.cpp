//
//  Cell.cpp
//  VSColony
//
//  Created by Patricio Gonzalez Vivo on 6/26/13.
//
//

#include "cCell.h"

colonyCell::colonyCell(){
    
    this->set( ofRandom(ofGetWidth()),ofGetHeight() );  // same as Location
    acc = ofPoint(0,0);
    vel = ofPoint( ofRandom(-1,1), ofRandom(-1,1) );
    
    //  Default variables
    //
    r = 3.0;
    cellSize = .5;
    age = 0;
    maxSize = ofRandom(15,25);
    maxspeed = 1.1;
    maxforce = 0.8;
    fertilityAge = ofRandom(400,900);
    lifeSpan = ofRandom(100,650);
    nutrientLevel = 35;
    dead = false;
    hasReplicated = false;
    shouldReplicate = false;
	
	spriteMesh.clear();
	
	spriteMesh.addVertex(ofVec3f(0,0,0));  //top left
	spriteMesh.addVertex(ofVec3f(1,0,0)); //top right
	spriteMesh.addVertex(ofVec3f(0,1,0));  //bottom left
	spriteMesh.addVertex(ofVec3f(1,1,0));  //bottom right

	spriteMesh.addTexCoord(ofVec2f(0,0));  //top left
	spriteMesh.addTexCoord(ofVec2f(60,0)); //top right
	spriteMesh.addTexCoord(ofVec2f(0,60));  //bottom left
	spriteMesh.addTexCoord(ofVec2f(60,60));  //bottom right

	spriteMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
}

void colonyCell::applyForce(ofPoint _force) {
    
    // We could add mass here if we want A = F / M
    //
    acc += _force;
}

// We accumulate a new acceleration each time based on three rules
void colonyCell::applyFlock( vector<colonyCell*> &_cells ) {
    
    ofPoint sep = separate( _cells );   // Separation
    ofPoint ali = align( _cells );      // Alignment
    ofPoint coh = cohesion( _cells );   // Cohesion
    
    // Arbitrarily weight these forces
    //
    sep *= 250.0;
    ali *= 0.2;
    coh *= 0.1;
    
    // Add the force vectors to acceleration
    //
    applyForce(sep);
    applyForce(ali);
    applyForce(coh);
}

// Method to update location
void colonyCell::update() {
    
    // Update velocity
    //
    vel += acc;
    
    // Limit speed
    //
    vel.limit(maxspeed);
    *this += vel;
    
    // Reset accelertion to 0 each cycle
    //
    acc *= 0;
}

ofPoint colonyCell::seek(ofPoint _target) {
    ofPoint desired = _target - *this;  // A vector pointing from the location to the target
    
    // Normalize desired and scale to maximum speed
    //
    desired.normalize();
    desired *= maxspeed;
    
    // Steering = Desired minus Velocity
    //
    ofPoint steer = desired - vel;
    steer.limit(maxforce);  // Limit to maximum steering force
    
    return steer;
}

void colonyCell::applyBorders() {
    
    if (x >= ofGetWidth() + 30) {
        float diff = x - ofGetWidth();
        vel.x = vel.x * -1*diff;
    }
    
    if (x <= -30) {
        float diff = x - 0;
        vel.x = vel.x * -1*diff;
    }
    
    if (y >= ofGetHeight() +30) {
        float diff = y - ofGetHeight();
        vel.y = vel.y * -1*diff;
    }
    
    if (y <= - 30) {
        float diff = y - 0;
        vel.y = vel.y * -1*diff;
    }
}

// Separation
// Method checks for nearby cells and steers away
//
ofPoint colonyCell::separate( vector<colonyCell*> &_cells ) {
    
    float desiredseparation = 20.0f;
	float desiredseparation2 = powf(desiredseparation, 2.0f);
	
    ofPoint steer = ofPoint(0,0,0);
    int count = 0;
    
    // For every Cell in the system, check if it's too close
    for (int i = 0; i < _cells.size() ; i++) {
        
//        float d = distance( *_cells[i] );
		float d2 = distanceSquared( *_cells[i] );
        // If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
        if ( (d2 > 0) && (d2 < desiredseparation2)) {
            // Calculate vector pointing away from neighbor
            ofPoint diff = *this - *_cells[i];
            diff.normalize();
            diff /= sqrt(d2);        // Weight by distance
            steer += diff;
            count++;            // Keep track of how many
        }
    }
    // Average -- divide by how many
    if (count > 0) {
        steer /= (float)count;
    }
    
    // As long as the vector is greater than 0
    if (steer.length() > 0) {
        // Implement Reynolds: Steering = Desired - Velocity
        steer.normalize();
        steer *= maxspeed;
        steer -= vel;
        steer.limit(maxforce);
    }
    
    return steer;
}

// Alignment
// For every nearby Cell in the system, calculate the average velocity
//
ofPoint colonyCell::align( vector<colonyCell*> &_cells ) {
    float neighbordist = 50;
	float neighbordistSqrd = sqrt(neighbordist);
    ofPoint sum = ofPoint(0,0);
    int count = 0;
    for (int i = 0; i < _cells.size() ; i++) {
        
//        float d = distance( *_cells[i] );
		float d2 = distanceSquared( *_cells[i] );
        if ((d2 > 0) && (d2 < neighbordistSqrd)) {
            sum += _cells[i]->vel;
            count++;
        }
    }
    if (count > 0) {
        sum /= (float)count;
        sum.normalize();
        sum *= maxspeed;
        ofPoint steer = sum - vel;
        steer.limit(maxforce);
        return steer;
    } else {
        return ofPoint(0,0);
    }
}

// Cohesion
// For the average location (i.e. center) of all nearby cells, calculate steering vector towards that location
//
ofPoint colonyCell::cohesion( vector<colonyCell*> &_cells ) {
    float neighbordist = 50;
    ofPoint sum = ofPoint(0,0);   // Start with empty vector to accumulate all locations
    int count = 0;
	float neighborhoodSquared = powf(neighbordist, 2.0);
    for (int i = 0; i < _cells.size(); i++) {
        float d2 = distanceSquared( *_cells[i] );
        if ((d2 > 0) && (d2 < neighborhoodSquared)) {		
//        float d = distance( *_cells[i] );
//        if ((d > 0) && (d < neighbordist)) {
            sum += *_cells[i]; // Add location
            count++;
        }
    }
    if (count > 0) {
        sum /= count;
        return seek(sum);  // Steer towards the location
    } else {
        return ofPoint(0,0);
    }
}

//  Looks for the brightness of the nearest pixel
//
void colonyCell::feedCellWidth( ofPixels &_pixels ){
    int safeX = ofClamp(x,0,ofGetWidth()-1);
	int safeY = ofClamp(y,0,ofGetHeight()-1);
//    if ( x >= 0 && x < ofGetWidth() && y >= 0 && y < ofGetWidth()){
	float value = _pixels.getColor(safeX, safeY).getBrightness();
	
	if(value > nutrientLevel && cellSize <= maxSize){
		
		//grows the cell if there's sufficient nutrients
		//
		cellSize = cellSize + (value/2500.0);
	}
	if (value < nutrientLevel){
		
		// shrinks cell if there's not enough nutrients
		//
		cellSize = cellSize - .01;
	}
	if (age > lifeSpan || hasReplicated){
		cellSize = cellSize - .5;
	}
	
	if (cellSize <= .001){
		dead = true;
	}
	
	if (value > nutrientLevel && age >= fertilityAge){
		shouldReplicate = true;
		hasReplicated = true;
	}
//    }
    age ++;
}

void colonyCell::draw() {
   
    ofPushStyle();
    
//    ofFill();
//    ofSetColor(160 + cellSize*6,255);
//    ofCircle(*this, cellSize);
//    
//    ofFill();
//    ofSetColor(255 - cellSize*4,255);
//    ofCircle(*this, 5 + cellSize/3);
    
    ofPushMatrix();
	ofScale(cellSize, cellSize);
	ofTranslate(*this/cellSize);
	
	spriteMesh.draw();
	
	ofPopMatrix();

	
 //   ofEnableAlphaBlending();
 //	transparency.draw(*this, cellSize);
    
    ofPopStyle();
}