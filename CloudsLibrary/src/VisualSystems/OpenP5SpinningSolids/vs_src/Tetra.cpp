//
//  Tetra.cpp
//  OpenP5SpinningSolids
//
//  Created by Jonathan Minard on 11/2/13.
//
//

#include "Tetra.h"

Tetra::Tetra(float _posX, float _posY, float _posZ, float _edgeLength){
    
    edgeLength = _edgeLength;
    posX = _posX;
    posY = _posY;
    posZ = _posZ;
	
	
	speed = .005;
    rotNoise = 0;
    
    period = 120; //period of oscillation
    
    previousMouseX = 0;
    previousMouseY = 0;
	
	mesh.clear();
	
	//mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    //mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	//mesh.setMode(OF_PRIMITIVE_LINES);
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);
	
	//0
	ofPoint a = ofPoint(edgeLength, edgeLength, edgeLength);
	//1
	ofPoint b = ofPoint(-edgeLength, -edgeLength, edgeLength);
	//2
	ofPoint c = ofPoint(-edgeLength, edgeLength, -edgeLength);
	//3
	ofPoint d = ofPoint(edgeLength, -edgeLength, -edgeLength);
	
	//0
	mesh.addColor(ofColor(0,0,20));
    mesh.addVertex(a);
	//1
	mesh.addColor(ofColor(230,20,80));
    mesh.addVertex(b);
	//2
	mesh.addColor(ofColor(100,20,160));
	mesh.addVertex(c);
	

	//0
    mesh.addColor(ofColor(200,20,20));
    mesh.addVertex(a);
	//1
	mesh.addColor(ofColor(230,20,80));
    mesh.addVertex(b);
	//3
    mesh.addColor(ofColor(100,20,220));
	mesh.addVertex(d);

	
	//1
	mesh.addColor(ofColor(230,20,80));
    mesh.addVertex(b);
	//2
	mesh.addColor(ofColor(100,20,160));
	mesh.addVertex(c);
	//3
    mesh.addColor(ofColor(100,20,220));
	mesh.addVertex(d);

	
	//0
    mesh.addColor(ofColor(200,20,20));
    mesh.addVertex(a);
	//2
	mesh.addColor(ofColor(100,20,160));
	mesh.addVertex(c);
	//3
    mesh.addColor(ofColor(100,20,220));
	mesh.addVertex(d);
	
	//redundant verts
//    mesh.addColor(ofColor(20,5,220));
//    mesh.addVertex(ofPoint(edgeLength, edgeLength, edgeLength));
//	
//    mesh.addColor(ofColor(5,5,230));
//    mesh.addVertex(ofPoint(-edgeLength, -edgeLength, edgeLength));
	
//    mesh.addIndex(0);
//    mesh.addIndex(1);
//    mesh.addIndex(2);
	
//    mesh.addIndex(1);
//    mesh.addIndex(2);
//    mesh.addIndex(3);
//	
//    mesh.addIndex(0);
//    mesh.addIndex(2);
//    mesh.addIndex(1);
//
//	mesh.addIndex(0);
//    mesh.addIndex(2);
//    mesh.addIndex(3);

}

void Tetra::drawShape(){
    int frameCount = ofGetFrameNum();
    
    ofTranslate(posX, posY, posZ);
    noiseRotate(posX, posY);
        
    float currentMouseX = lerp(ofGetMouseX(), previousMouseX, 0.8);
    float currentMouseY = lerp(ofGetMouseY(), previousMouseY, 0.8);
    
    float Xthrottle = ofMap(currentMouseX, 0, ofGetWidth(), .005, .7);
    float Ythrottle = ofMap(currentMouseY, 0, ofGetHeight(), .05, 6.0);
    
    previousMouseX = currentMouseX;
    previousMouseY = currentMouseY;
    
    //speed = ofMap(ofGetMouseY(), 0, ofGetHeight(), .01, .03);
    //cout << "speed = "<< cos(rotNoise) << endl;
    
    float rotNoise = ofNoise(posX + frameCount*speed, posY + frameCount*speed) * ofMap(ofGetMouseX(), 0, ofGetWidth(), .01, 2);
    
    float rotX = frameCount * cos(rotNoise) * Xthrottle;
   // cout << "noiseX = "<< cos(rotNoise) << endl;
    float rotY = frameCount * sin(rotNoise) * Xthrottle;
  //  cout << "noiseY = "<< sin(rotNoise) << endl;
    float rotZ =(frameCount * Ythrottle);
    
   // ofRotateX(ofRadToDeg(rotX+=rotX));
   // ofRotateY(ofRadToDeg(rotY+=rotY));
    // ofRotateZ(rotZ);
    
    ofRotateX(rotX);
    ofRotateY(rotY);
    ofRotateZ(rotZ);
    
    
  
/* //figure out the distance b/w mouse and object
    float dx = posX-ofGetMouseX();
 //   cout << "posX = "<< posX << endl;
  //  cout << "mouseX = "<< ofGetMouseX() << endl;
  //  cout << "posX - mouseX = "<< dx << endl;
    float dy = posY-ofGetMouseY();
 //   cout << "posY = "<< posY << endl;
 //   cout << "mouseY = "<< ofGetMouseY() << endl;
 //   cout << "posY - mouseY = "<< dy << endl;

    float d = (sqrt(dx*dx+dy*dy));
    
    if(d < edgeLength*2){
        float rxp = ((ofGetMouseX()-(posX)*0.3));
        float ryp = ((ofGetMouseY()-(posY)*0.3));
        rx = (rx*0.9)+(rxp*0.1);
        ry = (ry*0.9)+(ryp*0.1);
    }
*/

    
    mesh.draw();
    
    /*
     beginShape(TRIANGLE_STRIP);
     vertex(edgeLength, edgeLength, edgeLength);
     vertex(-edgeLength, -edgeLength, edgeLength);
     vertex(-edgeLength, edgeLength, -edgeLength);
     vertex(edgeLength, -edgeLength, -edgeLength);
     vertex(edgeLength, edgeLength, edgeLength);
     vertex(-edgeLength, -edgeLength, edgeLength);
     endShape(CLOSE);
     */
}

float Tetra::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void Tetra::noiseRotate(float posX, float posY){
        
        float rotNoise = ofNoise(posX + ofGetFrameNum()*speed, posY + ofGetFrameNum()*speed);  //y/chaossqr + ofGetFrameNum()*speed
        return rotNoise;
    
    }
