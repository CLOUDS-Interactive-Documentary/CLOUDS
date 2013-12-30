/*
 *  Creature.cpp
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
#include "Creature.h"

namespace itg
{
    float Creature::fogStart = 1000.f;
    float Creature::fogEnd = 1200.f;
    
    Creature::Creature() : magic(false)
	{
	}

    void Creature::slerp(const ofVec3f& target, const ofVec3f& upVector)
    {
        ofVec3f zaxis = (getPosition() - target).normalized();
        ofVec3f xaxis = upVector.getCrossed(zaxis).normalized();
        ofVec3f yaxis = zaxis.getCrossed(xaxis);
        
        ofMatrix4x4 m;
        m._mat[0].set(xaxis.x, xaxis.y, xaxis.z, 0);
        m._mat[1].set(yaxis.x, yaxis.y, yaxis.z, 0);
        m._mat[2].set(zaxis.x, zaxis.y, zaxis.z, 0);
        
        ofQuaternion targetQuat = m.getRotate();
        ofQuaternion currentQuat = getOrientationQuat();
        currentQuat.slerp(0.1, currentQuat, targetQuat);
        setOrientation(currentQuat);
    }
    
    void Creature::integrate()
    {
        velocity += accumulated;
        //velocity *= (1.0 + fear );
        //fear -= 0.2f * fear;
    }
    
    void Creature::updateNormalisedVelocity()
    {
        normalisedVelocity = velocity.normalized();
    }
    
    void Creature::draw(const ofCamera& cam)
    {
        ofPushMatrix();
        if (cam.getPosition().z - getPosition().z > Creature::fogEnd)
        {
            ofTranslate(0, 0, Creature::fogEnd * floor((cam.getPosition().z - getPosition().z) / Creature::fogEnd));
        }
        else if (getPosition().z > cam.getPosition().z)
        {
            ofTranslate(0, 0, -Creature::fogEnd * ceil((getPosition().z - cam.getPosition().z) / Creature::fogEnd));
        }
        ofMultMatrix(getGlobalTransformMatrix());
        customDraw();
        ofPopMatrix();
    }
        
    float Creature::randomGauss(float mean, float stdDev)
    {
        float x, y, r2;
        do
        {
            //Generate a point in a unit circle that is not zero.
            x = ofRandomf();
            y = ofRandomf();
            r2 = x * x + y * y;
        }
        while (r2 > 1.0 || r2 == 0);
        return mean + x * stdDev * sqrt(-2.f * log(r2) / r2);
    }
}