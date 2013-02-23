
#include "CloudsFCPVisualizer.h"

#define	SPRING_MIN_STRENGTH		0.005
#define SPRING_MAX_STRENGTH		0.1

CloudsFCPVisualizer::CloudsFCPVisualizer(){
    database = NULL;
}

void CloudsFCPVisualizer::setupPhysics(){
	physics.clear();
    
    width = ofGetWidth();
	height = ofGetHeight();
    
    cout << "width and height " << width << " " << height << endl;
    
	physics.setWorldSize(ofVec2f(0,0), ofVec2f(width,height));
	physics.setSectorCount(1);
    physics.setDrag(0.94f);
	physics.setDrag(1);		// FIXTHIS
    
    
    //make a particle for each keyword
    //for each clip, create a spring for each other clip it shares a keyword with based on the number of shared words
    if(database == NULL){
        return;
    }
    
    vector<string>& allKeywords = database->getAllKeywords();
    cout << " total of " << allKeywords.size() << endl;
    
    for(int i = 0; i < allKeywords.size(); i++){
        msa::physics::Particle2D *p = physics.makeParticle(ofVec2f(ofRandomWidth(), ofRandomHeight()));
        p->setMass(database->occurrencesOfKeyword(allKeywords[i]));
    }
    
    for(int i = 0; i < allKeywords.size(); i++){
        for(int j = i+1; j < allKeywords.size(); j++){
        }
    }
    
    vector<ClipMarker>& clips = database->getAllClips();
    for(int k = 0; k < allKeywords.size(); k++){

        for(int i = k+1; i < allKeywords.size(); i++){
            int clipsInCommon = 0;

            for(int c = 0; c < clips.size(); c++){
                if(ofContains(clips[c].keywords, allKeywords[k]) &&
                   ofContains(clips[c].keywords, allKeywords[i]))
                {
                    clipsInCommon++;
                }
            }
            
            if(clipsInCommon > 1){
                msa::physics::Particle2D *a = physics.getParticle(k);
                msa::physics::Particle2D *b = physics.getParticle(i);
                physics.makeSpring(a, b, .005 * clipsInCommon, 200 - clipsInCommon*2);
                physics.makeAttraction(a, b, -.05);
                
                //cout << allKeywords[k] << " and " << allKeywords[i] << " share " << clipsInCommon << " clips " << endl;
            }
        }
    }
}

void CloudsFCPVisualizer::updatePhysics(){
    physics.update();
}

void CloudsFCPVisualizer::drawPhysics(){
    
    ofPushStyle();
    
    for(int i = 0; i < physics.numberOfParticles(); i++){
        
        msa::physics::Particle2D *a = physics.getParticle(i);
        ofSetColor(255, 50);
        ofFill();
        ofCircle(a->getPosition(), ( a->getMass()+1));
        
        ofNoFill();
        ofSetColor(0, 50);
        ofCircle(a->getPosition(), ( a->getMass()+1));
    }
    
    
    ofSetColor(0, 130, 200, 40);
    for(int i = 0; i < physics.numberOfSprings(); i++){
        msa::physics::Spring2D* s = physics.getSpring(i);
        ofLine(s->getOneEnd()->getPosition(),
               s->getTheOtherEnd()->getPosition());
    }
    
    ofPopStyle();
}

