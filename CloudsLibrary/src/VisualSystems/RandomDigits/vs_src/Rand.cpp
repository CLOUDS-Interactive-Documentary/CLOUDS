//
//  Rand.cpp
//  OpenP5SpinningSolids
//
//  Created by Jonathan Minard on 11/2/13.
//
//

#include "Rand.h"

ofTrueTypeFont Rand::Font;

Rand:: Rand(float _posX, float _posY, float _posZ){
    
    posX = _posX;
    posY = _posY;
    posZ = _posZ;
    ID;
    
   
}

void Rand::setup(){
    
    ofBackground(54, 54, 54, 255);
	
	//old OF default is 96 - but this results in fonts looking larger than in other programs.
	ofTrueTypeFont::setGlobalDpi(72);
    
   	Font.setLineHeight(14.0f);
	Font.setLetterSpacing(1.037);

}


void Rand::drawNumbers(){
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
    
   // ofRotateX(rotX);
   // ofRotateY(rotY);
   // ofRotateZ(rotZ);
    
    ofSetColor(225);
    
    std::ostringstream ostr; //output string stream
   // ostr << rotX*.10; //use the string stream just like cout
    
    ostr << int(ofRandom(10000,99999)+.5);
    std::string number = ostr.str();
    
	Font.drawString(number, 14.0f, 1.037);
    
    /*
    text.size = 10;
    text = &rotX;
    font.drawString(ofToUpper(text), (text.size), 0);
    */
}

float Rand::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void Rand::noiseRotate(float posX, float posY){
        
        float rotNoise = ofNoise(posX + ofGetFrameNum()*speed, posY + ofGetFrameNum()*speed);  //y/chaossqr + ofGetFrameNum()*speed
        return rotNoise;
    
    }
