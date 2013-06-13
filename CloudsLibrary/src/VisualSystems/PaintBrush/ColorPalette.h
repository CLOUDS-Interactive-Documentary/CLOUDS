//
//  Palette.h
//
//  Created by Patricio Gonzalez Vivo on 1/21/13.
//
//

#ifndef PALETTE
#define PALETTE

#include "ofMain.h"

#include "ofxXmlSettings.h"

class ColorPalette : public ofFbo {
public:
    
    void        setup();
    
    bool        loadPalette(string _sFile);
    void        setVisible(bool _bvisible);
    void        toggleVisible();
    
    void        clear();
    
    bool        getVisible();
    float       getY();
    
    void        update();
    void        draw();
    
    string      path;
    
private:
    
    ofImage         background;
    
    vector<ofImage> images;
    vector<ofColor> colors;
    float           pct, inc, shp;
    
    bool            bSetup;
    bool            bVisible;
    
    int             width,height,element_offset;
    bool            bChange;
};

#endif
