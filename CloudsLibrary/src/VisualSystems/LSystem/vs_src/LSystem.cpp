//
//  LSystem.cpp
//  lSystem
//
//  Created by Patricio Gonzalez Vivo on 5/2/13.
//
//

#include "LSystem.h"

LSystem::LSystem(){
    mesh.setMode(OF_PRIMITIVE_LINES);
    
    axiom = NULL;
    
    //  Set each rule to initially be defined as itself.
    //
    for(int i = 0; i < ('z' - 'a' + 1); i++) {
        stubs[i * 2] = i + 'a';
        stubs[i * 2 + 1] = 0;
        rules[i] = &stubs[i * 2];
    }
    
    initialPos      = ofPoint(0.0f,0.0f);
    initialAngle    = ofDegToRad(0);
    angle           = ofDegToRad(20);
    
    ds              = 1.0f;
    unoise          = 0.0f;
    utime           = 0.0f;
}

LSystem::~LSystem(){
    delete []axiom;
}

void LSystem::addAxion(string _axiom){
    //  Delete previus axioms
    //
    if (axiom == NULL) delete axiom;
    
    //  Copy the string to it
    //
    axiom = new char [_axiom.length()+1];
    strcpy(axiom, _axiom.c_str());
    
    //  Convert the axiom to lower-case, just to be consistent.
    //
    int len = strlen(axiom);
    for(int i = 0; i < len; i++)
        if(axiom[i] >= 'A' && axiom[i] <= 'Z')
            axiom[i] = axiom[i] - 'A' + 'a';
}

void LSystem::addRule(string _rule){
    int len = _rule.length();
    if (len > 2){
        
        /* Step through the rule string and convert upper-case letters to
         * lower-case letters. (extra: copy to a tmp array)
         */
        for(int i = 0; i < len; i++){
            if(_rule[i] >= 'A' && _rule[i] <= 'Z')
                _rule[i] = _rule[i] - 'A' + 'a';
        }
        
        /* Check that the rule looks like "x=..." where "x" is any letter.
         * If it is not, return an error.
         */
        char letter = _rule[0];
        if(letter < 'a' || letter > 'z' || _rule[1] != '='){
            ofLog(OF_LOG_ERROR, "L-System Rule: [" + _rule + "] Doesn't seams like a rule");
            return 0;
        }
        
        //  Set the rule table entry to everything after the "=" character.
        //
//        cout << "Loading rule: " << letter << "=";
        rules[letter - 'a'] = new char[len-2];
        for(int i = 2; i < len; i++){
//            cout << _rule[i];
            rules[letter - 'a'][i-2] = _rule[i];
        }
//        cout << endl;
        
    } 
}

void LSystem::make(int _depth){
    
    //  Calculate bounds
    //
    s = 1.0;
    a = initialAngle;
    pos = initialPos;
    
    maxx = -10000;
    maxy = -10000;
    minx = 10000;
    miny = 10000;
    
    // Finally, plot the fractal.
    //
    mesh.clear();
    compute_figure(axiom, _depth);
}

ofRectangle LSystem::getBoundingBox(){
    ofRectangle rta;
    
    rta.x = minx;
    rta.y = miny;
    rta.width = maxx - minx;
    rta.height = maxy - miny;
    
    return rta;
}

void LSystem::clear(){
    mesh.clear();
    
    if (axiom != NULL)
        delete []axiom;
    
    for(int i = 0; i < ('z' - 'a' + 1); i++) {
        stubs[i * 2] = i + 'a';
        stubs[i * 2 + 1] = 0;
        rules[i] = &stubs[i * 2];
    }
};

/* This function is essentially a turtle graphics interpreter.  If
 calcbounds is set, then the plotting bounds are computed.  Otherwise,
 the figure is plotted. */

void LSystem::compute_figure(char *rule, int d){
    
    //  Temporal values
    //
    ofVec4f sPos;
    float   sA, sS;
    
    int num = 0;
    int len = (rule != NULL) ? strlen(rule) : 0;
    
    /* For each character in the rule ... */
    for(int i = 0; i < len; i++) {
        /* If it is a letter or a '|' ... */
        if((rule[i] >= 'a' && rule[i] <= 'z') || rule[i] == '|') {
            
            /* For any letter, reduce the scale and recursively expand things
             * by expanding the letter's rule.  Restore the scale afterwards.
             */
            if(d > 0 && rule[i] != '|') {
                sS = s; s *= ds;
                compute_figure(rules[rule[i] - 'a'], d - 1);
                s = sS;
            }
            
            /* It is a command that requires movement of some form. */
            else if(rule[i] == 'f' || rule[i] == 'g' || rule[i] == '|') {
                
                /* Calculate where we should step. */
                if ( utime == 0.0){
                    sPos.x = pos.x + sin( a + unoise * (ofNoise(pos.x*0.01)-0.5)*2.0) * s;
                    sPos.y = pos.y + -cos( a + unoise * (ofNoise(pos.y*0.01)-0.5)*2.0) * s;
                } else {
                    float time = ofGetElapsedTimef() * utime;
                    sPos.x = pos.x + sin( a + unoise * (ofNoise(time,pos.x*0.01)-0.5)*2.0) * s;
                    sPos.y = pos.y + -cos( a + unoise * (ofNoise(time,pos.y*0.01)-0.5)*2.0) * s;
                }
            
                /* We need to plot any 'f' or '|' commands ... */
                if(rule[i] == 'f' || rule[i] == '|') {
                
                    /* Save the boundaries if appropriate ... */
                    maxx = MAX(maxx, MAX(pos.x, sPos.x));
                    maxy = MAX(maxy, MAX(pos.y, sPos.y));
                    minx = MIN(minx, MIN(pos.x, sPos.x));
                    miny = MIN(miny, MIN(pos.y, sPos.y));

                    /* ... or plot the line. */
                    mesh.addVertex(pos);
                    mesh.addVertex(sPos);
                }
                /* Set the current position to the new position calculated. */
                pos = sPos;
            }
        }
        /* If it is a number digit, then calculate the num for future use. */
        else if(rule[i] >= '0' && rule[i] <= '9')
            num = num * 10 + (rule[i] - '0');
        
        /* If it is a turn request ... */
        else if(rule[i] == '+' || rule[i] == '-') {
            /* ... then use any saved num and modify the angle. */
            num = (num == 0) ? 1 : num;
            a += (rule[i] == '+') ? num * angle : num * -angle;
            num = 0;
        }
        
        /* If it is a state save request ... */
        else if(rule[i] == '[') {
            /* ... save the state on the stack, ... */
            sPos = pos;
            sA = a,
            sS = s;
            
            /* ... recursively call on the next characters, ... */
            compute_figure(rule + i + 1, d);
            
            /* ... and restore the state. */
            pos = sPos;
            a = sA;
            s = sS;
            num = 1;
            
            /* Everything between the '[' and ']' characters has been interpreted,
             * so gobble up everything until we see a properly nested pair of
             * brackets.
             */
            do {
                i++;
                num = num + ( (rule[i] == '[') ? 1 : (rule[i] == ']') ? -1 : 0);
            } while(num != 0);
        }
        
        /* If it is a state restore request, then just return.  The
         * previous state will be restored on the caller's side.
         */
        else if(rule[i] == ']') return;
    }
}
