//
//  Tweeter.cpp
//  Twitter
//
//  Created by Surya Mattu on 23/11/13.
//
//

#include "Tweeter.h"

Tweeter::Tweeter(){
    name = " ";
    ID = -1 ;
    position = ofVec3f(-1, -1,-1);
}

Tweeter::Tweeter(string _name, int _id){
    name = _name;
    ID = _id;
    position = ofVec3f(-1, -1,-1);
}

vector<Tweet>& Tweeter::getTweetsByDate(string t){
//    if(tweetDateMap.find(t) != tweetDateMap.end()){
            return tweetDateMap[t];
//    }
    
//    return dummyTweets;
    
}

vector<Tweet> Tweeter::sortTweetsByDate(Date d){
    vector<Tweet> dateTweets;
    for(int i =0; i < tweets.size(); i++){
        if(tweets[i].tweetDate.day == d.day &&
           tweets[i].tweetDate.month == d.month &&
           tweets[i].tweetDate.year== d.year){
            dateTweets.push_back(tweets[i]);
        }
    }
    
    return dateTweets;
}

void Tweeter::indexTweetsByDate(vector<Date> dates){
    
    for(int i=0; i<dates.size(); i++){
        vector<Tweet> t = sortTweetsByDate(dates[i]);
        if(t.size() > 0){
        tweetDateMap[getDateAsString(dates[i])] = t;
        }
    cout<<"Sorting tweets for : "<<name<<" on date " <<getDateAsString(dates[i])<<endl;
    }

}

void Tweeter::addTweetsToDate(Tweet t){
    tweetDateMap[t.dateString].push_back(t);
}
bool Tweeter::hasTweetOnDate(string tweetDate){
    
    if(tweetDateMap.find(tweetDate) != tweetDateMap.end()){
        return true;
    }
    
    return false;
}

string Tweeter::getDateAsString(Date d){
    string dateString;
    dateString += ofToString(d.day) + " - ";
    dateString += ofToString(d.month) + " - ";
    dateString += ofToString(d.year);
    return dateString;
}




