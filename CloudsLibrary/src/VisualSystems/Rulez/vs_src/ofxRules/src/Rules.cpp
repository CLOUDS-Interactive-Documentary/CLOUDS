/*
 *  Rules.cpp
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
#include "Rules.h"
#include "ofxXmlSettings.h"
#include "LineAction.h"
#include "TransformAction.h"
#include "PointAction.h"
#include "CubeAction.h"
#include "TubeAction.h"
#include "IcosphereAction.h"
#include "PlaneAction.h"
#include "ConeAction.h"

namespace itg
{
    const string Rules::DEFAULT_START_RULE = "start";
    
    Rules::Rules() :
        maxDepth(numeric_limits<unsigned>::max()), numSteps(0)
    {
        registerAction<LineAction>("line");
        registerAction<PointAction>("point");
        registerAction<TransformAction>("transform");
        registerAction<CubeAction>("cube");
        registerAction<TubeAction>("tube");
        registerAction<IcosphereAction>("ico");
        registerAction<IcosphereAction>("icosphere");
        registerAction<PlaneAction>("plane");
        registerAction<ConeAction>("cone");
        
        mesh.setUsage(GL_DYNAMIC_DRAW);
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
    }
    
    void Rules::start()
    {
        if (startRule.empty()) addBranch(DEFAULT_START_RULE);
        else addBranch(startRule);
    }
    
    unsigned Rules::step()
    {
        return step(mesh);
    }
    
    unsigned Rules::step(ofMesh& mesh)
    {
        numSteps++;
        list<Branch::Ptr> newBranches;
        unsigned activeRuleSets = 0;
        
        // step over the system like this rather than recursively to save on memory and
        // so that we can have easy access to what's going on with the branches
        for (list<Branch::Ptr>::iterator it = branches.begin(); it != branches.end(); ++it)
        {
            if ((*it)->getDepth() < maxDepth && !(*it)->getNextRuleName().empty())
            {
                auto ruleSet = ruleSets.find((*it)->getNextRuleName());
                if (ruleSet != ruleSets.end())
                {
                    //RuleSet::Ptr ruleSet = ruleSets[(*it)->getNextRuleName()];
                    
                    vector<Branch::Ptr> children = ruleSet->second->randomRule()->step(*it, mesh);
                    newBranches.insert(newBranches.end(), children.begin(), children.end());
                    activeRuleSets++;
                }
                else ofLogError() << "No ruleSet with name " << (*it)->getNextRuleName();
            }
        }
        branches = newBranches;
        return activeRuleSets;
    }
    
    void Rules::draw()
    {
        ofPushStyle();
        // draw mesh
        if (!enableVertexColours)
        {
            mesh.disableColors();
            ofSetColor(meshColour);
        }
        mesh.draw();
        if (!enableVertexColours) mesh.enableColors();
        
        // draw wireframe
        if (enableWireframe)
        {
            ofSetLineWidth(1.5);
            mesh.disableColors();
            ofSetColor(wireframeColour);
            mesh.drawWireframe();
            mesh.enableColors();
        }
        ofPopStyle();
    }
    
    void Rules::drawNormals(float size)
    {
        for (int i = 0; i < mesh.getNumVertices(); ++i)
        {
            ofVec3f vertex = mesh.getVertex(i);
            ofVec3f normal = mesh.getNormal(i);
            
            ofLine(vertex, vertex + size * normal);
        }
    }
    
    void Rules::load(const string& fileName)
    {
        ruleSets.clear();
        
        ofLogNotice() << "=================================================";
        ofLogNotice() << "Parsing " << fileName;

        ofxXmlSettings xml;
        xml.loadFile(fileName);
        
        string meshColourStr = xml.getAttribute("rules", "meshColour", "");
        if (meshColourStr.empty()) enableVertexColours = true;
        else
        {
            enableVertexColours = false;
            meshColour = Action::parseColour(meshColourStr);
        }
        
        string wireframeColourStr = xml.getAttribute("rules", "wireframeColour", "");
        if (wireframeColourStr.empty()) enableWireframe = false;
        else
        {
            enableWireframe = true;
            wireframeColour = Action::parseColour(wireframeColourStr);
        }
        
        setMaxDepth(xml.getAttribute("rules", "maxDepth", (int)numeric_limits<unsigned>::max()));
        setStartRule(xml.getAttribute("rules", "startRule", ""));
        string primitive = xml.getAttribute("rules", "primitive", "");
        if (!primitive.empty())
        {
            if (primitive == "triangles") mesh.setMode(OF_PRIMITIVE_TRIANGLES);
            else if (primitive == "triangle_strip") mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            else if (primitive == "triangle_fan") mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            else if (primitive == "lines") mesh.setMode(OF_PRIMITIVE_LINES);
            else if (primitive == "line_strip") mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
            else if (primitive == "line_loop") mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
            else if (primitive == "points") mesh.setMode(OF_PRIMITIVE_POINTS);
        }
        xml.pushTag("rules");
        for (unsigned i = 0; i < xml.getNumTags("ruleSet"); ++i)
        {
            string name = xml.getAttribute("ruleSet", "name", "", i);
            xml.pushTag("ruleSet", i);
            ofLogNotice() << "Ruleset: " << name << ", num rules: " << xml.getNumTags("rule");
            for (unsigned j = 0; j < xml.getNumTags("rule"); ++j)
            {
                Rule::Ptr rule = addRule(name, xml.getAttribute("rule", "weight", 0.f, j));
                xml.pushTag("rule", j);
                
                for (CreatorIt it = creators.begin(); it != creators.end(); ++it)
                {
                    for (unsigned k = 0; k < xml.getNumTags(it->first); ++k)
                    {
                        Action::Ptr action = (this->*it->second)();
                        action->setName(it->first);
                        action->setRuleName(name);
                        action->load(xml, it->first, k);
                        rule->addAction(action);
                    }
                }
                xml.popTag();
            }
            xml.popTag();
        }
    }

    void Rules::watchFile(const string& watchedFileName, bool autoCheck, float checkPeriod)
    {
        this->watchedFileName = watchedFileName;
        watchedLastModified = Poco::Timestamp(0);
        if (autoCheck)
        {
            lastChecked = 0.f;
            this->checkPeriod = checkPeriod;
            ofAddListener(ofEvents().update, this, &Rules::onUpdate);
        }
    }

    void Rules::onUpdate(ofEventArgs& args)
    {
        if (ofGetElapsedTimef() - lastChecked > checkPeriod)
        {
            checkWatchedFile();
            lastChecked = ofGetElapsedTimef();
        }
    }
    
    void Rules::checkWatchedFile()
    {
        Poco::File file = ofFile(watchedFileName).getPocoFile();
        Poco::Timestamp timestamp = file.getLastModified();
        if (timestamp != watchedLastModified)
        {
            clear();
            load(watchedFileName);
            ofNotifyEvent(fileReloaded, branches, this);
            watchedLastModified = timestamp;
        }
    }
    
    void Rules::clear()
    {
        mesh.clear();
        branches.clear();
    }
    
    Rule::Ptr Rules::addRule(const string& ruleName, float weight)
    {
        if (!ruleSets[ruleName]) ruleSets[ruleName] = RuleSet::Ptr(new RuleSet(ruleName));
        ruleSets[ruleName]->addRule(Rule::Ptr(new Rule(weight)));
        return ruleSets[ruleName]->back();
    }
    
    Branch::Ptr Rules::addBranch(const string& ruleName, const ofMatrix4x4& transform)
    {
        if (ruleSets.find(ruleName) == ruleSets.end())
        {
            ofLogError() << "No ruleSet with name " << ruleName << ".  Add ruleSet name start or attribute startRule to outermost tag.";
            return Branch::Ptr();
        }
        Branch::Ptr branch = Branch::Ptr(new Branch(ruleName, 0, transform));
        branches.push_back(branch);
        return branch;
    }
}
