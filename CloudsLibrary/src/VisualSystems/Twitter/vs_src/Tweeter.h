//
//  Tweeter.h
//  Twitter
//
//  Created by Surya Mattu on 23/11/13.
//
//

#pragma once

#include "ofMain.h"

struct Date{
    int day;
    int month;
    int year;    
};

class Tweet 
{
  public:
    string tweet;
    Date tweetDate;
    string dateString;
    vector<string> hashtags;
    vector<string> mentionedUsers;  
};

class Tweeter
{
  public:
    Tweeter();
    Tweeter(const string& _name, int _id);
    ~Tweeter();

    int ID;
    string name; //twitter handle: like @kcimc
    string fullName; //real name: like Kyle McDonald
    
    ///// draw commands
    ofVec3f position;
    ofRectangle screenRectangle;
    bool onScreen;
    bool hovered;
    float hoverChangedTime;
    float attenuation;
    ofMatrix4x4 billboardMat;
    /////////
    
    int nodeVertexIndex;
    int edgeVertexIndex;
	int refreshNum;
    float textDecayRate;
	
    bool hasTweetOnDate(const string& tweetDate);
    vector<Tweet*>& getTweetsByDate(const string& t);

    string getDateAsString(const Date& d);
    Date getDateFromString(const string& dString);
    void addTweetsToDate(Tweet* t);
    string tweetDate;

    vector<Tweet*> tweets;
    map<string, vector<Tweet*> > tweetDateMap;

    vector<Tweet*> dummyTweets;
    vector<string> userLinks;
    vector<int> linksById;
	ofVec2f activityMapCoord;

};

