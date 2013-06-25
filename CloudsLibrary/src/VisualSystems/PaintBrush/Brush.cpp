//
//  Brush.cpp
//  week11_DrawingTool
//
//  Created by Patricio Gonzalez Vivo on 12/17/12.
//
//

#include "Brush.h"

Brush::Brush(){
    brushWidth = 50;
    lineWidth = 1;
    
    bDown = false;
    
    damp = 0.1;
    k = 0.1;
    repPct = 0.5;
    repRad = 5.0;
}

void Brush::setBrushWidth(float _width){
    brushWidth = _width;
    
    int n = springs.size();
    
    init(n);
}

void Brush::setLineWidth(float _width){
    lineWidth = _width;
}

void Brush::init(int _numSprings){
    
    float angle = getAngle() + HALF_PI;
    float step = brushWidth/(float)_numSprings;
    
    while ( _numSprings > colors.size() ){
        colors.push_back(ofFloatColor(1.0,1.0) );
    }
    
    ofPoint top;
    top.x = x + cos(angle) * brushWidth*0.5;
    top.y = y + sin(angle) * brushWidth*0.5;
    ofPoint buttom;
    buttom.x = x + cos(angle+PI) * brushWidth*0.5;
    buttom.y = y + sin(angle+PI) * brushWidth*0.5;
    
    ofPoint diff = buttom - top;
    diff.normalize();
    diff *= step;
    
    for(int i = 0; i < springs.size(); i++){
        delete As[i];
        delete Bs[i];
    }
    As.clear();
    Bs.clear();
    springs.clear();
    
    for(int i = 0; i < _numSprings; i++){
        Particle *a = new Particle();
        a->set( top + diff * i );
        a->size = step;
        a->bFixed = true;
        As.push_back(a);
        
        Particle *b = new Particle();
        b->set( ofPoint(x+ofRandom(-brushWidth*0.5,brushWidth*0.5), y+ofRandom(-brushWidth*0.5,brushWidth*0.5) ) );
        b->size = step;
        b->damping = damp;
        b->bFixed = false;
        b->color.set(colors[i]);
        Bs.push_back(b);
        
        Spring newSpring;
        newSpring.A = a;
        newSpring.B = b;
        newSpring.k = k;
        springs.push_back(newSpring);
        
    }
}

void Brush::pickColorFrom(ofTexture &_tex, float _lerpAmount, float _randAmount){
    
    ofRectangle palleteArea(0,0,_tex.getWidth(),_tex.getHeight());
    
    ofFloatPixels pixels;
    pixels.allocate(_tex.getWidth(), _tex.getHeight(), OF_IMAGE_COLOR_ALPHA);
    _tex.readToPixels(pixels);
    
    for(int i = 0; i < Bs.size(); i++){
        
        if ( palleteArea.inside( *Bs[i] ) ){
        
            ofFloatColor color = pixels.getColor(Bs[i]->x, Bs[i]->y);
            Bs[i]->color.lerp(color, _lerpAmount * color.a);
            Bs[i]->color.setHue( Bs[i]->color.getHue() + ofRandom(-_randAmount,_randAmount) );
            colors[i].set(Bs[i]->color);
            colors[i].a = 1.0;
        }
    }
}

void Brush::setColor( ofFloatColor &_color, float _lerpAmount, float _randAmount){
    for(int i = 0; i < Bs.size(); i++){
        
        Bs[i]->color.lerp( _color, _lerpAmount * _color.a);
        Bs[i]->color.setHue( Bs[i]->color.getHue() + ofRandom(-_randAmount,_randAmount) );
        colors[i].set(Bs[i]->color);
        colors[i].a = 1.0;
    }
}

void Brush::addParticles( vector<Particle> &particles, float _threshold, float _alpha ){
    
    for(int i = ofRandom(Bs.size()-1); i < Bs.size(); i++){
        if ( Bs[i]->getVel().length() > _threshold ){
            Particle newParticle;
            newParticle.init(*Bs[i], Bs[i]->getVel());
            newParticle.color = Bs[i]->color;
            newParticle.color.a = _alpha;
            newParticle.size = ofRandom(0.5, 1.0);
            newParticle.damping = ofRandom(0.1,0.7);
            newParticle.bSingle = true;
            
            particles.push_back( newParticle );
        }
    }
}

void Brush::begin(){
    
    if (!bDown){
        tail.clear();
    }
    
    bDown = true;
}

void Brush::end(){
    bDown = false;
}

void Brush::set(int _x, int _y){
    set(ofPoint(_x,_y));
}

void Brush::set(ofPoint _pos){
    
    if (bDown){
        prevPos.set(*this);
        
        ofPoint::set(_pos);
        tail.addVertex(_pos);
        
        if (tail.size() == 1){
            int n = springs.size();
            init(n);
        } else if ( tail.size() >= 2){
            
            //  FLAT BRUSH
            //
            float angle = getAngle() + HALF_PI;
            float step = brushWidth/(float)As.size();
            
            ofPoint top;
            top.x = x + cos(angle) * brushWidth*0.5;
            top.y = y + sin(angle) * brushWidth*0.5;
            ofPoint buttom;
            buttom.x = x + cos(angle+PI) * brushWidth*0.5;
            buttom.y = y + sin(angle+PI) * brushWidth*0.5;
            
            ofPoint diff = buttom - top;
            diff.normalize();
            diff *= step;
            
            for(int i = 0; i < As.size(); i++){
                As[i]->set( top + diff * i );
            }
        }
    }
}

ofPoint Brush::getVel(){
    if (tail.size() > 0)
        return  prevPos - *this;
    else
        return  ofPoint(0,0);
}

float Brush::getAngle(){
    if (tail.size() > 0){
        ofPoint vel = getVel();
        return  atan2(vel.y, vel.x);
    } else {
        return 0.0f;
    }
}

void Brush::clear(){
    for(int i = 0; i < springs.size(); i++){
        delete As[i];
        delete Bs[i];
    }
    As.clear();
    Bs.clear();
    springs.clear();
    tail.clear();
}

void Brush::update(){
    if(bDown){
        for(int i = 0; i < springs.size(); i++){
            As[i]->update();
            
            springs[i].update();
        
            for (int j = 0; j < i; j++){
                Bs[i]->addRepulsionForce( Bs[j], repRad, repPct);
            }
            
            Bs[i]->update();
        }
    }
}

void Brush::draw(){
    ofPushStyle();
    
    for(int i = 0; i < springs.size(); i++){
        if (tail.size() < 5){
            ofPushStyle();
            ofSetColor(Bs[i]->color);
            glLineWidth( lineWidth+5  -ofMap(Bs[i]->tail.size(),0,5,lineWidth,lineWidth+5) );
            Bs[i]->trail.draw();
            ofPopStyle();
        }
        ofPushStyle();
        glLineWidth( lineWidth );
        Bs[i]->trail.draw();
        ofPopStyle();
    }
    
    ofPopStyle();
}

void Brush::drawDebug(){
    ofPushStyle();
    ofSetColor(0);
    for(int i = 0; i < springs.size(); i++){
        ofSetColor(255);
        As[i]->draw();
        springs[i].draw();
        Bs[i]->draw();
    }
    
    float angle = getAngle() + HALF_PI;
    ofPoint a;
    a.x = x + cos(angle) * brushWidth*0.5;
    a.y = y + sin(angle) * brushWidth*0.5;
    ofPoint b;
    b.x = x + cos(angle+PI) * brushWidth*0.5;
    b.y = y + sin(angle+PI) * brushWidth*0.5;
    
    ofSetColor(255, 0, 0);
    ofLine(a, b);
    
//    ofSetColor(255, 0, 255);
//    tail.draw();
    
    ofPopStyle();
}