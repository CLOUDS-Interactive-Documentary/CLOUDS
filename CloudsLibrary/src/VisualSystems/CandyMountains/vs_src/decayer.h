//
//  decayer.h
//  just_you_AA_NI_GLSL
//
//  Created by Jason Levine on 2013-11-15.
//
//

#pragma once

class decayer {
public:
    void setDecay(float _decay) { decay = _decay; }
    void addValue(float val) { (val > value) ? value = val : val *= decay; }
    void update() { value *= decay; }
    float getValue() { return value; }
    
    float decay, value;
};