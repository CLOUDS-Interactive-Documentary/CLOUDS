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
}

void Tetra::drawShape(){
    
    ofTranslate(posX, posY, posZ);
    
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

    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);

    mesh.addVertex(ofPoint(edgeLength, edgeLength, edgeLength));
    mesh.addColor(ofFloatColor(255,0,50));
    mesh.addVertex(ofPoint(-edgeLength, -edgeLength, edgeLength));
    mesh.addColor(ofFloatColor(200,0,200));
    mesh.addVertex(ofPoint(-edgeLength, edgeLength, -edgeLength));
    mesh.addColor(ofFloatColor(0,255,50));
    mesh.addVertex(ofPoint(edgeLength, -edgeLength, -edgeLength));
    mesh.addColor(ofFloatColor(0,200,200));
    mesh.addVertex(ofPoint(edgeLength, edgeLength, edgeLength));
    mesh.addColor(ofFloatColor(0,50,230));
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