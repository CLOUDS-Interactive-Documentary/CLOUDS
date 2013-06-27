//
//  cCell.h
//  VSColony
//
//  Created by Patricio Gonzalez Vivo on 6/26/13.
//
//

#pragma once

#include "ofMain.h"

class colonyCell : public ofPoint {
public:
    
    colonyCell();
    
    void    applyForce( ofPoint _force );
    void    applyFlock( vector<colonyCell*> &_cells );
    void    applyBorders();
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
    
    bool    dead;
    bool    hasReplicated;
    bool    shouldReplicate;
    
private:
    ofPoint separate ( vector<colonyCell*> &_cells );
    ofPoint align ( vector<colonyCell*> &_cells );
    ofPoint cohesion ( vector<colonyCell*> &_cells );
    
    ofPoint seek( ofPoint _target);
    
    
};
