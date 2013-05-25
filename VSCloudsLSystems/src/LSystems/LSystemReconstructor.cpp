//
//  LSystemReconstructor.cpp
//  lSystem
//
//  Created by Patricio Gonzalez Vivo on 5/14/13.
//
//

#include "LSystemReconstructor.h"
LSystemReconstructor::LSystemReconstructor(){
    bGrow = false;
    speed = 1.0;
    mesh.setMode(OF_PRIMITIVE_LINES);
}

void LSystemReconstructor::setup(LSystem &_lsys, int _deep){
    clear();
    nodes.clear();
    
    _lsys.make(_deep);
    if (_lsys.mesh.getVertices().size() > 2){
        lineTo(_lsys.mesh.getVertices()[0]);
        addNode(_lsys.mesh.getVertices()[0]);
        
        for (int i = 1; i < _lsys.mesh.getVertices().size();i++){
            if ( i%2 == 1){
                lineTo(_lsys.mesh.getVertices()[i]);
            } else {
                if ( _lsys.mesh.getVertices()[i] != _lsys.mesh.getVertices()[i-1]){
                    newSubPath();
                    lineTo(_lsys.mesh.getVertices()[i]);
                    addNode(_lsys.mesh.getVertices()[i]);
                }
            }
        }
        
        setFilled(false);
    }
    
    if (nodes.size() > 0){
        nodes[0].startTime = 0;//1.5;
    }
}

void LSystemReconstructor::addNode(ofPoint &_pnt){
    for (int i = nodes.size()-1; i >=0; i--) {
        if ( nodes[i] == _pnt ){
            nodes[i].branchesIndex.push_back( getSubPaths().size()-1 );
            return;
        }
    }
    
    LNode node;
    node.set(_pnt);
    node.startTime = -1.0;
    node.branchesIndex.push_back( getSubPaths().size()-1 );
    nodes.push_back( node );
}

float LSystemReconstructor::getPct(){
    return ofClamp(pct,0.0,1.0);
}

void LSystemReconstructor::start(){
    bGrow = true;
}

void LSystemReconstructor::stop(){
    lastTime = ofGetElapsedTimef();
    pct = time = 0.0;
    bGrow = false;
}

void LSystemReconstructor::pause(){
    bGrow = false;
}

void LSystemReconstructor::update(){
    
    //  Update Time
    //
    float now = ofGetElapsedTimef();
    float diff = now - lastTime;
    if (bGrow){
        
        //  Go foward on time
        //
        time += diff*speed;
        
        //  Render
        //
        mesh.clear();
        activeNodes.clear();
        
        pct = 0.0;
        for(int i = 0; i < nodes.size(); i++){
            if (nodes[i].startTime >= 0.0){
                float relativeTime = time - nodes[i].startTime;
                for (int j = 0; j < nodes[i].branchesIndex.size(); j++){
                    renderBranch( nodes[i].branchesIndex[j], relativeTime, ofNoise(nodes[i].x+ofGetElapsedTimef()*0.01,nodes[i].y));
                }
            }
        }
    }
    
    lastTime = now;
}

void LSystemReconstructor::draw(){
    mesh.draw();
    
    for(int i = 0; i < activeNodes.size(); i++){
        ofNoFill();
        ofCircle(activeNodes.getVertices()[i], 1);
    }
}

void LSystemReconstructor::renderBranch(int _index, float _relativeTime, float _speed){
    
    int totalPoints = getOutline()[_index].size();
    int drawPoints = 0;
    
    ofFloatColor color = ofFloatColor(1.0,0.0,0.0);
    float hue = 0.0;
    
    for (int k = 0 ; k < totalPoints-1; k++){
        float thisTime = _speed*(float)k;
        float nextTime = _speed*((float)k+1.0f);;
        
        if ( k == totalPoints-1){
            nextTime = thisTime;
        }
    
        if (_relativeTime > thisTime && _relativeTime < nextTime ){
            float part = _relativeTime - thisTime;
            float whole = nextTime - thisTime;
            float pct = part / whole;
            
            ofPoint A = getOutline()[ _index ][k];
            ofPoint B = getOutline()[ _index ][k+1];
            
            // figure out where we are between a and b
            //
            ofPoint pos = (1.0-pct)*A + (pct)*B;
            
            activeNodes.addVertex(pos);
            
            hue = (float)(k)/(float)totalPoints;
            color.setHue(hue);
            mesh.addColor(color);
            mesh.addVertex(getOutline()[ _index ][k]);
            
            hue = (float)(k+1)/(float)totalPoints;
            color.setHue(hue);
            mesh.addColor(color);
            mesh.addVertex(pos);
            
        } else if ( _relativeTime > thisTime ){
            ofPoint pos = getOutline()[ _index ][k];
            
            hue = (float)k/(float)totalPoints;
            color.setHue(hue);
            mesh.addColor(color);
            mesh.addVertex(pos);
            
            hue = (float)(k+1)/(float)totalPoints;
            color.setHue(hue);
            mesh.addColor(color);
            mesh.addVertex(getOutline()[ _index ][k+1]);
            
            //  check if pass over a node
            //
            int index = isNode(pos);
            if (index != -1){
                if ( nodes[index].startTime == -1.0 ){
                    nodes[index].startTime = time+ofRandom(2.0f)*_speed;
                }
            }
            
        } else {
            break;
        }
    }
}

int LSystemReconstructor::isNode(ofPoint &_pnt){
    for (int i = nodes.size()-1; i >= 0; i--) {
        if ( nodes[i] == _pnt ){
            return i;
        }
    }
    
    return -1;
}