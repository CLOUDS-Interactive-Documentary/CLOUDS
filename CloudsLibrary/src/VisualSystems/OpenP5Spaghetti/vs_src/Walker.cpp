//
//  Walker.cpp
//  OpenP5Spaghetti
//
//  Created by Jonathan Minard on 10/25/13.
//
//

#include "Walker.h"

float Walker::stepSizex = 1.0f;
float Walker::stepSizey = 1.0f;
float Walker::stepSizez = 1.0f;

float Walker::noiseSpeedx = .01f;
float Walker::noiseSpeedy = .01f;
float Walker::noiseSpeedz = .01f;

bool Walker:: drawPoints = true;
bool Walker:: drawLines = false;

float Walker::saturation = .3;
float Walker::brightness = .9;

int Walker:: i;
float Walker:: j;
float Walker:: nParticles = 40;

Walker::Walker(){
    
    drawPoints = true;
    drawLines = false;
	
}

void Walker::init(int _nParticles, ofColor _color1){
	position = ofVec3f(0,0,0);
	nParticles = _nParticles;
        
	seed = ofRandom(50);
	//c = new color[1];
//        particles.reserve(500);
//        particles[0].position.x = 0;
//        particles[0].position.y = 0;
//        particles[0].position.z = 0;
	maxX = minX = maxY = minY = 0;
	minZ = maxZ = 0;
	i = 0;
    j = 0;
    
    
    mesh.addColor(color);
	mesh.addVertex(position);
	
    
}


void Walker::setColor(ofColor _newColor){
    
    color = _newColor;

}
	
void Walker::step(){

	position.x += (ofRandom(3) - 1.5);
	position.y += (ofRandom(3) - 1.5);
	position.z += (ofRandom(3) - 1.5);

    ofSetColor(color);
	mesh.addColor(color);
	mesh.addVertex(position);

    numVertices = mesh.getNumVertices();
    
	if (numVertices >= nParticles){
		mesh.getVertices().erase( mesh.getVertices().begin() );
		mesh.getColors().erase( mesh.getColors().begin() );
		
//		particles.erase(particles.begin() );
	}

//		i--;
            
}

void Walker::smoothTrails(){
    
    j = j + 1;
    float t = (ofGetElapsedTimeMillis());
    float t2 = t/900;
   
    previousPosition.x = position.x;
    previousPosition.y = position.y;
    previousPosition.z = position.z; 
	position.x += (ofNoise(t2 * (noiseSpeedx/20.0), j * (noiseSpeedx/20.0), seed) * stepSizex*2.0 - stepSizex); //noiseSpeed .01 stepSize 3
    position.y += (ofNoise(seed , t2 * (noiseSpeedy/20.0), j * (noiseSpeedy/20.0)) * stepSizey*2.0 - stepSizey);
	position.z += (ofNoise( j * (noiseSpeedz/20.0), seed, t2 * (noiseSpeedz/20.0)) * stepSizez*2.0 - stepSizez);
    currentPosition.x = (previousPosition.x + position.x)/2.0;
    currentPosition.y = (previousPosition.y + position.y)/2.0;
    currentPosition.z = (previousPosition.z + position.z)/2.0;
    currentPosition.x = (currentPosition.x + previousPosition.x)/2.0;
    currentPosition.y = (currentPosition.y + previousPosition.y)/2.0;
    currentPosition.z = (currentPosition.z + previousPosition.z)/2.0;
    
	mesh.addColor(color);
	mesh.addVertex(currentPosition);
    
    numVertices = mesh.getNumVertices();
	if (numVertices >= nParticles){
		mesh.getVertices().erase( mesh.getVertices().begin() );
		mesh.getColors().erase( mesh.getColors().begin() );
		
	}
    position = currentPosition;
  
    
}

void Walker::gnarlyTrails(){
    
    j = j + 1;
    float t = (ofGetElapsedTimeMillis());
    float t0 = t/20;
    
	position.x += (ofNoise(t0 * noiseSpeedx, j * noiseSpeedx, seed) * stepSizex*2.0 - stepSizex);
    position.y += (ofNoise(seed , t0 * noiseSpeedy, j * noiseSpeedy) * stepSizey*2.0 - stepSizey);
	position.z += (ofNoise( j * noiseSpeedz, seed, t0 * noiseSpeedz  ) * stepSizez*2.0 - stepSizez);

    
    /* float t0 = t/25;
    float t1 = t/100;
    float t2 = t/400;
    float t3 = t/900;
    float t4 = t*2;
    
    
	position.x += (ofNoise(t0 * .2, j * .4, seed) * .5 - .25);
    position.y += (ofNoise(seed , t0 * .2, j * 0.5) * .5 - .25);
	position.z += (ofNoise( j * .5, seed, t0 * .2  ) * .5 - .25);
    
    */
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
    
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glPointSize(1);
  
    if(drawPoints == true){
      
        mesh.setMode(OF_PRIMITIVE_POINTS);
       
        mesh.drawVertices();
    }
    else if(drawLines == true){
        mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
       
        mesh.drawVertices();
    }

	

    
};