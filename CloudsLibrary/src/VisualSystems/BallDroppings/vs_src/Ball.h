/**
@author jtnimoy
*/

#ifndef _BALL_H_
#define _BALL_H_

#include <stdio.h>
#include <stdlib.h>

#include "LinkedList.h"
#include "ofSoundPlayer.h"
#include "V3.h"

/*
  Ball object
  bounces around and has controllable forces.
*/

class Ball: public V3{
	public:
		ofSoundPlayer sound;
		char *netstr;//buffer for net printing.
		V3 oldPos;
		V3 force;
		int channel;
		float jitter;
		unsigned char volume;
		long *lastBounceTimes;
		unsigned long  bounceTimeDelta;
		unsigned long tooMuchBouncingThreshold;

		void setPos(float x,float y);
    Ball(V3 v,int channel, std::string soundfile);
    Ball(V3 v,float oldX_,float oldY_,float forceX_,float forceY_,float jitter, std::string soundfile);
    Ball(std::string soundfile);
		~Ball();
		void stepPhysics();
		void applyForce(float x,float y);
    void initMem(std::string soundfile);
		void bounce(float x1,float y1,float x2,float y2);
		void amnesia();
};

#endif

