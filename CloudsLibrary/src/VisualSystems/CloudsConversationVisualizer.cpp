//
//  CloudsConversationVisualizer.cpp
//  ConversationNetwork
//
//  Created by James George on 4/17/13.
//
//

#include "CloudsConversationVisualizer.h"

CloudsConversationVisualizer::CloudsConversationVisualizer(){
	
}

CloudsConversationVisualizer::~CloudsConversationVisualizer(){
	
}

void CloudsConversationVisualizer::setup(CloudsFCPParser& parser){
	
	springStrength = .01;
	repulsionForce = .001;
	springRestLength = 1.0;
	
	physics.setWorldSize(ofVec2f(0,0), ofVec2f(ofGetWidth()*10,ofGetHeight()*10));
	physics.setSectorCount(1);
	physics.setDrag(0.1f);
	
	this->parser = &parser;
	
	//TODO: add links!
	for(int i = 0; i < parser.getAllClips().size(); i++){
		ClipMarker& clipA = parser.getAllClips()[i];
		string nameA = clipA.getLinkName();
		vector<ClipMarker> links = parser.getClipsWithKeyword(clipA.keywords);
		for(int j = 0; j < links.size(); j++){
			ClipMarker& clipB = links[j];
			string nameB = clipB.getLinkName();
			if(clipA.person != clipB.person &&
			   parser.getNumberOfSharedKeywords(clipA, clipB) > 1 )
			{
				Particle2D* particleA = NULL;
				Particle2D* particleB = NULL;
				if(nameToParticle.find(nameA) == nameToParticle.end()){
					particleA = physics.makeParticle(ofVec2f(ofRandomWidth(), ofRandomHeight()));
					//add repulsion
					for(int p = 0; p < physics.numberOfParticles()-1; p++){
						physics.makeAttraction(particleA, physics.getParticle(p), -repulsionForce);
					}
				}
				else{
					particleA = nameToParticle[nameA];
				}
				
				if(nameToParticle.find(nameB) == nameToParticle.end()){
					particleB = physics.makeParticle(ofVec2f(ofRandomWidth(), ofRandomHeight()));
					//add repulsion
					for(int p = 0; p < physics.numberOfParticles()-1; p++){
						physics.makeAttraction(particleB, physics.getParticle(p), -repulsionForce);
					}
				}
				else{
					particleB = nameToParticle[nameB];
				}
				
				if(namesToSpring.find(make_pair(nameA,nameB)) == namesToSpring.end() &&
				   namesToSpring.find(make_pair(nameB,nameA)) == namesToSpring.end())
				{
					Spring2D* spring = physics.makeSpring(particleA, particleB, springStrength, springRestLength);
					namesToSpring[make_pair(nameA, nameB)] = spring;
					namesToSpring[make_pair(nameB, nameA)] = spring;
					cout << "added connection between " << nameA << " " << nameB << endl;
				}
			}
		}
	}
	
	//Add analytics...
}


void CloudsConversationVisualizer::update(){
	physics.update();
}

void CloudsConversationVisualizer::draw(){
	
	ofPushMatrix();
	
	ofRectangle screenRect(0,0,ofGetWidth(),ofGetHeight());
	
	float scaleAmount = MIN(screenRect.width/totalRectangle.width,
							screenRect.height/totalRectangle.height);
	
	currentScale += (scaleAmount-currentScale)*.1;
	ofVec2f topCenter = totalRectangle.getTopRight().getInterpolated(totalRectangle.getTopLeft(), .5);
	currentTop += (topCenter-currentTop)*.1;
	
	ofTranslate(ofGetWidth()/2, 0);
	ofScale(currentScale,currentScale);
	ofTranslate(-currentTop);

	physics.draw();
	
	ofPopMatrix();
	
}


