#pragma once
/**
	 introductory example of subclassing Rezanator.
	 spinning square is drawn with 1 spinning slider.
	 
	 @author jtnimoy
*/
#include "CloudsVisualSystemRezanator.h"

#define _C CloudsVisualSystemSpinningSquares

class _C:public CloudsVisualSystemRezanator{
 public:
	void selfSetup();
  string getSystemName();
  void selfUpdate();
  void selfDrawBackground();
  void selfDraw();
	void selfSetupGuis(); 
	ofxUISlider *spinSlider;
	float rotation;
};

