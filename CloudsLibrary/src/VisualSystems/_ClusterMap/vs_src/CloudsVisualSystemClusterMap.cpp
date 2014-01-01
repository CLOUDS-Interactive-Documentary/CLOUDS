//
//  CloudsVisualSystemClusterMap.cpp
//

#include "CloudsVisualSystemClusterMap.h"

#include "CloudsFCPParser.h"

CloudsVisualSystemClusterMap::CloudsVisualSystemClusterMap(){
	run = NULL;
}

//called once from start up of the app
void CloudsVisualSystemClusterMap::buildEntireCluster(CloudsFCPParser& parser){
	this->parser = &parser;
	resetGeometry();
}

void CloudsVisualSystemClusterMap::resetGeometry(){
	
	firstClip = true;
	lastTraverseStartedIndex = -1;

	networkMesh.clear();
	traversalMesh.clear();
	optionsMeshNext.clear();
	optionsMeshPrev.clear();
	nodeMesh.clear();
	nodes.clear();
	traversalPath.clear();
	clipIdToNodeIndex.clear();
	
	currentTraversalIndex = 0;
	
	ofVec3f centroid(0,0,0);
	ofVec3f maxBounds(0,0,0);
	ofVec3f minBounds(0,0,0);
	
	for(int i = 0; i < parser->getAllClips().size(); i++){
		CloudsClusterNode n;
		CloudsClip& clip = parser->getAllClips()[i];
		n.clipId = clip.getID();
		n.mesh = &nodeMesh;
		n.vertexIndex = nodeMesh.getNumVertices();
		n.networkPosition = clip.networkPosition;
		clipIdToNodeIndex[clip.getID()] = nodes.size();
		nodeMesh.addVertex( n.networkPosition );
		
		centroid += clip.networkPosition;
		maxBounds = ofVec3f(MAX(maxBounds.x,clip.networkPosition.x),
							MAX(maxBounds.y,clip.networkPosition.y),
							MAX(maxBounds.z,clip.networkPosition.z));
		
		minBounds = ofVec3f(MIN(minBounds.x,clip.networkPosition.x),
							MIN(minBounds.y,clip.networkPosition.y),
							MIN(minBounds.z,clip.networkPosition.z));
		
		nodes.push_back(n);
	}
	nodeMesh.setMode(OF_PRIMITIVE_POINTS);
	
	centroid /= nodes.size();
	float maxDistance = 0;
	for(int i = 0; i < parser->getAllClips().size(); i++){
		maxDistance = MAX(maxDistance, parser->getAllClips()[i].networkPosition.distance(centroid));
	}
	
	cout << "CENTROID " << centroid << " MAX D " << maxDistance << endl;
	
	//add all connections to connection mesh
	set< pair<string,string> > connections;
	
	for(int i = 0; i < parser->getAllClips().size(); i++){
		CloudsClip& clip = parser->getAllClips()[i];
		vector<CloudsClip> meta = parser->getClipsWithKeyword(clip.getKeywords());
		vector<CloudsLink> links = parser->getLinksForClip(clip);
		string nameA = clip.getID();
		CloudsClusterNode& n1 = nodes[ clipIdToNodeIndex[nameA] ];
		
//		for(int l = meta.size()-1; l >= 0; l--){
//			if( parser.linkIsSuppressed(nameA, meta[l].getLinkName()) ){
//				meta.erase(meta.begin() + l);
//			}
//		}

		for(int l = 0; l < links.size(); l++){
			meta.push_back(parser->getClipWithLinkName(links[l].targetName));
		}
		
		ofVec3f randDir = randomDirection();
		for(int j = 0; j < meta.size(); j++){
			string nameB = meta[j].getID();
			bool valid = true;
			valid &= (nameA != nameB);
			valid &= (clip.person != meta[j].person || parser->clipLinksTo(nameA, nameB));
			valid &= !parser->linkIsSuppressed(nameA, nameB);
//			valid &= parser.getNumberOfSharedKeywords(clip, meta[j]) > 1;
			if(valid)
			{
				CloudsClusterNode& n2 = nodes[ clipIdToNodeIndex[nameB] ];
				n1.adjascentClipIds.push_back(n2.clipId);
				
				if(connections.find( make_pair(nameA, nameB) ) != connections.end() ||
				   connections.find( make_pair(nameB, nameA) ) != connections.end())
				{
					continue;
				}
				
				connections.insert(make_pair(nameA, nameB));
				
				if(parser->getNumberOfSharedKeywords(clip, meta[j]) < 2){
					continue;
				}
				
				if(clip.networkPosition.distance(centroid) > maxDistance * .6) {
//					cout << "connecting outer ring clip " << meta[j].getLinkName() << endl;
					continue;
				}
				
				if(meta[j].networkPosition.distance(centroid) > maxDistance * .6 ) {
//					cout << "connecting outer ring clip " << meta[j].getLinkName() << endl;
					continue;
				}
				
//				n1.connectionMeshVertexIds.push_back( connectionMesh.getNumVertices() );
//				connectionMesh.addVertex(clip.networkPosition);
//				connectionMesh.addNormal(ofVec3f(0,0,0));
				
//				n2.connectionMeshVertexIds.push_back( connectionMesh.getNumVertices() );
//				connectionMesh.addVertex(meta[j].networkPosition);
//				connectionMesh.addNormal(ofVec3f(0,0,0));

				NetworkEdge connectionEdge;
				//create curved connection mesh
				//naive simple spherical interpolation over 10 steps
				connectionEdge.startIndex = networkMesh.getNumVertices();
				ofVec3f vecToStart = clip.networkPosition - centroid;
				ofVec3f vecToDest  = meta[j].networkPosition - centroid;
				float radStart = vecToStart.length();
				float radDest  = vecToDest.length();
				ofVec3f dirToStart = vecToStart / radStart;
				ofVec3f dirToDest  = vecToDest  / radDest;
				ofQuaternion arc;
				arc.makeRotate(dirToStart,dirToDest);
				
				//handle
				networkMesh.addColor(ofFloatColor(0,0));
				networkMesh.addNormal(ofVec3f(0.0, 0.0, 0.0));
				networkMesh.addVertex(clip.networkPosition);

				int numSteps = 10;
				for(int i = 0; i <= numSteps; i++){
					float stepPercent = (1.* i) / numSteps;
					ofQuaternion step;
					step.slerp(stepPercent, ofQuaternion(), arc);
					ofVec3f arcDir = step * dirToStart;
					float arcRad = ofLerp(radStart, radDest, stepPercent);
					ofVec3f arcPoint = arcDir * arcRad + centroid;
					
					networkMesh.addColor(ofFloatColor(1,1));
					networkMesh.addNormal(ofVec3f(stepPercent, 0.0, 0.0));
					networkMesh.addVertex(arcPoint);
				}
				
				//handle
				networkMesh.addColor(ofFloatColor(0,0));
				networkMesh.addNormal(ofVec3f(1.0, 0.0, 0.0));
				networkMesh.addVertex(meta[j].networkPosition);
				
				connectionEdge.endIndex = networkMesh.getNumVertices();
				connectionEdge.source = true;
				n1.connectionCurves[n2.clipId] = connectionEdge;
				connectionEdge.source = false;
				n2.connectionCurves[n1.clipId] = connectionEdge;
			}
		}
	}
	
//	connectionMesh.setMode(OF_PRIMITIVE_LINES);
	networkMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	traversalMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	optionsMeshNext.setMode(OF_PRIMITIVE_LINE_STRIP);
	optionsMeshPrev.setMode(OF_PRIMITIVE_LINE_STRIP);
	
}

void CloudsVisualSystemClusterMap::traverse(){
	
	cout << " CloudsVisualSystemClusterMap::traverse TRAVERSING 1" << endl;
	
	if(run == NULL){
		ofLogError("CloudsVisualSystemClusterMap::traverse") << "Traversed without RUN" << endl;
		return;
	}
	
//	traversalMesh.clear();
//	cout << "CloudsVisualSystemClusterMap::traverse TRAVERSING 2 " << run->topicHistory.size() << endl;
//	for(int  i = 0; i < run->topicHistory.size(); i++){
//		cout << "CloudsVisualSystemClusterMap::traverse -- traversed to topic " << run->topicHistory[i] << endl;
//	}
	
	///BEGIN OLD LINEAR TRAVERSAL
//	ofVec3f lastPos;
	if(currentTraversalIndex < run->clipHistory.size()){
		CloudsClip& clip = run->clipHistory[currentTraversalIndex];
		traverseToClip( clip );
		currentTraversalIndex++;
	}

}

void CloudsVisualSystemClusterMap::traverseToClip(CloudsClip& clip){
	
	if(clipIdToNodeIndex.find(clip.getID()) == clipIdToNodeIndex.end()){
		ofLogError("CloudsVisualSystemClusterMap::traverseToClip") << "Isn't included in cluster map";
		return;
	}
	

	ofIndexType newNodeIndex = clipIdToNodeIndex[ clip.getID() ];
	CloudsClusterNode& n = nodes[ newNodeIndex ];
	if(firstClip){
		cout << "** TRAVERSE START JOURNEY AT " << clip.getID() << endl;
	}
	else{
		cout << "** TRAVERSE FROM :	" << nodes[currentNodeIndex].clipId << " >>> " << nodes[ newNodeIndex ].clipId << endl;
	}
	
//	for(int i = 0; i < n.adjascentClipIds.size(); i++){
//		cout << "	** ADJASCENT CLIPS " << n.adjascentClipIds[i] << endl;
//	}

	ofVec3f startDirection;
	if(firstClip){
		currentTraversalDirection = randomDirection();
		currentTraversalPosition = clip.networkPosition;
	}
	else{
		//startDirection = (clip.networkPosition - currentNodePosition).normalized();
		
		//trace out the connection that we took
		/*
		if(ofContains( nodes[ currentNodeIndex ].adjascentClipIds, clip.getID()) ){
			TraversalCurve& t = nodes[ currentNodeIndex ].connectionCurves[ clip.getID() ];
			ofIndexType lastTraverseEndedIndex = traversalMesh.getNumVertices();
			
			for(ofIndexType i = t.startIndx; i < t.endIndx; i++){
//				traversalMesh.addVertex( optionsMeshNext.getVertices()[i] );
//				traversalMesh.addNormal( optionsMeshNext.getNormals()[i] );
			}
			if(lastTraverseStartedIndex != -1){
				for(ofIndexType i = lastTraverseStartedIndex; i < lastTraverseEndedIndex; i++){
					traversalMesh.setNormal(i, ofVec3f(1,1,0));
				}
			}
			lastTraverseStartedIndex = lastTraverseEndedIndex;
		}
		else{
			ofLogError("CloudsVisualSystemClusterMap::traverseToClip") << "Traversed to non connect clip "
				<< clip.getID() << " from " <<  nodes[ currentNodeIndex ].clipId << endl;
		}
		*/
		
		//this is to correct the scales in the sliders to more friendly numbers...
		float localMinSolve = traverseMinSolvedDistance * .001;
		float localStepSize = traverseStepSize * .001;
		float localMinHomingDist = traverseHomingMinDistance * .001;
		TraversalSegment newSegment;
		newSegment.startIndex = traversalMesh.getNumVertices();
		int maxSteps = 5000;
		float currentDistance = currentTraversalPosition.distance(clip.networkPosition);
		while(localMinSolve < currentDistance && maxSteps-- > 0){
			ofVec3f toNodeDirection  = (clip.networkPosition - currentTraversalPosition).normalized();
			ofVec3f vectorToNode = clip.networkPosition - currentTraversalPosition;
			float distanceToNode = vectorToNode.length();
			ofVec3f directionToNode = vectorToNode/distanceToNode;
			
			ofQuaternion rotToNode, incrementalRotate;
			rotToNode.makeRotate(currentTraversalDirection, directionToNode);
			float angleDampen = ofMap(currentDistance, 0, localMinHomingDist, 1.0, traverseAngleDampen, true);
			incrementalRotate.slerp(angleDampen, ofQuaternion(), rotToNode);

			currentTraversalDirection = incrementalRotate * currentTraversalDirection;
			currentTraversalPosition += currentTraversalDirection * localStepSize;
			
			traversalMesh.addVertex(currentTraversalPosition);
			currentDistance = currentTraversalPosition.distance(clip.networkPosition);
		}
		
		//cap it off
		currentTraversalPosition = clip.networkPosition;
		traversalMesh.addVertex(clip.networkPosition);
		
		newSegment.endIndex = traversalMesh.getNumVertices();
		cout << "Step took " << (newSegment.endIndex - newSegment.startIndex) << " vertices" << endl;

		//solidify old segement by flagging n.y = 1
		if(traversalPath.size() > 0){
			TraversalSegment& s = traversalPath.back();
			for(ofIndexType i = s.startIndex; i < s.endIndex; i++){
				traversalMesh.setNormal(i, ofVec3f(1,1,0));
			}
		}
		
		//add normals along percent to facilitate transition
		for(int i = newSegment.startIndex; i < newSegment.endIndex; i++){
			float percentAlong = ofMap(i, newSegment.startIndex,newSegment.endIndex,0, 1.0);
			traversalMesh.addNormal(ofVec3f(percentAlong,0., 0.));
		}
		traversalPath.push_back(newSegment);
	}
			  
	cameraStartPosition = easyCamera.getPosition();
	
	swap(optionsMeshPrev,optionsMeshNext);
	
	optionsMeshNext.clear();
	
	//copy all the traversed options into the option mesh
	map<string, NetworkEdge>::iterator it;
	for(it = n.connectionCurves.begin(); it != n.connectionCurves.end(); it++){
		
		NetworkEdge& edge = it->second;
		for(ofIndexType i = edge.startIndex; i < edge.endIndex; i++){
			float percentComplete;
			if(edge.source){
				percentComplete = ofMap(i,edge.startIndex,edge.endIndex-1, 1.0, 0.0);
			}
			else{
				percentComplete = ofMap(i,edge.startIndex,edge.endIndex-1, 0.0, 1.0);
			}
			optionsMeshNext.addColor( networkMesh.getColor(i));
			optionsMeshNext.addNormal(ofVec3f(percentComplete,0,0) );
			optionsMeshNext.addVertex(networkMesh.getVertex(i));
		}
	}
	
	/*
	for(int i = 0; i < n.adjascentClipIds.size(); i++){
		CloudsClusterNode& destNode = nodes[ clipIdToNodeIndex[ n.adjascentClipIds[i] ] ];
		ofVec3f currentPosition = clip.networkPosition;
		
		ofVec3f currentDirection = startDirection;
		ofVec3f toNodeDirection  = (destNode.networkPosition - currentPosition).normalized();
		ofQuaternion nodeCorrectionRot;
		nodeCorrectionRot.makeRotate(currentDirection, toNodeDirection);
		nodeCorrectionRot.slerp(0.0, ofQuaternion(), nodeCorrectionRot);
		currentDirection = nodeCorrectionRot * currentDirection;
		
		float percentTraversed = 0;
		float stepPercent = .05;

		TraversalCurve curve;
		curve.destinationClipId = destNode.clipId;
		curve.startIndx = optionsMeshNext.getNumVertices();
		
		//HANDLE
		optionsMeshNext.addNormal(ofVec3f(0,0,0));
		optionsMeshNext.addColor(ofFloatColor(0,0));
		optionsMeshNext.addVertex(currentPosition);

		//start node
		optionsMeshNext.addNormal(ofVec3f(0,0,0));
		optionsMeshNext.addColor(ofFloatColor(1.0,1.0));
		optionsMeshNext.addVertex(currentPosition);

		do {
			ofVec3f vectorToNode = destNode.networkPosition - currentPosition;
			float distanceToNode = vectorToNode.length();
			ofVec3f directionToNode = vectorToNode/distanceToNode;
			
			ofQuaternion rotToNode, incrementalRotate;
			rotToNode.makeRotate(currentDirection, directionToNode);
			incrementalRotate.slerp(percentTraversed, ofQuaternion(), rotToNode);
			currentDirection = incrementalRotate * currentDirection;
			currentPosition += currentDirection * (distanceToNode * stepPercent);
			
			optionsMeshNext.addNormal(ofVec3f(percentTraversed,0,0));
			optionsMeshNext.addVertex(currentPosition);
			optionsMeshNext.addColor(ofFloatColor(1.0, 0.0, 0.0, 1.0));
			
			percentTraversed += stepPercent;
			
		} while(percentTraversed < 1.0);

		//end node
		optionsMeshNext.addNormal(ofVec3f(1.0,0.0,0.0));
		optionsMeshNext.addColor(ofFloatColor(1.0,1.0));
		optionsMeshNext.addVertex(destNode.networkPosition);

		//handle
		optionsMeshNext.addNormal(ofVec3f(1.0,0.0,0.0));
		optionsMeshNext.addColor(ofFloatColor(0.0,0.0));
		optionsMeshNext.addVertex(destNode.networkPosition);
		
		curve.endIndx = optionsMeshNext.getNumVertices();
		
		n.connectionCurves[curve.destinationClipId] = curve;
	}
	 */
	
	optionsMeshPrev.setMode(OF_PRIMITIVE_LINE_STRIP);
	
	currentNodeIndex  = newNodeIndex;
	traverseStartTime = ofGetElapsedTimef();
//	traversalMesh.addVertex(clip.networkPosition);
//	traversalMesh.addColor(ofFloatColor());
//	cout << ("CloudsVisualSystemClusterMap::traverseToClip") << "After traversing node we have " << traversalMesh.getNumVertices() << " verts" << endl;
//	for(int c = 0; c < n.connectionMeshVertexIds.size(); c++){
//		connectionMesh.setNormal(n.connectionMeshVertexIds[c], ofVec3f(1.0,0.0,0.0));
//	}
	
	firstClip = false;
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemClusterMap::selfSetupGui(){
	
	ofxUIToggle* toggle;
		
	nodesGui = new ofxUISuperCanvas("NODES", gui);
	nodesGui->copyCanvasStyle(gui);
	nodesGui->copyCanvasProperties(gui);
	nodesGui->setName("Nodes");
	toggle = nodesGui->addToggle("ENABLE", &drawNodes);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    nodesGui->resetPlacer();
    nodesGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    nodesGui->addWidgetToHeader(toggle);
	nodesGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	nodesGui->addSlider("MESH SCALE", 100, 1000, &meshExpansion);
	nodesGui->addLabel("POINTS");
	nodesGui->addRangeSlider("POINT SIZE RANGE", .5, 4., &pointSize.min, &pointSize.max);
	nodesGui->addSlider("TRAVERSED NODE SIZE", 1, 10, &traversedNodeSize);
	nodesGui->addSlider("NODE POP LENGTH", 50, 2000, &nodePopLength);
	ofAddListener(nodesGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(nodesGui);
	guimap[nodesGui->getName()] = nodesGui;
 
	linesGui = new ofxUISuperCanvas("LINES", gui);
	linesGui->copyCanvasStyle(gui);
	linesGui->copyCanvasProperties(gui);
	linesGui->setName("Lines");
	toggle = linesGui->addToggle("ENABLE", &drawLines);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    linesGui->resetPlacer();
    linesGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    linesGui->addWidgetToHeader(toggle);
	
	linesGui->addSlider("LINE ALPHA", 0.0, 1.0, &lineAlpha);
	linesGui->addSlider("LINE FOCAL DISTANCE", 0, sqrt(3000.0f), &lineFocalDistance);
	linesGui->addSlider("LINE FOCAL RANGE", 0, sqrt(3000.0f), &lineFocalRange);
	//TODO: line color A
	//TODO: line color B
	
	ofAddListener(linesGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(linesGui);
	guimap[linesGui->getName()] = linesGui;

	traversalGui = new ofxUISuperCanvas("TRAVERSAL", gui);
	traversalGui->copyCanvasStyle(gui);
	traversalGui->copyCanvasProperties(gui);
	traversalGui->setName("Traversal");
	traversalGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	toggle = traversalGui->addToggle("ENABLE", &drawTraversal);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    traversalGui->resetPlacer();
    traversalGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    traversalGui->addWidgetToHeader(toggle);
	
	traversalGui->addLabel("TRAVERSE");
	traversalGui->addSlider("CAMERA DISTANCE", 10, 400, &traversCameraDistance);
	traversalGui->addSlider("ANGLE DAMPEN", 0.01, .5, &traverseAngleDampen);
	traversalGui->addSlider("STEP SIZE", .2, 10, &traverseStepSize);
	traversalGui->addSlider("MIN HOMING DISTANCE", 1.0, 100.0, &traverseHomingMinDistance);
	traversalGui->addSlider("MIN SOLVE DISTANCE", .1, 5, &traverseMinSolvedDistance);
	traversalGui->addToggle("DRAW POINTS", &drawTraversalPoints);
	traversalGui->addToggle("DRAW DISTANCE DEBUG", &drawHomingDistanceDebug);
	traversalGui->addLabel("ANIMATE");
	traversalGui->addSlider("ANIMATE DURATION", 1, 4, &traverseAnimationDuration);

	ofAddListener(traversalGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(traversalGui);
	guimap[traversalGui->getName()] = traversalGui;
	
	optionPathsGui = new ofxUISuperCanvas("OPTION PATHS", gui);
	optionPathsGui->copyCanvasStyle(gui);
	optionPathsGui->copyCanvasProperties(gui);
	optionPathsGui->setName("Option Paths");
	optionPathsGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	toggle = optionPathsGui->addToggle("ENABLE", &drawOptionPaths);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    optionPathsGui->resetPlacer();
    optionPathsGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    optionPathsGui->addWidgetToHeader(toggle);
	optionPathsGui->addLabel("ANIMATE");
	optionPathsGui->addSlider("ANIMATE DURATION", 1, 4, &optionsAnimationDuration);

	ofAddListener(optionPathsGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(optionPathsGui);
	guimap[optionPathsGui->getName()] = optionPathsGui;
}

void CloudsVisualSystemClusterMap::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

void CloudsVisualSystemClusterMap::setRun(CloudsRun& newRun){
	run = &newRun;
}

void CloudsVisualSystemClusterMap::setQuestions(vector<CloudsClip>& questionClips){
	selectedQuestion = NULL;
	questions.clear();
	
	for(int i = 0; i < questionClips.size(); i++){
		
		CloudsQuestion q;
		q.cam = &getCameraRef();
		q.clip = questionClips[i];
		
		q.setup();
		
		questions.push_back(q);
	}
}

CloudsQuestion* CloudsVisualSystemClusterMap::getSelectedQuestion(){
	//TODO: interaction for selecting a question
	
	//Temporary to just return the first one
	return &questions[0];
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemClusterMap::selfSetupSystemGui(){
	
}

void CloudsVisualSystemClusterMap::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemClusterMap::selfSetupRenderGui(){

}

void CloudsVisualSystemClusterMap::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemClusterMap::selfSetupTimeline(){
	lineColor = timeline->addColors("line color", "LineColor.xml");
	nodeColor = timeline->addColorsWithPalette("node color","NodeColor.xml", getVisualSystemDataPath() + "images/nerve_palette.png");
}

void CloudsVisualSystemClusterMap::selfSetDefaults(){
	drawNodes = true;
	drawLines = true;
	drawTraversal = true;
	drawOptionPaths = true;
	drawTraversalPoints = false;
	drawHomingDistanceDebug = false;
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemClusterMap::selfSetup(){
	gameCamera.setup();
	gameCamera.autosavePosition = true;
	gameCamera.loadCameraPosition();
	
	firstClip = true;
	
	reloadShaders();
}

void CloudsVisualSystemClusterMap::reloadShaders(){
	
	cout << "reload shader" << endl;
	
	ofDisableArbTex();
	sprite.loadImage(getVisualSystemDataPath() + "images/dot.png");
	ofEnableArbTex();

	traversalShader.load(getVisualSystemDataPath() + "shaders/traversal");
	optionsShader.load(getVisualSystemDataPath() + "shaders/options");
	networkShader.load(getVisualSystemDataPath() +"shaders/network");
	nodesShader.load(getVisualSystemDataPath() + "shaders/nodes");
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemClusterMap::selfPresetLoaded(string presetPath){
	timeline->setLoopType(OF_LOOP_NONE);
	
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemClusterMap::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemClusterMap::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemClusterMap::selfUpdate(){
	
	gameCamera.applyRotation = gameCamera.applyTranslation = !cursorIsOverGUI();
	if(cursorIsOverGUI()){
		easyCamera.disableMouseInput();
	}
	else{
		easyCamera.enableMouseInput();
	}
	
	easyCamera.setDistance(traversCameraDistance);
	ofVec3f curPosition = easyCamera.getTarget().getPosition();
	ofVec3f curTarget = (currentTraversalPosition*meshExpansion);
	ofVec3f newPos = curPosition + (curTarget - curPosition) * .05;
	easyCamera.setTarget(newPos);
	percentToDest = (newPos - cameraStartPosition).length() / (curTarget - cameraStartPosition).length();
	
	percentTraversed = ofMap(ofGetElapsedTimef(),
							 traverseStartTime, traverseStartTime+traverseAnimationDuration,
							 0, 1.0, true);
	percentOptionsRevealed = ofMap(ofGetElapsedTimef(),
								   traverseStartTime+traverseAnimationDuration,
								   traverseStartTime+traverseAnimationDuration+optionsAnimationDuration,
								   0.0, 1.0, true);

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemClusterMap::selfDraw(){

	glDisable(GL_DEPTH_TEST);

	ofPushStyle();
	ofPushMatrix();
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	ofScale(meshExpansion,meshExpansion,meshExpansion);
	
	////POINTS
	if(drawNodes){
		nodesShader.begin();
		nodesShader.setUniformTexture("tex", sprite, 1);
		nodesShader.setUniform1f("minSize", pointSize.min);
		nodesShader.setUniform1f("maxSize", pointSize.max);
		nodesShader.setUniform3f("attractor", 0, 0, 0);
		nodesShader.setUniform1f("radius", 300.);
		ofEnablePointSprites();
		ofDisableArbTex();
		nodeMesh.draw();
		ofEnableArbTex();
		ofDisablePointSprites();
		nodesShader.end();
	}
	/////END POINTS
	
	///NETWORK LINES
	if(drawLines){
		networkShader.begin();
		networkShader.setUniform1f("focalPlane", powf(lineFocalDistance,2));
		networkShader.setUniform1f("focalRange", powf(lineFocalRange,2));
		networkShader.setUniform1f("lineFade", lineAlpha);
		networkShader.setUniform3f("attractor", trailHead.x, trailHead.y, trailHead.z);
		networkShader.setUniform1f("radius", 300.);
		networkShader.setUniform3f("lineColor", 100/255., 150/255., 200/255.);
		networkMesh.draw();
		networkShader.end();
	}
	///END NETWORK LINES
	
	//////TRAVERSAL
	if(drawTraversalPoints){
		traversalMesh.setMode(OF_PRIMITIVE_POINTS);
		glPointSize(2);
		traversalMesh.draw();
		traversalMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	}
	else if(drawTraversal){
		traversalShader.begin();
		traversalShader.setUniform1f("percentTraverseRevealed", percentTraversed);
		traversalMesh.draw();
		traversalShader.end();
	}
	if(drawHomingDistanceDebug){
		ofPushStyle();
		ofNoFill();
		
		ofSetColor(ofColor::royalBlue);
		ofSphere(currentTraversalPosition, traverseMinSolvedDistance*.001);
		
		ofSetColor(ofColor::yellowGreen);
		ofSphere(currentTraversalPosition, traverseHomingMinDistance*.001);
		
		ofPopStyle();
	}
	///END TRAVERSAL
	
	/////OPTIONS
	if(drawOptionPaths){
		optionsShader.begin();
		
		optionsShader.setUniform3f("lineColor", 100/255., 150/255., 200/255.);
		
		optionsShader.setUniform1f("percentOptionsRevealed", percentTraversed);
		optionsShader.setUniform1i("fadeIn", 0);
		optionsMeshPrev.draw();
		
		optionsShader.setUniform1f("percentOptionsRevealed", percentOptionsRevealed);
		optionsShader.setUniform1i("fadeIn", 1);
		optionsMeshNext.draw();

		optionsShader.end();
	}
	/////END OPTIONS
	
	//TEST CURVE MESH

	ofPopMatrix();
	ofPopStyle();

}

// draw any debug stuff here
void CloudsVisualSystemClusterMap::selfDrawDebug(){
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemClusterMap::selfDrawBackground(){
	//turn the background refresh off
	//bClearBackground = false;
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemClusterMap::selfEnd(){
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemClusterMap::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemClusterMap::selfKeyPressed(ofKeyEventArgs & args){
	
	int key = args.key;
	if(key == 'C'){
		gameCamera.reset();
	}
	if(key == 'G'){
		traverse();
	}
	if(key == 'R'){
		resetGeometry();
	}
	if(key == 'S'){
		reloadShaders();
	}
	
}


void CloudsVisualSystemClusterMap::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemClusterMap::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemClusterMap::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemClusterMap::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemClusterMap::selfMouseReleased(ofMouseEventArgs& data){
	
}

ofVec3f CloudsVisualSystemClusterMap::randomDirection(){
	
	float theta = ofRandom(TWO_PI);
	float u = ofRandomf();
	float usqrtinv = sqrt(1. - powf(u,2.));
	return ofVec3f(cos(theta) * usqrtinv,
				   sin(theta) * usqrtinv, u);
}
