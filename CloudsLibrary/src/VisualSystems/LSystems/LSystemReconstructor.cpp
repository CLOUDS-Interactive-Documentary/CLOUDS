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
    aNoise = 0.7;
    tNoise = 0.1;
    bornRandom = 2.0;
    growMesh.setMode(OF_PRIMITIVE_LINES);
}

void LSystemReconstructor::setup(LSystem &_lsys, int _deep){
    lines.clear();
    lines.push_back(ofPolyline());
    
    nodes.clear();
    
    _lsys.unoise = aNoise;
    _lsys.utime = tNoise;
    _lsys.make(_deep);
    
    originalMesh = _lsys.mesh;
    
    if (_lsys.mesh.getVertices().size() > 2){
        lineTo(_lsys.mesh.getVertices()[0]);
        addNode(_lsys.mesh.getVertices()[0]);
        
        for (int i = 1; i < _lsys.mesh.getVertices().size();i++){
            if ( i%2 == 1){
                lineTo(_lsys.mesh.getVertices()[i]);
            } else {
                if ( _lsys.mesh.getVertices()[i] != _lsys.mesh.getVertices()[i-1]){
                    lines.push_back(ofPolyline());
                    lineTo(_lsys.mesh.getVertices()[i]);
                    addNode(_lsys.mesh.getVertices()[i]);
                }
            }
        }
    }
    
    if (nodes.size() > 0){
        nodes[0].startTime = 0;//1.5;
    }
}

void LSystemReconstructor::lineTo(ofPoint &_pnt){
    lines[ lines.size()-1 ].addVertex(_pnt);
}

void LSystemReconstructor::addNode(ofPoint &_pnt){
    for (int i = nodes.size()-1; i >=0; i--) {
        if ( nodes[i] == _pnt ){
            nodes[i].branchesIndex.push_back( lines.size()-1 );
            return;
        }
    }
    
    LNode node;
    node.set(_pnt);
    node.startTime = -1.0;
    node.pct = -1.0;
    node.branchesIndex.push_back( lines.size()-1 );
    nodes.push_back( node );
}

float LSystemReconstructor::getPct(){
    return ofClamp(pct,0.0,1.0);
}

void LSystemReconstructor::init(){
    timeLast = ofGetElapsedTimef();
    pct = time = 0.0;
}

void LSystemReconstructor::update(){
    
    //  Update Time
    //
    float timeNow = ofGetElapsedTimef();
    float timeDiff = timeNow - timeLast;
    activeNodes.clear();
    
    if (bGrow){
        //  Go foward on time
        //
        time += timeDiff*speed;
        
        //  Render
        //
        growMesh.clear();
        pct = 0.0;
        for(int i = 0; i < nodes.size(); i++){
            if (nodes[i].startTime >= 0.0){
                float relativeTime = time - nodes[i].startTime;
                for (int j = 0; j < nodes[i].branchesIndex.size(); j++){
                    renderBranch( growMesh, nodes[i].branchesIndex[j], relativeTime, ofNoise(nodes[i].x+ofGetElapsedTimef()*0.01,nodes[i].y));
                }
            }
        }
    }
    
    if (bFlow){
        for(int i = 0; i < nodes.size(); i++){
            if (nodes[i].pct >= 0.0){
                
                if (bGrow){
                    if (nodes[i].startTime < 0.0){
                        break;
                    }
                }
                
                for (int j = 0; j < nodes[i].branchesIndex.size(); j++){
                    
                    int index = nodes[i].branchesIndex[j];
                    int totalPoints = lines[ index ].size();
                    
                    float line = (totalPoints-1)*nodes[i].pct;
                    int k = line;
                        
                    float pct = line-(int)(line);
                    
                    ofPoint A = lines[ index ][k];
                    ofPoint B = lines[ index ][k+1];
                    ofPoint pos = (1.0-pct)*A + (pct)*B;
                    
                    activeNodes.addVertex(pos);
                }
                
                nodes[i].pct += speed*0.01;
                
                if(nodes[i].pct > 1.0){
                    nodes[i].pct = -1.0;
                }
            } else {
                nodes[i].pct = ofRandom(-10.0,0.1);
            }
        }
    }
    
    timeLast = timeNow;
}

void LSystemReconstructor::renderBranch(ofMesh &_mesh, int _index, float _relativeTime, float _speed){
    
    int totalPoints = lines[_index].size();
    int drawPoints = 0;
    
    for (int k = 0 ; k < totalPoints-1; k++){                  
        float thisTime = _speed*(float)k;
        float nextTime = _speed*((float)k+1.0f);
        
        if ( k == totalPoints-1){
            nextTime = thisTime;
        }
    
        if (_relativeTime > thisTime && _relativeTime < nextTime ){
            float part = _relativeTime - thisTime;
            float whole = nextTime - thisTime;
            float pct = part / whole;
            
            ofPoint A = lines[ _index ][k];
            ofPoint B = lines[ _index ][k+1];
            
            // figure out where we are between a and b
            //
            ofPoint pos = (1.0-pct)*A + (pct)*B;
            
            activeNodes.addVertex(pos);
            
            _mesh.addVertex(lines[ _index ][k]);
            _mesh.addVertex(pos);
            
        } else if ( _relativeTime > thisTime ){
            ofPoint pos = lines[ _index ][k];
            
            _mesh.addVertex(pos);
            _mesh.addVertex(lines[_index][k+1]);
            
            //  check if pass over a node
            //
            int index = isNode(pos);
            if (index != -1){
                if ( nodes[index].startTime == -1.0 ){
                    nodes[index].startTime = time+bornRandom*_speed;
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