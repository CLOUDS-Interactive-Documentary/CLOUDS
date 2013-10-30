//
//  TUText.h
//  OpenP5TextUniverse
//
//  Created by Elie Zananiri on 2013-10-27.
//
//

#pragma once

#include "ofMain.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class TUSentence
{
    public:
        TUSentence(string str)
        {
            this->str = str;
            
            ofStringReplace(str, ",", " ");
            ofStringReplace(str, ";", " ");
            ofStringReplace(str, ":", " ");
            words = ofSplitString(str, " ", true, true);
        }
    
        void print()
        {
            cout << "    >> SENTENCE: " << str << endl;
            for (int i = 0; i < words.size(); i++) {
                cout << "      >> " << words[i] << endl;
            }
        }
    
        string str;
        vector<string> words;
};

//--------------------------------------------------------------
//--------------------------------------------------------------
class TUParagraph
{
    public:
        TUParagraph(string str)
        {
            this->str = str;
            
            ofStringReplace(str, "?", ".");
            ofStringReplace(str, "!", ".");
            vector<string> results = ofSplitString(str, ".", true, true);
            for (int i = 0; i < results.size(); i++) {
                sentences.push_back(TUSentence(results[i]));
            }
        }
    
        void print()
        {
            cout << "  >> PARAGRAPH: " << str << endl;
            for (int i = 0; i < sentences.size(); i++) {
                sentences[i].print();
            }
        }
    
        string str;
        vector<TUSentence> sentences;
};

//--------------------------------------------------------------
//--------------------------------------------------------------
class TUText
{
    public:
        TUText(string str)
        {
            this->str = str;
            
            vector<string> results = ofSplitString(str, "\n", true, true);
            for (int i = 0; i < results.size(); i++) {
                paragraphs.push_back(TUParagraph(results[i]));
            }
            
        }
    
        void print()
        {
            cout << ">> TEXT: " << str << endl;
            for (int i = 0; i < paragraphs.size(); i++) {
                paragraphs[i].print();
            }
        }

        string str;
        vector<TUParagraph> paragraphs;
};
