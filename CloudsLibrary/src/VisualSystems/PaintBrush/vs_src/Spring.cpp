#include "Spring.h"

//---------------------------------------------------------------------
Spring::Spring(){
	A = NULL;
	B = NULL;
}

//---------------------------------------------------------------------
void Spring::update(){
	if ((A == NULL) || (B == NULL)){
		return;
	}
	
    float mutualDist = (*A - *B).length();
    float springForce = (k * (0.0 - mutualDist));
    ofPoint frcToAdd = (*A - *B).normalized() * springForce;
    
    A->addForce(frcToAdd);
    frcToAdd *= -1;
    B->addForce(frcToAdd);
}


//---------------------------------------------------------------------
void Spring::draw(){
	
	if ((A == NULL) || (B == NULL)){
		return;
	}
	
	ofLine( *A, *B);
}