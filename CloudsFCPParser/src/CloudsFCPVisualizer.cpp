
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
	
	selectedSpring = NULL;
	hoverSpring = NULL;

	minRadius = 2;
	maxRadius = 50;
	minMass = 1;
	maxMass = 0;
	cursorRadius = 10;
	
	hoverColor = ofColor::fromHex(0xe81c0c);
	selectedColor = ofColor::fromHex(0xff530d);
	nodeColor = ofColor::fromHex(0xe8c57a);
	lineColor = ofColor::fromHex(0x1baa8f);
	
	currentScale = 1.0;
	currentTop = ofVec2f(0,0);
}

void CloudsFCPVisualizer::clear(){
	particleName.clear();
	particlesByTag.clear();
	springs.clear();
	linkSprings.clear();
	
	centerNode = NULL;
	physics.clear();
}

void CloudsFCPVisualizer::setup(){
	
	ofRegisterMouseEvents(this);
    ofRegisterKeyEvents(this);
	ofSetCircleResolution(200);
	
    database->sortKeywordsByOccurrence(true);
    vector<string>& allKeywords = database->getAllKeywords();
    vector<ClipMarker>& clips = database->getAllClips();
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
    
	physics.setWorldSize(ofVec2f(0,0), ofVec2f(width,height));
	physics.setSectorCount(1);
    physics.setDrag(0.1f);
	physics.setDrag(.6);
}

void CloudsFCPVisualizer::addTagToPhysics(string tag){
	
	vector<msa::physics::Particle2D*> newParticles;
	vector<ClipMarker> relatedClips = database->getClipsWithKeyword(tag);
	msa::physics::Particle2D * p;
	if(hasParticle(tag)){
		p = particlesByTag[tag];
	}
	else{
		//make a particle for the seed
		p = physics.makeParticle(ofVec2f(width/2, height/2));
		p->setMass(database->occurrencesOfKeyword(tag));
		p->makeFixed();
		newParticles.push_back(p);
		particleName[p] = tag;
		particlesByTag[tag] = p;
	}
	
	int particleStartIndex = physics.numberOfParticles();
	cout << "adding tag " << tag << endl;
	set<string> related = database->getRelatedKeywords(tag);
	set<string>::iterator it;
	for(it = related.begin(); it != related.end(); it++){
		msa::physics::Particle2D* a;
		if(hasParticle(*it)){
			a = particlesByTag[*it];
		}
		else {
			a = physics.makeParticle(p->getPosition() + ofVec2f(ofRandom(-5,5),ofRandom(-5,5)) );
			int mass = database->occurrencesOfKeyword(*it);
			a->setMass(mass);
			maxMass = MAX(maxMass, mass);
			particleName[a] = *it;
			particlesByTag[*it] = a;
			newParticles.push_back(a);
		}
		
		int clipsInCommon = database->getNumberOfSharedClips(tag,*it);
		if(springs.find( make_pair(a, p) ) == springs.end() &&
		   springs.find( make_pair(p, a) ) == springs.end() ){
			//use clips in common to weight the lines
			msa::physics::Spring2D* newSpring = physics.makeSpring(a, p, .1, 10 );
			springs.insert( make_pair(a, p) );
			clipsInSpring[ newSpring ] = clipsInCommon;
			cout << "Trying to find link spring between " << tag << " and " << *it << endl;
			if( database->keywordsShareLink(tag, *it) ){
				cout << "-->!!link exists!" << endl;
				linkSprings.insert(newSpring);
			}
		}
//		cout << "	added tag " << *it << " with " << clipsInCommon << " shared clips " << endl;
	}
	
	//repel existing particles
	for(int i = 0; i < newParticles.size(); i++){
		for(int j = 0; j < particleStartIndex; j++){
			physics.makeAttraction(newParticles[i], physics.getParticle(j), -20);
		}
	}
	
	//repel eachother
	for(int i = 0; i < newParticles.size(); i++){
		for(int j = 0; j < newParticles.size(); j++){
			if(j != i){
				physics.makeAttraction(newParticles[i], newParticles[j], -20);
			}
		}
	}

	centerNode = p;
}

bool CloudsFCPVisualizer::hasParticle(string tagName){
	return particlesByTag.find(tagName) != particlesByTag.end();
}

void CloudsFCPVisualizer::updatePhysics(){
    physics.update();
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
        font.loadFont("verdana.ttf", 8);
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
		int numClips = clipsInSpring[s];
		
		if(s == hoverSpring || s == selectedSpring){
			ofxEasingCubic cub;
			float alpha = (ofGetElapsedTimeMillis() % 750) / 749.;
			ofSetColor(s == hoverSpring ? hoverColor : selectedColor, (1-alpha)*200);
			ofSetLineWidth(.5 + ofxTween::map(alpha, 0, 1.0, 1.0, 3.0, true, cub));
			ofLine(s->getOneEnd()->getPosition(),
				   s->getTheOtherEnd()->getPosition());
		}

		if(linkSprings.find(s) != linkSprings.end()){
			ofSetColor(selectedColor, ofMap(numClips, 1, 10, 50, 255));
			ofSetLineWidth(.5 + numClips/2.0);
		}
		else{
			ofSetColor(lineColor, ofMap(numClips, 1, 10, 50, 255));
			ofSetLineWidth(.5 + numClips/2.0);
			
		}
        ofLine(s->getOneEnd()->getPosition(),
               s->getTheOtherEnd()->getPosition());
		
		
		ofPopStyle();
    }
	
    for(int i = 0; i < physics.numberOfParticles(); i++){
        
        msa::physics::Particle2D *a = physics.getParticle(i);
		float radius = radiusForNode(a->getMass());
		
		if(a == selectedParticle){
			ofSetColor(selectedColor);
		}
		else if(a == hoverParticle){
			ofSetColor(hoverColor);
		}
		else{
			ofSetColor(nodeColor);
		}
        ofFill();
        ofCircle(a->getPosition(), radius);
        
        ofNoFill();
        ofCircle(a->getPosition(), radius);
        
		if(a == selectedParticle || a == hoverParticle){
			ofxEasingCubic cub;
			float alpha = (ofGetElapsedTimeMillis() % 750) / 749.;
			ofSetColor(hoverColor, (1-alpha)*200);
			ofCircle(a->getPosition(), ofxTween::map(alpha, 0, 1.0, radius, radius+10, true, cub) );
			//ofCircle(a->getPosition(), radius + 10);
		}
    }
    
	//ofCircle(ofGetMouseX(), ofGetMouseY(), cursorRadius);
	ofCircle( graphPointForScreenPoint( ofVec2f(ofGetMouseX(), ofGetMouseY() )),cursorRadius*currentScale);
	
	ofPopMatrix();
	vector<string>& allKeywords = database->getAllKeywords();
    for(int i = 0; i < physics.numberOfParticles(); i++){
		msa::physics::Particle2D *a = physics.getParticle(i);
		if(a->getMass() > 1 || a == selectedParticle || a == hoverParticle){
			//ofSetColor(ofColor::fromHsb(215, 255, 255, a->getMass()*5 + 150));
			//ofSetColor(ofColor(20, a->getMass()*5 + 150) );
			ofSetColor(75);
			ofVec2f textPosition = screenPointForGraphPoint(a->getPosition());
			font.drawString(particleName[a], textPosition.x,textPosition.y);
		}
	}
	
	string allSelectedClips = selectionTitle + "\n";
	for(int i = 0; i < selectedClips.size(); i++){
		allSelectedClips += selectedClips[i].getLinkName() + "\n";
	}
	
	ofSetColor(25, 230);
	font.drawString(allSelectedClips, 20, 20);
	
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
		if(point.squareDistance( physics.getParticle(i)->getPosition()) < (particleRadiusSquared + cursorRadius*cursorRadius) ){
			return physics.getParticle(i);
		}
	}
	return NULL;
}

msa::physics::Spring2D* CloudsFCPVisualizer::springNearPoint(ofVec2f point){

	for(int i = 0; i < physics.numberOfSprings(); i++){
		msa::physics::Spring2D* spring = physics.getSpring( i );
		ofVec3f p = ofVec3f(point.x, point.y, 0);
		ofVec3f a = ofVec3f(physics.getSpring( i )->getOneEnd()->getPosition().x,
							physics.getSpring( i )->getOneEnd()->getPosition().y,0);
		ofVec3f b = ofVec3f(physics.getSpring( i )->getTheOtherEnd()->getPosition().x,
							physics.getSpring( i )->getTheOtherEnd()->getPosition().y,0);
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
	selectedLinks.clear();
	selectedClips.clear();
	selectionTitle = "";
	hoverParticle = particleNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
	if(hoverParticle != NULL){
		selectionTitle = "Clips with " + particleName[ hoverParticle ];
		selectedClips = database->getClipsWithKeyword( particleName[ hoverParticle ] );
	}
	else{
		hoverSpring = springNearPoint( graphPointForScreenPoint( ofVec2f(args.x,args.y) ) );
		if(hoverSpring != NULL){
			string keyA = particleName[ hoverSpring->getOneEnd() ];
			string keyB = particleName[ hoverSpring->getTheOtherEnd() ];
			selectionTitle = "Clips sharing " + keyA + " and " + keyB;
			selectedClips = database->getSharedClips(keyA,keyB);
		}
		else if(selectedParticle != NULL){
			selectionTitle = "Clips with " + particleName[ selectedParticle ];
			selectedClips = database->getClipsWithKeyword( particleName[ selectedParticle ] );		
		}
		else if(selectedSpring != NULL){
			string keyA = particleName[ selectedSpring->getOneEnd() ];
			string keyB = particleName[ selectedSpring->getTheOtherEnd() ];
			selectionTitle = "Clips sharing " + keyA + " and " + keyB;
			selectedClips = database->getSharedClips(keyA,keyB);	
		}
	}
}

void CloudsFCPVisualizer::mouseDragged(ofMouseEventArgs& args){
	
}

void CloudsFCPVisualizer::mouseReleased(ofMouseEventArgs& args){
	
}

void CloudsFCPVisualizer::keyPressed(ofKeyEventArgs& args){
	if(args.key == OF_KEY_RETURN){
		if(selectedParticle != NULL){
			addTagToPhysics( particleName[selectedParticle] );
		}
	}
}

void CloudsFCPVisualizer::keyReleased(ofKeyEventArgs& args){
	
}

void CloudsFCPVisualizer::windowResized(ofResizeEventArgs& args){
	width  = args.width;
	height = args.height;
	physics.setWorldMax(ofVec2f(width,height));
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
    cout << " total of " << allKeywords.size() << endl;
    
    for(int i = 0; i < allKeywords.size(); i++){
        msa::physics::Particle2D *p = physics.makeParticle(ofVec2f(ofRandomWidth(), ofRandomHeight()));
        p->setMass(database->occurrencesOfKeyword(allKeywords[i])*10);
        if(i == 0){
            p->moveTo(ofVec2f(width/2,height/2));
            p->makeFixed();
        }
    }
    
    vector<ClipMarker>& clips = database->getAllClips();
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
