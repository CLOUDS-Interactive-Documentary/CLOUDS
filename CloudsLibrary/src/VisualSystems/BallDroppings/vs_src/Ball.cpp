/**
@author jtnimoy
*/

#include "Ball.h"
#include <math.h>
//#include "testApp.h"


//----------------------------------------------------
Ball::Ball(std::string soundfile, float gain){
    initMem(soundfile, volume);
}
//----------------------------------------------------
void Ball::initMem(std::string soundfile, float gain){
  oldPos.copyFrom(0,0,0);
  force.copyFrom(0,0,0);
  netstr = new char[16];
  channel = 0;
  volume = 0;
  
  sound.loadSound(soundfile);
  sound.setVolume(gain);
  
  lastBounceTimes = new long[16];
  jitter = 0;
  bounceTimeDelta = 10000;
  tooMuchBouncingThreshold = 300;
}
//----------------------------------------------------
Ball::Ball(V3 v, int _channel, std::string soundfile, float gain):V3(v.x,v.y,0){
  initMem(soundfile, gain);
  oldPos.copyFrom(v.x,v.y,0);
  channel = _channel;
}
//------------------------------------------------------------------------------
Ball::Ball(V3 v,float oldX_,float oldY_,float forceX_,float forceY_,float jitter, std::string soundfile, float gain):V3(v.x,v.y,0){
  initMem(soundfile, gain);
  copyFrom(v);
  oldPos.copyFrom(oldX_,oldY_,0);
  force.copyFrom(forceX_,forceY_);
}
//----------------------------------------------------
Ball::~Ball(){
	sound.stop();
	delete [] lastBounceTimes;
	delete [] netstr;
}
//----------------------------------------------------
void Ball::setPos(float _x,float _y){
  x = _x;
  y = _y;
}
//----------------------------------------------------
void Ball::stepPhysics(float friction){
  //apply the forces
  oldPos.copyFrom(x,y,0);
  x+= force.x;
  y+= force.y;
  
  force *= friction;
  
  if(jitter>0)jitter-=0.1;  
}
//----------------------------------------------------
void Ball::applyForce(float applyX,float applyY){
  force += V3(applyX,applyY,0);
}
//----------------------------------------------------
void Ball::bounce(float x1,float y1,float x2,float y2, float freqRange){

	//Thank you to Theo Watson for helping me out here.
	//V
	V3 v(force);
	
	//N
	V3 n(x2-x1,y2-y1,0);
	n = n.getLeftNormal();
	n.normalize();

	//2 * V [dot] N
	float dotVec = v.dot(n) * 2;

	// ( 2 * V [dot] N ) N
	n.scaleVec(dotVec);

	//V - ( 2 * V [dot] N ) N
	//change direction
	V3 mvn(v.minVecNew(n));
	force.copyFrom(mvn);

	////now some checks, with some sound playback

  for(int i=15;i>0;i--){//shift the queue
    lastBounceTimes[i] = lastBounceTimes[i-1];
  }
	lastBounceTimes[0] = ofGetElapsedTimeMillis(); //(glutGet(GLUT_ELAPSED_TIME));//then add the new value
  
  //now check for unusual behavior
  bounceTimeDelta = lastBounceTimes[0] - lastBounceTimes[15];
  if (bounceTimeDelta<tooMuchBouncingThreshold){ //softeners for the balls
    force.copyFrom(0,0);//make it still
  } else { 
      unsigned long freq = 60 + force.getLength() * freqRange;
	sound.setSpeed(freq/44100.0);
	sound.play();
    jitter = force.getLength()/2;
  }
  
}

//----------------------------------------------------
void Ball::amnesia(){
  oldPos.copyFrom(x,y,0);
}
