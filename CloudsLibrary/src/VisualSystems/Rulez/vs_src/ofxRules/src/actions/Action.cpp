/*
 *  Action.cpp
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
#include "Action.h"

namespace itg
{
    Action::Action(const string& nextRuleName) : nextRuleName(nextRuleName)
    {
    }
    
    void Action::load(ofxXmlSettings& xml, const string& tagName, unsigned tagIdx)
    {
        setNextRuleName(xml.getAttribute(tagName, "next", "", tagIdx));
        setRepeat(xml.getAttribute(tagName, "repeat", 1, tagIdx));
    }
    
    void Action::save(ofxXmlSettings& xml)
    {
        ofLogError() << "Undefined action save function called";
    }
    
    //////////////////////////////////////////////////////////////////
    // static functions
    //////////////////////////////////////////////////////////////////
    ofFloatColor Action::parseColour(const string& colourString)
    {
        if (colourString.empty()) return ofFloatColor(1.f, 1.f, 1.f);
        
        vector<string> split = ofSplitString(colourString, " ");
        ofFloatColor colour;
        if (split.size() == 1)
        {
            if (colourString.find('.') == string::npos)
            {
                colour.set(atoi(split[0].c_str()) / 255.f, atoi(split[0].c_str()) / 255.f, atoi(split[0].c_str()) / 255.f);
            }
            else
            {
                colour.set(atof(split[0].c_str()), atof(split[0].c_str()), atof(split[0].c_str()));
            }
        }
        else if (split.size() == 3)
        {
            if (colourString.find('.') == string::npos)
            {
                colour.set(atoi(split[0].c_str()) / 255.f, atoi(split[1].c_str()) / 255.f, atoi(split[2].c_str()) / 255.f);
            }
            else
            {
                colour.set(atof(split[0].c_str()), atof(split[1].c_str()), atof(split[2].c_str()));
            }
        }
        else ofLogError() << "Colour format error";
        return colour;
    }
    
    ofMatrix4x4 Action::inverseTranspose(const ofMatrix4x4& transform)
    {
        ofMatrix4x4 normalMatrix = ofMatrix4x4::getTransposedOf(const_cast<ofMatrix4x4&>(transform).getInverse());
        return ofMatrix4x4(normalMatrix(0, 0), normalMatrix(0, 1), normalMatrix(0, 2), 0.f,
                           normalMatrix(1, 0), normalMatrix(1, 1), normalMatrix(1, 2), 0.f,
                           normalMatrix(2, 0), normalMatrix(2, 1), normalMatrix(2, 2), 0.f,
                           0.f,                0.f,                0.f,                1.f);
    }
    
    ofMesh Action::icosahedron(float radius)
    {
        ofMesh mesh;
        
        const float sqrt5 = sqrt(5.0f);
        const float phi = (1.0f + sqrt5) * 0.5f;
        
        /// Step 1 : Generate icosahedron
        float invnorm = 1/sqrt(phi*phi+1);
        
        mesh.addVertex(invnorm*ofVec3f(-1,  phi, 0));//0
        mesh.addVertex(invnorm*ofVec3f( 1,  phi, 0));//1
        mesh.addVertex(invnorm*ofVec3f(0,   1,  -phi));//2
        mesh.addVertex(invnorm*ofVec3f(0,   1,   phi));//3
        mesh.addVertex(invnorm*ofVec3f(-phi,0,  -1));//4
        mesh.addVertex(invnorm*ofVec3f(-phi,0,   1));//5
        mesh.addVertex(invnorm*ofVec3f( phi,0,  -1));//6
        mesh.addVertex(invnorm*ofVec3f( phi,0,   1));//7
        mesh.addVertex(invnorm*ofVec3f(0,   -1, -phi));//8
        mesh.addVertex(invnorm*ofVec3f(0,   -1,  phi));//9
        mesh.addVertex(invnorm*ofVec3f(-1,  -phi,0));//10
        mesh.addVertex(invnorm*ofVec3f( 1,  -phi,0));//11
        
        int firstFaces[] = {
            0,1,2,
            0,3,1,
            0,4,5,
            1,7,6,
            1,6,2,
            1,3,7,
            0,2,4,
            0,5,3,
            2,6,8,
            2,8,4,
            3,5,9,
            3,9,7,
            11,6,7,
            10,5,4,
            10,4,8,
            10,9,5,
            11,8,6,
            11,7,9,
            10,8,11,
            10,11,9
        };
        
        for(int i = 0; i < mesh.getNumVertices(); i++) {
            mesh.setVertex(i, mesh.getVertex(i) * radius);
        }
        
        for(int i = 0; i < 60; i+=3) {
            mesh.addTriangle(firstFaces[i], firstFaces[i+1], firstFaces[i+2]);
        }
        
        return mesh;
    }
    
    // based on code by Michael Broutin for the ogre-procedural library //
    // http://code.google.com/p/ogre-procedural/source/browse/library/src/ProceduralIcoSphereGenerator.cpp
    // For the latest info, see http://code.google.com/p/ogre-procedural/ //
    //----------------------------------------------------------
    ofMesh Action::icosphere(float radius, int iterations)
    {
        //ofMesh icosahedron = ofGetIcosahedronMesh( 1.f );
        ofMesh icosahedron = Action::icosahedron( 1.f );
        vector<ofVec3f> vertices = icosahedron.getVertices();
        vector<ofIndexType> faces = icosahedron.getIndices();
        
        int size = faces.size();
        
        /// Step 2 : tessellate
        for (unsigned short iteration = 0; iteration < iterations; iteration++)
        {
            size*=4;
            vector<ofIndexType> newFaces;
            newFaces.clear();
            //newFaces.resize(size);
            for (int i=0; i<size/12; i++)
            {
                int i1 = faces[i*3];
                int i2 = faces[i*3+1];
                int i3 = faces[i*3+2];
                int i12 = vertices.size();
                int i23 = i12+1;
                int i13 = i12+2;
                ofVec3f v1 = vertices[i1];
                ofVec3f v2 = vertices[i2];
                ofVec3f v3 = vertices[i3];
                //make 1 vertice at the center of each edge and project it onto the sphere
                vertices.push_back((v1+v2).normalized());
                vertices.push_back((v2+v3).normalized());
                vertices.push_back((v1+v3).normalized());
                //now recreate indices
                newFaces.push_back(i1);
                newFaces.push_back(i12);
                newFaces.push_back(i13);
                newFaces.push_back(i2);
                newFaces.push_back(i23);
                newFaces.push_back(i12);
                newFaces.push_back(i3);
                newFaces.push_back(i13);
                newFaces.push_back(i23);
                newFaces.push_back(i12);
                newFaces.push_back(i23);
                newFaces.push_back(i13);
            }
            faces.swap(newFaces);
        }
        
        /// Step 3 : generate texcoords
        vector<ofVec2f> texCoords;
        for (unsigned short i=0;i<vertices.size();i++)
        {
            const ofVec3f& vec = vertices[i];
            float u, v;
            float r0 = sqrtf(vec.x*vec.x+vec.z*vec.z);
            float alpha;
            alpha = atan2f(vec.z,vec.x);
            u = alpha/TWO_PI+.5f;
            v = atan2f(vec.y, r0)/PI + .5f;
            // reverse the u coord, so the default is texture mapped left to
            // right on the outside of a sphere //
            texCoords.push_back(ofVec2f(1.0-u,v));
        }
        
        /// Step 4 : fix texcoords
        // find vertices to split
        std::vector<unsigned int> indexToSplit;
        
        for (unsigned int i=0;i<faces.size()/3;i++)
        {
            ofVec2f& t0 = texCoords[faces[i*3+0]];
            ofVec2f& t1 = texCoords[faces[i*3+1]];
            ofVec2f& t2 = texCoords[faces[i*3+2]];
            
            if (abs(t2.x-t0.x)>0.5)
            {
                if (t0.x<0.5)
                    indexToSplit.push_back(faces[i*3]);
                else
                    indexToSplit.push_back(faces[i*3+2]);
            }
            if (abs(t1.x-t0.x)>0.5)
            {
                if (t0.x<0.5)
                    indexToSplit.push_back(faces[i*3]);
                else
                    indexToSplit.push_back(faces[i*3+1]);
            }
            if (abs(t2.x-t1.x)>0.5)
            {
                if (t1.x<0.5)
                    indexToSplit.push_back(faces[i*3+1]);
                else
                    indexToSplit.push_back(faces[i*3+2]);
            }
        }
        
        //split vertices
        for (unsigned short i=0;i<indexToSplit.size();i++)
        {
            unsigned int index = indexToSplit[i];
            //duplicate vertex
            ofVec3f v = vertices[index];
            ofVec2f t = texCoords[index] + ofVec2f(1.f, 0.f);
            vertices.push_back(v);
            texCoords.push_back(t);
            int newIndex = vertices.size()-1;
            //reassign indices
            for (unsigned short j=0;j<faces.size();j++)
            {
                if (faces[j]==index)
                {
                    int index1 = faces[(j+1)%3+(j/3)*3];
                    int index2 = faces[(j+2)%3+(j/3)*3];
                    if ((texCoords[index1].x>0.5) || (texCoords[index2].x>0.5))
                    {
                        faces[j] = newIndex;
                    }
                }
            }
        }
        
        // tig: flip face(=triangle) winding order, so that we are consistent with all other ofPrimitives.
        // i wish there was a more elegant way to do this, but anything happening before "split vertices"
        // makes things very, very complicated.
        
        for (int i = 0; i < (int)faces.size(); i+=3) {
            std::swap(faces[i+1], faces[i+2]);
        }
        
        ofMesh sphere;
        
        sphere.addIndices( faces );
        sphere.addNormals( vertices );
        sphere.addTexCoords( texCoords );
        
        for(unsigned int i = 0; i < vertices.size(); i++ ) {
            vertices[i] *= radius;
        }
        sphere.addVertices( vertices );
        
        return  sphere;
    }
}
