/*
 *  Rules.h
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
#pragma once

#include "ofMain.h"
#include "RuleSet.h"
#include "Branch.h"

namespace itg
{
    typedef list<Branch::Ptr>::iterator BranchIt;
    
    class Rules
    {
    public:
        static const string DEFAULT_START_RULE;
        
        typedef Action::Ptr (Rules::*ActionCreator)();
        typedef map<string, ActionCreator>::iterator CreatorIt;
        
        Rules();
        
        unsigned step();
        unsigned step(ofMesh& mesh);
        
        void draw();
        
        void drawNormals(float size);
        
        void clear();
        
        void start();
        
        Rule::Ptr addRule(const string& ruleName, float weight);
        
        Branch::Ptr addBranch(const string& ruleName, const ofMatrix4x4& transform = ofMatrix4x4());
        
        unsigned getMaxDepth() const { return maxDepth; }
        void setMaxDepth(unsigned maxDepth) { this->maxDepth = maxDepth; }
        
        unsigned getNumSteps() const { return numSteps; }
        
        void setStartRule(const string& startRule) { this->startRule = startRule; }
        
        void load(const string& fileName);
        
        void watchFile(const string& watchedFileName, bool autoCheck = true, float checkPeriod = 0.2f);
        
        void checkWatchedFile();
        
        list<Branch::Ptr>& getBranches() { return branches; }
        
        template<class T>
        Action::Ptr createAction()
        {
            return typename T::Ptr(new T);
        }

        template<class T>
        void registerAction(const string& tagName)
        {
            creators[tagName] = &Rules::createAction<T>;
        }
        
        ofEvent<list<Branch::Ptr> > fileReloaded;
        
        ofVboMesh& getMeshRef() { return mesh; }
        
    private:
        string startRule;
        void onUpdate(ofEventArgs& args);
        map<string, RuleSet::Ptr> ruleSets;
        list<Branch::Ptr> branches;
        unsigned maxDepth;
        map<string, ActionCreator> creators;
        ofVboMesh mesh;
        unsigned numSteps;
        
        bool enableVertexColours;
        ofFloatColor meshColour;
        
        bool enableWireframe;
        ofFloatColor wireframeColour;
        
        // file watching stuff
        Poco::Timestamp watchedLastModified;
        string watchedFileName;
        float lastChecked;
        float checkPeriod;
    };
}
