#include "testApp.h"
//#include "CloudsVisualSystemVoro.h"
//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    maxD =0.0001;
    minD = 0.1;
    maxClip =0;
    minClip =0;
    
    parser.loadFromFiles();
    
//	if(ofDirectory("../../../CloudsData/").exists()){
//		parser.parseLinks("../../../CloudsData/links/clouds_link_db.xml");
//		parser.setup("../../../CloudsData/fcpxml/");
//        parser.parseClusterMap("../../../CloudsData/gephi/2013_7_25_Clouds_conversation.SVG");
//	}
//	else{
//		cout << "SETTING UP IN DATA DIRECTORY" << endl;
//		parser.parseLinks("clouds_link_db.xml");
//		parser.setup("xml");
//	}
	
    
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
    
    keyWords = parser.getContentKeywords();
    

    MaxNumOfParticles = cohesiveCells.size();
    particleIndex = 0;
}

//--------------------------------------------------------------
void testApp::update(){

        
//        cellMeshes.clear();
//    cellMeshes.clear();
//		voro::container con(0,ofGetWidth(),
//                            0,ofGetHeight(),
//                            -0.1,0.1,
//                            1,1,1,
//                            containerPeriodic,containerPeriodic,containerPeriodic,
//                            1);
//        
//        for(int i = 0; i < cohesiveCells.size(); i++){
//            con.put(i, cohesiveCells[i].x*ofGetWidth(), cohesiveCells[i].y*ofGetHeight(), 0);
//        }
//    cellMeshes = getCellsFromContainer(con);
//    cout << cellMeshes.size() << endl;
 //   cells = getCellsPolylines(con);

}

//--------------------------------------------------------------
void testApp::draw(){
    vector<string> cohesiveKeywords;
    vector<ofRectangle> rects;
    vector<float> clipSize;
    vector<float> densities;
    vector<float> avgDistances;

    vector<float> minDistance;
    vector<float> sortedAvgDistance;
    maxDistances.clear();
    medianValues.clear();
    centres.clear();
    float minCohesion=10;
    float maxCohesion=0;
    ofBackground(0);
    for (int i=0; i<keyWords.size(); i++) {
        
        
        float maxDistance = 0;
        float minDistance = INT_MAX;
        ofVec2f centroid = ofVec2f(0,0);
        ofColor cellColor = ofColor(255,0,0);
        
        vector<CloudsClip> clips =  parser.getClipsWithKeyword(keyWords[i]);
        if( clips.size() == 0){
            
            continue;
        }
        ofRectangle rect = ofRectangle(clips[0].cluster.originalCentre, 0 ,0);
        float numClips=0;
        for(int j = 0; j<clips.size();j++){
            if(clips[j].cluster.originalCentre != ofVec2f(-1,-1)){
                rect.growToInclude(clips[j].cluster.originalCentre);
                centroid += clips[j].cluster.originalCentre;
                numClips++;
                
            }
            else{
             //   cout<<"clip ignored: "<< clips[j].getLinkName()<<endl;
            }
            
        }
        
        centroid /= numClips;
        vector<float> distancesPerClip;
        float totalDistance = 0;
        for (int k=0; k<clips.size(); k++) {
            if(clips[k].cluster.Centre != ofVec2f(-1,-1)){
                float distance = centroid.distance(clips[k].cluster.originalCentre);
                totalDistance += distance;
                distancesPerClip.push_back(distance);
                maxDistance = MAX(maxDistance,distance);
                //minDistance = MIN(minDistance,distance);
                if(distance<minDistance){
                    minDistance = distance;
                    cellColor = clips[k].cluster.Color;
                    
                }
            }
            
            
        }
        
        float avgDistance = totalDistance / numClips;
        float density = rect.getArea()/numClips;
        

        if(distancesPerClip.begin() != distancesPerClip.end()){
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
            }
            
            
            if(medianValue / maxDistance < .5){
                cohesiveKeywords.push_back(keyWords[i]);
                cohesiveCells.push_back(ofPoint(centres[i].x,centres[i].y));
                colors.push_back(cellColor);
                keywordsToCentre[keyWords[i]] = centroid;
            }
        }
        else{
//            cout<<"ignored distances per clip of size: "<<distancesPerClip.size()<<endl;
        }
        
        
        
    }
    //draw svg
//    ofBackground(0);
//    glEnable(GL_DEPTH_TEST);
//    ofPushMatrix();
//    
//    //        for(int i = 0; i < cellMeshes.size(); i++){
//    //            ofSetColor(colors[i],50);
//    //            cellMeshes[i].drawFaces();
//    //        }
//    
//    for(int i = 0; i < cellMeshes.size(); i++){
//        ofSetColor(colors[i],50);
//        cellMeshes[i].draw();
//    }
//    
//    ofPopMatrix();
    for(int i=0; i<parser.getAllClips().size();i++){
        ofSetColor(parser.getAllClips()[i].cluster.Color);
        ofCircle(parser.getAllClips()[i].cluster.Centre.x*ofGetWidth(),
                 parser.getAllClips()[i].cluster.Centre.y*ofGetHeight(),
                 parser.getAllClips()[i].cluster.Radius*2);
        
        
    }
    

    for(int i=0; i<centres.size();i++){
    
        if (medianValues[i] / maxDistances[i] < 1.*mouseX/ofGetWidth()) {
            ofSetColor(255,100);
            font.drawString(keyWords[i], centres[i].x*ofGetWidth(),centres[i].y*ofGetHeight());
            
        }
//        cout<<1.*mouseX/ofGetWidth()<<endl;

    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

    if(key=='f'){
        ofToggleFullscreen();
        ofClear(0);
    }
    if(key=='g'){
        parser.saveClusterMap(keywordsToCentre );
    }
    
}
