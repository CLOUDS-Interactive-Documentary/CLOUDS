#include "testApp.h"
#include "CloudsVisualSystemVoro.h"
//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    maxD =0.0001;
    minD = 0.1;
    maxClip =0;
    minClip =0;
	if(ofDirectory("../../../CloudsData/").exists()){
		parser.parseLinks("../../../CloudsData/links/clouds_link_db.xml");
		parser.setup("../../../CloudsData/fcpxml/");
        parser.parseClusterMap("../../../CloudsData/gephi/CLOUDS_test_5_26_13.SVG");
	}
	else{
		cout << "SETTING UP IN DATA DIRECTORY" << endl;
		parser.parseLinks("clouds_link_db.xml");
		parser.setup("xml");
	}
	
    
    if( font.loadFont("../../../CloudsData/font/materiapro_light.ttf", 10)){
        cout<<"font loaded!"<<endl;
    }
    else{
        cout<<"font not loaded!"<<endl;
    }
    
    
    std::set<string>::iterator colorIt = parser.clusterMapColors.begin();
    while(colorIt != parser.clusterMapColors.end()){
        cout << *colorIt << endl;
        colorIt++;
    }
    
    keyWords = parser.getAllKeywords();
}

//--------------------------------------------------------------
void testApp::update(){
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    vector<string> cohesiveKeywords;
    vector<ofRectangle> rects;
    vector<float> clipSize;
    vector<float> densities;
    vector<float> avgDistances;
    vector<float> maxDistances;
    vector<float> minDistance;
    vector<float> sortedAvgDistance;
    vector<float>medianValues;
    vector<ofVec2f> centres;
    
    //draw svg
    ofBackground(0);
    for(int i=0; i<parser.getAllClips().size();i++){
        ofSetColor(parser.getAllClips()[i].cluster.Color);
        ofCircle(parser.getAllClips()[i].cluster.Centre.x*ofGetWidth(),
                 parser.getAllClips()[i].cluster.Centre.y*ofGetHeight(),
                 parser.getAllClips()[i].cluster.Radius*2);
        
        
    }
    
    //cout << "START" << endl;
    float minCohesion=10;
    float maxCohesion=0;

    for (int i=0; i<keyWords.size(); i++) {
        float maxDistance = 0;
        ofVec2f centroid = ofVec2f(0,0);
        vector<CloudsClip> clips =  parser.getClipsWithKeyword(keyWords[i]);
        
        ofRectangle rect = ofRectangle(clips[0].cluster.Centre, 0 ,0);
        float numClips=0;
        for(int j = 0; j<clips.size();j++){
            if(clips[j].cluster.Centre != ofVec2f(-1,-1)){
                rect.growToInclude(clips[j].cluster.Centre);
                centroid += clips[j].cluster.Centre;
                numClips++;
            }
            else{
                cout<<"clip ignored: "<< clips[j].getLinkName();
            }

        }

//        centroid /= clips.size();
        centroid /= numClips;
        vector<float> distancesPerClip;
        float totalDistance = 0;
        for (int k=0; k<clips.size(); k++) {
            if(clips[k].cluster.Centre != ofVec2f(-1,-1)){
                float distance = centroid.distance(clips[k].cluster.Centre);
                totalDistance += distance;
                distancesPerClip.push_back(distance);
                maxDistance = MAX(maxDistance,distance);
            }

        }

        float avgDistance = totalDistance / numClips;
        float density = rect.getArea()/numClips;
        
        
        maxDistances.push_back(maxDistance);
        centres.push_back(centroid);
        clipSize.push_back(numClips);
        avgDistances.push_back(avgDistance);
        rects.push_back(rect);
        
        std::sort(distancesPerClip.begin(), distancesPerClip.end());
        float medianValue = distancesPerClip[distancesPerClip.size()/2];
        medianValues.push_back(medianValue);
        
        float c =0;
        c =(float)medianValue / (float)maxDistance;
        if(maxDistance>0){
            maxCohesion = MAX(maxCohesion,c);
            minCohesion = MIN(minCohesion,c);
            
            //   cout<<minCohesion<<","<<clips.size()<<","<<maxDistance<<endl;
        }
        
          //      cout<<numClips<<","<<clips.size()<<endl;
        
        if(medianValue / maxDistance < .35){
            //     cout << "keyword " << keyWords[i] << "(" << parser.getNumberOfClipsWithKeyword(keyWords[i]) << ") has score " << medianValue / maxDistance << endl;
            cohesiveKeywords.push_back(keyWords[i]);
        }
        
        
    }
    
    //    cout << "END" << endl << endl;
    
    for(int i=0; i<centres.size();i++){
        
        
        if (medianValues[i] / maxDistances[i] < 1.*mouseX/ofGetWidth()) {
            float cohesion = medianValues[i] / maxDistances[i];
            float alpha = ofMap(cohesion, minCohesion, maxCohesion, 0, 128);
            //cout<<minCohesion<<","<<maxCohesion<<endl;
            //ofNoFill();
            //  cout<<alpha<<endl;
            // cout<<cohesion<<endl;
            ofSetColor(255);
            font.drawString(keyWords[i], centres[i].x*ofGetWidth(),centres[i].y*ofGetHeight());
            //ofCircle(centres[i] * ofVec2f(ofGetWidth(),ofGetHeight()),
             //        (medianValues[i])*ofGetHeight());
            
            
        }
        //float x = medianValues[i] / maxDistances[i] < ;
        cout<<1.*mouseX/ofGetWidth()<<endl;
        //        if(ofGetKeyPressed(32)){
        //            vector<CloudsClip> cohesiveClips =  parser.getClipsWithKeyword(cohesiveKeywords);
        //            vector<CloudsClip>& allClips = parser.getAllClips();
        //            cout<<"% of clips: "<<((float)cohesiveClips.size()/(float)allClips.size())*100<<", % of keywords: "<<((float)cohesiveKeywords.size())/(float)keyWords.size()*100   <<endl;
        //        }
        
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    //    for(int i=0; i<parser.getAllClips().size();i++){
    //
    //        cout<<i<<"::"<<parser.getAllClips()[i].cluster.Color.getHex()<<endl;
    //    }
    //    for (int i=0; i<keyWords.size(); i++) {
    //        vector<CloudsClip> clips =  parser.getClipsWithKeyword(keyWords[i]);
    //        ofRectangle rect = ofRectangle(clips[0].cluster.Centre, 0 ,0);
    //
    //        for(int j = 0; j<clips.size();j++){
    //            rect.growToInclude(clips[j].cluster.Centre);
    //        }
    //        float density=rect.getArea()/clips.size();
    //        maxD = MAX(maxD,density);
    //        minD = MIN(minD,density);
    //        densities.push_back(density);
    //
    //    }
    //    std::sort(densities.begin(), densities.end());
    ////    cout<<maxD<<","<<minD<<","<<keyWords.size()<<","<<densities.size()<<endl;
    //    for(int i=0;i<densities.size();i++){
    //
    //        cout<<"Node Density for "<< keyWords[i]<<" : "<<ofMap(densities[i], minD, maxD, 0, 1000)<<endl;
    //
    //
    //    }
    
    //    for(int i =0;i<keyWords.size();i++){
    //        cout<<keyWords[i]<<": "<<avgDistance[i]<<","<<clipSize[i]<<endl;
    //    }
    //
    if(key=='f'){
        ofToggleFullscreen();
        ofClear(0);
    }
}
