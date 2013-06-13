//
//  ColorPalette.cpp
//
//  Created by Patricio Gonzalez Vivo on 1/21/13.
//
//

#include "ColorPalette.h"

void ColorPalette::setup(){
    bSetup = false;
    bVisible = false;
    
    inc = 0.06145;
    shp = 0.3;
    height = 200;
    
    background.loadImage( path+"wood.jpg");
    
}

bool ColorPalette::loadPalette(string _sFile){
    
    bool success = false;
    
    ofxXmlSettings XML;
    
    if ( XML.loadFile(_sFile)){
        if ( XML.pushTag("palette")) {
            
            height = XML.getValue("height", height);
            
            for (int i = 0; i < XML.getNumTags("img"); i++){
                ofImage image;
                if ( image.loadImage( path+XML.getValue("img", "01.png", i ) ) ){
                    images.push_back(image);
                }
            }
            
            for (int i = 0; i < XML.getNumTags("color"); i++){
                string hexString = XML.getValue("color", "ee1b12", i );
                int hexInt = ofHexToInt( hexString );
                
                ofColor color;
                color.setHex( hexInt );
                colors.push_back(color);
            }
        }
    }
}

void ColorPalette::setVisible(bool _bVisible){
    bVisible = _bVisible;
}

void ColorPalette::toggleVisible(){
    bVisible = !bVisible;
}

bool ColorPalette::getVisible(){
    if (pct >= 0.9){
        return true;
    } else {
        return false;
    }
}

float ColorPalette::getY(){
    float yPct = powf(pct, shp);
    return yPct * height;
}

void ColorPalette::update(){
    if ( width != ofGetWindowWidth()){
        bSetup = false;
    }
    
    if (!bSetup){
        width = ofGetWindowWidth();
        allocate(width, height);
        pct = 0.0f;
        
        clear();
        
        bSetup = true;
    }
    
    if ( bVisible ){
        if ( pct < 1.0 ){
            pct += inc;
        }
    } else {
        if ( pct > 0.0 ){
            pct -= inc;
        }
    }
}

void ColorPalette::clear(){
    //  Construct FBO with colors;
    //
    ofPushStyle();
    begin();
    ofClear(0,0);
    
    float iconWidth = 30;
    float totalWidth = width - iconWidth*2;
    float elementWidth = totalWidth/colors.size();
    float x = iconWidth + elementWidth*0.5;
    
    for (int i = 0; i < colors.size(); i++){
        ofSetColor( colors[i] );
        int imageIndex = i%images.size();
        images[imageIndex].setAnchorPercent(0.5, 0.5);
        images[imageIndex].draw(x,height*0.5);
        x += elementWidth;
    }
    
    end();
    ofPopStyle();
}

void ColorPalette::draw(){
    if ( pct > 0.0 ){
        background.draw(0, -background.getHeight() + getY());
        ofFbo::draw(0, -height + getY());
    }
}