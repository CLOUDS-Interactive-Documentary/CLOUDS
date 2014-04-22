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

float BalloonCredit::getTextWidth()
{
	//figure out the text width here
	width = font->stringWidth(title);//width = 75; //temp
	
	return width;
}

ofVec3f BalloonCredit::getLeft()
{
	return ofVec3f(pos.x - width*.5, pos.y, pos.z);
}

void BalloonCredit::update()
{
	
	ofNode n;
	n.setPosition( pos );
	n.lookAt(camera->getPosition(), ofVec3f(0,1,0));
	ofVec3f axis; float angle;
	n.getOrientationQuat().getRotate(angle, axis);
	
	width = font->stringWidth(title);
	
	left = ofVec3f(-width * .5, 0, 0) * n.getGlobalTransformMatrix();
	right = ofVec3f(width * .5, 0, 0) * n.getGlobalTransformMatrix();
}

void BalloonCredit::draw()
{
	ofPushStyle();
	ofPushMatrix();
	ofSetColor(255);
	ofDisableLighting();
	
	ofNode n;
	n.setPosition( pos );
	n.lookAt(camera->getPosition(), ofVec3f(0,1,0));
	ofVec3f axis; float angle;
	n.getOrientationQuat().getRotate(angle, axis);
	
	// Translate the object to its position.
	ofTranslate( pos );
	// Perform the rotation.
	ofRotate(angle, axis.x, axis.y, axis.z);
	ofRotate(180, 0, 0, 1);
		
	//draw text at the position
	float titleWidth  = font->stringWidth(title);
	float titleHeight = font->stringHeight(title);
	font->drawString(title, -titleWidth/2, titleHeight/2);
	
	ofEnableLighting();
	ofPopMatrix();
	ofPopStyle();
	
//	glLineWidth(4);
//	ofSetColor(225, 255, 0);
//	ofLine(left, right);
}
	

