//
//  Walker.cpp
//  OpenP5Spaghetti
//
//  Created by Jonathan Minard on 10/25/13.
//
//

#include "Walker.h"

Walker::Walker(){
    
    drawPoints = true;
    drawLines = false;
	
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
    

    
}
	
void Walker::step(){

	position.x += (ofRandom(3) - 1.5);
	position.y += (ofRandom(3) - 1.5);
	position.z += (ofRandom(3) - 1.5);

	mesh.addColor(color);
	mesh.addVertex(position);

    int numVertices = mesh.getNumVertices();
    
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
    float t0 = t/25;
    float t1 = t/100;
    float t2 = t/400;
    float t3 = t/900;
    float t4 = t*2;
    
	position.x += (ofNoise(t2 * .01, j * .01, seed) * 1 - .5);              
    position.y += (ofNoise(seed , t2 * .01, j * 0.01) * 1 - .5);           
	position.z += (ofNoise( j * .01, seed, t2 *.01 ) * 1 - .5);   
    
	mesh.addColor(color);
	mesh.addVertex(position);
    
	if (i >= nParticles){
		mesh.getVertices().erase( mesh.getVertices().begin() );
		mesh.getColors().erase( mesh.getColors().begin() );
		
	}
  
    
}

void Walker::gnarlyTrails(){
    
    j = j + 1;
    float t = (ofGetElapsedTimeMillis());
    
    float t0 = t * noiseSpeed1;
    
	position.x += (ofNoise(t0 * .2, j * .4, seed) * stepSize*2.0 - stepSize);
    position.y += (ofNoise(seed , t0 * .2, j * 0.5) * stepSize*2.0 - stepSize);
	position.z += (ofNoise( j * .5, seed, t0 * .2  ) * stepSize*2.0 - stepSize);

    
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
        
  
    if(drawPoints == true){
        mesh.setMode(OF_PRIMITIVE_POINTS);
       
        mesh.drawVertices();
    }
    else if(drawLines == true){
        mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
       
        mesh.drawVertices();
    }

	

    
};