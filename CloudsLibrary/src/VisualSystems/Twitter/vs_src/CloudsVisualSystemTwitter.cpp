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
    
    refreshRate = 1000;
    edgeDecayRate = 0.8;
    meshExpansion = 100;
//    pointSize =10;
    
    dateIndexMin = 0;
    dateIndexMax = 100;
    
    rotation = 0;
    
    randomRangeMin = 1;
    randomRangeMax = 10;
    
    xScale = 100;
    yScale = 100;
    zScale = 100;
    rotateModel = false;
    bRenderMesh = true;
    bRenderText = false;
    stringWidth = 10;

    avatarSize = 10;
    
    tweetFeedRect = ofRectangle (0, 0,  getCanvasWidth()/2, getCanvasHeight());
    font.loadFont(getVisualSystemDataPath() + "fonts/NewMedia Fett.ttf",5);
    tweetFont.loadFont(getVisualSystemDataPath() + "fonts/Helvetica.ttf",20);
    tweetFontSmall.loadFont(getVisualSystemDataPath() + "fonts/Helvetica.ttf",8);
    twitterHandleFont.loadFont(getVisualSystemDataPath() + "fonts/HelveticaNeue-Bold.ttf",8);
    ofxFTGLTextAlignment alignment;
    
    if( ! tweetDeckMenu.loadImage(getVisualSystemDataPath() + "tweetDeckMenu.png")){
        cout<<"Couldnt load tweet deck image "<<endl;
    }
    tweetDeckHeight = tweetDeckMenu.height;
    tweetDeckWidth = tweetDeckMenu.width;
    numberOfTweets = 10;
    currentTweetFeedIndex =0;
    heightOffset = 50;
    avatarTweetGap = 50;
    textHeightOffset = 10;
    timeTillNextUpdate = 0;
    minTimeGapForNextTweet =3;

    avatarSize = 10;
    tweetDeckHeightOffset = -15;
    tweetDeckWidthOffset= 30;
    tweetDeckLineOffset = 10;
    
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    animationLerpRate = 0.02;
    
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
//  gui->addSpacer();
//    gui->addMinimalSlider(prefix + " HUE", 0.0, 1.0, &col.r);
//    gui->addMinimalSlider(prefix + " SAT", 0.0, 1.0, &col.g);
//    gui->addMinimalSlider(prefix + " BRI", 0.0, 1.0, &col.b);
//	if(doAlpha){
//		gui->addMinimalSlider(prefix + " ALPHA", 0.0, 1.0, &col.a);
//	}
    
   	float length = (gui->getGlobalCanvasWidth() - gui->getWidgetSpacing()*5)/3.;
    float dim    = gui->getGlobalSliderHeight();
 
    string shortprefix;
    vector<string> comps = ofSplitString(prefix, " ", true,true);
    for(int i = 0; i < comps.size(); i++) shortprefix += comps[i].at(0);
    
    ofxUILabel* label = gui->addLabel(prefix);
    labelColors[&col] = label;
    
    gui->addMinimalSlider(shortprefix + " HUE", 0.0, 1.0, &col.r, length, dim)->setShowValue(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addMinimalSlider(shortprefix + " SAT", 0.0, 1.0, &col.g, length, dim)->setShowValue(false);
    gui->addMinimalSlider(shortprefix + " BRI", 0.0, 1.0, &col.b, length, dim)->setShowValue(false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    if(doAlpha){
        gui->addMinimalSlider(shortprefix + " ALPHA", 0.0, 1.0, &col.a);
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
    clusterGui->addMinimalSlider("EDGE DECAY", 0.2, 1.0, &activityMapDamping);
    clusterGui->addMinimalSlider("NORMALS DECAY", 0.2, 1.0, &normalDecay);
    clusterGui->addMinimalSlider("SYNAPSE LEVEL", 0.0, 1.0, &synapseLevel);
	
    //TWEET POP
	addColorToGui(clusterGui,"LINE NODE BASE",lineNodeBaseHSV);
	addColorToGui(clusterGui,"LINE EDGE BASE",lineEdgeBaseHSV);
	addColorToGui(clusterGui,"LINE NODE POP",lineNodePopHSV);
	addColorToGui(clusterGui,"LINE EDGE POP",lineEdgePopHSV);
	addColorToGui(clusterGui,"SYNAPSE",synapseColorHSV);
	clusterGui->addSlider("EDGE COLOR EXPONENT", 1.0, 5., &edgeInterpolateExponent);
    

    spriteGui = new ofxUISuperCanvas("SPRITE PARAMS", gui);
    spriteGui->copyCanvasStyle(gui);
	spriteGui->copyCanvasProperties(gui);
    spriteGui->setName("Sprite");
    spriteGui->addToggle("ANIMATE SPRITES ON TWEETS",&bAnimateSpriteSize);
    spriteGui->addRangeSlider("SPRITE SIZE RANGE", 0,  100, &minSize, & maxSize);
    addColorToGui(spriteGui,"SPRITE BASE COLOR",spriteBaseColorHSV);
    addColorToGui(spriteGui,"SPRITE POP COLOR",spritePopColorHSV);
    ofAddListener(spriteGui->newGUIEvent, this, &CloudsVisualSystemTwitter::selfGuiEvent);
	guis.push_back(spriteGui);
	guimap[spriteGui->getName()] = spriteGui;
	
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
    textGui->addToggle("DRAW SPEAKER NAMES ", &bStaticNameDraw);
    textGui->addSpacer();
    addColorToGui(textGui,"TEXT ",textColorHSV,true);
    addColorToGui(textGui,"TEXT ",tweetDeckColorHSV,true);

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
    twitterFeedGui->addMinimalSlider("FEED X", 1, getCanvasWidth(), &tweetFeedRect.x);
    twitterFeedGui->addMinimalSlider("FEED Y", 1, getCanvasHeight(), &tweetFeedRect.y);
    twitterFeedGui->addMinimalSlider("HEIGHT OFFSET", 1, 500, &heightOffset);
    twitterFeedGui->addMinimalSlider("AVATAR TWEET GAP", 1, 100, &avatarTweetGap);
    twitterFeedGui->addMinimalSlider("TEXT HEIGHT OFFSET", 1, 100, &textHeightOffset);
    twitterFeedGui->addMinimalSlider("GAP BETWEEN UPDATES", 0.1, 5, &minTimeGapForNextTweet);
    twitterFeedGui->addRangeSlider("RANDOM OFFSET RANGE", 0.1,  10, &randomRangeMin, & randomRangeMax);
    twitterFeedGui->addLabel("OTHER PARAMS");
    twitterFeedGui->addIntSlider("NUM TWEETS",1, 20, &numberOfTweets);
    twitterFeedGui->addIntSlider("AVATAR SIZE", 10, 50, &avatarSize);
    twitterFeedGui->addMinimalSlider("ANIMATION LERP RATE", 0.01    , 1.0, &animationLerpRate);
    twitterFeedGui->addLabel("FONT PARAMS");
    twitterFeedGui->addMinimalSlider("FONT SIZE", 1, 20, &tweetFontSize);
    twitterFeedGui->addMinimalSlider("LINE LENGTH", 1, 1000, &tweetLineLength);
    twitterFeedGui->addMinimalSlider("HANDLE FONT SIZE", 1, 20, &tweetHandleFontSize);
    twitterFeedGui->addMinimalSlider("HANDLE LINE LENGTH", 1, 1000, &tweetHandleLineLength);
    twitterFeedGui->addMinimalSlider("STRING WIDTH CAP", 1, 100, &feedStringWidthCap);
    twitterFeedGui->addLabel("ICON PARAMS");
    twitterFeedGui->addMinimalSlider("ICON HEIGHT OFFSET ", -40, 30, &tweetDeckHeightOffset);
    twitterFeedGui->addMinimalSlider("ICON WIDTH OFFSET ", -30, 30, &tweetDeckWidthOffset);
    twitterFeedGui->addMinimalSlider("ICON WIDTH", 100, 200, &tweetDeckWidth);
    twitterFeedGui->addMinimalSlider("ICON HEIGHT", 10, 200, &tweetDeckHeight);
    twitterFeedGui->addMinimalSlider("LINE OFFSET", -100, 100, &tweetDeckLineOffset);
    twitterFeedGui->addMinimalSlider("MAX ALPHA",0.1,1.0,&maxAlphaTweetFeed);
    twitterFeedGui->addMinimalSlider("LINE ALPHA", 0, 1, &lineAlpha);
    twitterFeedGui->addMinimalSlider("AVATAR ALPHA", 0, 1, &avatarAlpha);
    ofAddListener(twitterFeedGui->newGUIEvent, this, &CloudsVisualSystemTwitter::selfGuiEvent);
	guis.push_back(twitterFeedGui);
	guimap[textGui->getName()] = twitterFeedGui;
    
    
    
}

void CloudsVisualSystemTwitter::loadJSONData(string folderName){
    
    ofFile f = ofFile();
    
    ofDirectory dir(getVisualSystemDataPath(true)+folderName+"/");
    dir.listDir();
	
	ofVec2f curActivityMapCoord(0,0);
	int activityMapCoordWidth = 100;
    maxUserLinks = 0;
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
                    cur.activityMapCoord = curActivityMapCoord;
					curActivityMapCoord.x++;
					if(curActivityMapCoord.x >= activityMapCoordWidth){
						curActivityMapCoord.x = 0;
						curActivityMapCoord.y++;
					}
                    maxUserLinks = MAX(maxUserLinks,cur.userLinks.size());
                    tweeters.push_back(cur);
                } else {
                    cout  << "Failed to parse JSON" << endl;
                }
            }
        }
    }
    
//    cout<<"Max no. of user links = "<<maxUserLinks<<endl;
    addUsersFromMentions(curActivityMapCoord, activityMapCoordWidth);
	
	activityMap.allocate(activityMapCoordWidth, curActivityMapCoord.y+1, OF_IMAGE_GRAYSCALE);

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
                
                if ( img.loadImage(filePath) ){}
                else { cout<<filePath<<" not loaded "<<endl; }
                
                avatars["default"] =img;
                cout<<"Adding default avatar "<< handle[0]<<endl;
            }
            
            for (int j =0; j<tweeters.size(); j++) {
                
                if(tweeters[j].name == "@" +handle[0]){
                    ofImage img;
                    if( img.loadImage(filePath) ){
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
void CloudsVisualSystemTwitter::setActiveTweeters(int index){
    
    string currentDate = getDateAsString(dateIndex[index]);
    
    for (int i = 0 ; i<activeTweeters.size(); i++) {
//        cout<<ofGetFrameNum() % refreshRate<< " : "<<activeTweeters[i]->refreshNum<<endl;
        if (ofGetFrameNum() % refreshRate  != activeTweeters[i]->refreshNum) {
            continue;
        }

        activeTweeters[i]->refreshNum = 0;
        activeTweeters[i]->textDecayRate = 1.0;
        vector<Tweet>&  tweetsOnDate = activeTweeters[i]->getTweetsByDate(currentDate);
        ///
		int activityMapIndex = activeTweeters[i]->activityMapCoord.y * activityMap.getWidth() + activeTweeters[i]->activityMapCoord.x;
		activityMap.getPixels()[activityMapIndex] = 1.0;
		
        
		// pop the active tweeter node
		nodeMesh.getNormals()[activeTweeters[i]->nodeVertexIndex].y = 1.0;
//        cout<<		nodeMesh.getNormals()[activeTweeters[i]->nodeVertexIndex].y<<endl;
		for(int k = 0; k < tweetsOnDate.size(); k ++){
            
            activeTweetPairs.push_back(make_pair(&activeTweeters[i]->name, &tweetsOnDate[k].tweet));
            
			for(int l = 0; l < tweetsOnDate[k].mentionedUsers.size(); l++){
				int user = getUserIdByName(tweetsOnDate[k].mentionedUsers[l]);
				if(user == -1){
					continue;
				}
				
				Tweeter& t = getTweeterByID(user);
                int activityMapIndex = t.activityMapCoord.y * activityMap.getWidth() + t.activityMapCoord.x;
                activityMap.getPixels()[activityMapIndex] = 1.0;
                //                activeTweetPairs.push_back(make_pair(&tweeters[i].name, &tweetsOnDate[k].tweet));
				//find the nodes
				pair<int, int> currentIndeces;
                
				if(lineIndexPairs.find(make_pair(activeTweeters[i]->name, t.name)) != lineIndexPairs.end()){
					currentIndeces = lineIndexPairs[make_pair(tweeters[i].name, t.name)];
				}
				else if(lineIndexPairs.find(make_pair(t.name,activeTweeters[i]->name)) != lineIndexPairs.end()){
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
                
                nodeMesh.getNormals()[t.nodeVertexIndex].y = 1.0;

        ////
//		int activityMapIndex = tweeters[i].activityMapCoord.y * activityMap.getWidth() + tweeters[i].activityMapCoord.x;
//		activityMap.getPixels()[activityMapIndex] = 1.0;
//		
//		// pop the active tweeter node
//		nodeMesh.getNormals()[tweeters[i].nodeVertexIndex].y = 1.0;
//        
//		for(int k = 0; k < tweetsOnDate.size(); k ++){
//            
//            activeTweetPairs.push_back(make_pair(&tweeters[i].name, &tweetsOnDate[k].tweet));
//            
//			for(int l = 0; l < tweetsOnDate[k].mentionedUsers.size(); l++){
//				int user = getUserIdByName(tweetsOnDate[k].mentionedUsers[l]);
//				if(user == -1){
//					continue;
//				}
//				
//				Tweeter& t = getTweeterByID(user);
//                int activityMapIndex = t.activityMapCoord.y * activityMap.getWidth() + t.activityMapCoord.x;
//                activityMap.getPixels()[activityMapIndex] = 1.0;
//                //                activeTweetPairs.push_back(make_pair(&tweeters[i].name, &tweetsOnDate[k].tweet));
//				//find the nodes
//				pair<int, int> currentIndeces;
//
//				if(lineIndexPairs.find(make_pair(tweeters[i].name, t.name)) != lineIndexPairs.end()){
//					currentIndeces = lineIndexPairs[make_pair(tweeters[i].name, t.name)];
//				}
//				else if(lineIndexPairs.find(make_pair(t.name,tweeters[i].name)) != lineIndexPairs.end()){
//					pair<int, int> currentIndeces = lineIndexPairs[make_pair(tweeters[i].name, t.name)];
//				}
//				else{
//					//error!!
//					continue;
//				}
//                
//				//set the edges
//				edgeMesh.getNormals()[currentIndeces.first].y = 1.0;
//				edgeMesh.getNormals()[currentIndeces.second].y = 1.0;
//				int ind = MIN(currentIndeces.first,currentIndeces.second) + 1;
//				edgeMesh.getNormals()[ind++].y = 1.0;
//				edgeMesh.getNormals()[ind  ].y = 1.0;
            }
        }
    }
}

void CloudsVisualSystemTwitter::updateActiveTweeters(int index){
    
    activeTweeters.clear();
    activeTweetPairs.clear();
    string currentDate = getDateAsString(dateIndex[index]);
    int frameNum = 0;
    for(int i = 0; i < tweeters.size(); i++){
        if(!tweeters[i].hasTweetOnDate(currentDate)){
			continue;
		}

		activeTweeters.push_back(&tweeters[i]);

    }
    
    for(int i = 0; i < activeTweeters.size(); i++){

        
        activeTweeters[i]->refreshNum = ofMap(i, 0, activeTweeters.size(),0, refreshRate );
        
    }
    
    
}

void CloudsVisualSystemTwitter::updateMesh(){
	for(int i = 0; i < nodeMesh.getVertices().size(); i++){
		nodeMesh.getNormals()[i].y *= activityMapDamping;
	}
	
	for(int i = 0; i < edgeMesh.getVertices().size(); i++){
		edgeMesh.getNormals()[i].y *= normalDecay;
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
			edgeMesh.addTexCoord(ofVec2f(tweeters[j].activityMapCoord.x,tweeters[j].activityMapCoord.y));
//			tweeters[j].edgeVertexIndex = currentIndex;
			currentIndex++;
			
			//adding midpoint
			ofVec3f midpoint = tweeters[j].position.getInterpolated(t.position, .5);
			edgeMesh.addVertex(midpoint);
			edgeMesh.addNormal(ofVec3f(0,0,0));
			edgeMesh.addTexCoord(ofVec2f(tweeters[j].activityMapCoord.x,tweeters[j].activityMapCoord.y));
			currentIndex++;
			
			//adding midpoint
			edgeMesh.addVertex(midpoint);
			edgeMesh.addNormal(ofVec3f(0,0,0));
			edgeMesh.addTexCoord(ofVec2f(t.activityMapCoord.x,t.activityMapCoord.y));
			currentIndex++;
			
			edgeMesh.addVertex(t.position);
			edgeMesh.addNormal(ofVec3f(1,0,0));
			edgeMesh.addTexCoord(ofVec2f(t.activityMapCoord.x,t.activityMapCoord.y));
//			t.edgeVertexIndex = currentIndex;
			
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
        float userLinkFactor = ofMap(tweeters[j].userLinks.size(), 0, maxUserLinks, 0.1, 1,true);
        cout<<tweeters[j].userLinks.size()<<endl;
        nodeMesh.addVertex(tweeters[j].position);
        nodeMesh.addNormal(ofVec3f(userLinkFactor,1.0,0));
        tweeters[j].nodeVertexIndex = currentIndex;
        currentIndex++;
    }
	
    cout<<"No of vertices in node "<<  nodeMesh.getVertices().size()<<endl;
    nodeMesh.setUsage(GL_STATIC_DRAW);
    nodeMesh.setMode(OF_PRIMITIVE_POINTS);
}

void CloudsVisualSystemTwitter::addUsersFromMentions(ofVec2f& curActivityMapCoord, int activityMapWidth ){
    
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
            Tweeter cur = Tweeter(it->first, tweeters.size());
            cur.activityMapCoord = curActivityMapCoord;
            curActivityMapCoord.x++;
            if(curActivityMapCoord.x >= activityMapWidth){
                curActivityMapCoord.x = 0;
                curActivityMapCoord.y++;
            }
            tweeters.push_back(cur);
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
    else if(e.getName() == "RENDER FEEDS"){
        cout<<"Updating selection from gui event"<<endl;
        updateCurrentSelection(currentDateIndex, true);
    }
    
    font.setSize(fontSize);
    font.setLineLength(stringWidth);
    
    tweetFont.setSize(tweetFontSize);
    tweetFont.setLineLength(tweetLineLength);
    
    twitterHandleFont.setSize(tweetHandleFontSize);
    twitterHandleFont.setLineLength(tweetHandleLineLength);
    
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
    updateActiveTweeters(currentDateIndex);
    
    if(bRenderFeed){
        updateCurrentSelection(currentDateIndex, true);
    }
    
    
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
    
    if(bRenderFeed){
        if(ofGetElapsedTimef() > timeTillNextUpdate){
            timeTillNextUpdate = ofGetElapsedTimef()+ minTimeGapForNextTweet + ofRandom(randomRangeMin, randomRangeMax);
            updateCurrentSelection(currentDateIndex,false);
        }
    }

    if(ofGetFrameNum() % refreshRate == 0 && bAnimate){
        
        if (currentDateIndex >= dateIndexMax){
            currentDateIndex = (int)dateIndexMin;
        }
        else{
            currentDateIndex++;
        }

        updateActiveTweeters(currentDateIndex);

    }
    updateMesh();
    //
//    if(ofGetFrameNum() % activeTweeterRefreshRate == 0 && bAnimate){
        setActiveTweeters(currentDateIndex);
//    }

    if(bRenderText){
        for(int i = 0; i < activeTweeters.size(); i++){
            activeTweeters[i]->textDecayRate *= activityMapDamping;
        }
    }
    
	for(int i = 0; i < activityMap.getWidth()*activityMap.getHeight(); i++){
		activityMap.getPixels()[i] *= activityMapDamping;
	}
	activityMap.update();
}

ofFloatColor CloudsVisualSystemTwitter::getRGBfromHSV(ofFloatColor& hsv){
    ofFloatColor col = ofFloatColor::fromHsb(hsv.r,hsv.g,hsv.b,hsv.a);
    if(labelColors.find(&hsv) != labelColors.end()){
        ofFloatColor colcpy = col;
        colcpy.a = .5 + colcpy.a*.5;
        labelColors[&hsv]->setColorFill(colcpy);
    }
    return col;
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
    ofEnableSmoothing();
	ofFloatColor lineNodeBase = getRGBfromHSV(lineNodeBaseHSV);
	ofFloatColor lineEdgeBase = getRGBfromHSV(lineEdgeBaseHSV);
	ofFloatColor lineNodePop = getRGBfromHSV(lineNodePopHSV);
	ofFloatColor lineEdgePop = getRGBfromHSV(lineEdgePopHSV);
	ofFloatColor nodeBaseColor = getRGBfromHSV(nodeBaseColorHSV);
	ofFloatColor nodePopColor = getRGBfromHSV(nodePopColorHSV);
	ofFloatColor synapseColor = getRGBfromHSV(synapseColorHSV);
	ofFloatColor spriteBaseColor = getRGBfromHSV(spriteBaseColorHSV);
    ofFloatColor spritePopColor = getRGBfromHSV(spritePopColorHSV);
    if(bRenderMesh){
        
////POINTS
		pointsShader.begin();
		
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
		glEnable(GL_POINT_SMOOTH);
		
//		pointsShader.setUniform1f("pointSize", ofRandom(4));
//		pointsShader.setUniform4f("nodeBaseColor",
//								  nodeBaseColor.r,
//								  nodeBaseColor.g,
//								  nodeBaseColor.b,
//								  nodeBaseColor.a);
//		pointsShader.setUniform4f("nodePopColor",
//								  nodePopColor.r,
//								  nodePopColor.g,
//								  nodePopColor.b,
//								  nodePopColor.a);
        
        pointsShader.setUniformTexture("tex", sprite, 1);

        pointsShader.setUniform1f("maxSize", maxSize);
        pointsShader.setUniform1f("minSize", minSize);
//        pointsShader.setUniform1f("sizeMultiplier", sizeMultiplier);
//        pointsShader.setUniform3f("attractor", 0, 0, 0);
//        pointsShader.setUniform1f("radius", 300.);
		pointsShader.setUniform4f("spriteBaseColor",
								spriteBaseColor.r,
								spriteBaseColor.g,
								spriteBaseColor.b,
								spriteBaseColor.a);
        pointsShader.setUniform4f("spritePopColor",
                                  spritePopColor.r,
                                  spriteBaseColor.g,
                                  spritePopColor.b,
                                  spritePopColor.a);
        pointsShader.setUniform1f("animateSpriteSize", (float)bAnimateSpriteSize);
        
        ofEnablePointSprites();
        ofDisableArbTex();
        nodeMesh.draw();
        ofEnableArbTex();
        ofDisablePointSprites();
		pointsShader.end();
/////END POINTS
        
        
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
		lineShader.setUniform4f("synapse",
                                synapseColor.r,
                                synapseColor.g,
                                synapseColor.b,
                                synapseColor.a);
        
		lineShader.setUniform1f("edgeInterpolateExponent",
								edgeInterpolateExponent);
		lineShader.setUniformTexture("activityMap", activityMap, 1);
        lineShader.setUniform1f("synapseLevel",synapseLevel);
		
        edgeMesh.draw();
		lineShader.end();
    }
    
    if(bRenderText){
        for(int i = 0; i < activeTweeters.size(); i++){
//            activeTweeters[i]->textDecayRate *= edgeDecayRate;
//            string test = ofToString(activeTweeters[i]->position.x) + " , "+ ofToString(activeTweeters[i]->position.y) +" , "+ofToString(activeTweeters[i]->position.z);
//            drawText(test,activeTweeters[i]->position,activeTweeters[i]->textDecayRate);
            drawText(activeTweeters[i]->name,activeTweeters[i]->position,activeTweeters[i]->textDecayRate);
        }
        
        if (bStaticNameDraw) {
            for (int i= 0 ; i<tweeters.size(); i++) {
                if(tweeters[i].tweets.size() > 0){
                    drawText(tweeters[i].name,tweeters[i].position,1.0);
                }
            }
        }
    }
    ofPopMatrix();
    ofPopStyle();
    
}
void CloudsVisualSystemTwitter::updateCurrentSelection(int index, bool firstTime){
    // trigger for updating the current selecition

    if(firstTime) {
        currentSelection.clear();
        while(currentSelection.size() < numberOfTweets){
            
            for(int i=0; i< numberOfTweets; i++){
                bool alreadySelected = false;
                string currentDate = getDateAsString(dateIndex[index]);
                
                vector<pair<string*, string*> > :: iterator it;
                for(int i = 0; i < tweeters.size(); i++){
                    
                    if( ! tweeters[i].hasTweetOnDate(currentDate) ){
                        continue;
                    }
                    
                    //if tweeter is already in the current selection ignore them
                    for( it = currentSelection.begin(); it != currentSelection.end(); it++){
                        
                        if ( *it->first == tweeters[i].name) {
                            alreadySelected = true;
                        }
                    }
                    
                    if (! alreadySelected) {
                        vector<Tweet>&  tweetsOnDate = tweeters[i].getTweetsByDate(currentDate);
                        if(tweetsOnDate.size() == 0){
                            ofLogError()<<"hasTweetOnDate returned true for "<<tweeters[i].name<<" even thought no. of tweets on that date = 0"<<endl;
                            continue;
                        }
                        //add a new tweet to the start
                        Tweet& randTweet = tweetsOnDate[ofRandom(tweetsOnDate.size())];
                        
                        currentSelection.push_back(make_pair(&tweeters[i].name, &randTweet.tweet));
                        
                    }
                    if(currentSelection.size() >=numberOfTweets){
                        break;
                    }
                    
                }
            }
            if(index == 0){
                index = dateIndex.size() -1;
            }
            else{
                index--;
            }
            cout<<"current size : "<<currentSelection.size()<<" required size : "<<numberOfTweets<<endl;
        }
    }
    else{
  
        string currentDate = getDateAsString(dateIndex[index]);
        vector<pair<string*, string*> > :: iterator it;

        for(int i = 0; i < tweeters.size(); i++){
            bool alreadySelected = false;

            if( ! tweeters[i].hasTweetOnDate(currentDate)){

                continue;
            }
            //if tweeter is already in the current selection ignore them
            for( it = currentSelection.begin(); it != currentSelection.end(); it++){
                
                if ( *it->first == tweeters[i].name ) {
                    alreadySelected = true;
                }
            }
            
            if (! alreadySelected ) {
                vector<Tweet>&  tweetsOnDate = tweeters[i].getTweetsByDate(currentDate);
                
                if(tweetsOnDate.size() == 0){
                    ofLogError()<<"hasTweetOnDate returned true for "<<tweeters[i].name<<" even thought no. of tweets on that date = 0"<<endl;
                    continue;
                }
                
                int index = int(ofRandom(tweetsOnDate.size()));
                Tweet& randTweet =tweetsOnDate[index];

                currentSelection.pop_back();
                //add a new tweet to the start
                currentSelection.insert(currentSelection.begin(), make_pair(&tweeters[i].name,&tweetsOnDate[index].tweet ));
                break;
            }
            
        }
    }
    
    animationLerpAmt = 0;
    bAnimateFeed = true;
    
}

void CloudsVisualSystemTwitter::drawFeed(){
    ofFloatColor col = getRGBfromHSV(tweetDeckColorHSV);
    if (currentSelection.size() > 0) {
        
        if(bAnimateFeed){
            for(int i=0;i<currentSelection.size(); i++ ){
                ofPushStyle();

                float textX = getCanvasWidth() - tweetFeedRect.x;
                float avatarX = textX -avatarTweetGap;
                float menuX = textX + tweetFont.getLineLength() - tweetDeckMenu.width + tweetDeckWidthOffset;
                float lineX1 = avatarX -3;
                float lineX2 = textX + tweetFont.getLineLength() ;
                
                // Only nede to lerp y values
                float sourceTextY = tweetFeedRect.y + +(i-1)*heightOffset +textHeightOffset;
                float targetTextY = tweetFeedRect.y + +i*heightOffset +textHeightOffset;

                float sourceAvatarY = tweetFeedRect.y +(i-1)*heightOffset;
                float targetAvatarY = tweetFeedRect.y +i*heightOffset;
                                
                float sourceMenuY = tweetFeedRect.y +(i -1)*heightOffset +textHeightOffset - tweetDeckMenu.height + tweetDeckHeightOffset;
                float targetMenuY = tweetFeedRect.y +(i )*heightOffset +textHeightOffset - tweetDeckMenu.height + tweetDeckHeightOffset;

                float sourceLineY = tweetFeedRect.y +(i -1)*heightOffset +textHeightOffset - tweetDeckMenu.height + tweetDeckHeightOffset- tweetDeckLineOffset;
                float targetLineY = tweetFeedRect.y +(i )*heightOffset +textHeightOffset - tweetDeckMenu.height + tweetDeckHeightOffset - tweetDeckLineOffset;

//                float curYpos = tweetFeedRect.y +i*heightOffset + textHeightOffset;
//                float menuY = curYpos + heightOffset  - tweetDeckMenu.height + tweetDeckHeightOffset;
//                float lineY = curYpos + heightOffset - tweetDeckLineOffset;
                //lerp them lines and text
                float curTextY = ofLerp(sourceTextY, targetTextY, animationLerpAmt);
                float curAvatarY = ofLerp(sourceAvatarY, targetAvatarY, animationLerpAmt);
                float curMenuY = ofLerp(sourceMenuY, targetMenuY, animationLerpAmt);
                float curLineY = ofLerp(sourceLineY, targetLineY, animationLerpAmt);
                
                col.a = 1.0 -powf(ofMap(i, 0, currentSelection.size()-1,  maxAlphaTweetFeed,1.0),2);
//                ofSetColor(col);
                
                //50 is a magic number right now using this to not draw tweets that intersect with the edge of the screen
                if(tweetFeedRect.y + i*heightOffset + textHeightOffset + 50 < getCanvasHeight()){
                    
                    if (avatars.find(*currentSelection[i].first)== avatars.end() ){
                        cout<<"Cant find avatar for : "<<*currentSelection[i].first<< "  using default"<<endl;
                        avatars["default"].draw(avatarX,curAvatarY, avatarSize, avatarSize);
                    }
                    else{
                        ofFloatColor imgcol = col;
                        imgcol.a = avatarAlpha;
                        ofSetColor(imgcol);
                        avatars[*currentSelection[i].first].draw(avatarX,curAvatarY, avatarSize, avatarSize);
                    }
//                    cout<<*currentSelection[i].first<<" : "<<*currentSelection[i].second<<endl;
                    ofSetColor(col);
                    twitterHandleFont.drawString(ofToString(*currentSelection[i].first), textX, curTextY );
                    tweetFont.drawString(ofToString(*currentSelection[i].second), textX, curTextY + 15 );
                    tweetDeckMenu.draw(menuX,curMenuY, tweetDeckWidth, tweetDeckHeight);
                    col.a = lineAlpha;
                    ofSetColor(col);
                    ofSetLineWidth(1);
                    ofLine( lineX1 ,curLineY,lineX2 , curLineY);
                    
                }
                
                ofPopStyle();
                
            }
            if(animationLerpAmt >= 1){
                bAnimateFeed = false;
                animationLerpAmt = 0;
            }
            else{
                animationLerpAmt  += animationLerpRate;
            }
            
        }
        else{
            for(int i=0;i<currentSelection.size(); i++ ){
                ofPushStyle();

                col.a = 1.0 -powf(ofMap(i, 0, currentSelection.size()-1, maxAlphaTweetFeed, 1.),2);                
                //SM: Local variables for drawings. Apologies to anyone who sees this that isnt me.
                float textX = getCanvasWidth() - tweetFeedRect.x;
                float avatarX = textX -avatarTweetGap;
                float menuX = textX + tweetFont.getLineLength() - tweetDeckMenu.width + tweetDeckWidthOffset;
                float lineX1 = avatarX -3;
                float lineX2 = textX + tweetFont.getLineLength() ;
                
                
                float curYpos = tweetFeedRect.y +i*heightOffset + textHeightOffset;
                float twitterHandleY =  curYpos ;
                float tweetY = curYpos + 15;
                float menuY = curYpos + heightOffset  - tweetDeckMenu.height + tweetDeckHeightOffset;
                float lineY = menuY  - tweetDeckLineOffset;

//                ofSetColor(col);
                
                //50 is a magic number right now using this to not draw tweets that intersect with the edge of the screen
                if(tweetFeedRect.y + i*heightOffset + textHeightOffset + 50 < getCanvasHeight()){
                    
                    if (avatars.find(*currentSelection[i].first)== avatars.end() ){
                        cout<<"Cant find avatar for : "<<*currentSelection[i].first<< "  using default"<<endl;
                        avatars["default"].draw(avatarX,tweetFeedRect.y +i*heightOffset, avatarSize, avatarSize);
                    }
                    else{
                        ofFloatColor imgcol = col;
                        imgcol.a = avatarAlpha;
                        ofSetColor(imgcol);
                        avatars[*currentSelection[i].first].draw(avatarX,tweetFeedRect.y +i*heightOffset, avatarSize, avatarSize);
                    }
                    ofSetColor(col);
                    twitterHandleFont.drawString(ofToString(*currentSelection[i].first), textX, twitterHandleY);
                    tweetFont.drawString(ofToString(*currentSelection[i].second), textX, tweetY);
                    
                    tweetDeckMenu.draw(menuX,menuY, tweetDeckWidth, tweetDeckHeight);
                    ofSetLineWidth(1);
                    col.a = lineAlpha;
                    ofSetColor(col);
                    ofLine( lineX1 ,lineY,lineX2 , lineY);
                }
                


                ofPopStyle();
                
            }
        }
    }
    else{
        cout<<"selection is 0"<<endl;
    }
}

// draw any debug stuff here
void CloudsVisualSystemTwitter::selfDrawDebug()
{
    
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemTwitter::selfDrawBackground()
{
//    ofFloatColor  col = getRGBfromHSV(textColorHSV);
//    ofPushMatrix();
//    ofxBillboardBeginSphericalCheat(ofVec3f(0,0,0));
//    ofPushStyle();
//    ofSetColor(col);
////    ofTranslate(100, -100);
////    ofScale(0.01,-0.01,0.01);
//    font.drawString(getDateAsString(dateIndex[currentDateIndex]), 0, 0);
//    ofPopStyle();
//    ofxBillboardEnd();
//    ofPopMatrix();
    
    
    if(bRenderFeed){
        ofPushStyle();
        ofNoFill();
        drawFeed();
        ofPopStyle();
    }
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
    
    ofDisableArbTex();
	sprite.loadImage(getVisualSystemDataPath() + "images/dot.png");
	ofEnableArbTex();
    
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
        updateCurrentSelection(currentDateIndex,false);
    }
    if(args.key == 'd'){
 
    }
}

void CloudsVisualSystemTwitter::drawText2D(string text, ofVec2f pos){
    
    ofFloatColor  col = getRGBfromHSV(textColorHSV);
    ofPushStyle();
    ofSetColor(col);
    font.drawString(ofToUpper(text),pos.x,pos.y);
    ofPopStyle();
}

void CloudsVisualSystemTwitter::drawText(string text,ofVec3f pos, float alpha){
    ofFloatColor  col = getRGBfromHSV(textColorHSV);
    col.a = alpha;
    ofxBillboardBeginSphericalCheat(pos);
    ofPushStyle();
    ofSetColor(col);
//    ofSetColor(col.r,col.g,col.b,col.a);
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
