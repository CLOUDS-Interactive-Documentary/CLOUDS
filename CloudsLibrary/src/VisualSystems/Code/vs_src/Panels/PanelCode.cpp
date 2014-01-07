
#include "PanelCode.h"
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

PanelCode::PanelCode(){

}

void PanelCode::selfSetup(){
	
	string codeFile = dataPath + "code_test.txt";
	if(ofFile(codeFile).exists()){
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
	}
	
	maxCharsOnLine = 0;
}

void PanelCode::selfDraw(){

	if(sharedFont == NULL){
		return;
	}
	
	float lineHeight = sharedFont->getStringBoundingBox("Q",0,0).height;
	int line = 0;

	for(int l = lineHeight*1.5; l < drawRect.height; l += lineHeight*1.5){
		int curline = (line + int(offset)) % syntaxLines.size();
		ofPushStyle();
		
		ofSetColor(tint);
		sharedFont->drawString( syntaxLines[curline].baseLine, drawRect.x + *marginLeft, drawRect.y + l);
		
		for(int c = 0; c < MATCH_TYPES; c++){
			ofSetColor( matchColorTypes[c] );
			sharedFont->drawString( syntaxLines[curline].colored[c], drawRect.x + *marginLeft, drawRect.y + l);
		}
		ofPopStyle();
		
		line++;
	}
}


