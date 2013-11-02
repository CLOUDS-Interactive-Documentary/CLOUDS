//
//  Tetra.cpp
//  OpenP5SpinningSolids
//
//  Created by Jonathan Minard on 11/2/13.
//
//

#include "Tetra.h"

Tetra:: Tetra(float _posX, float _posY, float _posZ, float _edgeLength){
    
    edgeLength = _edgeLength;
    posX = _posX;
    posY = _posY;
    posZ = _posZ;
    ID; 
}

void Tetra::drawShape(){
    int frameCount = ofGetFrameNum();
    
    ofTranslate(posX, posY, posZ);
   
    noiseRotate(posX, posY);
    
    float Xthrottle = ofMap(ofGetMouseX(), 0, ofGetWidth(), .005, .7);
    float Ythrottle = ofMap(ofGetMouseY(), 0, ofGetHeight(), .05, 6.0);
    
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
     mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    //mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
   // mesh.setMode(OF_PRIMITIVE_LINES);

    mesh.addColor(ofColor(200,20,20));
    mesh.addVertex(ofPoint(edgeLength, edgeLength, edgeLength));
    mesh.addColor(ofColor(230,20,80));
    mesh.addVertex(ofPoint(-edgeLength, -edgeLength, edgeLength));
    mesh.addColor(ofColor(100,20,160));
    mesh.addVertex(ofPoint(-edgeLength, edgeLength, -edgeLength));
    mesh.addColor(ofColor(100,20,220));
    mesh.addVertex(ofPoint(edgeLength, -edgeLength, -edgeLength));
    mesh.addColor(ofColor(20,5,220));
    mesh.addVertex(ofPoint(edgeLength, edgeLength, edgeLength));
    mesh.addColor(ofColor(5,5,230));
    mesh.addVertex(ofPoint(-edgeLength, -edgeLength, edgeLength));
    mesh.addIndex(0);
    mesh.addIndex(1);
    mesh.addIndex(2);
    mesh.addIndex(3);
    mesh.addIndex(4);
    mesh.addIndex(5);
    mesh.addIndex(6);
    
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

void Tetra::noiseRotate(float posX, float posY){
        
        float rotNoise = ofNoise(posX + ofGetFrameNum()*speed, posY + ofGetFrameNum()*speed);  //y/chaossqr + ofGetFrameNum()*speed
        return rotNoise;
    
    }
