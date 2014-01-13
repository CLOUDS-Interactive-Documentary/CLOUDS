#include "ofMain.h"
#include "anglePoint.h"


#pragma once

class angleLengthLine {
  
public:
    
  anglePoint    ang[250];
  ofPoint       pts[250];
  int          pointCounter;
  float       length;
  int         nPoints;         // how big is the ring buffer?
         
  //-------------------------------------------------------------
  void setup(  int nPointsToRecord  ) {
    nPoints       = nPointsToRecord;
    clear();
  }

  void clear(){
    pointCounter   = 0;    // simple, just clear the counter
  }


  

  void 	convertFromAngleLengthLine(ofPolyline mom){


    // I will have less points then a pointRecorder since, eh, I represent angles not points.
    // there for I will be n - 2, as the first and last point DONT have angles.
    // angle from 3 points routine:
    
    
      mom = mom.getResampledByCount(250);
      
    float f1, f2, f3;
    f2 = 0;
    
    pointCounter = mom.size() - 2;
    
    for(int i = 2; i < mom.size(); i++){
      ofPoint anglepointdata    = mom.getVertices()[i - 2];
      ofPoint anglepointdata1   = mom.getVertices()[i - 1];
      ofPoint anglepointdata2   = mom.getVertices()[i    ];
      if(i == 2){
       f2 = (float)(6.2831853071795862 - (returnAngle(anglepointdata1, anglepointdata2) + 3.1415926535897931));
      } 
      else {
        f1 = (float)(6.2831853071795862 - (returnAngle(anglepointdata1, anglepointdata2) + 3.1415926535897931));
        f3 = f1 - f2;
        if((double)f3 >= 3.1415926535897931)
          f3 = (float)(-((double)f2 + (6.2831853071795862 - (double)f1)));
        else
          if((double)f3 < -3.1415926535897931)
            f3 = (float)((double)f1 + (6.2831853071795862 - (double)f2));
        
        
          ang[i-2].set(f3);
        
        
        f2 = f1;
      }
    }
  }

  //-----------------------------------------------------------
  float returnAngle(ofPoint pointdata, ofPoint pointdata1){
    return (float)atan2(pointdata1.x - pointdata.x, pointdata1.y - pointdata.y);
  }
  
  
  void draw(float scalef, float offsetx, float offsety, float rot, ofRectangle & boundsToDraw){
    
      ofPoint p0, p1;
      //p0 = new gesturePoint();
      //p1 = new gesturePoint();
      p0.x = offsetx;
      p0.y = offsety;
      length = scalef;
      ofMesh mesh;
      ofRectangle boundsOfLine;
      mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
      float angleAdder = rot;
      for (int i = 0; i < 250; i++){
        angleAdder += ang[i].angle;
        p1.x = p0.x - length * cos(angleAdder);
        p1.y = p0.y - length * sin(angleAdder);
        mesh.addVertex( ofPoint(p0.x, p0.y));
        p0.x = p1.x;
        p0.y = p1.y;
      }
      
      
      ofPoint midPt;
      for (int i = 0; i < mesh.getVertices().size(); i++){
          ofPoint pt = mesh.getVertices()[i];
          midPt += pt;
      }
      
      midPt /= (float)mesh.getVertices().size();
      
      for (int i = 0; i < mesh.getVertices().size(); i++){
          ofPoint pt = mesh.getVertices()[i];
          mesh.getVertices()[i] = pt - midPt;
          if (i == 0) boundsOfLine.set(pt.x, pt.y, 1,1);
          else boundsOfLine.growToInclude(pt);
      }
   
      
      ofRectangle origBounds = boundsOfLine;
      ofRectangle newBounds = boundsOfLine;
      newBounds.scaleTo(boundsToDraw);
      float scale =  (float)newBounds.width / (float)origBounds.width;
      ofPushMatrix();
      ofTranslate(newBounds.getCenter());
      ofScale(scale, scale, 1.0);
      mesh.draw();
      ofPopMatrix();
      
       
      
      
  
  
  }
  
  
};
