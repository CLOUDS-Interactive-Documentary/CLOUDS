//
//  Rand.cpp
//  OpenP5SpinningSolids
//
//  Created by Jonathan Minard on 11/2/13.
//
//

#include "Rand.h"

ofTrueTypeFont Rand::Font;
//int Rand:: columns = 15;
//int Rand:: rows = 25;

Rand::Rand(float _posX, float _posY, float _randomNumber,
            int& minBri, int& maxBri)
{
    
    posX = _posX;
    posY = _posY;
    randomNumber = _randomNumber;
    currentTime =0;
    previousRandomNumber =ofRandom(1000);
    nextRandomNumber =0;
    previousTime = 0;
    this->minBri = &minBri;
    this->maxBri = &maxBri;
}

void Rand::setup(){
  
	//old OF default is 96 - but this results in fonts looking larger than in other programs.
	ofTrueTypeFont::setGlobalDpi(72);
    
   	Font.setLineHeight(14.0f);
	Font.setLetterSpacing(1.037);
    
    //nextRandomNumber = ofRandom(10000, 99999);

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
    ofxEasingQuad eq;
 
    currentTime = ofGetElapsedTimeMillis() - previousTime;
//    cout<<currentTime<<","<<previousTime<<endl;
    float newRandomNumber = ofxTween::map(currentTime, 0, 1000.0, previousRandomNumber, nextRandomNumber, true, eq, ofxTween::easeInOut);
    randomNumber = int(newRandomNumber);
    ofTranslate(posX, posY);
    std::ostringstream ostr; //output string stream
    ostr << randomNumber;
    std::string number = ostr.str();

    if (abs(newRandomNumber - nextRandomNumber) >= 2){
        float brightness = ofxTween::map(currentTime, 0, 1000.0, *minBri, *maxBri, true, eq, ofxTween::easeInOut);
    ofSetColor(brightness);
    }
    if (abs(newRandomNumber - nextRandomNumber) <= 2){
        float brightness = ofxTween::map(currentTime, 0, 1000.0, *maxBri, *minBri, true, eq, ofxTween::easeInOut);
        ofSetColor(brightness);
    }
    
    ofFill();
    Font.drawString(number, 14.0f, 1.037);
    
}

void Rand::generateRandomNumber(){
 
    this->randomNumber = int(ofRandom(10000, 99999)+.5);
}


void Rand::update(){

}

void Rand::changeRandomNumber(){
    

    previousTime  = ofGetElapsedTimeMillis();
    previousRandomNumber = randomNumber;
    nextRandomNumber = ofRandom(10000, 99999);    

//    randomNumber = int(newRandomNumber);
   // cout << "Button pressed!" << endl;
   
}

//void Rand::counter(){
//    for (float i = 0; i<1000.0; i+=1.0){
//        return i; 
//    }
//}

void Rand::increaseRandomNumber(){
    
    this->randomNumber =  this->randomNumber + int(ofRandom(0, 200))-100;
}

//float Rand::noiseRotate(float posX, float posY){
//        
//	float rotNoise = ofNoise(posX + ofGetFrameNum()*speed, posY + ofGetFrameNum()*speed);  //y/chaossqr + ofGetFrameNum()*speed
//    return rotNoise;
//}
