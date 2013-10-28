//
//  FlowField.cpp
//  yearLong
//
//  Created by Surya on 01/04/13.
//
//

#include "FlowField.h"

FlowField::FlowField(int _resolution, int _w, int _h){
    
    resolution = _resolution;
    width = _w;
    height = _h;
    cols = width/resolution;
    rows = height/resolution;
    
    field.resize(height);
    
    for(int i=0; i<cols;i++){
        field[i].resize(rows);
    }
}

void FlowField::init(){
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            field[i][j] = ofVec2f(0, 1);
        
        }
    }
}

ofVec2f FlowField::lookUp(ofVec2f lookup){
    int column = int (ofClamp(lookup.x/resolution, 0, cols-1));
    int row = int (ofClamp(lookup.y/resolution, 0, rows-1));
    
    return field[column][row];
}