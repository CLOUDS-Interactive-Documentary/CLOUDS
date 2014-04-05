//
//  Rand.h
//  OpenP5SpinningSolids
//
//  Created by Jonathan Minard on 11/2/13.
//
//

#pragma once 
#include "ofMain.h"
#include "ofxTween.h"

class Rand {
    
public:
    
	Rand(){
		index = 0;
		speed = .02;
		rotNoise = 0; 
		period = 120; //period of oscillation
		previousMouseX = 0;
		previousMouseY = 0;
		tweenRandom = 0;
    
	    i = 0;
	};

    int ID; 
    float posX;
    float posY;
    int randomNumber;
    
 
    int* minBri;
    int* maxBri;
    
    Rand(float posX, float posY, float posZ,
         int& minBri, int& maxBri);
    void setup(); 
    void noiseRotate(float _x, float _y);
    //void generateNoisyNumber();
    void generateRandomNumber();
    void changeRandomNumber();
    //void counter();
    void increaseRandomNumber();
    void drawNumbers();
    void update();

    int index;
    float previousRandomNumber;
    float nextRandomNumber;
    float currentTime;
    float previousTime;
    // text
    //FTGL
    //string text;
    //float fontSize;
    //static ofxFTGLSimpleLayout font;
    
    string typeStr;
    
    ofTrueTypeFont  franklinBook14;
    ofTrueTypeFont	verdana14;
    static ofTrueTypeFont  Font;
    
protected:
    
    float rx;
    float ry;
    float speed;
    float rotNoise; 
    
    float period; //period of oscillation
    
    float previousMouseX;
    float previousMouseY;
    
    int tweenRandom;
    
    int i;

    
};