//
//  Tweeter.h
//  Twitter
//
//  Created by Surya Mattu on 23/11/13.
//
//

#ifndef __Twitter__Tweeter__
#define __Twitter__Tweeter__

#include "ofMain.h"

struct Date{
    int day;
    int month;
    int year;
    
};

struct Tweet{
    string tweet;
    Date tweetDate;
    vector<string> hashtags;
    vector<string> mentionedUsers;
};

class Tweeter{
public:
    Tweeter();
    Tweeter(string _name, int _id);
    string name;
    int ID;
    ofVec3f position;
    int nodeVertexIndex;
    int edgeVertexIndex;
    
    vector<Tweet> getTweetsByDate(Date d);
    bool hasTweetOnDate(Date d);
    vector<Tweet> tweets;
    vector<string> userLinks;
    vector<int> linksById;

};

#endif /* defined(__Twitter__Tweeter__) */
