/**
@author jtnimoy
*/

#ifndef _BALL_H_
#define _BALL_H_

#include <stdio.h>
#include <stdlib.h>

#include "LinkedList.h"
#include "ofOpenAlSoundPlayer.h"
#include "V3.h"

/*
  Ball object
  bounces around and has controllable forces.
*/

class Ball: public V3{
	public:
		static ofOpenALSoundPlayer* sound;
		char *netstr;//buffer for net printing.
		V3 oldPos;
		V3 force;
    float gain;
		int channel;
		float jitter;
		unsigned char volume;
		long *lastBounceTimes;
		unsigned long  bounceTimeDelta;
		unsigned long tooMuchBouncingThreshold;

		void setPos(float x,float y);
    Ball(V3 v,int channel, std::string soundfile, float gain);
    Ball(V3 v,float oldX_,float oldY_,float forceX_,float forceY_,float jitter, std::string soundfile, float gain);
    Ball(std::string soundfile, float gain);
		~Ball();
		void stepPhysics(float friction);
		void applyForce(float x,float y);
    void initMem(std::string soundfile, float volume);
		void bounce(float x1,float y1,float x2,float y2, float freqRange);
		void amnesia();
    
    void update(ofEventArgs& args);
};

#endif

