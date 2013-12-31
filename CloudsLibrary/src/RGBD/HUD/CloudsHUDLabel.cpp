//
//  CloudsHUDLabel.cpp
//  CloudsHUDDesigner
//
//  Created by Charlie Whitney on 12/30/13.
//
//

#include "CloudsHUDLabel.h"

CloudsHUDLabel::CloudsHUDLabel(){
    text = "";
}

void CloudsHUDLabel::setup( ofxFTGLSimpleLayout *textLayout, ofRectangle textBounds ){
    layout = textLayout;
    bounds = textBounds;
}

void CloudsHUDLabel::draw(){
    if( layout ){
        layout->drawString( text , bounds.x, bounds.y + layout->getStringBoundingBox("W", 0, 0).height );
    }
}

void CloudsHUDLabel::setText( string newText ){
    text = newText;
}
