//
//  ParkedCar.cpp
//  yearLong
//
//  Created by Surya on 31/03/13.
//
//

#include "ParkedCar.h"

ParkedCar::ParkedCar(){
    
}
//TODO: Initialist variables correctly
ParkedCar::ParkedCar(ofImage _img, ofVec2f _location){
    
    img = _img;
    acceleration = ofVec2f(0,0);
    velocity = ofVec2f(0,0);
    location = _location;
    maxSpeed =50;
    maxForce = 5;
    
    // TODO check if you need W,H if the image has fron through resize
    float fbo_x, fbo_y; // fbo_w, fbo_h;

}


void ParkedCar::update(){
    velocity += acceleration;
    velocity.limit(maxSpeed);
    location += velocity;
    acceleration *=0;
}

void ParkedCar::seek(ofVec2f target){
    ofVec2f desired = target - location;
    desired = desired.normalized();
    desired *= 0.5;
    
    ofVec2f steer = desired - velocity;
    steer.limit(maxForce);
    applyForce(steer);
}

void ParkedCar::applyForce(ofVec2f force){
    acceleration += force;
    
}

void ParkedCar::follow(Path p){
    ofVec2f predict = ofVec2f(velocity);
    predict =predict.normalized();
    predict *= 25;
    ofVec2f predictLoc = predict+location;
    
    ofVec2f a = p.start;
    ofVec2f b = p.end;
    ofVec2f normalPoint = getNormalPoint(predictLoc, a, b);
    
    ofVec2f dir = b-a;
    dir=dir.normalized();
    dir *= 10;
    
    ofVec2f target = normalPoint+dir;
    
    float distance = normalPoint.distance(predictLoc);
    
    if(distance>p.radius){
        seek(target);
    }
}

ofVec2f ParkedCar::getNormalPoint(ofVec2f p, ofVec2f a, ofVec2f b){
    ofVec2f ap = p-a;
    ofVec2f ab = b-a;

    ab = ab.normalized();
        ab *= ap.dot(ab);
    ofVec2f normalPoint = a +ab;
    
    return normalPoint;
}
//void ParkedCar::follow(FlowField flow){
//    ofVec2f desired = flow.lookUp(location);
//    desired.limit(maxSpeed);
//    
//    ofVec2f steer = desired - velocity;
//    steer.limit(maxForce);
//    applyForce(steer);
//    
//}
void ParkedCar::draw(){
    img.draw(location.x,location.y,50,100);
}