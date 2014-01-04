
#include "Panel.h"
#include "CloudsGlobal.h"
#include "ofxRegex.h"
#include "ofxTween.h"


Panel::Panel(){
	offset = 0;
	scanSpeed = .1;
	maxCharsOnLine = 0;
	
	sharedFont = NULL;
}

void Panel::setup(){

	selfSetup();
}

void Panel::startAnimation(){
	animationStartTime = ofGetElapsedTimef() + ofRandom(.3);
	animationEndTime = animationStartTime + .8;	
}

void Panel::update(){
	offset += scanSpeed;
	selfUpdate();
}

void Panel::draw(float screenHeight){
	
	float animationPercent = ofMap(ofGetElapsedTimef(), animationStartTime, animationEndTime, 0, 1.0, true);
	ofRectangle currRect = drawRect;
	
	currRect.scale( ofxTween::map(animationPercent, 0.0, 0.5, 0.0, 1.0, true, ofxEasingSine(), ofxTween::easeIn),
				    ofxTween::map(animationPercent, 0.5, 1.0, 0.2, 1.0, true, ofxEasingSine(), ofxTween::easeIn) );
	
	ofNoFill();
	ofEnableAlphaBlending();
	ofSetColor(255, *outlineAlpha*255);
	ofRect(currRect);
	
	ofFill();
	glEnable(GL_SCISSOR_TEST);
	glScissor(currRect.x, screenHeight - currRect.y - currRect.height,
			  currRect.width, currRect.height);
	ofSetColor(255);
	
	selfDraw();
	
	glDisable(GL_SCISSOR_TEST);
}

void Panel::myReplace(string& str, const string& oldStr, const string& newStr)
{
	size_t pos = 0;
	while((pos = str.find(oldStr, pos)) != string::npos)
	{
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}


int Panel::indexOf(string s, string f, int startIndex){
	int fI = 0;
	for(int i = startIndex; i < s.length(); i++){
		//if the curr string char == the curr match char
		if (s[i] == f[fI]){
			fI++;
			//if we have matched all the match chars
			if (fI >= f.length()){
				//return where we started matching at
				return i - (fI - 1);
			}
		}
		else if (fI > 0){
			//if we have already matched a few chars,
			//lets re-start attempting to match from
			//the next index after the first matched char.
			i -= (fI - 1);
			fI = 0;
		}
	}
	//we didn't find a match
	return -1;
}

void Panel::selfSetup(){
	
}
void Panel::selfUpdate(){
	
}
void Panel::selfDraw(){
	
}
