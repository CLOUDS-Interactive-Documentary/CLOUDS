/*
 *  Rule.h
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
#include <tr1/memory>
#include "Action.h"
#include "Branch.h"

namespace itg
{
    using namespace tr1;
    
    class Rule
    {
    public:
        typedef shared_ptr<Rule> Ptr;
        
        vector<Branch::Ptr> step(Branch::Ptr branch, ofMesh& mesh);
        
        Rule(const float weight);
        
        float getWeight() const { return weight; }
        
        template<class T>
        typename T::Ptr addAction(const string& nextRuleName = "")
        {
            typename T::Ptr action = typename T::Ptr(new T());
            if (!nextRuleName.empty()) action->setNextRuleName(nextRuleName);
            actions.push_back(action);
            return action;
        }
        
        void addAction(Action::Ptr action) { actions.push_back(action); }
        
        unsigned size() const { return actions.size(); }

        Action::Ptr operator[](unsigned i) const { return actions[i]; }
        
    private:
        float weight;
        vector<Action::Ptr> actions;
    };
}