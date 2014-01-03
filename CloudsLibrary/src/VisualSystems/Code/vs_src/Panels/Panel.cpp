
#include "Panel.h"
#include "CloudsGlobal.h"
#include "ofxRegex.h"
#include "ofxTween.h"


static string toMatch[MATCH_TYPES] = {
	//comment
	"^\\s*//.*",
	//strings
	"\\\".*?[^\\\\]\\\"",
	//base types
	"(int|string|bool|void|float|char|vector|\\d)",
	//preprocessors
	"#\\w+",
	//keywords
	"(class|public|protected|private|const|for|while)"
};

Panel::Panel(){
	offset = 0;
	scanSpeed = .1;
	maxCharsOnLine = 0;
}

void Panel::setup(string codeFile){
	
	initialBuffer = ofBufferFromFile(codeFile);
	while(!initialBuffer.isLastLine()){
		string line = initialBuffer.getNextLine();
		myReplace(line, "\n", "\\n");
		myReplace(line, "\"", "\\\"");
		myReplace(line, "\t", "\\t");
		
		lines.push_back(line);
	}

	offset = ofRandom(lines.size() - 40);
	
	ofxRegex r;
	int startIndex;
	int matchLen;
	int lastIndex;
	for(int lI = 0; lI < lines.size(); lI++){
		
		SyntaxLine sL;
		//for each line
		string line = lines[lI];
		//calculate char counts
		for(int i = 0; i < line.size(); i++){
			sL.charCounts[ line.at(i) ]++;
		}
		
		map<unsigned char,int>::iterator it;
		for(it = sL.charCounts.begin(); it != sL.charCounts.end(); it++){
			maxCharsOnLine = MAX(maxCharsOnLine,it->second);
		}
		
		for(int mI = 0; mI < MATCH_TYPES; mI++){
			
			vector<string> matched = r.getMatchedStrings(line, toMatch[mI]);
			
			//reset the "search from" index
			lastIndex = 0;
			string cLine = "";
            
			for (vector<string>::iterator it = matched.begin(); it != matched.end(); ++it){
				//for each match
				//find where the match is in the rest of the current line
				startIndex = indexOf(line, *it, lastIndex);
				if (startIndex < 0){
					cout<<"BIG PROBLEM"<<endl;
				}
				
				//in the colored output, insert spaces between last match and current match
				for(int i = lastIndex; i < startIndex; i++){
					cLine += " ";
				}
				matchLen = it->length();
				//move the matched text to the colored output
				//and replace it with spaces in the source
				for(int i = 0; i < matchLen; i++){
					cLine += line[startIndex+i];
					line.replace(startIndex + i, 1, " ");
				}
				lastIndex = startIndex + matchLen;
			}
			sL.colored[mI] += cLine;
		}
		sL.baseLine = line;
		syntaxLines.push_back(sL);
		
	}
	
	
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
