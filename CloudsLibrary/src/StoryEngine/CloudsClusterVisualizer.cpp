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
    minMass = 0;
    maxMass = 1;
    minScore = 0;
    maxScore = 1;
    cursorRadius = 1;
    zoom =1;
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
    zoomView = false;
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


void CloudsClusterVisualiser::clipBegan(CloudsStoryEventArgs &args){
    zoomView =false;

    vector<msa::physics::Particle2D*> newParticles;
	//int particleStartIndex = physics.numberOfParticles();
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
	
	for(int i = 0; i < args.clipOptions.size(); i++){
		maxScore = MAX(maxScore, args.clipOptions[i].currentScore);
		minScore = MIN(minScore, args.clipOptions[i].currentScore);
	}
	
	msa::physics::Particle2D* p;
	if(hasParticle( mainLinkName )){
		p = particlesByTag[ mainLinkName];
	}
	else{
        ofVec2f scaledPos = args.chosenClip.cluster.Centre;
        p = physics.makeParticle(scaledPos);
        cout << "POSITION " << scaledPos << " CLIP " << args.chosenClip.getLinkName() << endl;
        
        dampendPositions[p] = p->getPosition();
		
        p->setMass(args.chosenClip.cluster.Radius*10);
		//p->setMass(args.chosenClip.cluster.Radius);
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
	cout<<"Center Position changed: "<<centerNode->getPosition()<<endl;
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
            ofVec2f optionPos = relatedClip.cluster.Centre;
            
            cout << "POSITION " << optionPos << " CLIP " << relatedClip.getLinkName() << endl;
            
			a = physics.makeParticle(optionPos);
			dampendPositions[a] =  a->getPosition();
			particleBirthOrder[a] = physics.numberOfParticles();
            ///            int mass = relatedClip.cluster.Radius*10;
            int mass = relatedClip.cluster.Radius;
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
			//use clips in common to weight the lines
            float length = ofDist(p->getPosition().x, p->getPosition().y, a->getPosition().x, a->getPosition().y);
            
            ParticleEdge* e = new ParticleEdge(p,a,length);
            edgeScores[e] =args.clipOptions[i].currentScore;
            edgePairs[make_pair(p, a)]= e;
            edges.push_back(e);
            keywordsinEdges[e] = database->getSharedKeywords(args.chosenClip, relatedClip);
            
		}
		else if(isLink) {
            if(edgePairs.find( make_pair(a, p) ) != edgePairs.end()) linkEdges.insert(edgePairs[ make_pair(a, p)]);
			else if(edgePairs.find( make_pair(p, a) ) != edgePairs.end()) linkEdges.insert(edgePairs[ make_pair(p, a)]);
		}
		else if(isSuppressed){
            if(edgePairs.find( make_pair(a, p) ) != edgePairs.end()) suppressedEdges.insert(edgePairs[ make_pair(a, p)]);
			else if(edgePairs.find( make_pair(p, a) ) != edgePairs.end()) suppressedEdges.insert(edgePairs[ make_pair(p, a)]);
            
		}
	}
    
    
	if(oldCenter != NULL){
        ParticleEdge* pathEdge = edgePairs[make_pair(oldCenter, centerNode)];
        pathEdge->setLength(ofDist(oldCenter->getPosition().x, oldCenter->getPosition().y, centerNode->getPosition().x, centerNode->getPosition().y));
        pathByEdges.push_back(pathEdge);
	}
    
}

bool CloudsClusterVisualiser::getPathChanged(){
	bool t = pathChanged;
	pathChanged = false;
	return t;
}

bool CloudsClusterVisualiser::isEdgeSelected(){
    return selectedEdge !=NULL;
}

bool CloudsClusterVisualiser::isSelectedEdgeLink(){
	
    return isEdgeSelected() && linkEdges.find(selectedEdge) != linkEdges.end();
}

CloudsClip CloudsClusterVisualiser::getEdgeSource(){
	if( isEdgeSelected() ){
		cout << "birth order " << particleBirthOrder[ selectedEdge->p ] << " " << particleBirthOrder[ selectedEdge->a ] << endl;
		return (particleBirthOrder[ selectedEdge->p ] < particleBirthOrder[ selectedEdge->a ] ?
				particleToClip[ selectedEdge->p ] : particleToClip[ selectedEdge->a ]);
	}
	return CloudsClip();
}

CloudsClip CloudsClusterVisualiser::getEdgeDestination(){
	if( isEdgeSelected() ){
		cout << "birth order " << particleBirthOrder[ selectedEdge->p ] << " " << particleBirthOrder[ selectedEdge->a ] << endl;
		return (particleBirthOrder[ selectedEdge->p ] > particleBirthOrder[ selectedEdge->a ] ?
				particleToClip[ selectedEdge->p ] : particleToClip[ selectedEdge->a ]);
	}
	return CloudsClip();
}

void CloudsClusterVisualiser::linkedEdge(){
	if(isEdgeSelected() && !isSelectedEdgeSuppressed()){
		linkEdges.insert(selectedEdge);
	}
}

void CloudsClusterVisualiser::unlinkEdge(){
	if(isSelectedEdgeLink()){
		linkEdges.erase( linkEdges.find(selectedEdge) );
	}
}

bool CloudsClusterVisualiser::isSelectedEdgeSuppressed(){
	return isEdgeSelected() && suppressedEdges.find(selectedEdge) != suppressedEdges.end();
}

void CloudsClusterVisualiser::suppressEdge(){
	if(isEdgeSelected() && !isSelectedEdgeLink()){
		suppressedEdges.insert(selectedEdge);
	}
}

void CloudsClusterVisualiser::unsuppressEdge(){
	if(isSelectedEdgeSuppressed()){
		suppressedEdges.erase( suppressedEdges.find(selectedEdge) );
	}
}

bool CloudsClusterVisualiser::hasParticle(string tagName){
	return particlesByTag.find(tagName) != particlesByTag.end();
}

void CloudsClusterVisualiser::linkPathEdge(int pathIndex){
	if(pathIndex < pathByEdges.size()){
		if(linkEdges.find(pathByEdges[pathIndex]) == linkEdges.end()){
			linkEdges.insert(pathByEdges[pathIndex]);
		}
		else{
			linkEdges.erase(pathByEdges[pathIndex]);
		}
	}
}

void CloudsClusterVisualiser::suppressPathEdge(int pathIndex){
	if(pathIndex < pathByEdges.size()){
		if(suppressedEdges.find(pathByEdges[pathIndex]) == suppressedEdges.end()){
			cout << "SUPRRESSED CONNECTION in VISUALZIER" << endl;
			suppressedEdges.insert(pathByEdges[pathIndex]);
		}
		else{
			cout << "REMOVING SUPPRESSION in VISUALZIER!" << endl;
			suppressedEdges.erase(pathByEdges[pathIndex]);
		}
	}
}

void CloudsClusterVisualiser::linkLastEdge(){
	linkPathEdge(pathByEdges.size()-1);
}

void CloudsClusterVisualiser::updatePhysics(){
	
	if(physics.numberOfParticles() == 0){
		return;
	}
    
	for(int i = 0; i < physics.numberOfParticles(); i++){
		msa::physics::Particle2D* p = physics.getParticle(i);
		dampendPositions[ p ] += ( p->getPosition() - dampendPositions[ p ])*.05;
	}
	//totalRectangle = ofRectangle(centerNode->getPosition(),0,0);
    
    totalRectangle = ofRectangle(physics.getParticle(0)->getPosition(), 0,0);
	for(int i = 0; i < physics.numberOfParticles(); i++){
        
		totalRectangle.growToInclude(physics.getParticle(i)->getPosition());
	}
	totalRectangle.scaleFromCenter(1.2, 1.5);
    
    
    zoomRectangle = ofRectangle(centerNode->getPosition(),0,0);
    
	for(int i = 0; i < physics.numberOfParticles(); i++){
        float dist = ofDist(centerNode->getPosition().x,centerNode->getPosition().y,physics.getParticle(i)->getPosition().x,physics.getParticle(i)->getPosition().y);
        
        if(dist<zoom){
            zoomRectangle.growToInclude(physics.getParticle(i)->getPosition());
        }
        
		
	}
	zoomRectangle.scaleFromCenter(1.2, 1.5);
    
    
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
    
    //	float scaleAmount = MIN(screenRect.width/totalRectangle.width,
    //							screenRect.height/totalRectangle.height);
    //
    //	currentScale += (scaleAmount-currentScale)*.1;
    //	ofVec2f topCenter = totalRectangle.getTopRight().getInterpolated(totalRectangle.getTopLeft(), .5);
    //	currentTop += (topCenter-currentTop)*.1;
    //
    //	ofTranslate(ofGetWidth()/2, 0);
    //	ofScale(currentScale,currentScale);
    //	ofTranslate(-currentTop);
    //
    if(zoomView){
        float  scaleAmount = MIN(screenRect.width/zoomRectangle.width,
                                 screenRect.height/zoomRectangle.height);
        
        currentScale += (scaleAmount-currentScale)*.1;
        ofVec2f topCenter = zoomRectangle.getTopRight().getInterpolated(zoomRectangle.getTopLeft(), .5);
        currentTop += (topCenter-currentTop)*.1;
        
        ofTranslate(ofGetWidth()/2, 0);
        ofScale(currentScale,currentScale);
        ofTranslate(-currentTop);
    }
    else{
        float scaleAmount = MIN(screenRect.width/totalRectangle.width,
                                screenRect.height/totalRectangle.height);
        
        currentScale += (scaleAmount-currentScale)*.1;
        ofVec2f topCenter = totalRectangle.getTopRight().getInterpolated(totalRectangle.getTopLeft(), .5);
        currentTop += (topCenter-currentTop)*.1;
        
        ofTranslate(ofGetWidth()/2, 0);
        ofScale(currentScale,currentScale);
        ofTranslate(-currentTop);
    }
    
    for(int i=0; i<database->getAllClips().size();i++){
        ofSetColor(128);
        ofCircle(database->getAllClips()[i].cluster.Centre,
                 database->getAllClips()[i].cluster.Radius / currentScale);
        
        
    }
    ofPushStyle();
    
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
			ofSetColor(c.cluster.Color);
		}
		else if(a == hoverParticle){
            ofSetColor(c.cluster.Color);
            //	ofSetColor(selectedColor);
		}
		else if(a == centerNode){
			ofSetColor(selectedColor);
            //  ofSetColor(hoverColor);
		}
		else if(ofContains(currentOptionParticles, a)){
            ofSetColor(nodeColor);
            //ofSetColor(c.cluster.Color);
		}
		else if(ofContains(pathByParticles, a)){
			ofSetColor(visitedColor);
		}
		else {
			ofSetColor(0,0);
            //ofSetColor(abandonedColor);
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
            float rad = ofxTween::map(alpha, 0, 1, cursorRadius/currentScale, (cursorRadius/currentScale)+10, true, cub)/currentScale;
            ofCircle(dampendPositions[a],rad );
//            if(ofGetKeyPressed()){
//                cout<<ofxTween::map(alpha, 0, 1, cursorRadius/currentScale, (cursorRadius/currentScale)+1, true, cub)<<endl;
//            }
		}
    }
	ofPushStyle();
	ofSetColor(traceColor, 80);
    for(int i = 0; i < pathByEdges.size(); i++){
		ofVec2f pos1 = dampendPositions[ pathByEdges[i]->p ];
		ofVec2f pos2 = dampendPositions[ pathByEdges[i]->a ];
		ofVec2f middle = pos1.getInterpolated(pos2, .5);
		ofLine(pos1, pos2);
	}
	ofPopStyle();
	ofPushStyle();
	ofSetColor(255, 0, 0);
    ofCircle( graphPointForScreenPoint( ofVec2f(ofGetMouseX(), ofGetMouseY() )),cursorRadius/currentScale);
    ofPopStyle();
	ofPopMatrix();
	
	vector<string>& allKeywords = database->getAllKeywords();
    for(int i = 0; i < physics.numberOfParticles(); i++){
		msa::physics::Particle2D *a = physics.getParticle(i);
		if( a == selectedParticle || a == hoverParticle){
			ofSetColor(190);
			ofVec2f textPosition = screenPointForGraphPoint(a->getPosition());
			font.drawString(particleName[a], textPosition.x,textPosition.y);
		}
	}
	ofPushStyle();
    for(int i=0;i<edges.size();i++){
        ParticleEdge* e =edges[i];
        if(e==selectedEdge||e==hoverEdge){
            ofVec2f pos1 = dampendPositions[e->p];
            ofVec2f pos2 = dampendPositions[e->a];
            ofVec2f middle = screenPointForGraphPoint( pos1.getInterpolated(pos2, .5) );
			string keywordString = "Score: " + ofToString(edgeScores[e],2) + "\n" + ofJoinString(keywordsinEdges[e], "\n");
            
			ofSetColor(220);
			font.drawString(keywordString, middle.x/currentScale, middle.y/currentScale);
            
            
        }
    }
    //cout<<"clipLog"<<endl;
	for(int i = 0; i < clipLog.size(); i++){
		float drawHeight = (ofGetHeight() - font.getLineHeight() * (i+2))/currentScale;
		ofSetColor(255, ofMap(i,2, 6, 255, 0, true));
		font.drawString(clipLog[clipLog.size() - i - 1], 30, drawHeight - 30);
	}
	
	ofPopStyle();
}


ofVec2f CloudsClusterVisualiser::graphPointForScreenPoint(ofVec2f screenPoint){
	
    return ( (screenPoint - ofVec2f(ofGetWidth()/2, 0)) / currentScale) + currentTop;
}

ofVec2f CloudsClusterVisualiser::screenPointForGraphPoint(ofVec2f graphPoint){
	return (graphPoint - currentTop) /currentScale + ofVec2f(ofGetWidth()/2, 0);
}

msa::physics::Particle2D* CloudsClusterVisualiser::particleNearPoint(ofVec2f point){
	
	for(int i = 0; i < physics.numberOfParticles(); i++){
  		float particleRadiusSquared = powf(radiusForNode( physics.getParticle(i)->getMass() ), 2);
		if(point.squareDistance( dampendPositions[ physics.getParticle(i) ] ) < (particleRadiusSquared + cursorRadius*cursorRadius/currentScale) ){
			return physics.getParticle(i);
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
        if( DistancePointLine(p, a, b, distance) && distance < cursorRadius/currentScale){
            return edge;
        }
    }
    return NULL;
}

float CloudsClusterVisualiser::radiusForNode( float mass ){
    //cout<<"radius for node"<<endl;
	return ofMap(mass, minMass, maxMass, minRadius, maxRadius) / currentScale;
}


void CloudsClusterVisualiser::mousePressed(ofMouseEventArgs& args){
    
	selectedSpring = NULL;
    selectedEdge = NULL;
	selectedParticle = particleNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
    
	if(selectedParticle == NULL){
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
        hoverEdge = edgeNearPoint(graphPointForScreenPoint(ofVec2f(args.x, args.y)));
	}
	
    
}

void CloudsClusterVisualiser::mouseDragged(ofMouseEventArgs& args){
	
}

void CloudsClusterVisualiser::mouseReleased(ofMouseEventArgs& args){
	
}

void CloudsClusterVisualiser::keyPressed(ofKeyEventArgs& args){
    cout<<"CURRENT NODE:"<<particleName[centerNode]<<endl;
    if(args.key == 'z'){
        zoomView = !zoomView;
        cout<<"Zoom View: "<<zoomView<<endl;
    }
    
    
    
}

void CloudsClusterVisualiser::keyReleased(ofKeyEventArgs& args){
	
}

void CloudsClusterVisualiser::windowResized(ofResizeEventArgs& args){
	width  = args.width;
	height = args.height;
	physics.setWorldMax(ofVec2f(width*10,height*10));
}

void CloudsClusterVisualiser::drawGrid(){
    
    if(!font.isLoaded()){
        font.loadFont("materiapro_light.ttf", 8);
    }
    
    ofPushStyle();
    ofSetColor(255);
    int size = 15;
    int color = 40;
    int leftX = 200;
    int topY = 40;
    vector<string>& allKeywords = database->getAllKeywords();
    for(int y = 0; y < MIN(allKeywords.size(), 50); y++){
        
        ofSetColor(255);
        font.drawString(allKeywords[y], leftX - font.stringWidth(allKeywords[y]), topY + y*size+size/2+4);
        int x;
        for(x = 0; x <= y; x++){
            int connections = sharedClips[ make_pair(allKeywords[x], allKeywords[y]) ];
            ofRectangle drawRect(leftX + x*size, topY + y*size, size, size);
            if(connections > 0){
                ofFill();
                ofSetColor(ofColor::fromHsb(color, connections*40, connections*40));
                ofRect(drawRect);
                
                ofSetColor(ofColor::fromHsb(255-color, 255, 255));
                font.drawString(ofToString(connections), leftX + x*size + 2, topY + y*size+size/2+4);
            }
            else{
                ofSetColor(255, 30);
                ofNoFill();
                ofRect(drawRect);
            }
        }
		
        font.drawString(allKeywords[y], leftX + (x-1)*size + 2, topY + (y-1)*size+size/2+4);
    }
    
    ofPopStyle();
}

