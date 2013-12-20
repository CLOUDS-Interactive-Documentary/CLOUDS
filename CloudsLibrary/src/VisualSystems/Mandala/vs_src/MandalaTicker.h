//
//  MandalaTicker.h
//  Mandala
//
//  Created by lars berg on 10/31/13.
//
//

#pragma once
#include "ofxTween.h"

template <class T> class MandalaTicker
{
public:
	MandalaTicker( int easeType = OFX_TWEEN_QUINT)
	{
		bLoop = false;
		bContinue = false;
		endTrigger = false;
		bReverse = false;
		
		easeType = ofxTween::easeInOut;
		ease = OFX_TWEEN_QUINT;
		
		note = "_";
		miscVal = 1;
	};
	~MandalaTicker()
	{
	};
	
	enum tickerEases{
		OFX_TWEEN_LINEAR = 0,
		OFX_TWEEN_CUBIC = 1,
		OFX_TWEEN_ELASTIC = 2,
		OFX_TWEEN_BOUNCE = 3,
		OFX_TWEEN_QUART = 4,
		OFX_TWEEN_BACK = 5,
		OFX_TWEEN_CIRC = 6,
		OFX_TWEEN_EXPO = 7,
		OFX_TWEEN_QUAD = 8,
		OFX_TWEEN_QUINT = 9,
		OFX_TWEEN_SINE = 10
	};
	
	
	float mix( float t)
	{
		switch ( ease ) {
			case OFX_TWEEN_BOUNCE:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingBounce(), easeType);
				
			case OFX_TWEEN_CUBIC:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingCubic(), easeType);
				
			case OFX_TWEEN_ELASTIC:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingElastic(), easeType);
				
			case OFX_TWEEN_LINEAR:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingLinear(), easeType);
				
			case OFX_TWEEN_QUART:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingQuart(), easeType);
				
			case OFX_TWEEN_BACK:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingBack(), easeType);
				
			case OFX_TWEEN_CIRC:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingCirc(), easeType);
				
			case OFX_TWEEN_EXPO:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingExpo(), easeType);
				
			case OFX_TWEEN_QUAD:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingQuad(), easeType);
				
			case OFX_TWEEN_QUINT:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingQuint(), easeType);
				
			case OFX_TWEEN_SINE:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingSine(), easeType);
				
			default:
				return ofxTween::map(t, startTime, endTime, 0, 1, true, ofxEasingLinear(), easeType);
		}
	}
	
	void end(bool allStop = true, float t = ofGetElapsedTimef())
	{
		bEnded = true;
		bStarted = false;
		
		if (!allStop)
		{
			if(bLoop)
			{
				begin(*value, startVal, targetVal, span, t);
				replayCount++;
			}
			else if(bContinue)
			{
				begin(*value, *value, targetVal + delta, span, t);
				replayCount++;
			}
			
			else if(bReverse)
			{
				begin(*value, *value, *value + delta, span, t);
				delta = -delta;
				replayCount++;
			}
		}
	}
	
	T& getValue(){
		*value;
	}
	
	void setValue( T& val ){
		value = &val;
	}
	
	
	void setContinue( bool keepGoing=true )
	{
		bContinue = keepGoing;
		if(bContinue){
			bLoop = false;
			bReverse = false;
		}
	}
	void setLoop( bool loop=true )
	{
		bLoop = loop;
		if(bLoop){
			bContinue = false;
			bReverse = false;
		}
	}
	void setReverse( bool reverse=true )
	{
		bReverse = reverse;
		delta = -delta;
		if(bReverse){
			bLoop = false;
			bContinue = false;
		}
	}
	
	void setDelay( float d){ delay = d;}
	
	void setEaseType( int easeType = OFX_TWEEN_LINEAR )
	{
		ease = easeType;
	}
	
	void begin( T& _value, T _startVal, T _targetVal, float _span, float _startTime=ofGetElapsedTimef(), ofxTween::ofxEasingType _easeType = ofxTween::easeOut )
	{
		value = &_value;
		startVal = _startVal;
		targetVal = _targetVal;
		delta = targetVal - startVal;
		
		span = _span;
		startTime = _startTime + delay;
		endTime = startTime + span;
		
		easeType = _easeType;
		
		bStarted = true;
		bEnded = false;
	}
	
	void setEase(ofxTween::ofxEasingType e){
		easeType = e;
	}
	
	void update(float t = ofGetElapsedTimef() )
	{
		endTrigger = false;
		if(bStarted && !bEnded)
		{
			blend = mix( t );
			
			*value = (1. - blend) * startVal + blend * targetVal;
			
			if(t >= endTime)
			{
				end( false, t );
				endTrigger = true;
			}
		}
	}
	
	void setTargetVal( T _targetVal )
	{
		targetVal = _targetVal;
		delta = startVal - targetVal;
	}
	
	void setStartVal( T _startVal )
	{
		startVal = _startVal;
		delta = startVal - targetVal;
	}
	
	T getTargetVal()
	{
		return targetVal;
	}
	
	bool getLoop(){	return bLoop;}
	bool getContinue(){	return bContinue;}
	
	bool ended(){return bEnded;}
	
public:
	bool endTrigger;
	string note;
	float miscVal;
	
private:
	bool bLoop, bContinue, bReverse, bPaused, bStarted, bEnded;
	float startTime, endTime, span, delay, blend;
	T startVal, targetVal, localVal, delta;
	T* value;
	
	ofxEasingQuint transitionEase;
	ofxTween::ofxEasingType easeType;
	
	int ease;
	
	int replayCount;
	
};
