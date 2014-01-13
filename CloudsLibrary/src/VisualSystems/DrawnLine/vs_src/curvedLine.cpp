
#include "curvedLine.h"


void curvedLine::setup(){
    
    a.setPosition(500,300,0);
    b.setParent(a);
    b.setPosition(0,200,0);
    c.setParent(b);
    c.setPosition(0,100,0);
    
    aEnergy = 1;
    bEnergy = 1;
    
    aEnergySmth = 1;
    bEnergySmth = 1;

    
    timeAccumulator = 0;
    timeStep = 0.5;
    timeFrame = 0;
    prevTimeFrame = 0;
    
    prevPosition = c.getGlobalPosition();
    
    matchCount = 0;
    
}


void curvedLine::translateNodeLine( ofPoint trans){
    
    //trans.z = -1;
    for (int i = 0; i < nodeLine.size(); i++){
        nodeLine[i] += trans * 2.0 * timeStep;
    }
    
    
    for (int i = 0; i < nodeLineForMatch.size(); i++){
        nodeLineForMatch[i] += trans * 2.0*timeStep;
    }
}


void curvedLine::update(){
    
    
    
    timeAccumulator += timeStep;
    int nSteps = 0; /// floor(timeAccumulator);
    while (timeAccumulator > 1.0) {
        nSteps++;
        timeAccumulator -= 1.0;
        
    }
    timeFrame += nSteps;
    int nDiffTime = timeFrame - prevTimeFrame;
    
    //cout << nSteps  << endl;
    float speed = powf(0.97, timeStep);
    //0.97;
    prevTimeFrame = timeFrame;
    
    
    //myVar = lerp(myVar, target, 1. - pow(.5, dt))
    
    
    aEnergySmth = speed * aEnergySmth + (1-speed) *aEnergy;
    bEnergySmth = speed * bEnergySmth + (1-speed) *bEnergy;
    
    a.roll(aEnergySmth * timeStep);
    b.roll(bEnergySmth * timeStep);
    //c.getGlobalPosition();
    
    
    for (int i = 0; i < nSteps; i++){
        if ( ofRandom(0,1) > 0.95 ){
            aEnergy = 0.4 * ofRandom(4, 8) * (ofRandom(0,1) > 0.5 ? 1 : -1);
        }
        
        // bEnergy *= 0.96f;
        if (ofRandom(0,1) > 0.95){
            bEnergy = ofRandom(4,8) * (ofRandom(0,1) > 0.5 ? 1 : -1);
        }
    }
    
    nodeLine.addVertex(c.getGlobalPosition());
    
    if (nodeLineForMatch.size() > 0){
        ofPoint pt = nodeLineForMatch[nodeLineForMatch.size()-1];
        if ((pt - c.getGlobalPosition()).length() > 1){
            nodeLineForMatch.addVertex(c.getGlobalPosition());
            matchCount++;
        }
    } else {
        nodeLineForMatch.addVertex(c.getGlobalPosition());
    }
    
    if (nodeLineForMatch.size() > 101){
        nodeLineForMatch.getVertices().erase(nodeLineForMatch.getVertices().begin());
    }
    
    if (nodeLine.size() > 1000){
        nodeLine.getVertices().erase(nodeLine.getVertices().begin());
    }

    prevPosition = c.getGlobalPosition();

}