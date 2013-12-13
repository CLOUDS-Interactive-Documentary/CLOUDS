/*
 *  Creature.h
 *
 *  Copyright (c) 2012, Neil Mendoza, http://www.neilmendoza.com
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met: 
 *  
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE. 
 *
 */
#pragma once

#include "ofMain.h"
#ifdef TARGET_WIN32
#include <memory>
#else
#include <tr1/memory>
#endif
#include "ofxUi.h"

namespace itg
{
    using namespace tr1;
    
    class Creature : public ofNode
    {
    public:
        enum CreatureType
        {
            GEOMETRIC,
            WOBBLY,
            MODEL,
            JELLY,
            POINT
        };
        
        typedef shared_ptr<Creature> Ptr;
        
        static float fogStart, fogEnd;
        
		Creature();
        ~Creature() {}
        
		virtual void update() {};
        virtual void customDraw() = 0;
        
        void setVelocity(float x, float y, float z) { velocity.set(x, y, z); }
        void setVelocity(const ofVec3f& velocity) { this->velocity = velocity; }
        ofVec3f getVelocity() const { return velocity; }
        
        ofVec3f getNormalisedVelocity() const { return normalisedVelocity; }
        
        // spherical linear interpolation
        // at the moment, this function assumes that this node has no parents
        void slerp(const ofVec3f& target, const ofVec3f& upVector);
        
        // flocking physics
        void zeroAccumulated() { accumulated.set(0, 0, 0); }
        //virtual void accumulateForces(vector<Creature::Ptr>& creatures, float zoneRadiusSqrd, float alignmentLower, float alignmentUpper, 
        //                              float repelStrength, float attractStrength, float alignStrength, VectorXi& indices);
        // for the moment this is simple euler
        void accumulate(const ofVec3f& force) { accumulated += force; }
        virtual void integrate();
        void updateNormalisedVelocity();
        virtual void accumulateOtherForces() {}
        
        void setFrequency(const float frequency) { this->frequency = frequency; }
        float getFrequency() { return frequency; }
        
        CreatureType getType() const { return type; }
        
		// make the creature magic if someone is interacting with it
		void setMagic(bool magic) { this->magic = magic; }
		bool getMagic() { return magic; }
        
        static float randomGauss(float mean, float stdDev);
        
        virtual ofFloatColor getColour() const = 0;

    protected:
        // forces
        ofVec3f accumulated;
        CreatureType type;
        float frequency;
        ofVec3f velocity;
		
    private:
        bool magic;
        //float fear;
        ofVec3f normalisedVelocity;
    };
}