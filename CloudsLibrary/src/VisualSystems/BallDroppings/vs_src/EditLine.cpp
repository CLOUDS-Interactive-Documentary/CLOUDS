/**
@author jtnimoy
*/

#include "EditLine.h"

EditLine::EditLine(){
  initMem();
}
//------------------------------------------------------------------------------
void EditLine::initMem(){
  x1 = 0;
  y1 = 0;
  x2 = 0;
  y2 = 0;
}
//------------------------------------------------------------------------------
EditLine::EditLine(float _x1,float _y1, float _x2,float _y2){
  initMem();
  //this makes sure that x1 is always the smallest of the pair.
  x1 = _x2;
  y1 = _y2;
  x2 = _x1;
  y2 = _y1;
  fixDirection();
}
//------------------------------------------------------------------------------
EditLine::~EditLine(){

}
//------------------------------------------------------------------------------
void EditLine::set1(float x,float y){
  x1 = x;
  y1 = y;
}
//------------------------------------------------------------------------------
void EditLine::set2(float x,float y){
  x2 = x;
  y2 = y;
}
//------------------------------------------------------------------------------
int EditLine::whichSideY(float x,float y){
  //get the slope - M in y=mx+b
  float m = (y2-y1)/(x2-x1);
  float b = y1 - m*x1;
  
  int fallen_outside=0;
  //now find out if it's hitting the line seg, and not the entire ray.
  
  if(x>x1||x<x2){//if fallen outside
    return 3;
  }else{
    return  (x*m+b)>y;// here is whether or not it's above.
  }
}
//------------------------------------------------------------------------------
int EditLine::fixDirection(){
  //this makes sure that x1 is always the smallest of the pair.
  //swap everyone
  int swapReport=0;
  if(x1 < x2){
    float t = x1;
    x1=x2;
    x2=t;

    t = y1;
    y1=y2;
    y2=t;
    swapReport = 1;
  } else {
    swapReport = 0;
  }
  
  //also fix verticality.
  if(x1==x2){
    x1+=0.1;
  }
  return swapReport;
}
//--------------------------------------------------------------------------------
  bool EditLine::checkBallCollide(Ball *ball){

    V3 lineLocalVec(x2-x1, y2-y1, 0);

    //get the angle between the ball and one end of the wall
    float angleCurrent1 = checkAngle(ball->x , ball->y , x1,y1, lineLocalVec);  
    float angleCurrent2 = checkAngle(ball->x , ball->y , x2,y2, lineLocalVec);  

    //lets get the angle between the ball and one end of the wall
    float angleFuture1 = checkAngle(ball->x+ball->force.x, ball->y+ball->force.y ,x1,y1, lineLocalVec);
    float angleFuture2 = checkAngle(ball->x+ball->force.x, ball->y+ball->force.y ,x2,y2, lineLocalVec);

    if(diffSign(angleCurrent1,angleFuture1) && diffSign(angleCurrent2,angleFuture2)){
      float d1x = ball->x - x1;
      float d1y = ball->y - y1;
      float d2x = ball->x - x2;
      float d2y = ball->y - y2;
      float wallLength = lineLocalVec.getLength();
      if( (sqrt(d1x*d1x + d1y*d1y) < wallLength) && (sqrt(d2x*d2x + d2y*d2y) < wallLength)){
        return true;
      }
      else return false;
    }
    else return false;
  }

//--------------------------------------------------------------------------------

  float EditLine::checkAngle( float point_x, float point_y,float line_x, float line_y, V3 lineVec){

    V3 vec(line_x - point_x, line_y - point_y, 0);

    V3 vecline(0,0,0);
    vecline.copyFrom(lineVec);
    vecline = vecline.getRightNormal();

    vec.normalize();
    vecline.normalize();

    return vec.dot(vecline);

  }

//--------------------------------------------------------------------------------

bool EditLine::diffSign(float v1,float v2){
  if( (v1 >= 0 && v2 < 0 ) || (v2 >= 0 && v1 < 0 ) )return true;
  else return false; 
}
