//
//  CouldsClusterVisualizer.cpp
//  CloudsFCPParser
//
//  Created by Surya Mattu on 12/06/13.
//
//

#include "CloudsClusterVisualizer.h"
#define	SPRING_MIN_STRENGTH		0.005
#define SPRING_MAX_STRENGTH		0.1
#include "ofxTween.h"

static bool DistancePointLine( ofVec3f p, ofVec3f a, ofVec3f b, float& distance )
{
    float lineMagnitudeSqr = a.distanceSquared( b );
	float u;
    u = (( ( p.x - a.x ) * ( b.x - a.x ) ) +
		 ( ( p.y - a.y ) * ( b.y - a.y ) ) +
		 ( ( p.z - a.z ) * ( b.z - a.z ) ) ) / lineMagnitudeSqr;
	
    if( u < 0.0f || u > 1.0f )
        return false;   // closest point does not fall within the line segment
	
	ofVec3f intersection = a + u * (b - a);
    //    intersection.X = a.X + U * ( b.X - a.X );
    //    intersection.Y = a.Y + U * ( b.Y - a.Y );
    //    intersection.Z = a.Z + U * ( b.Z - a.Z );
	
    distance = p.distance(intersection);
	
    return true;
}


CloudsClusterVisualiser::CloudsClusterVisualiser(){
    
    database = NULL;
    selectedParticle = NULL;
    hoverParticle = NULL;
    pathChanged = false;
    selectedSpring = NULL;
    hoverSpring = NULL;
    
    minRadius = 2;
    maxRadius = 50;
    minMass = 1;
    maxMass = 0;
    minScore = 0;
    maxScore = 1;
    cursorRadius = 10;
    
    /*
     BRIGHT
     hoverColor = ofColor::fromHex(0xfc790c);
     selectedColor = ofColor::fromHex(0xff530d);
     nodeColor = ofColor::fromHex(0xacd75c);
     lineColor = ofColor::fromHex(0x1baa8f);
     visitedColor = ofColor::fromHex(0x73bbc4);
     abandonedColor = ofColor::fromHex(0x434343);
     traceColor = ofColor::fromHex(0xe57417);
     */
    
    hoverColor = ofColor::fromHex(0xc965e2);
    selectedColor = ofColor::fromHex(0xe79301);
    nodeColor = ofColor::fromHex(0x4e7ac7);
    lineColor = ofColor::fromHex(0xadd5f7);
    visitedColor = ofColor::fromHex(0x9947ad);
    abandonedColor = ofColor::fromHex(0x373d52);
    traceColor = ofColor::fromHex(0xd0e9ff);
    suppressedColor = ofColor::fromHex(0xb14444);
    
    currentScale = 1.0;
    currentTop = ofVec2f(0,0);
    
    springStrength = .05;
    restLength = 20;
    repulsionForce = 20;
    
}


void CloudsClusterVisualiser::storyBegan(CloudsStoryEventArgs& clips){
	clear();
}
void CloudsClusterVisualiser::clear(){
	edges.clear();
    linkEdges.clear();
    keywordsinEdges.clear();
    suppressedEdges.clear();
    pathByEdges.clear();
    edgeScores.clear();
    edgePairs.clear();
    
    
	particleName.clear();
	particlesByTag.clear();
	particleToClip.clear();
	springs.clear();
	linkSprings.clear();
	suppressedSprings.clear();
	
	keywordsInSpring.clear();
	particleBirthOrder.clear();
	dampendPositions.clear();
	
	currentTopic = "";
	currentOptionClips.clear();
	currentOptionParticles.clear();
	pathByClip.clear();
	pathByParticles.clear();
	pathBySprings.clear();
	springScores.clear();
	
	clipLog.clear();
	
	pathChanged = true;
	centerNode = NULL;
	physics.clear();
}

void CloudsClusterVisualiser::setup(CloudsFCPParser& dbref){
    
	this->database = &dbref;
	
	ofRegisterMouseEvents(this);
    ofRegisterKeyEvents(this);
	ofSetCircleResolution(200);
    database->parseClusterMap("../../../CloudsData/gephi/CLOUDS_test_5_26_13.SVG");
    database->sortKeywordsByOccurrence(true);
    vector<string>& allKeywords = database->getAllKeywords();
    vector<CloudsClip>& clips = database->getAllClips();
    for(int k = 0; k < allKeywords.size(); k++){
        for(int c = 0; c < clips.size(); c++){
            if( ofContains(clips[c].keywords, allKeywords[k]) ){
                for(int i = k+1; i < allKeywords.size(); i++){
                    if( ofContains(clips[c].keywords, allKeywords[i]) ){
                        sharedClips[ make_pair(allKeywords[k], allKeywords[i]) ]++;
                    }
                }
            }
        }
    }
}

void CloudsClusterVisualiser::setupPhysics(){
	physics.clear();
    
    width = ofGetWidth();
	height = ofGetHeight();
    
    cout << "width and height " << width << " " << height << endl;
    
	physics.setWorldSize(ofVec2f(0,0), ofVec2f(width*10,height*10));
	physics.setSectorCount(1);
    physics.setDrag(0.1f);
}

void CloudsClusterVisualiser::clipChanged(CloudsStoryEventArgs &args){
    vector<msa::physics::Particle2D*> newParticles;
	int particleStartIndex = physics.numberOfParticles();
	string mainLinkName = args.chosenClip.getLinkName();
	clipLog.push_back(mainLinkName);
	edges.clear(); 
	if( particlesByTag.find(mainLinkName) != particlesByTag.end() &&
	   ofContains(pathByParticles, particlesByTag[mainLinkName] ))
	{
		cout << "re adding " << args.chosenClip.getLinkName() << endl;
		return;
	}
	
	pathChanged = true;
	
	//for(int i = 0; i < scores.size(); i++){
	for(int i = 0; i < args.clipOptions.size(); i++){
		maxScore = MAX(maxScore, args.clipOptions[i].currentScore);
		minScore = MIN(minScore, args.clipOptions[i].currentScore);
	}
	
	msa::physics::Particle2D* p;
	if(hasParticle( mainLinkName )){
		p = particlesByTag[ mainLinkName];
	}
	else{
        //SURYA MOD: Position based on cliup cluster location
        ofVec2f scaledPos = ofVec2f(args.chosenClip.cluster.Centre.x*width,args.chosenClip.cluster.Centre.y*height);
        p = physics.makeParticle(scaledPos);
		//p = physics.makeParticle(ofVec2f(width/2, height/2));
        
        dampendPositions[p] = p->getPosition();
		p->makeFixed();
		
        //SURYA MOD: Setting mass based on the radius of the circle
        p->setMass(args.chosenClip.cluster.Radius*10);
        //p->setMass(args.chosenClip.keywords.size());
		
		newParticles.push_back(p);
		particleName[p] = mainLinkName;
		particlesByTag[mainLinkName] = p;
		particleToClip[p] = args.chosenClip;
		particleBirthOrder[p] = physics.numberOfParticles();
	}
	
	msa::physics::Particle2D* oldCenter = centerNode;
	centerNode = p;
	pathByClip.push_back(args.chosenClip);
	pathByParticles.push_back(p);
    
	currentOptionClips.clear();
	currentOptionParticles.clear();
	
	for(int i = 0; i < args.clipOptions.size(); i++){
		CloudsClip& relatedClip = args.clipOptions[i];
		string clipName = relatedClip.getLinkName();
        
		msa::physics::Particle2D* a;
		if(hasParticle(clipName)){
			a = particlesByTag[clipName];
			if(ofContains(pathByParticles, a)){
				continue;
			}
		}
		else{
			//make a particle for the seed
            ofVec2f optionPos = ofVec2f(relatedClip.cluster.Centre.x*width,relatedClip.cluster.Centre.y*height);
            //SURYA MOD:
			a = physics.makeParticle(optionPos);
            //            a = physics.makeParticle(p->getPosition() + ofVec2f(ofRandom(-2,2), ofRandom(-2,2)));
            //SURYA MOD:
			dampendPositions[a] =  a->getPosition();//p->getPosition();
			particleBirthOrder[a] = physics.numberOfParticles();
			//SURYA MOD:
            int mass = relatedClip.cluster.Radius*5;
            //int mass = relatedClip.keywords.size();
			a->setMass(mass);
			maxMass = MAX(maxMass, mass);
			newParticles.push_back(a);
			particleName[a] = clipName;
			particlesByTag[clipName] = a;
			particleToClip[a] = relatedClip;
		}
		
        bool isLink = database->clipLinksTo(mainLinkName, clipName);
		bool isSuppressed = database->linkIsSuppressed(mainLinkName, clipName);
		if(isLink && isSuppressed){
			ofSystemAlertDialog("Clips " + mainLinkName + " " + clipName + " are both linked and suppressed!");
		}
        
		currentOptionClips.push_back(relatedClip);
		currentOptionParticles.push_back(a);
        
		if(edgePairs.find( make_pair(a, p) ) == edgePairs.end() &&
		   edgePairs.find( make_pair(p, a) ) == edgePairs.end() ){
//		if(springs.find( make_pair(a, p) ) == springs.end() &&
//		   springs.find( make_pair(p, a) ) == springs.end() ){
			//use clips in common to weight the lines
            float length = ofDist(p->getPosition().x, p->getPosition().y, a->getPosition().x, a->getPosition().y);
            //SURYA MOD:
            msa::physics::Spring2D* newSpring = physics.makeSpring(a, p, springStrength, length );
            //          msa::physics::Spring2D* newSpring = physics.makeSpring(a, p, springStrength, restLength );
            ParticleEdge* e = new ParticleEdge(p,a,length);
            edgeScores[e] =args.clipOptions[i].currentScore;
            edgePairs[make_pair(p, a)]= e;
            edges.push_back(e);
            keywordsinEdges[e] = database->getSharedKeywords(args.chosenClip, relatedClip);
			
            springScores[newSpring] = args.clipOptions[i].currentScore;
			springs[ make_pair(p, a) ] = newSpring;
			keywordsInSpring[ newSpring ] = database->getSharedKeywords(args.chosenClip, relatedClip);
			if(isLink){
				linkSprings.insert(newSpring);
                linkEdges.insert(e);
			}
			else if(isSuppressed){
				suppressedSprings.insert(newSpring);
                suppressedEdges.insert(e);
			}
		}
		else if(isLink) {
			if(springs.find( make_pair(a, p) ) != springs.end()) linkSprings.insert(springs[ make_pair(a, p)]);
			else if(springs.find( make_pair(p, a) ) != springs.end()) linkSprings.insert(springs[ make_pair(p, a)]);
		
            if(edgePairs.find( make_pair(a, p) ) != edgePairs.end()) linkEdges.insert(edgePairs[ make_pair(a, p)]);
			else if(edgePairs.find( make_pair(p, a) ) != edgePairs.end()) linkEdges.insert(edgePairs[ make_pair(p, a)]);
		}
		else if(isSuppressed){
			if(springs.find( make_pair(a, p) ) != springs.end()) suppressedSprings.insert(springs[ make_pair(a, p)]);
			else if(springs.find( make_pair(p, a) ) != springs.end()) suppressedSprings.insert(springs[ make_pair(p, a)]);
            
            if(edgePairs.find( make_pair(a, p) ) != edgePairs.end()) suppressedEdges.insert(edgePairs[ make_pair(a, p)]);
			else if(edgePairs.find( make_pair(p, a) ) != edgePairs.end()) suppressedEdges.insert(edgePairs[ make_pair(p, a)]);
            
		}
	}
    

	if(oldCenter != NULL){
		msa::physics::Spring2D* pathSpring = springs[make_pair(oldCenter, centerNode)];
        ParticleEdge* pathEdge = edgePairs[make_pair(oldCenter, centerNode)];
        
        pathEdge->setLength(ofDist(oldCenter->getPosition().x, oldCenter->getPosition().y, centerNode->getPosition().x, centerNode->getPosition().y));
        pathByEdges.push_back(pathEdge);
        
		pathSpring->setRestLength(restLength*3);
		pathBySprings.push_back( pathSpring );
	}
    
}

//SURYA MOD: Havent touched this stuff yet
bool CloudsClusterVisualiser::getPathChanged(){
	bool t = pathChanged;
	pathChanged = false;
	return t;
}

bool CloudsClusterVisualiser::isEdgeSelected(){
	return selectedSpring != NULL;
}

bool CloudsClusterVisualiser::isSelectedEdgeLink(){
	return isEdgeSelected() && linkSprings.find(selectedSpring) != linkSprings.end();
}

CloudsClip CloudsClusterVisualiser::getEdgeSource(){
	if( isEdgeSelected() ){
		cout << "birth order " << particleBirthOrder[ selectedSpring->getOneEnd() ] << " " << particleBirthOrder[ selectedSpring->getTheOtherEnd() ] << endl;
		return (particleBirthOrder[ selectedSpring->getOneEnd() ] < particleBirthOrder[ selectedSpring->getTheOtherEnd() ] ?
				particleToClip[ selectedSpring->getOneEnd() ] : particleToClip[ selectedSpring->getTheOtherEnd() ]);
	}
	return CloudsClip();
}

CloudsClip CloudsClusterVisualiser::getEdgeDestination(){
	if( isEdgeSelected() ){
		cout << "birth order " << particleBirthOrder[ selectedSpring->getOneEnd() ] << " " << particleBirthOrder[ selectedSpring->getTheOtherEnd() ] << endl;
		return (particleBirthOrder[ selectedSpring->getOneEnd() ] > particleBirthOrder[ selectedSpring->getTheOtherEnd() ] ?
				particleToClip[ selectedSpring->getOneEnd() ] : particleToClip[ selectedSpring->getTheOtherEnd() ]);
	}
	return CloudsClip();
}

void CloudsClusterVisualiser::linkedEdge(){
	if(isEdgeSelected() && !isSelectedEdgeSuppressed()){
		linkSprings.insert(selectedSpring);
	}
}

void CloudsClusterVisualiser::unlinkEdge(){
	if(isSelectedEdgeLink()){
		linkSprings.erase( linkSprings.find(selectedSpring) );
	}
}

bool CloudsClusterVisualiser::isSelectedEdgeSuppressed(){
	return isEdgeSelected() && suppressedSprings.find(selectedSpring) != suppressedSprings.end();
}

void CloudsClusterVisualiser::suppressEdge(){
	if(isEdgeSelected() && !isSelectedEdgeLink()){
		suppressedSprings.insert(selectedSpring);
	}
}

void CloudsClusterVisualiser::unsuppressEdge(){
	if(isSelectedEdgeSuppressed()){
		suppressedSprings.erase( suppressedSprings.find(selectedSpring) );
	}
}

bool CloudsClusterVisualiser::hasParticle(string tagName){
	return particlesByTag.find(tagName) != particlesByTag.end();
}

void CloudsClusterVisualiser::linkPathEdge(int pathIndex){
	if(pathIndex < pathBySprings.size()){
		if(linkSprings.find(pathBySprings[pathIndex]) == linkSprings.end()){
			linkSprings.insert(pathBySprings[pathIndex]);
		}
		else{
			linkSprings.erase(pathBySprings[pathIndex]);
		}
	}
}

void CloudsClusterVisualiser::suppressPathEdge(int pathIndex){
	if(pathIndex < pathBySprings.size()){
		if(suppressedSprings.find(pathBySprings[pathIndex]) == suppressedSprings.end()){
			cout << "SUPRRESSED CONNECTION in VISUALZIER" << endl;
			suppressedSprings.insert(pathBySprings[pathIndex]);
		}
		else{
			cout << "REMOVING SUPPRESSION in VISUALZIER!" << endl;
			suppressedSprings.erase(pathBySprings[pathIndex]);
		}
	}
}

void CloudsClusterVisualiser::linkLastEdge(){
	linkPathEdge(pathBySprings.size()-1);
}
//SURYA MOD: Havent touched this stuff yet

void CloudsClusterVisualiser::updatePhysics(){
	
 //   physics.update();
	
	if(physics.numberOfParticles() == 0){
		return;
	}
	
	for(int i = 0; i < physics.numberOfParticles(); i++){
		msa::physics::Particle2D* p = physics.getParticle(i);
		dampendPositions[ p ] += ( p->getPosition() - dampendPositions[ p ])*.05;
	}
	
	totalRectangle = ofRectangle(physics.getParticle(0)->getPosition(), 0,0);
	for(int i = 0; i < physics.numberOfParticles(); i++){
		totalRectangle.growToInclude(physics.getParticle(i)->getPosition());
	}
	totalRectangle.scaleFromCenter(1.2, 1.5);
}

void CloudsClusterVisualiser::drawPhysics(){
	
	if(database == NULL){
		return;
	}
	
    if(!font.isLoaded()){
        font.loadFont("materiapro_light.ttf", 15);
    }
    
	ofPushMatrix();
	
	ofRectangle screenRect(0,0,width,height);
	
	float scaleAmount = MIN(screenRect.width/totalRectangle.width,
							screenRect.height/totalRectangle.height);
	
	currentScale += (scaleAmount-currentScale)*.1;
	ofVec2f topCenter = totalRectangle.getTopRight().getInterpolated(totalRectangle.getTopLeft(), .5);
	currentTop += (topCenter-currentTop)*.1;
	
	ofTranslate(ofGetWidth()/2, 0);
	ofScale(currentScale,currentScale);
	ofTranslate(-currentTop);
    for(int i=0; i<database->allClips.size();i++){
        ofSetColor(128);
        //ofSetColor(database->allClips[i].cluster.Color);
        ofCircle(database->allClips[i].cluster.Centre.x*width,database->allClips[i].cluster.Centre.y*height,database->allClips[i].cluster.Radius*10);
        
    }
    ofPushStyle();
//    for(int i = 0; i < physics.numberOfSprings(); i++){
//		ofPushStyle();
//        msa::physics::Spring2D* s = physics.getSpring(i);
//		int numClips = keywordsInSpring[s].size();
//        
//		if(s == hoverSpring || s == selectedSpring){
//			ofxEasingCubic cub;
//			float alpha = (ofGetElapsedTimeMillis() % 750) / 749.;
//			ofSetColor(s == hoverSpring ? hoverColor : selectedColor, (1-alpha)*200);
//			ofSetLineWidth(.5 + ofxTween::map(alpha, 0, 1.0, 1.0, 3.0, true, cub));
//			ofLine(dampendPositions[s->getOneEnd()], dampendPositions[s->getTheOtherEnd()] );
//		}
//        
//		if(linkSprings.find(s) != linkSprings.end()){
//			ofSetColor(hoverColor, 128);
//			ofSetLineWidth(4);
//		}
//		else if(suppressedSprings.find(s) != suppressedSprings.end()){
//			ofSetColor(suppressedColor, 128);
//			ofSetLineWidth(4);
//		}
//		else{
//			ofSetColor(lineColor, ofMap(numClips, 1, 10, 50, 255));
//			ofSetLineWidth( ofMap(springScores[s], minScore, maxScore, 2, 5) );
//		}
//		
//        ofVec2f pos1 = dampendPositions[ s->getOneEnd() ];
//		ofVec2f pos2 = dampendPositions[ s->getTheOtherEnd() ];
//		ofVec2f middle = pos1.getInterpolated(pos2, .5);
//		ofLine(pos1, pos2);
//		
//		ofPopStyle();
//    }
    
    for(int i=0;i<edges.size();i++){
        ofPushStyle();
        ParticleEdge* e = edges[i];
        int numClips = keywordsinEdges[e].size();
        
        if(e == hoverEdge | e == selectedEdge ){
			ofxEasingCubic cub;
			float alpha = (ofGetElapsedTimeMillis() % 750) / 749.;
            ofSetColor(e == hoverEdge ? hoverColor : selectedColor, (1-alpha)*200);
			ofSetLineWidth(.5 + ofxTween::map(alpha, 0, 1.0, 1.0, 3.0, true, cub));
			ofLine(dampendPositions[e->p], dampendPositions[e->a] );
        }
        
        if(linkEdges.find(e)!=linkEdges.end()){
            ofSetColor(hoverColor,128);
            ofSetLineWidth(4);
        }
        else if(suppressedEdges.find(e) != suppressedEdges.end()){
            ofSetColor(suppressedColor,128);
            ofSetLineWidth(4);
        }
        ofVec2f pos1 = dampendPositions[e->a ];
		ofVec2f pos2 = dampendPositions[ e->p ];
		ofVec2f middle = pos1.getInterpolated(pos2, .5);
		ofLine(pos1, pos2);
        ofPopStyle();
    }
	
	ofSetLineWidth(2);
	
    for(int i = 0; i < physics.numberOfParticles(); i++){
        
        msa::physics::Particle2D *a = physics.getParticle(i);
		float radius = radiusForNode(a->getMass());
        CloudsClip& c = particleToClip[a];
		
		if(a == selectedParticle){
			ofSetColor(selectedColor);
		}
		else if(a == hoverParticle){
            ofSetColor(c.cluster.Color);
		//	ofSetColor(selectedColor);
		}
		else if(a == centerNode){
			ofSetColor(c.cluster.Color);
          //  ofSetColor(hoverColor);
		}
		else if(ofContains(currentOptionParticles, a)){
		//	ofSetColor(nodeColor);
         ofSetColor(c.cluster.Color);
		}
		else if(ofContains(pathByParticles, a)){
			ofSetColor(visitedColor);
		}
		else {
			ofSetColor(abandonedColor);
          //  ofSetColor(c.cluster.Color);
		}
		
		ofSetColor(ofGetStyle().color, 180);
		
        ofFill();
        ofCircle(dampendPositions[a], radius);
        
        ofNoFill();
        ofCircle(dampendPositions[a], radius);
        
		if(a == selectedParticle || a == hoverParticle || a == centerNode){
			ofxEasingCubic cub;
			float alpha = (ofGetElapsedTimeMillis() % 750) / 749.;
			ofSetColor(hoverColor, (1-alpha)*200);
			ofCircle(dampendPositions[a], ofxTween::map(alpha, 0, 1.0, radius, radius+10, true, cub) );
		}
    }
    
	ofPushStyle();
	ofSetColor(traceColor, 80);
//	for(int i = 0; i < pathBySprings.size(); i++){
//		ofVec2f pos1 = dampendPositions[ pathBySprings[i]->getOneEnd() ];
//		ofVec2f pos2 = dampendPositions[ pathBySprings[i]->getTheOtherEnd() ];
//		ofVec2f middle = pos1.getInterpolated(pos2, .5);
//		ofLine(pos1, pos2);
//	}
    for(int i = 0; i < pathByEdges.size(); i++){
		ofVec2f pos1 = dampendPositions[ pathByEdges[i]->p ];
		ofVec2f pos2 = dampendPositions[ pathByEdges[i]->a ];
		ofVec2f middle = pos1.getInterpolated(pos2, .5);
		ofLine(pos1, pos2);
	}
	ofPopStyle();
    
	ofCircle( graphPointForScreenPoint( ofVec2f(ofGetMouseX(), ofGetMouseY() )),cursorRadius*currentScale);
	ofPopMatrix();
	
	vector<string>& allKeywords = database->getAllKeywords();
    for(int i = 0; i < physics.numberOfParticles(); i++){
		msa::physics::Particle2D *a = physics.getParticle(i);
		if( a == selectedParticle || a == hoverParticle){
			//ofSetColor(ofColor::fromHsb(215, 255, 255, a->getMass()*5 + 150));
			//ofSetColor(ofColor(20, a->getMass()*5 + 150) );
			ofSetColor(190);
			ofVec2f textPosition = screenPointForGraphPoint(a->getPosition());
			font.drawString(particleName[a], textPosition.x,textPosition.y);
		}
	}
	
//	for(int i = 0; i < physics.numberOfSprings(); i++){
//		msa::physics::Spring2D* s = physics.getSpring(i);
//		if(s == selectedSpring || s == hoverSpring){
//			ofVec2f pos1 = dampendPositions[ s->getOneEnd() ];
//			ofVec2f pos2 = dampendPositions[ s->getTheOtherEnd() ];
//			ofVec2f middle = screenPointForGraphPoint( pos1.getInterpolated(pos2, .5) );
//			string keywordString = "Score: " + ofToString(springScores[s],2) + "\n" + ofJoinString(keywordsInSpring[s], "\n");
//			
//			ofSetColor(220);
//			font.drawString(keywordString, middle.x, middle.y);
//		}
//	}
	
    for(int i=0;i<edges.size();i++){
        ParticleEdge* e =edges[i];
        if(e==selectedEdge||e==hoverEdge){
            ofVec2f pos1 = dampendPositions[e->p];
            ofVec2f pos2 = dampendPositions[e->a];
            ofVec2f middle = screenPointForGraphPoint( pos1.getInterpolated(pos2, .5) );
			string keywordString = "Score: " + ofToString(edgeScores[e],2) + "\n" + ofJoinString(keywordsinEdges[e], "\n");
			
			ofSetColor(220);
			font.drawString(keywordString, middle.x, middle.y);
            
        }
    }
	for(int i = 0; i < clipLog.size(); i++){
		float drawHeight = ofGetHeight() - font.getLineHeight() * (i+2);
		ofSetColor(255, ofMap(i,2, 6, 255, 0, true));
		font.drawString(clipLog[clipLog.size() - i - 1], 30, drawHeight - 30);
	}
	
	ofPopStyle();
}


ofVec2f CloudsClusterVisualiser::graphPointForScreenPoint(ofVec2f screenPoint){
	return ( (screenPoint - ofVec2f(ofGetWidth()/2, 0)) / currentScale) + currentTop;
}

ofVec2f CloudsClusterVisualiser::screenPointForGraphPoint(ofVec2f graphPoint){
	return (graphPoint - currentTop) * currentScale + ofVec2f(ofGetWidth()/2, 0);
}

msa::physics::Particle2D* CloudsClusterVisualiser::particleNearPoint(ofVec2f point){
	
	for(int i = 0; i < physics.numberOfParticles(); i++){
		float particleRadiusSquared = powf(radiusForNode( physics.getParticle(i)->getMass() ), 2);
		if(point.squareDistance( dampendPositions[ physics.getParticle(i) ] ) < (particleRadiusSquared + cursorRadius*cursorRadius) ){
			return physics.getParticle(i);
		}
	}
	return NULL;
}

msa::physics::Spring2D* CloudsClusterVisualiser::springNearPoint(ofVec2f point){
    
	for(int i = 0; i < physics.numberOfSprings(); i++){
		msa::physics::Spring2D* spring = physics.getSpring( i );
		ofVec3f p = ofVec3f(point.x, point.y, 0);
		ofVec3f a = ofVec3f(dampendPositions[ physics.getSpring( i )->getOneEnd()].x,
							dampendPositions[ physics.getSpring( i )->getOneEnd()].y,0);
		ofVec3f b = ofVec3f(dampendPositions[ physics.getSpring( i )->getTheOtherEnd()].x,
							dampendPositions[ physics.getSpring( i )->getTheOtherEnd()].y,0);
		float distance;
		if( DistancePointLine(p, a, b, distance) && distance < cursorRadius){
			return spring;
		}
	}
	return NULL;
}

ParticleEdge* CloudsClusterVisualiser:: edgeNearPoint(ofVec2f point){
    for(int i =0; i<edges.size();i++){
        ParticleEdge* edge = edges[i];
        ofVec3f p  = ofVec3f(point.x, point.y,0);
        ofVec3f a = ofVec3f(dampendPositions[edge->a].x,dampendPositions[edge->a].y,0);
        ofVec3f b = ofVec3f(dampendPositions[edge->p].x,dampendPositions[edge->p].y,0);
        float distance;
        if( DistancePointLine(p, a, b, distance) && distance < cursorRadius){
            return edge;
        }
    }

    return NULL;
    
}

float CloudsClusterVisualiser::radiusForNode( float mass ){
	return ofMap(mass, minMass, maxMass, minRadius, maxRadius);
}


void CloudsClusterVisualiser::mousePressed(ofMouseEventArgs& args){
    
	selectedSpring = NULL;
    selectedEdge = NULL;
	selectedParticle = particleNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
	if(selectedParticle == NULL){
		selectedSpring = springNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
        selectedEdge = edgeNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
	}
}

void CloudsClusterVisualiser::mouseMoved(ofMouseEventArgs& args){
	hoverSpring = NULL;
    hoverEdge = NULL;
	hoverParticle = particleNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );

	if(hoverParticle != NULL){
		selectionTitle = "Clips with " + particleName[ hoverParticle ];
	}
	else{
		hoverSpring = springNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
        hoverEdge = edgeNearPoint(graphPointForScreenPoint(ofVec2f(args.x, args.y)));
	}
	
    
}

void CloudsClusterVisualiser::mouseDragged(ofMouseEventArgs& args){
	
}

void CloudsClusterVisualiser::mouseReleased(ofMouseEventArgs& args){
	
}

void CloudsClusterVisualiser::keyPressed(ofKeyEventArgs& args){
    
}

void CloudsClusterVisualiser::keyReleased(ofKeyEventArgs& args){
	
}

void CloudsClusterVisualiser::windowResized(ofResizeEventArgs& args){
	width  = args.width;
	height = args.height;
	physics.setWorldMax(ofVec2f(width*10,height*10));
}
