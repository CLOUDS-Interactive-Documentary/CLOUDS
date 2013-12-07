
#include "CodePanel.h"
#include "CloudsGlobal.h"
#include "ofxRegex.h"

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
	//cout << "setup buffer " << initialBuffer.getText();
	
//    for(int i = 0; i < NUM_MATCH; i++){
//		output[i] = "";
//	}
	
	//char buff[s.length()*3];
//	string replaced = s;
//	sprintf(buff, s.c_str(), replaced.c_str());
//	s = buff;
//	vector<string> lines = ofSplitString(s, "\n");
	ofxRegex r;
	int startIndex;
	int matchLen;
	int lastIndex;
//	s = "";
//	sG = "";
	for(int lI = 0; lI < lines.size(); lI++){
	
		SyntaxLine sL;
		//for each line
		string line = lines[lI];
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
		
		//s += line + "\n";
	}
	
}

void CodePanel::update(){
	offset += scanSpeed;
//	cout << "offset is " << offset << endl;
}

void CodePanel::draw(){
//	layout.setLineLength( drawRect.width );
//	layout.drawString( initialBuffer.getText().substr(ofGetElapsedTimef()*10, 50), drawRect.x, drawRect.y );
	
	float lineHeight = layout.getStringBoundingBox("Q",0,0).height;
	int line = 0;
	
	ofSetColor(0);
	ofRect(drawRect);
	ofSetColor(255);

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
