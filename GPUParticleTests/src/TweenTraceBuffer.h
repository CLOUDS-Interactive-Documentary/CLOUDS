#pragma once

#include <set>

#include "ofMain.h"

#include "ofxTween.h"
#include "ofxEasingLinear.h"
#include "ofxEasingQuad.h"
#include "ofxEasingExpo.h"

class TweenTraceBuffer {
public:
    typedef float time_t;
    typedef pair<time_t, float> point;
	
    struct EasingFunction {
        EasingFunction(ofxEasing *, ofxTween::ofxEasingType);
        float tween(float value, float inputMin, float inputMax, float outputMin, float outputMax) const;
        ofxEasing * function;
        ofxTween::ofxEasingType type;
    };
    
    typedef map<string, EasingFunction> EasingDictionary;
    
    class KeyFrame {
	public:
		void draw() const;
		bool operator<(const KeyFrame &) const;
        EasingDictionary::const_iterator function;
		point start;
		point end;
    };
    
    class InputBuffer {
	public:
		InputBuffer();
		void add(time_t, float);
		void add(point);
		void clear();
		void draw() const;
		
        map<time_t, float> buffer;
		size_t size() const;
		
		bool checkTrace(const EasingFunction & function, float threshold) const;
		
		point getStart() const;
		point getEnd() const;
		point getEndMinusOne() const;
	protected:
		point start, end;
    };
    
    TweenTraceBuffer(float threshold=0.05);

    void add(float time, float data);
	void add(pair<float, float>);
	
	void clear();
	void flush(); ///<flush current input buffer into keyframe
	
    void draw() const;
	
	float getThreshold() const;
	void setThreshold(float);
	
protected:
    void clearInputBuffer();
    bool traceInputBuffer();
    
    set<KeyFrame> keyframes;
    InputBuffer inputBuffer;
    
    float threshold;
    static EasingDictionary * easingFunctions;
    
    KeyFrame current;
    
private:
    void addFunc(string name, ofxEasing*);
	bool searchFunc();
};