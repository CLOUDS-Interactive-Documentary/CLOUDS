//
//  Rand.cpp
//  OpenP5SpinningSolids
//
//  Created by Jonathan Minard on 11/2/13.
//
//

#include "Rand.h"

ofTrueTypeFont Rand::Font;
int Rand:: columns = 20;
int Rand:: rows = 20;

Rand:: Rand(float _posX, float _posY, float _randomNumber){
    
    posX = _posX;
    posY = _posY;
    randomNumber = _randomNumber;
    
   
}

void Rand::setup(){
    
    ofBackground(54, 54, 54, 255);
	
	//old OF default is 96 - but this results in fonts looking larger than in other programs.
	ofTrueTypeFont::setGlobalDpi(72);
    
   	Font.setLineHeight(14.0f);
	Font.setLetterSpacing(1.037);

}


/*
void Rand::generateNoisyNumber(){
    int frameCount = ofGetFrameNum();
    

    float noise = ofNoise(posX + frameCount*.01, posY + frameCount*speed) * ofMap(ofGetMouseX(), 0, ofGetWidth(), .01, 2);
    
    float noiseX = frameCount * cos(rotNoise) * Xthrottle;
    float noiseY = frameCount * sin(rotNoise) * Xthrottle;
    float noiseZ =(frameCount * Ythrottle);
    
    ofSetColor(225);
    
}
 */



void Rand::drawNumbers(){
    
    //draws an array of random numbers
   // ofPushMatrix();{
        
    ofTranslate(posX, posY);
    std::ostringstream ostr; //output string stream
    ostr << this->randomNumber;
    std::string number = ostr.str();
    Font.drawString(number, 14.0f, 1.037);
  //  }
 //   ofPopMatrix();
}

void Rand::generateRandomNumber(){
 
    this->randomNumber = int(ofRandom(10000, 99999)+.5);
}


void Rand::noiseRotate(float posX, float posY){
        
        float rotNoise = ofNoise(posX + ofGetFrameNum()*speed, posY + ofGetFrameNum()*speed);  //y/chaossqr + ofGetFrameNum()*speed
        return rotNoise;
    
    }
