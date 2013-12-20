//
//  CloudsVisualSystemTwitter.cpp
//

#include "CloudsVisualSystemTwitter.h"
//#include "CloudsRGBDVideoPlayer.h"

//#include <Poco/URI.h>

bool dateSorter(Date const& lhs, Date const& rhs) {
    if (lhs.year != rhs.year)
        return lhs.year < rhs.year;
    if (lhs.month != rhs.month)
        return lhs.month < rhs.month;
    return lhs.day < rhs.day;
}

void CloudsVisualSystemTwitter::selfSetDefaults(){
    // fix default colors
    //    nodeModifier.r = 1.0;
    //    nodeModifier.g = 0.65;
    //    nodeModifier.b = 0.54;
    //    nodeModifier.a = 1.0;
    //
    //    nodeActiveModifier.r = 1.0;
    //    nodeActiveModifier.g = 0.65;
    //    nodeActiveModifier.b = 0.54;
    //    nodeActiveModifier.a = 1.0;
    //
    //    nodeMidpointModifier.r = 0.0;
    //    nodeMidpointModifier.g = 1.;
    //    nodeMidpointModifier.b = 1.;
    //    nodeMidpointModifier.a = 1.0;
    //
    //    nodeActiveMidpointModifier.r = 0.5f;
    //    nodeActiveMidpointModifier.g = 0.65;
    //    nodeActiveMidpointModifier.b = 0.54;
    //    nodeActiveMidpointModifier.a = 1.0;
    //
    //    tweetModifier.r = 1.0;
    //    tweetModifier.g = 0.65;
    //    tweetModifier.b = 0.54;
    //    tweetAlpha = 1.0;
    //
    //    textColorModifier.r = 0.0;
    //    textColorModifier.g = 1.0;
    //    textColorModifier.b = 1.0;
    //    textColorModifier.a = 1.0;
    
    //    nodeColor.setHsb(nodeModifier.r, nodeModifier.g, nodeModifier.b,nodeModifier.a);
    //    nodeActiveColor.setHsb(nodeActiveModifier.r, nodeActiveModifier.g, nodeActiveModifier.b,nodeActiveModifier.a);
    //    nodeMidpointColor.setHsb(nodeMidpointColor.r, nodeMidpointColor.g, nodeMidpointColor.b,nodeMidpointColor.a);
    //    nodeActiveMidpointColor.setHsb(nodeActiveMidpointColor.r, nodeActiveMidpointColor.g, nodeActiveMidpointColor.b,nodeMidpointColor.a);
    //    textColor.setHsb(textColorModifier.r, textColorModifier.g, textColorModifier.b,textColorModifier.a);
    
    //    baseModifier.r = 0.62;
    //    baseModifier.g = 0.46;
    //    baseModifier.b = 0.90;
    //    baseAlpha = 0.01;
    //
    //    baseColor  = ofFloatColor(0.0,0.0,1.0,0.1);
    //    tweetColor = ofFloatColor(1.0,0.0,0.0,1.0);
    
    refreshRate = 1000;
    edgeDecayRate = 0.8;
    meshExpansion = 100;
    pointSize =10;
    
    dateIndexMin = 0;
    dateIndexMax = 100;
    
    rotation = 0;
    currentTweetFeedIndex = 0;
    
    avatarTweetGap = 50;
    heightOffset = 50;
    textHeightOffset = 10;
    
    timeTillNextUpdate = 0;
    minTimeGapForNextTweet =3;
    randomRangeMin = 1;
    randomRangeMax = 10;
    //    tweetModifier.r = 1.0;
    //    tweetModifier.g = 0.65;
    //    tweetModifier.b = 0.54;
    //    tweetAlpha = 1.0;
    //
    //    baseModifier.r = 0.62;
    //    baseModifier.g = 0.46;
    //    baseModifier.b = 0.90;
    //    baseAlpha = 0.01;
    
    xScale = 100;
    yScale = 100;
    zScale = 100;
    rotateModel = false;
    bRenderMesh = true;
    bRenderText = false;
    stringWidth = 10;
    tweetFeedRect = ofRectangle (0, 0,  ofGetWidth()/2, ofGetHeight());
    font.loadFont(getVisualSystemDataPath() + "fonts/NewMedia Fett.ttf",5);
    font2.loadFont(getVisualSystemDataPath() + "fonts/NewMedia Fett.ttf",3);
    ofEnableSmoothing();
    
    
}

void CloudsVisualSystemTwitter::selfSetup()
{
    /*  Use this to create new network for graphinsight
     
     //Minimum num of users to add to tweeter links.
     minUserMentions = 0;
     
     createNewGraph("twitterNewData0Men.net","tweets_");
     cout<<"created new network"<<endl;
     while(1);
     */
    initSystem(getVisualSystemDataPath() +"graphs/NotSimple_Twitter4Men_new.net");
    reloadShaders();
    
}

void CloudsVisualSystemTwitter::selfBegin()
{
    updateLabelWithCurrentMeshName(currentMeshFileName);
}

void CloudsVisualSystemTwitter::addColorToGui(ofxUISuperCanvas* gui,string prefix,ofFloatColor& col, bool doAlpha){
    gui->addSpacer();
    gui->addMinimalSlider(prefix + " HUE", 0.0, 1.0, &col.r);
    gui->addMinimalSlider(prefix + " SAT", 0.0, 1.0, &col.g);
    gui->addMinimalSlider(prefix + " BRI", 0.0, 1.0, &col.b);
	if(doAlpha){
		gui->addMinimalSlider(prefix + " ALPHA", 0.0, 1.0, &col.a);
	}
}

void CloudsVisualSystemTwitter::selfSetupGui()
{
	clusterGui = new ofxUISuperCanvas("MESH PARAMS", gui);
    clusterGui->copyCanvasStyle(gui);
	clusterGui->copyCanvasProperties(gui);
    clusterGui->setName("Mesh");
    clusterGui->addToggle("ANIMATE", &bAnimate);
    clusterGui->addButton("LOAD GRAPH", false);
	clusterGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    clusterGui->addToggle("RENDER MESH", &bRenderMesh);
    clusterGui->addIntSlider("REFRESH RATE", 1, 500, &refreshRate);
    clusterGui->addRangeSlider("DATE RANGE", 1,  (dateIndex.size()), &dateIndexMin, & dateIndexMax);
    clusterGui->addToggle("ROTATE", &rotateModel);
    clusterGui->addMinimalSlider("ROTATION AMT", 0.1, 1, &rotationAmount);
    clusterGui->addLabel("MESH FILE",currentMeshFileName);
    clusterGui->addMinimalSlider("EDGE DECAY", 0.01, 0.9, &edgeDecayRate);
	
	//TWEET POP
	addColorToGui(clusterGui,"LINE NODE BASE",lineNodeBaseHSV);
	addColorToGui(clusterGui,"LINE EDGE BASE",lineEdgeBaseHSV);
	addColorToGui(clusterGui,"LINE NODE POP",lineNodePopHSV);
	addColorToGui(clusterGui,"LINE EDGE POP",lineEdgePopHSV);
	clusterGui->addSlider("EDGE COLOR EXPONENT", 1.0, 5., &edgeInterpolateExponent);
    
	addColorToGui(clusterGui,"NODE BASE",nodeBaseColorHSV);
	addColorToGui(clusterGui,"NODE POP",nodePopColorHSV);
	
    clusterGui->addMinimalSlider("X POS", 1, 500, &xScale);
    clusterGui->addMinimalSlider("Y POS", 1, 500, &yScale);
    clusterGui->addMinimalSlider("Z POS", 1, 500, &zScale);
    clusterGui->addButton("RELOAD MESH", false);
    
    
	ofAddListener(clusterGui->newGUIEvent, this, &CloudsVisualSystemTwitter::selfGuiEvent);
	guis.push_back(clusterGui);
	guimap[clusterGui->getName()] = clusterGui;
    
    textGui = new ofxUISuperCanvas("TEXT PARAMS", gui);
    textGui->copyCanvasStyle(gui);
	textGui->copyCanvasProperties(gui);
    textGui->setName("text");
    textGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    textGui->addToggle("RENDER TEXT", &bRenderText);
    textGui->addSpacer();
    textGui->addMinimalSlider("TEXT HUE", 0.0, 1, &textColorModifier.r);
    textGui->addMinimalSlider("TEXT SAT", 0.0, 1, &textColorModifier.g);
    textGui->addMinimalSlider("TEXT BRI", 0.0, 1, &textColorModifier.b);
    textGui->addMinimalSlider("TEXT ALPHA", 0.0, 1, &textColorModifier.a);
    textGui->addSpacer();
    textGui->addMinimalSlider("STRING WIDTH", 1, 2000, &stringWidth);
    textGui->addMinimalSlider("SET SIZE", 0.1, 100, &fontSize);
    
    ofAddListener(textGui->newGUIEvent, this, &CloudsVisualSystemTwitter::selfGuiEvent);
	guis.push_back(textGui);
	guimap[textGui->getName()] = textGui;
    
    twitterFeedGui = new ofxUISuperCanvas("TWITTER FEED",gui);
    twitterFeedGui->copyCanvasStyle(gui);
    twitterFeedGui->setName("TWITTER FEED");
    twitterFeedGui->addSpacer();
    twitterFeedGui->addToggle("RENDER FEEDS", &bRenderFeed);
    twitterFeedGui->addSpacer();
    twitterFeedGui->addLabel("FEED RECT");
    twitterFeedGui->addMinimalSlider("FEED X", 1, ofGetWidth(), &tweetFeedRect.x);
    twitterFeedGui->addMinimalSlider("FEED Y", 1, ofGetHeight(), &tweetFeedRect.y);
    twitterFeedGui->addMinimalSlider("HEIGHT OFFSET", 1, 100, &heightOffset);
    twitterFeedGui->addMinimalSlider("AVATAR TWEET GAP", 1, 100, &avatarTweetGap);
    twitterFeedGui->addMinimalSlider("TEXT HEIGHT OFFSET", 1, 100, &textHeightOffset);
    twitterFeedGui->addMinimalSlider("GAP BETWEEN UPDATES", 1, 5, &minTimeGapForNextTweet);
    twitterFeedGui->addRangeSlider("RANDOM OFFSET RANGE", 1,  10, &randomRangeMin, & randomRangeMax);
    twitterFeedGui->addIntSlider("NUM TWEETS",1, 50, &numberOfTweets);

    ofAddListener(twitterFeedGui->newGUIEvent, this, &CloudsVisualSystemTwitter::selfGuiEvent);
	guis.push_back(twitterFeedGui);
	guimap[textGui->getName()] = twitterFeedGui;
    
    
    
}

void CloudsVisualSystemTwitter::loadJSONData(string folderName){
    
    ofFile f = ofFile();
    
    ofDirectory dir(getVisualSystemDataPath(true)+folderName+"/");
    dir.listDir();
    if(dir.exists()){
        int size = dir.size();
        vector<ofFile>files= dir.getFiles();
        
        for(int i = 0; i< files.size(); i++){
            string filePath =getVisualSystemDataPath(true)+folderName+"/"+files[i].getFileName();
            
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
                    userNameIdMap[cur.name] = i;
                    
                    for(int j =0; j<tweets.size(); j ++){
                        Tweet t;
                        t.tweet = tweets[j]["Tweet"].asString();
                        
                        if(ofIsStringInString("\\ud83c",t.tweet))
                        {
                            cout<<" Replacing shit"<<endl;
                            ofStringReplace(t.tweet, "\\ud83c", " TEST ");
                        }
                        
                        ofStringReplace(t.tweet, "ud83d", " TEST ");
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
                                    t.mentionedUsers.push_back(users[k].asString());
                                    cur.userLinks.push_back(users[k].asString());
                                }
                            }
                        }
                        if(tweets[j].isMember("Date")){
                            bool alreadyExists = false;
                            ofxJSONElement date = tweets[j]["Date"];
                            t.tweetDate.day = date["Day"].asInt()   ;
                            t.tweetDate.month =date["Month"].asInt();
                            t.tweetDate.year =date["Year"].asInt();
                            t.dateString = getDateAsString(t.tweetDate);
                            cur.addTweetsToDate(t);
                            for(int i=0; i<dateIndex.size(); i++){
                                
                                if(dateIndex[i].year == t.tweetDate.year && dateIndex[i].month == t.tweetDate.month && dateIndex[i].day == t.tweetDate.day){
                                    alreadyExists = true;
                                    break;
                                }
                            }
                            if(! alreadyExists){
                                dateIndex.push_back(t.tweetDate);
                                
                                dateIndexMap[t.dateString] = dateIndex.size()-1;
                                
                            }
                        }
                        userTweets.push_back(t);
                    }
                    cur.tweets= userTweets;
                    
                    tweeters.push_back(cur);
                } else {
                    cout  << "Failed to parse JSON" << endl;
                }
            }
        }
    }
    addUsersFromMentions();
}

void CloudsVisualSystemTwitter::loadAvatars(){
    ofFile f = ofFile();
    
    ofDirectory dir(getVisualSystemDataPath()+"avatars/");
    dir.listDir();
    if(dir.exists()){
        int size = dir.size();
        vector<ofFile>files= dir.getFiles();
        
        for(int i = 0; i< files.size(); i++){
            string filePath =getVisualSystemDataPath()+"avatars/"+files[i].getFileName();
            
            vector<string> handle = ofSplitString(files[i].getFileName(),".");
            if (handle[0] == "default") {
                ofImage img;
                if(img.loadImage(filePath)){
                    
                }
                else{
                    cout<<filePath<<" not loaded "<<endl;
                }
                avatars["default"] =img;
                cout<<"Adding default avatar "<< handle[0]<<endl;
            }
            for (int j =0; j<tweeters.size(); j++) {
                if(tweeters[j].name == "@" +handle[0]){
                    //                    cout<<"adding avatar for "<<handle[0]<<endl;
                    ofImage img;
                    if(img.loadImage(filePath)){
                        
                    }
                    else{
                        cout<<filePath<<" not loaded "<<endl;
                    }
                    avatars[tweeters[j].name] =img;
                    break;
                    
                }
                
                
                
                if( j == tweeters.size() - 1){
                    cout<<"Avatar not found for "<<handle[0]<<endl;
                }
                
                
            }
            
            
            
            
        }
    }
    
}

void CloudsVisualSystemTwitter::clearData(){
    dateIndex.clear();
    dateIndexMap.clear();
    links.clear();
    lineIndexPairs.clear();
    numberOfMentions.clear();
    
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
            
            Tweeter& tweeter = getTweeterByID(id);
            
			int numcomp = components.size();
            max = ofVec3f(0,0,0);
            
			tweeter.position = ofVec3f(ofToFloat(components[2])*xScale,
                                       ofToFloat(components[3])*yScale,
                                       ofToFloat(components[4])*zScale);
		}
        
        if(findingEdges){
			vector<string> components = ofSplitString(line, " ");
            int id = ofToInt(components[0]);
            //428 4 8 9 11 15 17 18
            Tweeter& tweeter = getTweeterByID(id);
            if(tweeter.ID > 0){
                for(int i =1; i< components.size()-1; i++){
                    if(tweeter.ID != ofToInt(components[i]) ){
                        tweeter.linksById.push_back(ofToInt(components[i]));
                    }
                    else{
                        cout<<"Error! "<<tweeter.name<<"  : "<<tweeter.ID<<" index "<< ofToInt(components[i])<<endl;
                    }
                }
            }
        }
	}
}

void CloudsVisualSystemTwitter::updateMeshFromTweets(int index){
    
    activeTweeters.clear();
    activeTweets.clear();
    activeTweetPairs.clear();
    string currentDate = getDateAsString(dateIndex[index]);
    
    for(int i = 0; i < tweeters.size(); i++){
		//pop their connections
        if(!tweeters[i].hasTweetOnDate(currentDate)){
			continue;
		}
        
        
        //Highlighting all links when tweeter activated
        
        //        for (int l= 0; l<tweeters[i].linksById.size(); l++) {
        //            Tweeter& t = getTweeterByID(tweeters[i].linksById[l]);
        //
        //            pair<int, int> currentIndeces;
        //            if(lineIndexPairs.find(make_pair(tweeters[i].name, t.name)) != lineIndexPairs.end()){
        //                currentIndeces = lineIndexPairs[make_pair(tweeters[i].name, t.name)];
        //            }
        //            else if(lineIndexPairs.find(make_pair(t.name,tweeters[i].name)) != lineIndexPairs.end()){
        //                pair<int, int> currentIndeces = lineIndexPairs[make_pair(tweeters[i].name, t.name)];
        //            }
        //            else{
        //                //error!!
        //                continue;
        //            }
        //            //set the edges
        //            edgeMesh.getNormals()[currentIndeces.first].y = 1.0;
        ////            edgeMesh.getNormals()[currentIndeces.second].y = 1.0;
        //            int ind = MIN(currentIndeces.first,currentIndeces.second) + 1;
        ////            edgeMesh.getNormals()[ind++].y = 1.0;
        ////            edgeMesh.getNormals()[ind  ].y = 1.0;
        //
        //        }
        
		activeTweeters.push_back(&tweeters[i]);
		vector<Tweet>&  tweetsOnDate = tweeters[i].getTweetsByDate(currentDate);
		
		//pop the active tweeter node
		nodeMesh.getNormals()[tweeters[i].nodeVertexIndex].y = 1.0;
        
		for(int k = 0; k < tweetsOnDate.size(); k ++){
			activeTweets.push_back(&tweetsOnDate[k].tweet);
            activeTweetPairs.push_back(make_pair(&tweeters[i].name, &tweetsOnDate[k].tweet));
            
			for(int l = 0; l < tweetsOnDate[k].mentionedUsers.size(); l++){
				int user = getUserIdByName(tweetsOnDate[k].mentionedUsers[l]);
				if(user == -1){
					continue;
				}
				
				Tweeter& t = getTweeterByID(user);
                //                activeTweetPairs.push_back(make_pair(&tweeters[i].name, &tweetsOnDate[k].tweet));
				//find the nodes
				pair<int, int> currentIndeces;
				
				if(lineIndexPairs.find(make_pair(tweeters[i].name, t.name)) != lineIndexPairs.end()){
					currentIndeces = lineIndexPairs[make_pair(tweeters[i].name, t.name)];
				}
				else if(lineIndexPairs.find(make_pair(t.name,tweeters[i].name)) != lineIndexPairs.end()){
					pair<int, int> currentIndeces = lineIndexPairs[make_pair(tweeters[i].name, t.name)];
				}
				else{
					//error!!
					continue;
				}
				//set the edges
				edgeMesh.getNormals()[currentIndeces.first].y = 1.0;
				edgeMesh.getNormals()[currentIndeces.second].y = 1.0;
				int ind = MIN(currentIndeces.first,currentIndeces.second) + 1;
				edgeMesh.getNormals()[ind++].y = 1.0;
				edgeMesh.getNormals()[ind  ].y = 1.0;
            }
        }
    }
}

void CloudsVisualSystemTwitter::updateMesh(){
	for(int i = 0; i < nodeMesh.getVertices().size(); i++){
		nodeMesh.getNormals()[i].y *= .95;
	}
	
	for(int i = 0; i < edgeMesh.getVertices().size(); i++){
		edgeMesh.getNormals()[i].y *= .95;
	}
}

void CloudsVisualSystemTwitter::loadMesh(){
    
    edgeMesh.clear();
    nodeMesh.clear();
	
    int  currentIndex =0;
    
    for(int j = 0; j < tweeters.size(); j++){
        
        for (int k = 0; k < tweeters[j].linksById.size(); k++) {
            
			Tweeter& t = getTweeterByID( tweeters[j].linksById[k] );
            if(t.ID == -1){
				continue;
			}
			
			if(lineIndexPairs.find(make_pair(tweeters[j].name, t.name)) != lineIndexPairs.end() ||
			   lineIndexPairs.find(make_pair(t.name,tweeters[j].name)) != lineIndexPairs.end() )
			{
				continue;
			}
            
			edgeMesh.addVertex(tweeters[j].position);
			edgeMesh.addNormal(ofVec3f(1,0,0));
			tweeters[j].edgeVertexIndex = currentIndex;
			currentIndex++;
			
			//adding midpoint
			ofVec3f midpoint = tweeters[j].position.getInterpolated(t.position, .5);
			edgeMesh.addVertex(midpoint);
			edgeMesh.addNormal(ofVec3f(0,0,0));
			currentIndex++;
			
			//adding midpoint
			edgeMesh.addVertex(midpoint);
			edgeMesh.addNormal(ofVec3f(0,0,0));
			currentIndex++;
			
			edgeMesh.addVertex(t.position);
			edgeMesh.addNormal(ofVec3f(1,0,0));
			t.edgeVertexIndex = currentIndex;
			
			links.insert(make_pair(tweeters[j].ID, tweeters[j].linksById[k]));
			lineIndexPairs[make_pair(tweeters[j].name, t.name) ] = make_pair(currentIndex-3, currentIndex);
			
			currentIndex++;
        }
    }
	
	edgeMesh.setUsage(GL_STATIC_DRAW);
    edgeMesh.setMode(OF_PRIMITIVE_LINES);
    
    cout<<"No of vertices in edges "<< edgeMesh.getVertices().size()<<endl;
    currentIndex = 0;
    for(int j=0; j<tweeters.size(); j++){
        
        nodeMesh.addVertex(tweeters[j].position);
        nodeMesh.addNormal(ofVec3f(0,0,0));
        tweeters[j].nodeVertexIndex = currentIndex;
        currentIndex++;
    }
	
    cout<<"No of vertices in node "<<  nodeMesh.getVertices().size()<<endl;
    nodeMesh.setUsage(GL_STATIC_DRAW);
    nodeMesh.setMode(OF_PRIMITIVE_POINTS);
}

void CloudsVisualSystemTwitter::addUsersFromMentions(){
    
    vector<string> names;
    
    for (int i= 0; i < tweeters.size(); i++) {
        names.push_back(tweeters[i].name);
    }
    
    for (int i= 0; i < tweeters.size(); i++) {
        for(int j=0; j<tweeters[i].userLinks.size(); j++){
            
            if(! ofContains(names, tweeters[i].userLinks[j])){
                numberOfMentions[tweeters[i].userLinks[j]]++;
            }
        }
    }
    
    map<string,int>::iterator it;
    for(it = numberOfMentions.begin() ; it != numberOfMentions.end() ; it++){
        //Filter mentioned users by times mentioned
        if(it->second > minUserMentions){
            Tweeter t = Tweeter(it->first, tweeters.size());
            tweeters.push_back(t);
        }
    }
}


void CloudsVisualSystemTwitter::createPajekNetwork(string outputFileName){
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
    ofBufferToFile(getVisualSystemDataPath(true) + "/" +outputFileName,b);
}

int CloudsVisualSystemTwitter:: getUserIdByName(string name){
    
    if(userNameIdMap.find(name) != userNameIdMap.end()){
        return userNameIdMap[name];
    }
    return -1;
}

vector<Tweeter> CloudsVisualSystemTwitter::getTweetersForDate(int index){
    vector<Tweeter> tOnDate;
    
    for(int k=0; k<tweeters.size(); k++){
        if(tweeters[k].hasTweetOnDate(getDateAsString(dateIndex[index]))){
            tOnDate.push_back(tweeters[k]);
        }
    }
    
    return tOnDate;
}

void CloudsVisualSystemTwitter::drawTweetsForDate(int index){
    
    for(int k=0; k<tweeters.size(); k++){
        vector<Tweet> tweets =  tweeters[k].getTweetsByDate(getDateAsString(dateIndex[index]));
        
        if(tweets.size() > 0 ){
            
            for(int j = 0; j<tweets.size(); j++){
                ss<<tweets[j].tweet<<" "<<getDateAsString(dateIndex[index])<<endl;
            }
        }
    }
}

string CloudsVisualSystemTwitter::getDateAsString(Date d){
    string dateString;
    dateString += ofToString(d.day) + " - ";
    dateString += ofToString(d.month) + " - ";
    dateString += ofToString(d.year);
    return dateString;
}


Tweeter& CloudsVisualSystemTwitter::getTweeterByID(int _id ){
    
    for(int i=0; i< tweeters.size(); i++){
        if(tweeters[i].ID == _id){
            return tweeters[i];
        }
    }
}

void CloudsVisualSystemTwitter::CompareDates(Date d1,Date d2){
    
}

//--------------------------------------------------------------
void CloudsVisualSystemTwitter::selfGuiEvent(ofxUIEventArgs &e)
{
    if(e.getName() == "LOAD GRAPH"){
        ofxUIButton* t  = (ofxUIButton*) e.widget;
        if (t->getValue()) {
            ofFileDialogResult result = ofSystemLoadDialog("Load Images From Folder", false, getVisualSystemDataPath() +"graphs/");
            if(result.bSuccess && result.fileName.length())
            {
                loadGraphFromPath(result.filePath);
            }
        }
    }
    else if (e.getName() == "RELOAD MESH")
    {
        initSystem(currentMeshFilePath);
    }
	else if(e.getName() == "ROTATE"){
        ofxUIToggle* t = (ofxUIToggle*)e.widget;
        if(! t->getValue()){
            rotation = 0;
        }
    }
    
    font.setSize(fontSize);
    font.setLineLength(stringWidth);
    
}

void CloudsVisualSystemTwitter::updateLabelWithCurrentMeshName(string name){
    ofxUILabel* l =(ofxUILabel*)clusterGui->getWidget("MESH FILE");
    l->setLabel(name);
}

void CloudsVisualSystemTwitter::initSystem(string filePath){
    
    vector<string> strs =ofSplitString(filePath, "_");
    vector<string> strs1 =ofSplitString(filePath, "/");
    cout<<strs1[strs1.size()-1]<<endl;
    currentMeshFileName  =  strs1[strs1.size()-1];
    
    
    currentMeshFilePath = filePath;
    clearData();
    if(strs[strs.size()-1] =="old.net"){
        cout<<"old data, using tweetsOld folder"<<endl;
        loadJSONData("tweetsOld");
    }
    else if (strs[strs.size()-1] =="new.net"){
        cout<<"New data, using tweets clean folder"<<endl;
        //loadJSONData("tweetsNew");
        loadJSONData("tweetsClean");
        
    }
    else{
        cout<<"poopzz : " <<strs[strs.size()-1]<<endl;
    }
    xScale = 100;
    yScale = 100;
    zScale = 100;
    loadAvatars();
    parseClusterNetwork(filePath);
    
    loadMesh();
    std::sort(dateIndex.begin(), dateIndex.end(), &dateSorter);
    currentDateIndex = dateIndex.size() -1;
    updateMeshFromTweets(currentDateIndex);
    
    
}

void CloudsVisualSystemTwitter::createNewGraph(string outputFileName, string inputDataFolder){
    clearData();
    loadJSONData(inputDataFolder);
    createPajekNetwork(outputFileName);
}

void CloudsVisualSystemTwitter::loadGraphFromPath(string filePath){
    cout<<filePath<<endl;
    
    if(ofFile::doesFileExist(filePath)){
        initSystem(filePath);
    }
    else{
        cout<<filePath<<" does not exist "<<endl;
    }
    
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
    ofxUILabel* l =(ofxUILabel*)clusterGui->getWidget("MESH FILE");
    currentMeshFileName = l->getLabel();
    
    
    string presetMeshPath = getVisualSystemDataPath() + "graphs/" + currentMeshFileName;
    if(presetMeshPath == currentMeshFilePath){
        
        cout<<currentMeshFileName<<" was already loaded "<<endl;
    }
    else{
        cout<<"loading mesh from : "<<presetMeshPath<<endl;
        
        initSystem(presetMeshPath);
        updateLabelWithCurrentMeshName(currentMeshFileName);
    }
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemTwitter::selfSceneTransformation(){
    
    if(rotateModel){
        
        ofRotateZ(rotation );
        rotation += rotationAmount;
    }
    
}


//--------------------------------------------------------------
//normal update call
void CloudsVisualSystemTwitter::selfUpdate()
{
    
    if(ofGetElapsedTimef() > timeTillNextUpdate){
        timeTillNextUpdate = ofGetElapsedTimef()+ minTimeGapForNextTweet + ofRandom(randomRangeMin, randomRangeMax);
        cout<<"updated selection at time : "<<ofGetElapsedTimef() <<" next update at "<<timeTillNextUpdate<< endl;
    }
    
	
    if(ofGetFrameNum() % refreshRate < 1 && bAnimate){
        
        if (currentDateIndex >= dateIndexMax){
            currentDateIndex = (int)dateIndexMin;
        }
        else{
            currentDateIndex++;
        }
        
        updateMeshFromTweets(currentDateIndex);
        updateMesh();
    }
    
}

ofFloatColor CloudsVisualSystemTwitter::getRGBfromHSV(ofFloatColor hsv){
	return ofFloatColor::fromHsb(hsv.r,
								 hsv.g,
								 hsv.b,
								 hsv.a);
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemTwitter::selfDraw()
{
    
    ofPushStyle();
    ofPushMatrix();
    
    ofSetBackgroundColor(0,0,0);
    glDisable(GL_DEPTH_TEST);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
    
	ofFloatColor lineNodeBase = getRGBfromHSV(lineNodeBaseHSV);
	ofFloatColor lineEdgeBase = getRGBfromHSV(lineEdgeBaseHSV);
	ofFloatColor lineNodePop = getRGBfromHSV(lineNodePopHSV);
	ofFloatColor lineEdgePop = getRGBfromHSV(lineEdgePopHSV);
	ofFloatColor nodeBaseColor = getRGBfromHSV(nodeBaseColorHSV);
	ofFloatColor nodePopColor = getRGBfromHSV(nodePopColorHSV);
	
    if(bRenderMesh){
		pointsShader.begin();
		
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
		glEnable(GL_POINT_SMOOTH);
		
		pointsShader.setUniform1f("pointSize", ofRandom(4));
		pointsShader.setUniform4f("nodeBaseColor",
								  nodeBaseColor.r,
								  nodeBaseColor.g,
								  nodeBaseColor.b,
								  nodeBaseColor.a);
		pointsShader.setUniform4f("nodePopColor",
								  nodePopColor.r,
								  nodePopColor.g,
								  nodePopColor.b,
								  nodePopColor.a);
        
        nodeMesh.draw();
		pointsShader.end();
        
		lineShader.begin();
		lineShader.setUniform4f("lineNodeBase",
								lineNodeBase.r,
								lineNodeBase.g,
								lineNodeBase.b,
								lineNodeBase.a);
		lineShader.setUniform4f("lineEdgeBase",
								lineEdgeBase.r,
								lineEdgeBase.g,
								lineEdgeBase.b,
								lineEdgeBase.a);
		lineShader.setUniform4f("lineNodePop",
								lineNodePop.r,
								lineNodePop.g,
								lineNodePop.b,
								lineNodePop.a);
		lineShader.setUniform4f("lineEdgePop",
                                lineEdgePop.r,
                                lineEdgePop.g,
                                lineEdgePop.b,
                                lineEdgePop.a);
		lineShader.setUniform1f("edgeInterpolateExponent",
								edgeInterpolateExponent);
		
        edgeMesh.draw();
		lineShader.end();
    }
    
    if(bRenderText){
        for(int i = 0; i < activeTweeters.size(); i++){
            drawText(activeTweeters[i]->name,activeTweeters[i]->position);
        }
    }
    ofPopMatrix();
    ofPopStyle();
    
}
void CloudsVisualSystemTwitter::updateCurrentSelection(){
    // trigger for updating the current selecition
    currentSelection.clear();
    cout<<"Updating selection"<<endl;
    if (activeTweetPairs.size() > numberOfTweets) {
        for(int j=0; j<numberOfTweets; j++ ){
            int ind = ofRandom(activeTweetPairs.size() - 1);
            currentSelection.push_back(activeTweetPairs[ind]);
        }
    }
    else{
        cout<<"Not enought active tweets"<<endl;
    }
}

void CloudsVisualSystemTwitter::drawFeed(){
    
    if (currentSelection.size() > 0) {
        for(int i=0;i<currentSelection.size(); i++ ){
            ofPushStyle();
            
            ofSetColor(textColor);
            
            if (avatars.find(*currentSelection[i].first)== avatars.end() ){
                cout<<"Cant find avatar for : "<<*currentSelection[i].first<< "  using default"<<endl;
                avatars["default"].draw(tweetFeedRect.x -avatarTweetGap,tweetFeedRect.y +i*50, 50, 50);
            }
            else{
                avatars[*currentSelection[i].first].draw(tweetFeedRect.x -avatarTweetGap,tweetFeedRect.y +i*heightOffset, 50, 50);
            }
            
            font.drawString(ofToUpper(ofToString(*currentSelection[i].second)), tweetFeedRect.x, tweetFeedRect.y +i*heightOffset +textHeightOffset );
            
            ofPopStyle();
        }
    }
    /*
     if(numberOfTweets < activeTweetPairs.size()){
     //   for (int i=0; i<activeTweetPairs.size() -numberOfTweets; i++) {
     
     for(int j=0; j<numberOfTweets; j++ ){
     ofPushStyle();
     
     ofSetColor(textColor);
     int ind = ofRandom(activeTweetPairs.size() - 1);
     currentSelection.push_back(activeTweetPairs[ind]);
     if (avatars.find(*activeTweetPairs[ind].first)== avatars.end() ){
     cout<<"Cant find avatar for : "<<*activeTweetPairs[ind].first<< "  using default"<<endl;
     avatars["default"].draw(tweetFeedRect.x -50,tweetFeedRect.y +j*50, 50, 50);
     }
     else{
     avatars[*activeTweetPairs[ind].first].draw(tweetFeedRect.x -50,tweetFeedRect.y +j*50, 50, 50);
     }
     
     
     font.drawString(ofToUpper(ofToString(*activeTweetPairs[ind].second)), tweetFeedRect.x, tweetFeedRect.y +j*50 +5 );
     
     ofPopStyle();
     }
     }
     
     */
}

// draw any debug stuff here
void CloudsVisualSystemTwitter::selfDrawDebug()
{
    
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemTwitter::selfDrawBackground()
{
    
    if(bRenderFeed){
        ofPushStyle();
        ofNoFill();
        drawFeed();
        ofPopStyle();
    }
    
    ofxBillboardBeginSphericalCheat(ofVec3f(0,0,0));
    ofPushStyle();
    ofScale(0.01,-0.01,0.01);
    ofSetColor(textColor);
    font.drawString(getDateAsString(dateIndex[currentDateIndex]),0,0);
    ofxBillboardEnd();
    ofPopStyle();
    
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

void CloudsVisualSystemTwitter::reloadShaders(){
    lineShader.load(getVisualSystemDataPath() + "/shaders/linesShader");
    pointsShader.load(getVisualSystemDataPath() + "/shaders/pointsShader");
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemTwitter::selfKeyPressed(ofKeyEventArgs & args){
	if (args.key == 'R'){
		reloadShaders();
    }
    
    //set in and out points for date range
    if(args.key == 'i'){
        dateIndexMin = currentDateIndex;
    }
    if(args.key == 'o'){
        dateIndexMax = currentDateIndex;
    }
    if(args.key == 'a'){
        updateCurrentSelection();
    }
    if(args.key == 'd'){
        cout<<ofGetElapsedTimef()<<endl;
    }
}

void CloudsVisualSystemTwitter::drawText2D(string text, ofVec2f pos){
    
    ofPushStyle();
    ofSetColor(textColor);
    font.drawString(ofToUpper(text),pos.x,pos.y);
    ofPopStyle();
}

void CloudsVisualSystemTwitter::drawText(string text,ofVec3f pos){
	
    ofxBillboardBeginSphericalCheat(pos);
    ofPushStyle();
    ofSetColor(textColor);
    ofScale(0.01,-0.01,0.01);
    ofTranslate(pos.x,pos.y,pos.z);
    font.drawString(ofToUpper(text),0,0);
    ofPopStyle();
    ofxBillboardEnd();
    
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
