#ifndef PARTICLE_H
#define PARTICLE_H

#include "ofMain.h"

class VoroParticle : public ofPoint {
public:
    
    VoroParticle();
    
    void    init( ofPoint _pos, ofPoint _vel);
    
    void    addForce( ofPoint &_force );
    void    addRepulsionForce(ofPoint p, float radius, float scale);
    void    addAttractionForce(ofPoint p, float radius, float scale);
    void    addRepulsionForce(VoroParticle *p, float scale);
    void    addRepulsionForce(VoroParticle *p, float radius, float scale);
    void    addAttractionForce(VoroParticle *p, float radius, float scale);
    void    addClockwiseForce(VoroParticle *p, float radius, float scale);
    void    addCounterClockwiseForce(VoroParticle *p, float radius, float scale);
    
    void    infinitWalls();
    void    bounceOffWalls();
    
    void    update();
    
    ofPoint vel;
    ofPoint acc;

    ofTexture *nucle;
    
    float   damping;
    float   size;
};

#endif // PARTICLE_H
