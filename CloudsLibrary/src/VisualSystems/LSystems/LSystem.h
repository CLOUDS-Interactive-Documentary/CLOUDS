//
//  LSystem.h
//  lSystem
//
//  Created by Patricio Gonzalez Vivo on 5/2/13.
//
//  Based on Gary William Flake implementation for his book: The Computational Beauty of Nature
//  https://github.com/gwf/CBofN/blob/master/src/lsys.c
//  Copyright (c) 1997, Gary William Flake.
//

#ifndef L_SYSTEM
#define L_SYSTEM

#include "ofMain.h"

class LSystem {
public:
    
    LSystem();
    ~LSystem();
    
    void    setAngle(float _deg){angle = ofDegToRad(_deg);};
    void    addAxion(string _axiom);
    void    addRule(string _rule);
    
    ofRectangle getBoundingBox();
    void    make(int _depth = -1.0);
    
    void    clear();
    
    ofMesh  mesh;
    
    ofPoint initialPos;
    float   initialAngle;   // radials
    float   angle;          // radials
    float   ds;
    
    float   unoise;
    float   utime;
    
private:
    void    compute_figure(char *rule, int d);

    char    *rules['z' - 'a' + 1];
    char    stubs[('z' - 'a' + 1) * 2];
    char    *axiom;
 
    ofPoint pos;    //  computed positions
    float   a;      //  computed angle
    float   s;      //  computed scale
    
    int     maxx, maxy, minx, miny;
};

#endif
