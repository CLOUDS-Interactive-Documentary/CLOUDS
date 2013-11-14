//
//  cCell.h
//  VSColony
//
//  Created by Patricio Gonzalez Vivo on 6/26/13.
//
//


#pragma once

#include "ofMain.h"

class oldColonyCell : public ofPoint {
public:
    
    oldColonyCell();
    
    void    applyForce( ofPoint _force );
    void    applyFlock( vector<oldColonyCell*> &_cells );
    void    applyBorders();
    void    setup();
    void    update();
    
    void    feedCellWidth( ofPixels &_pixels );
    
    void    draw();
    
    ofPoint vel;
    ofPoint acc;
    
    float   cellSize;
    float   r;
    float   maxforce;    // Maximum steering force
    float   maxspeed;    // Maximum speed
    float   maxSize;
    
    float   fertilityAge;
    float   lifeSpan;
    
    int     nutrientLevel;
    int     age;
    bool     isFertile;
    
    bool    dead;
    bool    hasReplicated;
    bool    shouldReplicate;
    int		spriteIndex;
//    ofImage transparency;
    
private:
    ofPoint separate ( vector<oldColonyCell*> &_cells );
    ofPoint align ( vector<oldColonyCell*> &_cells );
    ofPoint cohesion ( vector<oldColonyCell*> &_cells );
    
    ofPoint seek( ofPoint _target);
   
    ofMesh spriteMesh;
    
};
