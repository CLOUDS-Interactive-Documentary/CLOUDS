//
//  ParticleEdge.h
//  CloudsFCPParser
//
//  Created by Surya Mattu on 12/06/13.
//
//

#ifndef __CloudsFCPParser__ParticleEdge__
#define __CloudsFCPParser__ParticleEdge__

#include <iostream>
#include "ofMain.h"
#include "MSAPhysics2D.h"

class ParticleEdge{
public:
    
    ParticleEdge(msa::physics::Particle2D* _p, msa::physics::Particle2D* _a, float _d);
    msa::physics::Particle2D* p;
    msa::physics::Particle2D* a;
    float length;
    void setLength(float _l);
    
    
};
#endif /* defined(__CloudsFCPParser__ParticleEdge__) */
