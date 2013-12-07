//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
// Welcome to the EMPTY CloudsVisualSystem
//
//
//

#pragma once

#include "CloudsVisualSystem.h"
#include "ofxRegex.h"
#include <algorithm>

//TODO: rename this to your own visual system
class QuineVisualSystem : public CloudsVisualSystem {
protected:
    
    //  Your Stuff
    //
    //When you update the code, set string s = "[precent]s";
    // (replace [percent] with %)
    // then copy all the text into a separate text editor
    // and find/replace the following pairs (in regex mode):
    // \\ -> \\\\
    // \n -> \\n
    // \t -> \\t
    // " -> \\"
    //\"(\w|\s|\}|\{|\)|\(|;|\\|\"|(\\\n)|\n|\t|%|:|/|,|\.|#)*\"
    string matchString = "\\\".*?[^\\\\]\\\"";
#define NUM_MATCH 5
    string toMatch[NUM_MATCH] = {
        //comment
        "^\\s*//.*",
        //strings
        matchString,
        //base types
        "(int|string|bool|void|float|char|vector|\\d)",
        //preprocessors
        "#\\w+",
        //keywords
        "(class|public|protected|private|const|for|while)"};
    string output[NUM_MATCH];
    int matchColors[NUM_MATCH*3] = {
        0, 255, 0,
        255, 0, 0,
        0, 0, 255,
        135, 96, 82,
        222, 69, 219
    };
    
    
    string s = "//\n// CLOUDS Interactive Documentary\n//\n// VISUAL SYSTEMS\n//\n// Welcome to the EMPTY CloudsVisualSystem\n//\n//\n//\n\n#pragma once\n\n#include \"CloudsVisualSystem.h\"\n#include \"ofxRegex.h\"\n#include <algorithm>\n\n//TODO: rename this to your own visual system\nclass QuineVisualSystem : public CloudsVisualSystem {\nprotected:\n    \n    //  Your Stuff\n    //\n    //When you update the code, set string s = \"%s\";\n    // then copy all the text into a separate text editor\n    // and find/replace the following pairs (in regex mode):\n    // \\\\ -> \\\\\\\\\n    // \\n -> \\\\n\n    // \\t -> \\\\t\n    // \" -> \\\\\"\n    //\\\"(\\w|\\s|\\}|\\{|\\)|\\(|;|\\\\|\\\"|(\\\\\\n)|\\n|\\t|%|:|/|,|\\.|#)*\\\"\n    string matchString = \"\\\\\\\".*?[^\\\\\\\\]\\\\\\\"\";\n#define NUM_MATCH 5\n    string toMatch[NUM_MATCH] = {\n        //comment\n        \"^\\\\s*//.*\",\n        //strings\n        matchString,\n        //base types\n        \"(int|string|bool|void|float|char|vector)\",\n        //preprocessors\n        \"#\\\\w+\",\n        //keywords\n        \"(class|public|protected|private|const|for|while)\"};\n    string output[NUM_MATCH];\n    int matchColors[NUM_MATCH*3] = {\n        0, 255, 0,\n        255, 0, 0,\n        0, 0, 255,\n        135, 96, 82,\n        222, 69, 219\n    };\n    \n    \n    string s = \"%s\";\n    string sG;\n    void myReplace(string& str, const string& oldStr, const string& newStr)\n    {\n        size_t pos = 0;\n        while((pos = str.find(oldStr, pos)) != string::npos)\n        {\n            str.replace(pos, oldStr.length(), newStr);\n            pos += newStr.length();\n        }\n    }\n    \n  public:\n    \n\t//TODO: Change this to the name of your visual system\n\t//This determines your data path so name it at first!\n\t//ie getVisualSystemDataPath() uses this\n    string getSystemName(){\n\t\treturn \"QuineVisualSystem\";\n\t}\n\n\t//These methods let us add custom GUI parameters and respond to their events\n    void selfSetupGui(){}\n    void selfGuiEvent(ofxUIEventArgs &e){}\n\n    \n\t//Use system gui for global or logical settings, for exmpl\n    void selfSetupSystemGui(){}\n    void guiSystemEvent(ofxUIEventArgs &e){}\n    \n\t//use render gui for display settings, like changing colors\n    void selfSetupRenderGui(){}\n    void guiRenderEvent(ofxUIEventArgs &e){}\n    \n    vector<string> &split(const string &s, char delim, vector<string> &elems) {\n        stringstream ss(s);\n        string item;\n        while (getline(ss, item, delim)) {\n            elems.push_back(item);\n        }\n        return elems;\n    }\n    \n    \n    vector<string> split(const string &s, char delim) {\n        vector<string> elems;\n        split(s, delim, elems);\n        return elems;\n    }\n    \n    int indexOf(string s, string f, int startIndex){\n        int fI = 0;\n        for(int i = startIndex; i < s.length(); i++){\n            //if the curr string char == the curr match char\n            if (s[i] == f[fI]){\n                fI++;\n                //if we have matched all the match chars\n                if (fI >= f.length()){\n                    //return where we started matching at\n                    return i - (fI - 1);\n                }\n            } else if (fI > 0){\n                //if we have already matched a few chars,\n                //lets re-start attempting to match from\n                //the next index after the first matched char.\n                i -= (fI - 1);\n                fI = 0;\n            }\n        }\n        //we didn't find a match\n        return -1;\n    }\n    \n\n\t// selfSetup is called when the visual system is first instantiated\n\t// This will be called during a \"loading\" screen, so any big images or\n\t// geometry should be loaded here\n    void selfSetup(){\n        for(int i = 0; i < NUM_MATCH; i++){\n            output[i] = \"\";\n        }\n        char buff[s.length()*3];\n        \n        string replaced = s;\n        myReplace(replaced, \"\\n\", \"\\\\n\");\n        myReplace(replaced, \"\\\"\", \"\\\\\\\"\");\n        myReplace(replaced, \"\\t\", \"\\\\t\");\n        sprintf(buff, s.c_str(), replaced.c_str());\n        s = buff;\n        vector<string> lines = ofSplitString(s, \"\\n\");\n        ofxRegex r;\n        int startIndex;\n        int matchLen;\n        int lastIndex;\n        s = \"\";\n        sG = \"\";\n        for(int lI = 0; lI < lines.size(); lI++){\n            //for each line\n            string line = lines[lI];\n            for(int mI = 0; mI < NUM_MATCH; mI++){\n                vector <string> matched = r.getMatchedStrings(line, toMatch[mI]);\n                //reset the \"search from\" index\n                lastIndex = 0;\n                string cLine = \"\";\n            \n                for (vector<string>::iterator it = matched.begin(); it != matched.end(); ++it){\n                    //for each match\n                    //find where the match is in the rest of the current line\n                    startIndex = indexOf(line, *it, lastIndex);\n                    if (startIndex < 0){\n                        cout<<\"BIG PROBLEM\"<<endl;\n                    }\n                    //in the colored output, insert spaces between last match and current match\n                    for(int i = lastIndex; i < startIndex; i++){\n                        cLine += \" \";\n                    }\n                    matchLen = it->length();\n                    //move the matched text to the colored output\n                    //and replace it with spaces in the source\n                    for(int i = 0; i < matchLen; i++){\n                        cLine += line[startIndex+i];\n                        line.replace(startIndex + i, 1, \" \");\n                    }\n                    lastIndex = startIndex + matchLen;\n                }\n                output[mI] += cLine + \"\\n\";\n            }\n            s += line + \"\\n\";\n        }\n        \n//        matched = r.getMatchedStrings(s, matchString);\n//        cout << \"STRINGS\" << endl;\n//        for (vector<string>::iterator it = matched.begin() ; it != matched.end(); ++it)\n//            cout << \"STRING:\" << *it << endl;\n    }\n\n\t// selfBegin is called when the system is ready to be shown\n\t// this is a good time to prepare for transitions\n\t// but try to keep it light weight as to not cause stuttering\n    void selfBegin(){}\n\n\t// selfPresetLoaded is called whenever a new preset is triggered\n\t// it'll be called right before selfBegin() and you may wish to\n\t// refresh anything that a preset may offset, such as stored colors or particles\n\tvoid selfPresetLoaded(string presetPath){}\n    \n\t//do things like ofRotate/ofTranslate here\n\t//any type of transformation that doesn't have to do with the camera\n    void selfSceneTransformation(){}\n\t\n\t//normal update call\n\tvoid selfUpdate(){}\n\n\t// selfDraw draws in 3D using the default ofEasyCamera\n\t// you can change the camera by returning getCameraRef()\n    void selfDraw(){\n        ofSetColor(255);\n        ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);\n        ofDrawBitmapString(s, 0, 0);\n        int c = 0;\n        for(int i = 0; i < NUM_MATCH; i++){\n            ofSetColor(matchColors[c], matchColors[c+1], matchColors[c+2]);\n            c+=3;\n            ofDrawBitmapString(output[i], 0, 0);\n        }\n    }\n\t\n    // draw any debug stuff here\n\tvoid selfDrawDebug(){}\n\n\t//draws through RGBD camera;\n//\tvoid selfDrawRGBD();\n\t\n\t// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera\n\tvoid selfDrawBackground(){}\n\n\t// this is called when your system is no longer drawing.\n\t// Right after this selfUpdate() and selfDraw() won't be called any more\n\tvoid selfEnd(){}\n\n\t// this is called when you should clear all the memory and delet anything you made in setup\n    void selfExit(){}\n\n\t//events are called when the system is active\n\t//Feel free to make things interactive for you, and for the user!\n    void selfKeyPressed(ofKeyEventArgs & args){}\n    void selfKeyReleased(ofKeyEventArgs & args){}\n    \n    void selfMouseDragged(ofMouseEventArgs& data){}\n    void selfMouseMoved(ofMouseEventArgs& data){}\n    void selfMousePressed(ofMouseEventArgs& data){}\n    void selfMouseReleased(ofMouseEventArgs& data){}\n\t\n\n    // if you use a custom camera to fly through the scene\n\t// you must implement this method for the transitions to work properly\n//\tofCamera& getCameraRef(){\n//\t\treturn myCustomCamera;\n//\t}\n\t\n\n};\n";
    string sG;
    void myReplace(string& str, const string& oldStr, const string& newStr)
    {
        size_t pos = 0;
        while((pos = str.find(oldStr, pos)) != string::npos)
        {
            str.replace(pos, oldStr.length(), newStr);
            pos += newStr.length();
        }
    }
    
  public:
    
	//TODO: Change this to the name of your visual system
	//This determines your data path so name it at first!
	//ie getVisualSystemDataPath() uses this
    string getSystemName(){
		return "QuineVisualSystem";
	}

	//These methods let us add custom GUI parameters and respond to their events
    void selfSetupGui(){}
    void selfGuiEvent(ofxUIEventArgs &e){}

    
	//Use system gui for global or logical settings, for exmpl
    void selfSetupSystemGui(){}
    void guiSystemEvent(ofxUIEventArgs &e){}
    
	//use render gui for display settings, like changing colors
    void selfSetupRenderGui(){}
    void guiRenderEvent(ofxUIEventArgs &e){}
    
    vector<string> &split(const string &s, char delim, vector<string> &elems) {
        stringstream ss(s);
        string item;
        while (getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }
    
    
    vector<string> split(const string &s, char delim) {
        vector<string> elems;
        split(s, delim, elems);
        return elems;
    }
    
    int indexOf(string s, string f, int startIndex){
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
            } else if (fI > 0){
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
    

	// selfSetup is called when the visual system is first instantiated
	// This will be called during a "loading" screen, so any big images or
	// geometry should be loaded here
    void selfSetup(){
        for(int i = 0; i < NUM_MATCH; i++){
            output[i] = "";
        }
        char buff[s.length()*3];
        
        string replaced = s;
        myReplace(replaced, "\n", "\\n");
        myReplace(replaced, "\"", "\\\"");
        myReplace(replaced, "\t", "\\t");
        sprintf(buff, s.c_str(), replaced.c_str());
        s = buff;
        vector<string> lines = ofSplitString(s, "\n");
        ofxRegex r;
        int startIndex;
        int matchLen;
        int lastIndex;
        s = "";
        sG = "";
        for(int lI = 0; lI < lines.size(); lI++){
            //for each line
            string line = lines[lI];
            for(int mI = 0; mI < NUM_MATCH; mI++){
                vector <string> matched = r.getMatchedStrings(line, toMatch[mI]);
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
                output[mI] += cLine + "\n";
            }
            s += line + "\n";
        }
        
//        matched = r.getMatchedStrings(s, matchString);
//        cout << "STRINGS" << endl;
//        for (vector<string>::iterator it = matched.begin() ; it != matched.end(); ++it)
//            cout << "STRING:" << *it << endl;
    }

	// selfBegin is called when the system is ready to be shown
	// this is a good time to prepare for transitions
	// but try to keep it light weight as to not cause stuttering
    void selfBegin(){}

	// selfPresetLoaded is called whenever a new preset is triggered
	// it'll be called right before selfBegin() and you may wish to
	// refresh anything that a preset may offset, such as stored colors or particles
	void selfPresetLoaded(string presetPath){}
    
	//do things like ofRotate/ofTranslate here
	//any type of transformation that doesn't have to do with the camera
    void selfSceneTransformation(){}
	
	//normal update call
	void selfUpdate(){}

	// selfDraw draws in 3D using the default ofEasyCamera
	// you can change the camera by returning getCameraRef()
    void selfDraw(){
        ofSetColor(255);
        ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
        ofDrawBitmapString(s, 0, 0);
        int c = 0;
        for(int i = 0; i < NUM_MATCH; i++){
            ofSetColor(matchColors[c], matchColors[c+1], matchColors[c+2]);
            c+=3;
            ofDrawBitmapString(output[i], 0, 0);
        }
    }
	
    // draw any debug stuff here
	void selfDrawDebug(){}

	//draws through RGBD camera;
//	void selfDrawRGBD();
	
	// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
	void selfDrawBackground(){}

	// this is called when your system is no longer drawing.
	// Right after this selfUpdate() and selfDraw() won't be called any more
	void selfEnd(){}

	// this is called when you should clear all the memory and delet anything you made in setup
    void selfExit(){}

	//events are called when the system is active
	//Feel free to make things interactive for you, and for the user!
    void selfKeyPressed(ofKeyEventArgs & args){}
    void selfKeyReleased(ofKeyEventArgs & args){}
    
    void selfMouseDragged(ofMouseEventArgs& data){}
    void selfMouseMoved(ofMouseEventArgs& data){}
    void selfMousePressed(ofMouseEventArgs& data){}
    void selfMouseReleased(ofMouseEventArgs& data){}
	

    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
//	ofCamera& getCameraRef(){
//		return myCustomCamera;
//	}
	

};
