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
    
    int ID; 
    float posX;
    float posY;
    int randomNumber;
    
    static int columns; //columns of numbers
    static int rows; //rows of numbers
    
    //vector <int> randomNumbers;
    //vector <int> noisyNumbers;
 
    Rand(float posX, float posY, float posZ);
    void setup(); 
    void noiseRotate(float _x, float _y);
    void generateNoisyNumber();
    void generateRandomNumber();
    void changeRandomNumber();
    void counter();
    void increaseRandomNumber();
    void drawNumbers();
    void update();

    int index =0;
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
    float speed = .02;
    float rotNoise; 
    
    float period = 120; //period of oscillation
    
    float previousMouseX = 0;
    float previousMouseY = 0;
    
    int tweenRandom;
    
    int i = 0;

    
};