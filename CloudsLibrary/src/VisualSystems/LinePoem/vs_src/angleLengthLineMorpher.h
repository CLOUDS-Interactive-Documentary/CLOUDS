#pragma once
#include "ofMain.h"
#include "anglePoint.h"
#include "angleLengthLine.h"



class angleLengthLineMorpher {
  
public: 
  
  anglePoint        * angPres;
  anglePoint        * angTarg;
  anglePoint        * angDiff;

  ofPoint      * pts;              // as I move the angles, these are the pts that are drawn

  float       length;                   // length
  int         nPts;                     // how big is the ring buffer?
  int         ptCount;                  // how many points have we added?

  // start and end points of the uncurling line
  ofPoint    gp0;
  ofPoint    gp1;
  ofPoint    p0, p1;  // temp pts
  ofPoint   pOrig;
 

  // for the "uncurling"
  float           angleTotalBack;
  float           angleTotalForw;
  float           angleToBeLevel;

  // because of the uncurling, and the way it works : (a) I'll have to scale the drawings, (b) I'll have to center the drawing
  float           dist;
  ofPoint    centroid;
  ofPoint    centroidOffsetCatch;

  // for timing:

  int             startTimeMillis;  
  int             totalTimeMillis;    
  bool         bAmAnimating;
  float           pctAnimating;
    bool          bSetNewTarget;
    



  //-------------------------------------------------------------
  angleLengthLineMorpher(  int nPtsToRecord  ) {
    
      bSetNewTarget = false;
    
    //FM                  = new FastMath();
    
    nPts                = nPtsToRecord;
    angPres                = new anglePoint[nPts]; 
    angTarg                = new anglePoint[nPts]; 
    angDiff                = new anglePoint[nPts]; 

    pts                 = new ofPoint[nPts]; 
    //centroid            = new ofPoint();
    //centroidOffsetCatch = new ofPoint();

    for (int i = 0; i < nPts; i++){
      /*angPres[i]   = new anglePoint();
      angTarg[i]   = new anglePoint(); 
      angDiff[i]   = new anglePoint(); 
      pts[i]   = new ofPoint();*/
    }

    ptCount   = 0;
//    gp0       = new ofPoint();
//    gp1       = new ofPoint();
//    p0        = new ofPoint();
//    p1        = new ofPoint();
//    pOrig     = new ofPoint();
    gp0.x     = 150;
    gp0.y     = 100;
    length    = 1.1f;
    angleTotalBack = 0;
    angleTotalForw = 0;
    clear();
      angleToBeLevel = 0;
  }

  void clear(){
    ptCount   = 0;    // simple, just clear the counter
    totalTimeMillis  = 1500;   // 1.5 seconds?
    startTimeMillis  = 0;
    bAmAnimating     = false;
    pctAnimating     = 0;  
  }


  //----------------------------------------------------------------
  void setPres(angleLengthLine * AL){
    for (int i = 0; i < nPts; i++){
      angPres[i].angle = AL->ang[i].angle;
    }
  }

  //----------------------------------------------------------------
  void setTarg(angleLengthLine * AL){

    bAmAnimating     = true;


    float totalDiff = 0;
    
    for (int i = 0; i < nPts; i++){
        angPres[i] = angTarg[i];
    }
      
      for (int i = 0; i < nPts; i++){
          angTarg[i] = AL->ang[i];
      }
      
    
      
    for (int i = 0; i < nPts; i++){
      angDiff[i].angle = angTarg[i].angle -  angPres[i].angle;
      totalDiff += fabs(angDiff[i].angle);
    }
    
    totalTimeMillis = max((int)(totalDiff * 35), 750);
    ptCount = nPts - 2;
    
      bSetNewTarget = true;
      
    //mom.println(" ---- " + AL);
  }



  void draw(float pct){
      
      if (bSetNewTarget){
          startTimeMillis = ofGetElapsedTimeMillis();
          bSetNewTarget = false;
      }
      
   if (bAmAnimating){
      if ((ofGetElapsedTimeMillis() - startTimeMillis) < (int)(totalTimeMillis)){
        pctAnimating = (float)(ofGetElapsedTimeMillis() - startTimeMillis) / (float)totalTimeMillis;
      } 
      else {
        bAmAnimating = false;
          
          for (int i = 0; i < nPts; i++){
              angPres[i] = angTarg[i];
          }
          
     	  //System.arraycopy(angTarg,0,angPres,0,nPts);
      }
    }
    
    float pctShapedByCos = 0.5f - 0.5f * cos(pctAnimating * M_PI);
    if (bAmAnimating == true){
      calcForward(pctShapedByCos);
      calcBackward(pctShapedByCos);
      calcCentroid();
        }
      
      
      drawPoints();
  }




  void calcForward(float pct){


    

    p0.x =  0.95f * gp0.x + 0.05f * 200.0f;
    p0.y =  0.95f * gp0.y + 0.05f * 180.0f;

    pOrig.x = p0.x;
    pOrig.y = p0.y;


    float addToAngleAdder = ( angleToBeLevel ) / 30.0f;
    float angleAdder = 0; //angleTotalForw + angleToBeLevel;

    float zeroTarget = 0;
    // scaling based on 


    if (dist > 0.01f){
      float newLen = (length * (100.0f / dist));
      if (newLen > length)
        length = 0.96f * length + 0.04f * (length * (100.0f / dist));
      else 
        length = 0.9f * length + 0.1f * (length * (100.0f / dist));
    }

  
    float angleA, angleB, angleToAdd;
    for (int i = 0; i < ptCount; i++){

      angleA =       angPres[i].angle;
      angleB = pct * angDiff[i].angle;
      angleToAdd = (angleA + angleB);

      /*if (angleToAdd > mom.PI) angleToAdd -= mom.TWO_PI;
       if (angleToAdd < mom.PI) angleToAdd += mom.TWO_PI;
       */

      //----------------------------------------------
      // totally hacked out "straightening routines" :
      if (i <= 30) angleAdder += addToAngleAdder;
      else if (i >= ptCount - 30){
        if (i == ptCount - 30){ 
          zeroTarget = angleAdder;
            //if (isNan(zeroTarget)) zeroTarget = 0;
          
            
            while (zeroTarget > TWO_PI) zeroTarget -= TWO_PI;
        }
        angleAdder += (-zeroTarget/30.0f);
        angleAdder -=  angleToAdd; 
      }
      //----------------------------------------------

      //----------------------------------------------
      angleAdder +=  angleToAdd; 
      //----------------------------------------------

      //----------------------------------------------
      // walk along, blur a little...
      p1.x = p0.x + length * (float)cos(angleAdder);
      p1.y = p0.y + length * (float)sin(angleAdder);

      pts[i].x = 0.9f *  pts[i].x + 0.1f * p1.x;
      pts[i].y = 0.9f *  pts[i].y + 0.1f * p1.y;
      //----------------------------------------------

      //----------------------------------------------
      // at the end, do something for unculring:
      
      //----------------------------------------------
      p0.set(p1.x, p1.y);
    }

    
    int i = (ptCount - 1);
    gp1.set(p1.x, p1.y);
    angleTotalBack = atan2(p1.y - p0.y, p1.x - p0.x);
    float angleNotLevel = atan2(p1.y - pOrig.y, p1.x - pOrig.x);
    dist            = sqrt((p1.y - pOrig.y)*(p1.y - pOrig.y) + (p1.x - pOrig.x)*(p1.x - pOrig.x));
    angleToBeLevel = -angleNotLevel;
      


    //------------------------------------------
  }


  void calcCentroid(){

    //-----------------------------------------
    // calc centroid:
    if (ptCount > 0){
      for (int i = 0; i < ptCount; i++){
        centroid.x += pts[i].x;
        centroid.y += pts[i].y;
      }

      centroid.x /= (float)ptCount;
      centroid.y /= (float)ptCount;
      
      //if (!bAmAnimating){
      //centroidOffsetCatch.x = 0.5f * centroidOffsetCatch.x + 0.15f * centroid.x;
      //centroidOffsetCatch.y = 0.85f * centroidOffsetCatch.y + 0.15f * centroid.y;
      //} else {
      centroidOffsetCatch.x = 0.1f * centroidOffsetCatch.x + 0.9f * centroid.x;
      centroidOffsetCatch.y = 0.1f * centroidOffsetCatch.y + 0.9f * centroid.y;
      //}
        
        
      float diffInY = 100 - centroidOffsetCatch.y;
      float diffInX = 540/2 - centroidOffsetCatch.x;

      for (int i = 0; i < ptCount; i++){
        pts[i].y += diffInY;
        pts[i].x += diffInX;
      }
    }
  }

  void drawPoints(){

    ofScale(2,2,1);
    if (ptCount <= 0) return;
      
      

    float xp = pts[0].x - (centroidOffsetCatch.x - pts[0].x) * 0.33f;
    float yp = pts[0].y - (centroidOffsetCatch.y - pts[0].y) * 0.33f;
    float xp2 = pts[ptCount-1].x - (centroidOffsetCatch.x - pts[ptCount-1].x) * 0.13f;
    float yp2 = pts[ptCount-1].y + (centroidOffsetCatch.y - pts[ptCount-1].y)* 0.13f;

      ofNoFill();
    ofBezier(60, 100, 150, 100,xp,yp,pts[0].x, pts[0].y);
    ofBezier(430, 100, 340, 100 ,xp2 +  0.7f * ( -pts[ptCount-1].x + 350),yp2 +  0.5f * ( pts[ptCount-1].y - 100),pts[ptCount-1].x, pts[ptCount-1].y); 
    ofLine(0,100,60,100);
    ofLine(430,100,550,100);	
    
    
    // 60
      ofNoFill();
    ofBeginShape();
    for (int i = 0; i < ptCount; i++){
      ofVertex(pts[i].x, pts[i].y);
    }
    ofEndShape();
  }

  //----------------------------------------------
  void calcBackward(float pct){
    p0.x = gp1.x;
    p0.y = gp1.y;
    p0.x =  0.95f * gp1.x + 0.05f * 100.0f;
    p0.y =  0.95f * gp1.y + 0.05f * 100.0f;
    float angleAdder = angleTotalBack; //angleTotalForw + angleToBeLevel;
    float angleToAdd;
    for (int i = ptCount-1; i >= 0; i--){
      angleAdder += (TWO_PI - (angPres[i].angle + (pct) * angDiff[i].angle));
      p1.x = p0.x - length * (float)cos(angleAdder);
      p1.y = p0.y - length * (float)sin(angleAdder);
      p0.set(p1.x, p1.y);
    }
  }
};



/*
  //-----------------------
 // 2d rotation around a n g l e 
 x1 = x cos () - y sin ();
 y1 = y cos () + x sin ();
 //-----------------------
 
 */
