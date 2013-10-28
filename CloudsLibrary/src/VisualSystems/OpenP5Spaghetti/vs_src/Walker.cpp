//
//  Walker.cpp
//  OpenP5Spaghetti
//
//  Created by Jonathan Minard on 10/25/13.
//
//

#include "Walker.h"

Walker::Walker(){
	
}

void Walker::init(int _nParticles, ofColor _color){
	position = ofVec3f(0,0,0);
	nParticles = _nParticles;
        
	seed = ofRandom(100);
	//c = new color[1];
//        particles.reserve(500);
//        particles[0].position.x = 0;
//        particles[0].position.y = 0;
//        particles[0].position.z = 0;
	maxX = minX = maxY = minY = 0;
	minZ = maxZ = 0;
	i = 0;
    j = 0; 
	
	color = ofFloatColor::fromHsb( ofRandomuf(), .3, .95 );
	
	mesh.addColor(color);
	mesh.addVertex(position);
	mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    //mesh.setMode(OF_PRIMITIVE_POINTS);
    
}
	
void Walker::step(){

	position.x += (ofRandom(3) - 1.5);
	position.y += (ofRandom(3) - 1.5);
	position.z += (ofRandom(3) - 1.5);

	mesh.addColor(color);
	mesh.addVertex(position);

	/*
	 x = append(x, x[i] + ofRandom(3) - 1.5);
	 y = append(y, y[i] + ofRandom(3) - 1.5);
	 z = append(z, z[i] + ofRandom(3) - 1.5);
	 
	 i++;
	 if(x[i] < minX) minX = x[i];
	 if(x[i] > maxX) maxX = x[i];
	 if(y[i] < minY) minY = y[i];
	 if(y[i] > maxY) maxY = y[i];
	 if(z[i] < minZ) minZ = z[i];
	 if(z[i] > maxZ) maxZ = z[i];
	 if(i > maxSteps) {
	 x = subset(x, 1);
	 y = subset(y, 1);
	 z = subset(z, 1);
	 i--;
	 */
	



	if (i >= nParticles){
		mesh.getVertices().erase( mesh.getVertices().begin() );
		mesh.getColors().erase( mesh.getColors().begin() );
		
//		particles.erase(particles.begin() );
	}

//		i--;
            
}

void Walker::noiseStep(){
    
    j = j + 1;
    float t = (ofGetElapsedTimeMillis());
    float t0 = t/25;
    float t1 = t/100;
    float t2 = t/400;
    float t3 = t/900;
    float t4 = t*2;
    
    
	position.x += (ofNoise(t2 * .01, j * .01, seed) * 1 - .5);              //ofNoise(frameCount, i * 0.01, seed) * 3)
    position.y += (ofNoise(seed , t2 * .01, j * 0.01) * 1 - .5);             //(ofNoise(seed, frameCount, i * 0.01 ) * 3) - 1.5;
	position.z += (ofNoise( j * .01, seed, t2 *.01 ) * 1 - .5);    //(ofNoise(i * 0.01, frameCount, seed) * 3) - 1.5;
    
	mesh.addColor(color);
	mesh.addVertex(position);
    
	if (i >= nParticles){
		mesh.getVertices().erase( mesh.getVertices().begin() );
		mesh.getColors().erase( mesh.getColors().begin() );
		
        //		particles.erase(particles.begin() );
	}
    
    //		i--;
    
}
    
void Walker::draw(){
        

	mesh.drawVertices();
	
//      //  ofColor color;
//        ofVec3f point;
//        ofVboMesh mesh;
//        int idx = 0;
//        
//        glEnable(GL_DEPTH_TEST);
//        
//        for (int j = 0; j < i ; j ++){  //particles.size()
//            
//            ofPushStyle();
//            
//           // point.set(particles[j].position);
//            
//            ofSetColor(particles[j].color);
//            
//            mesh.addVertex(particles[j].position);
//            
//            ofPopStyle();
//            
//        }
//        
//    mesh.drawVertices();
    
};