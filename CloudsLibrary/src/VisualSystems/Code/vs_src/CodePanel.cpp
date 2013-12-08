
#include "CodePanel.h"
#include "CloudsGlobal.h"
#include "ofxRegex.h"
#include "ofxTween.h"

#define NUM_MATCH 5
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

static ofColor matchColors[MATCH_TYPES] = {
	ofColor(0, 255, 0),
	ofColor(255, 0, 0),
	ofColor(0, 0, 255),
	ofColor(135, 96, 82),
	ofColor(222, 69, 219)
};

CodePanel::CodePanel(){
	offset = 0;
	scanSpeed = .1;
}

void CodePanel::myReplace(string& str, const string& oldStr, const string& newStr)
{
	size_t pos = 0;
	while((pos = str.find(oldStr, pos)) != string::npos)
	{
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}

int CodePanel::indexOf(string s, string f, int startIndex){
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


void CodePanel::setup(string codeFile){
	
	maxCharsOnLine = 0;
	
	//TODO: shared font
	layout.loadFont( getDataPath() + "font/Courier New Bold.ttf", 8);
	
	vector<string> lines;
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
		
		for(int mI = 0; mI < NUM_MATCH; mI++){
			
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
	
	randomBoxColor = ofColor::fromHsb(ofRandom(255),255,255);
	
	animationStartTime = ofGetElapsedTimef() + ofRandom(.3);
	animationEndTime = animationStartTime + .8;
}

void CodePanel::update(){
	offset += scanSpeed;
}

void CodePanel::draw(float screenHeight){
	
	
	float lineHeight = layout.getStringBoundingBox("Q",0,0).height;
	int line = 0;

	
	float animationPercent = ofMap(ofGetElapsedTimef(), animationStartTime, animationEndTime, 0, 1.0, true);
//	if(animationPercent < 1.){
//		
//	}
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
	if(drawAsHist){
		map<unsigned char, ofPolyline> charPolies;
		for(int i = drawRect.x; i < drawRect.getMaxX(); i++){
		int curline = (i + int(offset*10)) % syntaxLines.size();
		   map<unsigned char,int>::iterator it;
		   for(it = syntaxLines[curline].charCounts.begin(); it != syntaxLines[curline].charCounts.end(); it++){
			   charPolies[it->first].addVertex(ofPoint(i,ofMap(it->second, 0, maxCharsOnLine, drawRect.getMaxY(), drawRect.getMinY())));
		   }
		}
		
		int col=0;
		int numLines = charPolies.size();
		map<unsigned char, ofPolyline>::iterator it;
		for(it = charPolies.begin(); it != charPolies.end(); it++){
			ofColor c = ofColor::fromHsb( (128.*col)/numLines, 255, 255);
			c.a = 100;
			ofSetColor(c);
			it->second.draw();
			col++;
		}
		
	}
	else {
		for(int l = lineHeight*1.5; l < drawRect.height; l += lineHeight*1.5){
			int curline = (line + int(offset)) % syntaxLines.size();
			
			layout.drawString( syntaxLines[curline].baseLine, drawRect.x, drawRect.y + l);
			
			ofPushStyle();
			for(int c = 0; c < MATCH_TYPES; c++){
				ofSetColor(matchColors[c]);
				layout.drawString( syntaxLines[curline].colored[c], drawRect.x, drawRect.y + l);
			}
			ofPopStyle();
			
			line++;
		}
	}
	
	
	glDisable(GL_SCISSOR_TEST);
}
