//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
//
//
//

#include "BalloonCredit.h"

BalloonCredit::BalloonCredit(string _title, string _name, ofVec3f _pos) :
	name( _name ), title( _title ), pos(_pos)
{
	setup();
}

void BalloonCredit::setup()
{
	getTextWidth();
}

void BalloonCredit::getTextWidth()
{
	//figure out the text width here
	width = 75;//temp
}

ofVec3f BalloonCredit::getLeft()
{
	return ofVec3f(pos.x - width*.5, pos.y, pos.z);
}

void BalloonCredit::draw()
{
	//draw text at the position
	glDisable(GL_CULL_FACE);
	ofSetColor(255,255,255);
//		ofDrawBitmapString("YO DAWG, I PUT SOME WORDS IN YOUR BALLOONS", pos.x - width*.5, pos.y + );
//		ofDrawBitmapString("Dr. Van Nostron", pos.x - width*.5, pos.y - 6);
	
	ofSetColor(255, 255, 0);
	glLineWidth(2);
	ofLine(pos.x - width*.5, pos.y, pos.z, pos.x + width*.5, pos.y, pos.z);
}
	

