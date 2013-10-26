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

        nParticles = _nParticles;
        
        seed = ofRandom(100);
        //c = new color[1];
        particles.reserve(500);
        particles[0].position.x = 0;
        particles[0].position.y = 0;
        particles[0].position.z = 0;
        maxX = minX = maxY = minY = 0;
        minZ = maxZ = 0;
        i = 0;
    
        ofVec3f position;
        ofColor color;
    
        position.x = (ofRandom(3) - 1.5);
        position.y = (ofRandom(3) - 1.5);
        position.z = (ofRandom(3) - 1.5);
    
    
}
	
void Walker::step(){
        
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
        
    
        struct particle newParticle;
        newParticle.position = particles[i].position;
        newParticle.color = particle.color;
        particles.push_back(newParticle);
    
        i++;
        
        if( position.x < minX){
            minX = particles[i].position.x;
        }
        if( position.x > maxX){
            maxX = particles[i].position.x;
        }
        if( position.y < minY){
            minY = particles[i].position.y;
        }
        if( position.y > maxY){
            maxY =  particles[i].position.y;
        }
        if( position.z < minZ){
            minZ =  particles[i].position.z;
        }
        if( position.z > maxZ){
            maxZ =  particles[i].position.z;
        }
        
        if (i >= nParticles){
            particles.erase(particles.begin() );
        }
    
            i--;
            
        }
    
void Walker::draw(){
        
        
        ofColor color;
        ofVec3f point;
        ofVboMesh mesh;
        int idx = 0;
        
        glEnable(GL_DEPTH_TEST);
        
        for (int j = 0; j < i ; j ++){  //particles.size()
            
            ofPushStyle();
            
           // point.set(particles[j].position);
            
            ofSetColor(particles[j].color);
            
            mesh.addVertex(particles[j].position);
            
            ofPopStyle();
            
        }
        
        mesh.drawVertices()
        
};