/*
 *  RuleSet.cpp
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
 *    /Volumes/MacData/Projects/openFrameworks/addons/ofxGenerator/example-file-live/bin/data/test.xmlspecific prior written permission. 
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
#include "RuleSet.h"

namespace itg
{
    RuleSet::RuleSet(const string& name) : name(name), totalWeight(0)
    {
    }
    
    void RuleSet::addRule(Rule::Ptr rule, bool calcNormalised)
    {
        rules.push_back(rule);
        totalWeight += rule->getWeight();
        if (calcNormalised) calcWeights();
    }
    
    unsigned RuleSet::randomIdx() const
    {
        vector<float>::const_iterator it = lower_bound(weights.begin(), weights.end(), ofRandomuf());
        return it - weights.begin();
    }
    
    Rule::Ptr RuleSet::randomRule() const
    {
        return rules[randomIdx()];
    }

    Rule::Ptr RuleSet::front() const
    {
        return rules.front();
    }
    
    Rule::Ptr RuleSet::back() const
    {
        return rules.back();
    }
    
    void RuleSet::calcWeights()
    {
        weights.clear();
        for (int i = 0; i < rules.size(); ++i)
        {
            if (i == 0) weights.push_back(rules[i]->getWeight() / totalWeight);
            else weights.push_back(weights[i - 1] + rules[i]->getWeight() / totalWeight);
        }
    }
}