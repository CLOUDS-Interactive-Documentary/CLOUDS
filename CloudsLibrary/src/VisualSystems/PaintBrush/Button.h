//
//  Button.h
//
//  Created by Patricio Gonzalez Vivo on 1/22/13.
//
//

#ifndef SIMPLEBUTTON
#define SIMPLEBUTTON

#include "ofMain.h"

class SimpleBtn : public ofRectangle {
public:
    
    void    setImage(string _sFile);
    bool    checkOver(ofPoint _pos);
    void    draw();
    
    ofEvent<int>   clickPressed;
    
private:
    ofImage image;
    
};

#endif
