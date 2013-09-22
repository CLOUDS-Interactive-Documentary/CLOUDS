//
//  ParticleEdge.cpp
//  CloudsFCPParser
//
//  Created by Surya Mattu on 12/06/13.
//
//

#include "ParticleEdge.h"


ParticleEdge::ParticleEdge(msa::physics::Particle2D* _p, msa::physics::Particle2D* _a, float _d){
    
    p = _p;
    a = _a;
    length = _d;
}


void ParticleEdge::setLength(float _l){
    
    length = _l;
}