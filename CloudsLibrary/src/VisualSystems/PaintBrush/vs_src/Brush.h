//
//  Brush.h
//
//  Created by Patricio Gonzalez Vivo on 12/17/12.
//
//

#ifndef BRUSH
#define BRUSH

#include "ofMain.h"

#include "Particle.h"
#include "Spring.h"

class Brush : public ofPoint {
public:
    Brush();
    ~Brush();
    
    void    begin();
    void    end();
    
    void    init(int _numSprings);
    
    void    setBrushWidth(float _width);
    void    setLineWidth(float _width);
    void    pickColorFrom(ofTexture &Text, float _lerpAmount, float _randAmount);
    void    setColor( ofFloatColor &_color, float _lerpAmount, float _randAmount);
    
    void    addParticles( vector<Particle> &particles, float _threshold, float _alpha );
    
    void    set(int _x, int _y);
    void    set(ofPoint _pos);
    
    ofPoint getVel();
    float   getAngle();

    void    clear();
    
    void    update();
    
    void    draw();
    void    drawDebug();

    float   damp;
    float   k;
    float   repRad;
    float   repPct;
    
    bool    bDown;
    bool    bDebug;
    
private:
    vector<Particle*>   As;
    vector<Particle*>   Bs;
    vector<Spring>      springs;
    vector<ofFloatColor>     colors;
    
    ofPolyline          tail;
    
    ofPoint             prevPos;
    
    float               brushWidth;
    float               lineWidth;
};

#endif
