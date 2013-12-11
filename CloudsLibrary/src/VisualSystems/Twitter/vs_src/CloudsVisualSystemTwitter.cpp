//
//  CloudsVisualSystemTwitter.cpp
//

#include "CloudsVisualSystemTwitter.h"
#include "CloudsRGBDVideoPlayer.h"

#include <Poco/URI.h>
//--------------------------------------------------------------
// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemTwitter::selfSetup()
{
    loadJSONData();
    parseClusterNetwork(getVisualSystemDataPath() +"/twitter.net");
    loadMesh();
    clusterShader.load(getVisualSystemDataPath()+"/shaders/cluster");
    sprite.loadImage(getVisualSystemDataPath()+"/dot.png");
    meshExpansion = 100;
    pointSize =10;
}

//--------------------------------------------------------------
// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemTwitter::selfBegin()
{
    
}

//--------------------------------------------------------------
//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemTwitter::selfSetupGui()
{
	clusterGui = new ofxUISuperCanvas("CLUSTER PARAMS", gui);
    clusterGui->copyCanvasStyle(gui);
	clusterGui->copyCanvasProperties(gui);
	clusterGui->setName("Custom");
	clusterGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    clusterGui->addSlider("mesh expansion", 100, 10000, &meshExpansion);
	clusterGui->addSlider("point size", 1, 50, &pointSize);
	ofAddListener(clusterGui->newGUIEvent, this, &CloudsVisualSystemTwitter::selfGuiEvent);
	guis.push_back(clusterGui);
	guimap[clusterGui->getName()] = clusterGui;
    
    
    

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

            bool parsingSuccessful = result.openLocal(filePath);
            if (parsingSuccessful) {
                
                if(result.isMember("errors")) {
                    ofDrawBitmapString(result.getRawString(), 10, 14);
                }
                else if(result.isMember("Tweets")){

                    Tweeter cur;
                    vector<Tweet> userTweets;
                    ofxJSONElement tweets = result["Tweets"];

                     vector<string> names = ofSplitString(result["name"].asString(), ".")    ;
                    cur.name = "@" + names[0];
                    cur.ID = i;
//                      <<cur.name<<" has "<<tweets.size()<<" tweets"<<endl;
                    
                    for(int j =0; j<tweets.size(); j ++){
                        Tweet t;
                        t.tweet = tweets[j]["Tweet"].asString();
                        
                        if(tweets[j]["Hashtag"].isValidIndex(0)){
                            ofxJSONElement hashTags = tweets[j]["Hashtag"];
                            
                            for(int k=0; k<hashTags.size(); k++){
                                t.hashtags.push_back(hashTags[k ].asString());
                            }
                        }
                        
                        if(tweets[j]["Users"].isValidIndex(0)){
                            ofxJSONElement users = tweets[j]["Users"];
                            
                            for(int k=0; k<users.size(); k++){
                                
                                if( !ofContains(cur.userLinks, users[k].asString())){
                                    
                                    cur.userLinks.push_back(users[k].asString());

                                }
                            }
                        }
                        if(tweets[j].isMember("Date")){
                            ofxJSONElement date = tweets[j]["Date"];
                            t.tweetDate.day = date["Day"].asInt()   ;
                            t.tweetDate.month =date["Month"].asInt();
                            t.tweetDate.year =date["Year"].asInt();
                        }
                        
                        userTweets.push_back(t);

                    }
                    cur.tweets= userTweets;
//                   / cout<<cur.name<<endl;
                    tweeters.push_back(cur);
                } else {
                    cout  << "Failed to parse JSON" << endl;
                }
            }
            
        }
    }
    addUsersFromMentions();
}

void CloudsVisualSystemTwitter::parseClusterNetwork(string fileName){
	ofBuffer pajekFile = ofBufferFromFile(fileName);
	bool findingNodes = false;
    bool findingEdges = false;;
	while(!pajekFile.isLastLine()){
		string line = pajekFile.getNextLine();
		
		if(line == "" || line.at(0) == '%'){
			continue;
		}
		
		if (line.find("*Vertices") != string::npos ) {
			findingNodes = true;
			continue;
            
		}
		if (line.find("*Edgeslist") != string::npos ) {
			findingNodes = false;
            findingEdges = true;
            continue;
		}
		
		if(findingNodes){
			vector<string> components = ofSplitString(line, " ");

            int id = ofToInt(components[0]);

            Tweeter& tweeter = getTweeterByID(tweeters, id);
            
			int numcomp = components.size();
            max = ofVec3f(0,0,0);

            cout << "twitter.name   " <<  tweeter.name << endl;
            
			tweeter.position = ofVec3f(ofToFloat(components[2]),
										   ofToFloat(components[3]),
										   ofToFloat(components[4])*10);
		}
        
        
        
        if(findingEdges){
			vector<string> components = ofSplitString(line, " ");
            int id = ofToInt(components[0]);
            //428 4 8 9 11 15 17 18

            Tweeter& tweeter = getTweeterByID(tweeters, id);
//            cout<<tweeter.name<<"  has "<<" "<<tweeter.ID<<"  "<<(components.size()-2)<<" links."<<endl;
            for(int i =1; i< components.size()-1; i++){
                if(tweeter.ID != ofToInt(components[i]) ){
                    tweeter.linksById.push_back(ofToInt(components[i]));
                }
                else{
                    cout<<"Error! "<<tweeter.name<<"  : "<<tweeter.ID<<" index "<< ofToInt(components[i])<<endl;
                }
//                cout<<i<<" : "<<components[i]<<endl;
            }
        }
        
	}
}

void CloudsVisualSystemTwitter::loadMesh(){
//    ofVec3f centroid;
//    
//    for(int i=0; tweeters.size(); i++){
//        mesh.setMode(OF_PRIMITIVE_POINTS);
//        mesh.setupIndicesAuto();
//        mesh.addVertex(tweeters[i].position);
//        
//        max  =ofVec3f(MAX(tweeters[i].position.x,max.x),
//                      MAX(tweeters[i].position.y,max.y),
//                      MAX(tweeters[i].position.z,max.z));
//        
//        centroid += tweeters[i].position;
//    }
    
//    centroid /= tweeters.size();
    
    for(int j=0; j<tweeters.size(); j++){
        
        for (int k=0; k<tweeters[j].linksById.size(); k++) {
            tweeters[j].linksById[k];

            if (links.find(make_pair(tweeters[j].ID, tweeters[j].linksById[k])) == links.end() &&
                links.find(make_pair( tweeters[j].linksById[k],tweeters[j].ID)) == links.end() ) {
                
                linksMesh.addVertex(tweeters[j].position);
                linksMesh.addNormal(ofVec3f(0,0,0));
            
                Tweeter& t  = getTweeterByID(tweeters, tweeters[j].linksById[k]);
                linksMesh.addVertex(t.position);
                linksMesh.addNormal(ofVec3f(0,0,0));
                links.insert(make_pair(tweeters[j].ID, tweeters[j].linksById[k]));
            }
            else{
//                cout<<"Link already exists between "<<tweeters[j].ID<<" and "<<tweeters[j].linksById[k]<<endl;
            }
        }
        linksMesh.setMode(OF_PRIMITIVE_LINES);
    }

}
void CloudsVisualSystemTwitter::addUsersFromMentions(){
    
    vector<string> names;
    
    for (int i= 0; i < tweeters.size(); i++) {
        names.push_back(tweeters[i].name);
    }
    
    for (int i= 0; i < tweeters.size(); i++) {
        for(int j=0; j<tweeters[i].userLinks.size(); j++){
            
            if(! ofContains(names, tweeters[i].userLinks[j])){
                numberOfMentions[tweeters[i].userLinks[j]] ++;
                
            }
        
        }
    }
    
    map<string,int>::iterator it;
    for(it = numberOfMentions.begin() ; it != numberOfMentions.end() ; it++){
//        cout<<it->first <<" :  "<<it->second<<endl;
        
        if(it->second > 3){
            Tweeter t = Tweeter(it->first, tweeters.size());
//            cout<<"adding "<<t.name<<"  to the list as they have been mentioned more than twice "<<endl;
            tweeters.push_back(t);
        }
    }
    
}


void CloudsVisualSystemTwitter::createPajekNetwork(){
    stringstream ss;
    cout<<"Creating paejk file"<<endl;
    ss<<"*Vertices "<<tweeters.size()<<endl;
    
    for (int i= 0; i < tweeters.size(); i++) {
        ss<<tweeters[i].ID<<"  \""<<tweeters[i].name<<"\""<<endl;

        
    }
    
    ss<<"*EdgesList "<<tweeters.size()<<endl;
    for(int j =0; j<tweeters.size(); j++){
        
        string edges;
        if(tweeters[j].userLinks.size() > 0){
            for(int k=0; k< tweeters[j].userLinks.size(); k++){

                if(getUserIdByName(tweeters[j].userLinks[k]) != -1){
                    edges += ofToString(getUserIdByName(tweeters[j].userLinks[k])) +  " ";
                }
            }
            ss<<tweeters[j].ID<<" "<<edges<<endl;
        }

    }
    ofBuffer b = ofBuffer(ss);
    ofBufferToFile(getVisualSystemDataPath() + "/twitter.net",b);
}

int CloudsVisualSystemTwitter:: getUserIdByName(string name){
    
    for (int i=0; i<tweeters.size(); i++) {
        if(tweeters[i].name == name){
            return tweeters[i].ID;
        }
    }
    return -1;
}

Tweeter& CloudsVisualSystemTwitter::getTweeterByID(vector<Tweeter>& tweeters, int _id ){
    
    for(int i=0; i< tweeters.size(); i++){
        
        if(tweeters[i].ID == _id){            
            return tweeters[i];
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
// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemTwitter::selfPresetLoaded(string presetPath)
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
    glDisable(GL_DEPTH_TEST);
    
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    ofScale(10, 10);
    clusterShader.begin();
	clusterShader.setUniformTexture("tex", sprite, 0);
	clusterShader.setUniform1f("expansion", meshExpansion);
	clusterShader.setUniform1f("minSize", pointSize);
	clusterShader.setUniform3f("attractor", 0, 0, 0);
	clusterShader.setUniform1f("radius", 300.);
	
	ofEnablePointSprites();
	ofDisableArbTex();
	

    linksMesh.drawVertices();
//	clusterShader.end();
	ofDisablePointSprites();
//	ofEnableArbTex();
	clusterShader.end();
//    linksMesh.drawWireframe();


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
