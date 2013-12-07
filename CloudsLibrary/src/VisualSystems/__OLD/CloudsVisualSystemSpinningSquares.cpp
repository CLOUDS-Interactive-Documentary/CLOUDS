#include "CloudsVisualSystemSpinningSquares.h"

string _C::getSystemName(){
  return "SpinSquare";
}

void _C::selfSetup(){
	rotation = 0;
}

void _C::selfSetupGuis(){
	spinSlider = gui->addSlider("Spin Speed",-180.0, 10.0, 180.0);
}

void _C::selfUpdate(){
	rotation += spinSlider->getScaledValue();
}

void _C::selfDrawBackground(){
  ofBackground(255,0,0);
}

void _C::selfDraw(){
  float h = ofGetWindowHeight();
  ofFill();
  ofSetColor(255,255,255,128);
	ofRotate( rotation, 0,0,1 );
  ofRect( -debugGridSize , -debugGridSize , 
					debugGridSize*2, debugGridSize*2);
}

