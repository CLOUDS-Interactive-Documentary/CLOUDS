//
//  Tweeter.cpp
//  Twitter
//
//  Created by Surya Mattu on 23/11/13.
//
//

#include "Tweeter.h"

Tweeter::Tweeter() {
    name = " ";
    ID = -1 ;
    position = ofVec3f(-1, -1,-1);
}

Tweeter::Tweeter(const string& _name, int _id){
    name = _name;
    ID = _id;
    position = ofVec3f(-1, -1,-1);
}

Tweeter::~Tweeter(){

	for(int i = 0; i < tweets.size(); i++){
		delete tweets[i];
	}

	tweetDateMap.clear();
}

vector<Tweet*>& Tweeter::getTweetsByDate(const string& t){
    if(tweetDateMap.find(t) != tweetDateMap.end()){
		return tweetDateMap[t];
    }    
    return dummyTweets;   
}

void Tweeter::addTweetsToDate(Tweet* t){
    tweetDateMap[ t->dateString ].push_back( t );
}

bool Tweeter::hasTweetOnDate(const string& tweetDate){
    return tweetDateMap.find(tweetDate) != tweetDateMap.end();
}

Date Tweeter::getDateFromString(const string& dString){
    vector<string> ds = ofSplitString(dString, " ");
    Date d;
    d.day = ofToInt(ds[0]);
    d.month = ofToInt(ds[1]);
    d.year = ofToInt(ds[2]);
    return d;
}

string Tweeter::getDateAsString(const Date& d){
    string dateString;
    dateString += ofToString(d.day)   + " - ";
    dateString += ofToString(d.month) + " - ";
    dateString += ofToString(d.year);
    return dateString;
}
