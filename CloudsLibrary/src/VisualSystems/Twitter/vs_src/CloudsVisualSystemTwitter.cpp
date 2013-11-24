//
//  CloudsVisualSystemTwitter.cpp
//

#include "CloudsVisualSystemTwitter.h"
#include "CloudsRGBDVideoPlayer.h"

#include <Poco/URI.h>

//--------------------------------------------------------------
//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemTwitter::selfSetupGui()
{
	listGui = new ofxUISuperCanvas("SEARCH TERM LIST", gui);
    loadJSONData();
}

void CloudsVisualSystemTwitter::loadJSONData(){
    // Now parse the JSON
    ofFile f = ofFile();
    
    
    ofDirectory dir(getVisualSystemDataPath()+"tweets/");
    dir.listDir();
    if(dir.exists()){
        int size = dir.size();
        vector<ofFile>files= dir.getFiles();
        
        for(int i = 0; i< files.size(); i++){
            string filePath =getVisualSystemDataPath()+"tweets/" +files[i].getFileName();
//            cout<<filePath<<endl;
            bool parsingSuccessful = result.openLocal(filePath);
            if (parsingSuccessful) {
//                cout << result.getRawString() << endl;
                
                if(result.isMember("errors")) {
                    ofDrawBitmapString(result.getRawString(), 10, 14);
                }
                else if(result.isArray()) {
                    int n = 0;
                    ofxJSONElement trends = result[n]["trends"];
                    for(int i = 0; i < trends.size(); i++) {
                        std::string message = trends[i]["query"].asString();
                        ofDrawBitmapString(message, 10, 40*i+40);
                    }
                }
                else if(result.isMember("Tweets")){
                    
                    Tweeter cur;
                    vector<Tweet> userTweets;
                    ofxJSONElement tweets = result["Tweets"];

                    cur.name = result["name"].asString();
                    cout<<cur.name<<" has "<<tweets.size()<<" tweets"<<endl;
                    for(int i =0; i<tweets.size(); i ++){
                        
                        Tweet t;

                        t.tweet = tweets[i]["Tweet"].asString();

                        cout<<t.tweet<<endl;
                        if(tweets[i]["Hashtag"].isValidIndex(0)){
                            ofxJSONElement hashTags = tweets[i]["Hashtag"];
                            
                            for(int j=0; j<hashTags.size(); j++){
                                t.hashtags.push_back(hashTags[j].asString());
                            }
                            
                            
                        }
                        
                        if(tweets[i]["Users"].isValidIndex(0)){
                            ofxJSONElement users = tweets[i]["Users"];
                            
                            for(int k=0; k<users.size(); k++){
                                t.mentionedUsers.push_back(users[k].asString());
                            }
                            
                            
                        }
                        
                        if(tweets[i].isMember("Date")){
                            ofxJSONElement date = tweets[i]["Date"];
                            t.tweetDate.day = date[0].asFloat();
                            t.tweetDate.month =date[1].asFloat();
                            t.tweetDate.year =date[2].asFloat();
                            
                            cout<<t.tweetDate.day<< " - "<<t.tweetDate.month <<" - "<<t.tweetDate.year<<endl;

                        }
                        
                        userTweets.push_back(t);

                    }
                        cur.tweets= userTweets;
                        tweeters.push_back(cur);
                }
                
                
            } else {
                cout  << "Failed to parse JSON" << endl;
            }
        }
        
    }
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::selfGuiEvent(ofxUIEventArgs &e)
{
    
}
//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemTwitter::selfSetupSystemGui(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemTwitter::selfSetupRenderGui(){
    
}

void CloudsVisualSystemTwitter::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemTwitter::selfSetup()
{
    
}


//--------------------------------------------------------------
// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemTwitter::selfPresetLoaded(string presetPath)
{
	
}

//--------------------------------------------------------------
// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemTwitter::selfBegin()
{
    
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemTwitter::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
//normal update call
void CloudsVisualSystemTwitter::selfUpdate()
{
    
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemTwitter::selfDraw()
{
    
}

// draw any debug stuff here
void CloudsVisualSystemTwitter::selfDrawDebug()
{
	
}

//--------------------------------------------------------------
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemTwitter::selfDrawBackground()
{
    
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemTwitter::selfEnd()
{
    
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemTwitter::selfExit()
{
    
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemTwitter::selfKeyPressed(ofKeyEventArgs & args){
	for(int i=0; i<tweeters.size(); i++){
        cout<<tweeters[i].name<<" : "<<tweeters[i].tweets.size()<<endl;
        
        for(int j=0; j<tweeters[i].tweets.size(); j++){
            
            cout<<tweeters[i].tweets[j].tweet<<" , "<<tweeters[i].tweets[j].hashtags.size()<<" , "<<tweeters[i].tweets[j].mentionedUsers.size()<<endl;
        }
        
        
    }
}
void CloudsVisualSystemTwitter::selfKeyReleased(ofKeyEventArgs & args){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemTwitter::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemTwitter::selfMouseReleased(ofMouseEventArgs& data){
	
}
