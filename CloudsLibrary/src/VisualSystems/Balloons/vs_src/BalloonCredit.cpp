//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
//
//
//

#include "BalloonCredit.h"

void BalloonCredit::setup()
{
}

void BalloonCredit::getTextWidth()
{
	//figure out the text width here
	width = 75; //temp
}

ofVec3f BalloonCredit::getLeft()
{
	return ofVec3f(pos.x - width*.5, pos.y, pos.z);
}

void BalloonCredit::draw()
{
	
	ofPushMatrix();
	
	ofNode n;
	n.setPosition( pos );
	n.lookAt(camera->getPosition(), ofVec3f(0,1,0));
	ofVec3f axis; float angle;
	n.getOrientationQuat().getRotate(angle, axis);
	
	// Translate the object to its position.
	ofTranslate( pos );
	// Perform the rotation.
	ofRotate(angle, axis.x, axis.y, axis.z);
		
	//draw text at the position
	float titleWidth  = font->stringWidth(title);
	float titleHeight = font->stringHeight(title);
	font->drawString(title, titleWidth/2, titleHeight/2);
	
//	glDisable(GL_CULL_FACE);
//	ofSetColor(255,255,255);
////		ofDrawBitmapString("YO DAWG, I PUT SOME WORDS IN YOUR BALLOONS", pos.x - width*.5, pos.y + );
////		ofDrawBitmapString("Dr. Van Nostron", pos.x - width*.5, pos.y - 6);
//	
//	ofSetColor(255, 255, 0);
//	glLineWidth(2);
//	ofLine(pos.x - width*.5, pos.y, pos.z, pos.x + width*.5, pos.y, pos.z);
	
	ofPopMatrix();
}
	

