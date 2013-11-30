/*
 *  Rule.cpp
 *
 *  Copyright (c) 2013, Neil Mendoza, http://www.neilmendoza.com
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
#include "TransformAction.h"

namespace itg
{
    TransformAction::TransformAction(const string& nextRuleName, const ofMatrix4x4& transform) :
        Action(nextRuleName), transform(transform)
    {
        
    }
    
    void TransformAction::load(ofxXmlSettings& xml, const string& tagName, unsigned tagIdx)
    {
        Action::load(xml, tagName, tagIdx);
        parseTransforms(xml.getAttribute(tagName, "transforms", "", tagIdx));
    }
    
    /*void TransformAction::save(ofxXmlSettings& xml)
    {
        
    }*/
    
    Branch::Ptr TransformAction::step(Branch::Ptr branch, ofMesh& mesh)
    {
        // really this stuff should go in Action
        return Branch::Ptr(new Branch(getNextRuleName(), branch->getDepth() + 1, transform * branch->getTransform(), branch->getTransform(), getName()));
    }
    
    void TransformAction::translate(const ofVec3f& translation)
    {
        transform.translate(translation);
    }

    void TransformAction::translate(float x, float y, float z)
    {
        translate(ofVec3f(x, y, z));
    }
    
    void TransformAction::rotate(const ofQuaternion& rotation)
    {
        transform.rotate(rotation);
    }
    
    void TransformAction::rotate(const ofVec3f& euler)
    {
        ofQuaternion quat(euler.x, ofVec3f(1,0,0), euler.y, ofVec3f(0,1,0), euler.z, ofVec3f(0,0,1));
        rotate(quat);
    }
    
    void TransformAction::rotate(float x, float y, float z)
    {
        rotate(ofVec3f(x, y, z));
    }
    
    void TransformAction::scale(float scale)
    {
        transform.scale(ofVec3f(scale, scale, scale));
    }
    
    void TransformAction::scale(const ofVec3f& scale)
    {
        transform.scale(scale);
    }
    
    void TransformAction::parseTransforms(const string& transforms)
    {
        vector<string> split = ofSplitString(transforms, " ");
        for (int i = 0; i < split.size(); ++i)
        {
            string command = split[i];
            if (command == "tx")
            {
                float x = atof(split[++i].c_str());
                transform.translate(x, 0, 0);
            }
            else if (command == "ty")
            {
                float y = atof(split[++i].c_str());
                transform.translate(0, y, 0);
            }
            else if (command == "tz")
            {
                float z = atof(split[++i].c_str());
                transform.translate(0, 0, z);
            }
            else if (command == "t")
            {
                float x = atof(split[++i].c_str());
                float y = atof(split[++i].c_str());
                float z = atof(split[++i].c_str());
                transform.translate(x, y, z);
            }
            else if (command == "rx")
            {
                float theta = atof(split[++i].c_str());
                transform.rotate(theta, 1, 0, 0);
            }
            else if (command == "ry")
            {
                float theta = atof(split[++i].c_str());
                transform.rotate(theta, 0, 1, 0);
            }
            else if (command == "rz")
            {
                float theta = atof(split[++i].c_str());
                transform.rotate(theta, 0, 0, 1);
            }
            else if (command == "sx")
            {
                float x = atof(split[++i].c_str());
                transform.scale(x, 1, 1);
            }
            else if (command == "sy")
            {
                float y = atof(split[++i].c_str());
                transform.scale(1, y, 1);
            }
            else if (command == "sz")
            {
                float z = atof(split[++i].c_str());
                transform.scale(1, 1, z);
            }
            else if (command == "sa")
            {
                float s = atof(split[++i].c_str());
                transform.scale(s, s, s);
            }
            else if (command == "s")
            {
                float x = atof(split[++i].c_str());
                float y = atof(split[++i].c_str());
                float z = atof(split[++i].c_str());
                transform.scale(x, y, z);
            }
        }
    }
}