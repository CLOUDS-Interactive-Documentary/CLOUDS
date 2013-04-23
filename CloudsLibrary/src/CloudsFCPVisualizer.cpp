
#include "CloudsFCPVisualizer.h"

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

CloudsFCPVisualizer::CloudsFCPVisualizer(){
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

void CloudsFCPVisualizer::storyBegan(CloudsStoryEventArgs& clips){
	clear();
}

void CloudsFCPVisualizer::clear(){
	
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

void CloudsFCPVisualizer::setup(CloudsFCPParser& dbref){

	this->database = &dbref;
	
	ofRegisterMouseEvents(this);
    ofRegisterKeyEvents(this);
	ofSetCircleResolution(200);
	
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

void CloudsFCPVisualizer::setupPhysics(){
	physics.clear();
    
    width = ofGetWidth();
	height = ofGetHeight();
    
    cout << "width and height " << width << " " << height << endl;
    
	physics.setWorldSize(ofVec2f(0,0), ofVec2f(width*10,height*10));
	physics.setSectorCount(1);
    physics.setDrag(0.1f);
}


//void CloudsFCPVisualizer::addLinksToPhysics(CloudsClip& center,
//											vector<CloudsClip>& connections,
//											vector<float>& scores)
//{

void CloudsFCPVisualizer::clipChanged(CloudsStoryEventArgs& args){

	vector<msa::physics::Particle2D*> newParticles;
	int particleStartIndex = physics.numberOfParticles();
	string mainLinkName = args.chosenClip.getLinkName();
	clipLog.push_back(mainLinkName);
	
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
		p = physics.makeParticle(ofVec2f(width/2, height/2));
		dampendPositions[p] = p->getPosition();
		p->makeFixed();
		p->setMass(args.chosenClip.keywords.size());
		
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
			a = physics.makeParticle(p->getPosition() + ofVec2f(ofRandom(-2,2), ofRandom(-2,2)));
			dampendPositions[a] = p->getPosition();
			particleBirthOrder[a] = physics.numberOfParticles();
			int mass = relatedClip.keywords.size();
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

		if(springs.find( make_pair(a, p) ) == springs.end() &&
		   springs.find( make_pair(p, a) ) == springs.end() ){
			//use clips in common to weight the lines
			msa::physics::Spring2D* newSpring = physics.makeSpring(a, p, springStrength, restLength );
			springScores[newSpring] = args.clipOptions[i].currentScore;
			springs[ make_pair(p, a) ] = newSpring;
			keywordsInSpring[ newSpring ] = database->getSharedKeywords(args.chosenClip, relatedClip);
			if(isLink){
				linkSprings.insert(newSpring);
			}
			else if(isSuppressed){
				suppressedSprings.insert(newSpring);
			}
		}
		else if(isLink) {
			if(springs.find( make_pair(a, p) ) != springs.end()) linkSprings.insert(springs[ make_pair(a, p)]);
			else if(springs.find( make_pair(p, a) ) != springs.end()) linkSprings.insert(springs[ make_pair(p, a)]);
		}
		else if(isSuppressed){
			if(springs.find( make_pair(a, p) ) != springs.end()) suppressedSprings.insert(springs[ make_pair(a, p)]);
			else if(springs.find( make_pair(p, a) ) != springs.end()) suppressedSprings.insert(springs[ make_pair(p, a)]);			
		}
	}

	//repel existing particles
	for(int i = 0; i < newParticles.size(); i++){
		for(int j = 0; j < particleStartIndex; j++){
			physics.makeAttraction(newParticles[i], physics.getParticle(j), -repulsionForce);
		}
	}
	
	//repel eachother
	for(int i = 0; i < newParticles.size(); i++){
		for(int j = 0; j < newParticles.size(); j++){
			if(j != i){
				physics.makeAttraction(newParticles[i], newParticles[j], -repulsionForce);
			}
		}
	}
	
	if(oldCenter != NULL){
		msa::physics::Spring2D* pathSpring = springs[make_pair(oldCenter, centerNode)];
		pathSpring->setRestLength(restLength*3);
		pathBySprings.push_back( pathSpring );
	}
}

bool CloudsFCPVisualizer::getPathChanged(){
	bool t = pathChanged;
	pathChanged = false;
	return t;
}

bool CloudsFCPVisualizer::isEdgeSelected(){
	return selectedSpring != NULL;
}

bool CloudsFCPVisualizer::isSelectedEdgeLink(){
	return isEdgeSelected() && linkSprings.find(selectedSpring) != linkSprings.end();
}

CloudsClip CloudsFCPVisualizer::getEdgeSource(){
	if( isEdgeSelected() ){
		cout << "birth order " << particleBirthOrder[ selectedSpring->getOneEnd() ] << " " << particleBirthOrder[ selectedSpring->getTheOtherEnd() ] << endl;
		return (particleBirthOrder[ selectedSpring->getOneEnd() ] < particleBirthOrder[ selectedSpring->getTheOtherEnd() ] ?
				particleToClip[ selectedSpring->getOneEnd() ] : particleToClip[ selectedSpring->getTheOtherEnd() ]);
	}
	return CloudsClip();
}

CloudsClip CloudsFCPVisualizer::getEdgeDestination(){
	if( isEdgeSelected() ){
		cout << "birth order " << particleBirthOrder[ selectedSpring->getOneEnd() ] << " " << particleBirthOrder[ selectedSpring->getTheOtherEnd() ] << endl;		
		return (particleBirthOrder[ selectedSpring->getOneEnd() ] > particleBirthOrder[ selectedSpring->getTheOtherEnd() ] ?
				particleToClip[ selectedSpring->getOneEnd() ] : particleToClip[ selectedSpring->getTheOtherEnd() ]);
	}
	return CloudsClip();
}

void CloudsFCPVisualizer::linkedEdge(){
	if(isEdgeSelected() && !isSelectedEdgeSuppressed()){
		linkSprings.insert(selectedSpring);
	}
}

void CloudsFCPVisualizer::unlinkEdge(){
	if(isSelectedEdgeLink()){
		linkSprings.erase( linkSprings.find(selectedSpring) );
	}
}

bool CloudsFCPVisualizer::isSelectedEdgeSuppressed(){
	return isEdgeSelected() && suppressedSprings.find(selectedSpring) != suppressedSprings.end();
}

void CloudsFCPVisualizer::suppressEdge(){
	if(isEdgeSelected() && !isSelectedEdgeLink()){
		suppressedSprings.insert(selectedSpring);
	}
}

void CloudsFCPVisualizer::unsuppressEdge(){
	if(isSelectedEdgeSuppressed()){
		suppressedSprings.erase( suppressedSprings.find(selectedSpring) );
	}
}

bool CloudsFCPVisualizer::hasParticle(string tagName){
	return particlesByTag.find(tagName) != particlesByTag.end();
}

void CloudsFCPVisualizer::linkPathEdge(int pathIndex){
	if(pathIndex < pathBySprings.size()){
		if(linkSprings.find(pathBySprings[pathIndex]) == linkSprings.end()){
			linkSprings.insert(pathBySprings[pathIndex]);
		}
		else{
			linkSprings.erase(pathBySprings[pathIndex]);
		}
	}
}

void CloudsFCPVisualizer::suppressPathEdge(int pathIndex){
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

void CloudsFCPVisualizer::linkLastEdge(){
	linkPathEdge(pathBySprings.size()-1);	
}

void CloudsFCPVisualizer::suppressLastEdge(){
	suppressPathEdge(pathBySprings.size()-1);	
}

void CloudsFCPVisualizer::updatePhysics(){
	
    physics.update();
	
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

void CloudsFCPVisualizer::drawPhysics(){
	
	if(database == NULL){
		return;
	}
	
    if(!font.isLoaded()){
        font.loadFont("mplus-1c-regular.ttf", 15);
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
	
    ofPushStyle();
    for(int i = 0; i < physics.numberOfSprings(); i++){
		ofPushStyle();
        msa::physics::Spring2D* s = physics.getSpring(i);
		int numClips = keywordsInSpring[s].size();

		if(s == hoverSpring || s == selectedSpring){
			ofxEasingCubic cub;
			float alpha = (ofGetElapsedTimeMillis() % 750) / 749.;
			ofSetColor(s == hoverSpring ? hoverColor : selectedColor, (1-alpha)*200);
			ofSetLineWidth(.5 + ofxTween::map(alpha, 0, 1.0, 1.0, 3.0, true, cub));
			ofLine(dampendPositions[s->getOneEnd()], dampendPositions[s->getTheOtherEnd()] );
		}

		if(linkSprings.find(s) != linkSprings.end()){
			ofSetColor(hoverColor, 128);
			ofSetLineWidth(4);
		}
		else if(suppressedSprings.find(s) != suppressedSprings.end()){
			ofSetColor(suppressedColor, 128);
			ofSetLineWidth(4);
		}
		else{
			ofSetColor(lineColor, ofMap(numClips, 1, 10, 50, 255));
			ofSetLineWidth( ofMap(springScores[s], minScore, maxScore, 2, 5) );
		}
		
        ofVec2f pos1 = dampendPositions[ s->getOneEnd() ];
		ofVec2f pos2 = dampendPositions[ s->getTheOtherEnd() ];
		ofVec2f middle = pos1.getInterpolated(pos2, .5);
		ofLine(pos1, pos2);
		
		ofPopStyle();
    }
	
	ofSetLineWidth(2);
	
    for(int i = 0; i < physics.numberOfParticles(); i++){
        
        msa::physics::Particle2D *a = physics.getParticle(i);
		float radius = radiusForNode(a->getMass());
		
		
		if(a == selectedParticle){
			ofSetColor(selectedColor);
		}
		else if(a == hoverParticle){
			ofSetColor(selectedColor);
		}
		else if(a == centerNode){
			ofSetColor(hoverColor);
		}
		else if(ofContains(currentOptionParticles, a)){
			ofSetColor(nodeColor);
		}
		else if(ofContains(pathByParticles, a)){
			ofSetColor(visitedColor);
		}
		else {
			ofSetColor(abandonedColor);
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
	for(int i = 0; i < pathBySprings.size(); i++){
		ofVec2f pos1 = dampendPositions[ pathBySprings[i]->getOneEnd() ];
		ofVec2f pos2 = dampendPositions[ pathBySprings[i]->getTheOtherEnd() ];
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
	
	for(int i = 0; i < physics.numberOfSprings(); i++){
		msa::physics::Spring2D* s = physics.getSpring(i);
		if(s == selectedSpring || s == hoverSpring){
			ofVec2f pos1 = dampendPositions[ s->getOneEnd() ];
			ofVec2f pos2 = dampendPositions[ s->getTheOtherEnd() ];
			ofVec2f middle = screenPointForGraphPoint( pos1.getInterpolated(pos2, .5) );
			string keywordString = "Score: " + ofToString(springScores[s],2) + "\n" + ofJoinString(keywordsInSpring[s], "\n");
			
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

ofVec2f CloudsFCPVisualizer::graphPointForScreenPoint(ofVec2f screenPoint){
	return ( (screenPoint - ofVec2f(ofGetWidth()/2, 0)) / currentScale) + currentTop;
}

ofVec2f CloudsFCPVisualizer::screenPointForGraphPoint(ofVec2f graphPoint){
	return (graphPoint - currentTop) * currentScale + ofVec2f(ofGetWidth()/2, 0);
}

msa::physics::Particle2D* CloudsFCPVisualizer::particleNearPoint(ofVec2f point){
	
	for(int i = 0; i < physics.numberOfParticles(); i++){
		float particleRadiusSquared = powf(radiusForNode( physics.getParticle(i)->getMass() ), 2);
		if(point.squareDistance( dampendPositions[ physics.getParticle(i) ] ) < (particleRadiusSquared + cursorRadius*cursorRadius) ){
			return physics.getParticle(i);
		}
	}
	return NULL;
}

msa::physics::Spring2D* CloudsFCPVisualizer::springNearPoint(ofVec2f point){

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

float CloudsFCPVisualizer::radiusForNode( float mass ){
	return ofMap(mass, minMass, maxMass, minRadius, maxRadius);
}

void CloudsFCPVisualizer::mousePressed(ofMouseEventArgs& args){

	selectedSpring = NULL;
	selectedParticle = particleNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
	if(selectedParticle == NULL){
		selectedSpring = springNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
	}
}

void CloudsFCPVisualizer::mouseMoved(ofMouseEventArgs& args){
	hoverSpring = NULL;
	hoverParticle = particleNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
	if(hoverParticle != NULL){
		selectionTitle = "Clips with " + particleName[ hoverParticle ];
	}
	else{
		hoverSpring = springNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
	}
	
	/*
	selectionTitle = "";
	selectedClips.clear();

	if(hoverParticle != NULL){
		selectionTitle = "Clips with " + particleName[ hoverParticle ];
//		selectedClips = database->getClipsWithKeyword( particleName[ hoverParticle ] );
	}
	else{
		hoverSpring = springNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
		if(hoverSpring != NULL){
			string keyA = particleName[ hoverSpring->getOneEnd() ];
			string keyB = particleName[ hoverSpring->getTheOtherEnd() ];
			selectionTitle = "Clips sharing " + keyA + " and " + keyB;
//			selectedClips = database->getSharedClips(keyA,keyB);
		}
		else if(selectedParticle != NULL){
			selectionTitle = "Clips with " + particleName[ selectedParticle ];
//			selectedClips = database->getClipsWithKeyword( particleName[ selectedParticle ] );
		}
		else if(selectedSpring != NULL){
			string keyA = particleName[ selectedSpring->getOneEnd() ];
			string keyB = particleName[ selectedSpring->getTheOtherEnd() ];
			selectionTitle = "Clips sharing " + keyA + " and " + keyB;
//			selectedClips = database->getSharedClips(keyA,keyB);
		}
	}
	*/
}

void CloudsFCPVisualizer::mouseDragged(ofMouseEventArgs& args){
	
}

void CloudsFCPVisualizer::mouseReleased(ofMouseEventArgs& args){
	
}

void CloudsFCPVisualizer::keyPressed(ofKeyEventArgs& args){

}

void CloudsFCPVisualizer::keyReleased(ofKeyEventArgs& args){
	
}

void CloudsFCPVisualizer::windowResized(ofResizeEventArgs& args){
	width  = args.width;
	height = args.height;
	physics.setWorldMax(ofVec2f(width*10,height*10));
}


#pragma mark UNUSED!!
void CloudsFCPVisualizer::createClusterPhysics(){
    
    //make a particle for each keyword
    //for each clip, create a spring for each other clip it shares a keyword with based on the number of shared words
    if(database == NULL){
        return;
    }
    
    database->sortKeywordsByOccurrence(true);
    vector<string>& allKeywords = database->getAllKeywords();
//    cout << " total of " << allKeywords.size() << endl;
    
    for(int i = 0; i < allKeywords.size(); i++){
        msa::physics::Particle2D *p = physics.makeParticle(ofVec2f(ofRandomWidth(), ofRandomHeight()));
        p->setMass(database->occurrencesOfKeyword(allKeywords[i])*10);
        if(i == 0){
            p->moveTo(ofVec2f(width/2,height/2));
            p->makeFixed();
        }
    }
    
    vector<CloudsClip>& clips = database->getAllClips();
    for(int k = 0; k < allKeywords.size(); k++){
		
        for(int i = k+1; i < allKeywords.size(); i++){
            int clipsInCommon = 0;
			
            for(int c = 0; c < clips.size(); c++){
                if(ofContains(clips[c].keywords, allKeywords[k]) &&
                   ofContains(clips[c].keywords, allKeywords[i]))
                {
                    clipsInCommon++;
                }
            }
            
            if(clipsInCommon > 1){
                msa::physics::Particle2D *a = physics.getParticle(k);
                msa::physics::Particle2D *b = physics.getParticle(i);
                physics.makeSpring(a, b, .005 * powf(clipsInCommon,2), 20 );
                physics.makeAttraction(a, b, -50);
                
                //cout << allKeywords[k] << " and " << allKeywords[i] << " share " << clipsInCommon << " clips " << endl;
            }
        }
    }
}

void CloudsFCPVisualizer::drawGrid(){
    
    if(!font.isLoaded()){
        font.loadFont("verdana.ttf", 8);
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

void CloudsFCPVisualizer::exportForGraphviz(){
    
    map<pair<string,string>, int>::iterator it;
    ofBuffer dotFile;
    dotFile.append("graph G { \n");
    
    for(it = sharedClips.begin(); it != sharedClips.end(); it++){
        if(it->second > 2){
            dotFile.append("    \"" + it->first.first + "\" -- \"" + it->first.second  + "\";\n");
        }
    }
    
    dotFile.append("}\n");
    ofBufferToFile("clip_relationships.dot", dotFile);
}


void CloudsFCPVisualizer::addTagToPhysics(string tag){
	
	
	//NEW WAY PER CLIP NAVIGATOR
	
	cout << "NEW PHYSICS adding tag " << tag << endl;
	
	vector<msa::physics::Particle2D*> newParticles;
	vector<CloudsClip> relatedClips = database->getClipsWithKeyword(tag);
	int particleStartIndex = physics.numberOfParticles();
	
	for(int c = 0; c < relatedClips.size(); c++){
		
		string clipName = relatedClips[c].getLinkName();
		
		cout << "NEW PHYSICS adding clip " << clipName << endl;
		
		msa::physics::Particle2D* p;
		if(hasParticle(clipName)){
			p = particlesByTag[clipName];
		}
		else{
			//make a particle for the seed
			p = physics.makeParticle(ofVec2f(width/2 - c, height/2 + c));
			//p->setMass(database->occurrencesOfKeyword(tag));
			p->setMass(1);
			//			p->makeFixed();
			newParticles.push_back(p);
			particleName[p] = clipName;
			particlesByTag[clipName] = p;
			particleToClip[p] = relatedClips[c];
			dampendPositions[p] = p->getPosition();
			
			if(physics.numberOfParticles() == 1){
//				p->makeFixed();
			}
		}
		
		//connect to new particles
		for(int n = 0; n < newParticles.size(); n++){
			msa::physics::Particle2D * a = newParticles[n];
			if(a != p && springs.find( make_pair(a, p) ) == springs.end() &&
			   springs.find( make_pair(p, a) ) == springs.end() )
			{
				//use clips in common to weight the lines
				msa::physics::Spring2D* newSpring = physics.makeSpring(a, p, .02, 20 );
				springs[ make_pair(a, p) ] = newSpring;
			}
			
			//				clipsInSpring[ newSpring ] = clipsInCommon;
			//				if( database->keywordsShareLink(tag, *it) ){
			//					cout << "-->!!link exists!" << endl;
			//					linkSprings.insert(newSpring);
			//				}
			
		}
	}
	
	cout << "at the end there are " << physics.numberOfParticles() << " paricles and " << physics.numberOfSprings() << " springs" << endl;
	//repel existing particles
	for(int i = 0; i < newParticles.size(); i++){
		for(int j = 0; j < particleStartIndex; j++){
			physics.makeAttraction(newParticles[i], physics.getParticle(j), -20);
		}
	}
	
	//repel eachother
	for(int i = 0; i < newParticles.size(); i++){
		for(int j = i+1; j < newParticles.size(); j++){
			physics.makeAttraction(newParticles[i], newParticles[j], -20);
		}
	}
}


void CloudsFCPVisualizer::addAllClipsWithAttraction(){
	vector<CloudsClip>& clips = database->getAllClips();
	for(int i = 0; i < clips.size(); i++){
		
		CloudsClip& clip = clips[i];
		string clipName = clips[i].getLinkName();
		
		msa::physics::Particle2D* p;
		//p = physics.makeParticle(ofVec2f(width/2 - i/30, height/2 + i%30));
		p = physics.makeParticle(ofVec2f(ofRandom(width*10), ofRandom(height*10) ));
		//		p->setMass(clip.keywords.size());
		p->setMass(2);
		particleName[p] = clipName;
		particlesByTag[clipName] = p;
	}
	
	for(int i = 0; i < clips.size(); i++){
		CloudsClip& clip1 = clips[i];
		msa::physics::Particle2D* a = particlesByTag[ clip1.getLinkName() ];
		for(int j = i+1; j < clips.size(); j++){
			CloudsClip& clip2 = clips[j];
			msa::physics::Particle2D* b = particlesByTag[ clip2.getLinkName() ];
			
			int sharedKeywords = database->getSharedKeywords(clip1, clip2).size();
			if(sharedKeywords == 0){
				physics.makeAttraction(a, b, -.002);
			}
			else{
				physics.makeAttraction(b, b, 10);
			}
		}
	}
}