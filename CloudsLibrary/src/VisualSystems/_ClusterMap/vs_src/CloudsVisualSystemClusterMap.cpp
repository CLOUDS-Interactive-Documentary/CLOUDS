//
//  CloudsVisualSystemClusterMap.cpp
//

#include "CloudsVisualSystemClusterMap.h"

#include "CloudsFCPParser.h"

CloudsVisualSystemClusterMap::CloudsVisualSystemClusterMap(){
	run = NULL;
	firstClip = true;
	lastTraverseStartedIndex = -1;
}

//called once from start up of the app
void CloudsVisualSystemClusterMap::buildEntireCluster(CloudsFCPParser& parser){
	
	currentTraversalIndex = 0;
	
	nodes.clear();
	nodeMesh.clear();
	clipIdToNodeIndex.clear();
	traversalMesh.clear();
	
	
	ofVec3f centroid(0,0,0);
	ofVec3f maxBounds(0,0,0);
	ofVec3f minBounds(0,0,0);
	
	for(int i = 0; i < parser.getAllClips().size(); i++){
		CloudsClusterNode n;
		CloudsClip& clip = parser.getAllClips()[i];
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
	for(int i = 0; i < parser.getAllClips().size(); i++){
		maxDistance = MAX(maxDistance, parser.getAllClips()[i].networkPosition.distance(centroid));
	}
	
	cout << "CENTROID " << centroid << " MAX D " << maxDistance << endl;
	
	//add all connections to connection mesh
	set< pair<string,string> > connections;
	
	for(int i = 0; i < parser.getAllClips().size(); i++){
		CloudsClip& clip = parser.getAllClips()[i];
		vector<CloudsClip> meta = parser.getClipsWithKeyword(clip.getKeywords());
		vector<CloudsLink> links = parser.getLinksForClip(clip);
		string nameA = clip.getID();
		CloudsClusterNode& n1 = nodes[ clipIdToNodeIndex[nameA] ];
		
//		for(int l = meta.size()-1; l >= 0; l--){
//			if( parser.linkIsSuppressed(nameA, meta[l].getLinkName()) ){
//				meta.erase(meta.begin() + l);
//			}
//		}

		for(int l = 0; l < links.size(); l++){
			meta.push_back(parser.getClipWithLinkName(links[l].targetName));
		}
		
		ofVec3f randDir = randomDirection();
		for(int j = 0; j < meta.size(); j++){
			string nameB = meta[j].getID();
			bool valid = true;
			valid &= (nameA != nameB);
			valid &= (clip.person != meta[j].person || parser.clipLinksTo(nameA, nameB));
			valid &= !parser.linkIsSuppressed(nameA, nameB);
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
				
				if(parser.getNumberOfSharedKeywords(clip, meta[j]) < 2){
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
				
				n1.connectionMeshVertexIds.push_back( connectionMesh.getNumVertices() );
				connectionMesh.addVertex(clip.networkPosition);
				connectionMesh.addNormal(ofVec3f(0,0,0));
				
				n2.connectionMeshVertexIds.push_back( connectionMesh.getNumVertices() );
				connectionMesh.addVertex(meta[j].networkPosition);
				connectionMesh.addNormal(ofVec3f(0,0,0));

			}
		
		}
	}
	
	connectionMesh.setMode(OF_PRIMITIVE_LINES);
	curveConnectionMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	traversalMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	optionsMeshNext.setMode(OF_PRIMITIVE_LINE_STRIP);
	optionsMeshPrev.setMode(OF_PRIMITIVE_LINE_STRIP);
	
//	traversalMesh.setMode(OF_PRIMITIVE_POINTS);
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
	
//	for(int i = 0; i < run->clipHistory.size(); i++){
		
//		CloudsClusterNode& n = nodes[ clipIdToNodeIndex[ clip.getID() ] ];
//		
//		cout << "CloudsVisualSystemClusterMap::traverse	" << clip.getLinkName() << " at position " << (clip.networkPosition *300) << endl;
//		if(i > 0){
//			for(int s = 1; s < 100; s++){
//				traversalMesh.addVertex(lastPos + s * (clip.networkPosition-lastPos) / 100);
//				traversalMesh.addColor(ofFloatColor());
//			}
//		}
//		lastPos = clip.networkPosition;
//		traversalMesh.addVertex(clip.networkPosition);
//		traversalMesh.addColor(ofFloatColor());
//		
//		for(int c = 0; c < n.connectionMeshVertexIds.size(); c++){
//			connectionMesh.setNormal(n.connectionMeshVertexIds[c], ofVec3f(1.0,0.0,0.0));
//		}
//	}

	//END OLD LINEAR TRAVERSAL
	
	/*
	ofVec3f dirToTarget;
	ofVec3f position = run->clipHistory[0].networkPosition;
	ofVec3f currentDirection = run->clipHistory[1].networkPosition - position;
	if(currentDirection.isAligned(ofVec3f(1,0,0))){
		currentDirection = currentDirection.getCrossed(ofVec3f(0,0,1));
	}
	else{
		currentDirection = currentDirection.getCrossed(ofVec3f(1,0,0));
	}
	
	for(int i = 1; i < run->clipHistory.size(); i++){
		
		dirToTarget = run->clipHistory[i].networkPosition;
		
		CloudsClip& clip = run->clipHistory[i];
		dirToTarget = (clip.networkPosition - position);
		
		ofVec3f direction = (clip.networkPosition - position).normalized();
		
		ofVec3f dirToNode = (clip.networkPosition - position);
		float angleTo = dirToTarget.angle(dirToNode);
//		if(angleTo > maxTraverseAngle){
//			continue;
//		}

		int numSteps = 0;
		float currentDistance = dirToTarget.length();
		while(currentDistance > 2){
			float dampen = ofMap(currentDistance, 20, 2, .05, 1, true);
			direction += ( (dirToTarget / currentDistance) - direction) * dampen;
			direction.normalize();
			//			direction = dirToTarget.normalized();
			position += direction * MIN(1, direction.length());
			
			traversalMesh.addColor(ofFloatColor(0));
			traversalMesh.addVertex(position);
			
			dirToTarget = (clip.networkPosition - position);
			currentDistance = dirToTarget.length();
			if(numSteps++ > 10000){
				cout << "failed with 10000 steps";
				break;
			}
		}
	}
	*/
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
	
	for(int i = 0; i < n.adjascentClipIds.size(); i++){
		cout << "	** ADJASCENT CLIPS " << n.adjascentClipIds[i] << endl;
	}

	ofVec3f startDirection;
	if(firstClip){
		startDirection = randomDirection();
	}
	else{
		startDirection = (clip.networkPosition - currentNodePosition).normalized();
		//trace out the connection that we took
		if(ofContains( nodes[ currentNodeIndex ].adjascentClipIds, clip.getID()) ){
			TraversalCurve& t = nodes[ currentNodeIndex ].connectionCurves[ clip.getID() ];
			ofIndexType lastTraverseEndedIndex = traversalMesh.getNumVertices();
			
			for(ofIndexType i = t.startIndx; i < t.endIndx; i++){
				//maybe we want to outline a different path
				//traversalMesh.getNormals()[i].x = ofMap(i, t.startIndx, t.endIndx, 1.0, 0.0, true);
//				float percentOnTraverse = ofMap(i, t.startIndx, t.endIndx, 1.0, 0.0, true);
				traversalMesh.addVertex( optionsMeshNext.getVertices()[i] );
				traversalMesh.addNormal( optionsMeshNext.getNormals()[i] );
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
	}
	cameraStartPosition = easeCamera.getPosition();
	
	swap(optionsMeshPrev,optionsMeshNext);
	
	optionsMeshNext.clear();
	
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

	optionsMeshPrev.setMode(OF_PRIMITIVE_LINE_STRIP);
	
	currentNodePosition = clip.networkPosition;
	currentNodeIndex  = newNodeIndex;
	
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

	generatorGui = new ofxUISuperCanvas("GENERATOR", gui);
	generatorGui->copyCanvasStyle(gui);
	generatorGui->copyCanvasProperties(gui);
	generatorGui->setName("Generator");
	generatorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	generatorGui->addSlider("mesh expansion", 100, 10000, &meshExpansion);
	generatorGui->addSlider("point size", 1, 50, &pointSize);

//	generatorGui->addSlider("seed", 0, 100, &seed);
//	generatorGui->addSlider("hero nodes", 5, 20, &heroNodes);
//	generatorGui->addSlider("spawn radius",  5, 1000, &heroRadius);
//	generatorGui->addSlider("radius var",  5, 1000, &heroRadiusVariance);
//	generatorGui->addSlider("num iterations", 1, 20, &numIterations);
//	generatorGui->addSlider("num branches", 1, 20, &numBranches);
//	generatorGui->addSlider("surviving branches", 1, 10, &numSurvivingBranches);
//	generatorGui->addSlider("min branch dist",  10, 1000, &minDistance);
//	generatorGui->addSlider("branch dist rng",  0, 3.0, &distanceRange);
//	generatorGui->addSlider("step size",  1, 300, &stepSize);

//	generatorGui->addSlider("min attract radius",  10, 1000, &minAttractRadius);
//	generatorGui->addSlider("min repel radius",  0, 1000, &minRepelRadius);
//	generatorGui->addSlider("min fuse radius",  1, 100, &minFuseRadius);
//	generatorGui->addSlider("max attract force",  0, 1.0, &maxAttractForce);
//	generatorGui->addSlider("max repel force",  0, 1.0, &maxRepelForce);
	generatorGui->addSlider("max traverse angle",  0, 180, &maxTraverseAngle);

	ofAddListener(generatorGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(generatorGui);
	guimap[generatorGui->getName()] = generatorGui;

	displayGui = new ofxUISuperCanvas("DISPLAY", gui);
	displayGui->copyCanvasStyle(gui);
	displayGui->copyCanvasProperties(gui);
	displayGui->setName("Display");
	displayGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	displayGui->addToggle("incremental traversal", &incrementalTraversalMode);
	
	displayGui->addSlider("line alpha",  0, 1.0, &lineAlpha);
	displayGui->addSlider("line focal dist", 0, sqrt(3000.0f), &lineFocalDistance);
	displayGui->addSlider("line focal range", 0, sqrt(3000.0f), &lineFocalRange);
	
	displayGui->addSlider("line dissolve", 0, 1.0, &lineDissolve);
	
	displayGui->addSlider("traversed node size", 1, 10, &traversedNodeSize);
	displayGui->addSlider("node pop length", 50, 2000, &nodePopLength);
	
	
//	displayGui->addSlider("line blur amount",  0, 10, &lineBlurAmount);
//	displayGui->addSlider("line blur fade",  0, 1.0, &lineBlurFade);
//	
//	displayGui->addSlider("line start",  0, 1.0, &lineStartTime);
//	displayGui->addSlider("line end",  0, 1.0, &lineEndTime);
//	displayGui->addSlider("line fade verts", 1, 10, &lineFadeVerts);
//	displayGui->addSlider("line focal dist", 0, sqrt(3000), &lineFocalDistance);
//	displayGui->addSlider("line focal range", 0, sqrt(3000), &lineFocalRange);
//	displayGui->addSlider("line width", .5, 2.0, &lineWidth);

//	displayGui->addSlider("line thickness",  1, 10, &lineThickness);
//	displayGui->addSlider("line alpha",  0, 1.0, &lineAlpha);
//
//	displayGui->addSlider("node bounce", 0, 1.0, &nodeBounce);
//	displayGui->addSlider("cluster node size", 1, 10, &clusterNodeSize);


	ofAddListener(displayGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(displayGui);
	guimap[generatorGui->getName()] = displayGui;

//	traversedNodePoints.setUsage( GL_DYNAMIC_DRAW );
//	traversedNodePoints.setMode(OF_PRIMITIVE_POINTS);
//	nodeCloudPoints.enableNormals();
//	
//	loadShader();
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
//		q.font = &font;
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
//	incrementalTraversalMode = false;
//	currentVertIndex = 0.;
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemClusterMap::selfSetup(){
	cam.setup();
	cam.autosavePosition = true;
	cam.loadCameraPosition();
	
	firstClip = true;
	
	reloadShaders();

}

void CloudsVisualSystemClusterMap::reloadShaders(){
	
	cout << "reload shader" << endl;
	ofDisableArbTex();
	sprite.loadImage(getVisualSystemDataPath() + "images/dot.png");
//	nodeSpriteBasic.loadImage(getVisualSystemDataPath() + "images/dot_no_ring.png");
	ofEnableArbTex();

	traversalShader.load(getVisualSystemDataPath() + "shaders/traversal");
	optionsShader.load(getVisualSystemDataPath() + "shaders/options");
	lineShader.load(getVisualSystemDataPath() +"shaders/attenuatelines");
	clusterShader.load(getVisualSystemDataPath() + "shaders/cluster");
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
	
	cam.applyRotation = cam.applyTranslation = !cursorIsOverGUI();
	
	easeCamera.setDistance(100);
	ofVec3f curPosition = easeCamera.getTarget().getPosition();
	ofVec3f curTarget = (currentNodePosition*meshExpansion);
	ofVec3f newPos = curPosition + (curTarget - curPosition) * .05;
	easeCamera.setTarget(newPos);
	percentToDest = (newPos - cameraStartPosition).length() / (curTarget - cameraStartPosition).length();
	
//	cout << "** PERCENT TO DEST IS " << percentToDest << endl;
	
	/*
//	easeCamera.setTarget( clusterMesh.getCentroid() );
//	int vertEndIndex = ofMap(timeline->getPercentComplete(), lineStartTime, lineEndTime, 0, traversal.getVertices().size());
	int vertEndIndex;
	if(incrementalTraversalMode){
		currentVertIndex += (traversalMesh.getVertices().size() - currentVertIndex) * .1;
		vertEndIndex = currentVertIndex;
	}
	else{
		vertEndIndex = ofMap(timeline->getPercentComplete(), 0, 1.0, 0, traversalMesh.getVertices().size());
	}
	
	int vertsToHighlight = ofClamp(vertEndIndex,0, traversalMesh.getVertices().size() - 1);
	int lineDissolveVerts = vertEndIndex*lineDissolve;
	
	float nodeSize = powf(traversedNodeSize, 2);
	for(int i = 0; i < vertsToHighlight; i++){;
		//		float fade = ofMap(i, vertsToHighlight*.9, vertsToHighlight, 1.0, 0, true);
		float alpha = ofMap(i, vertEndIndex, vertEndIndex - nodePopLength, 0.0, 1.0, true);
		float dissolveAlpha = 1.0;
		if(lineDissolveVerts > 0){
			dissolveAlpha = ofMap(i, lineDissolveVerts, lineDissolveVerts+20, 0.0, 1.0, true);
		}
		
//		ofFloatColor currentColor = lineColor->getColorAtPosition(alpha);
//		traversalMesh.setColor(i, currentColor * dissolveAlpha);
		ofFloatColor currentColor(1.0,0,0,1.0);
		traversalMesh.setColor(i, currentColor * dissolveAlpha);
		
//JG RE ADD NODE POINTS!
//		if(traversalIndexToNodeIndex.find(i) != traversalIndexToNodeIndex.end()){
			//traversedNodePoints.getNormals()[ traversalIndexToNodeIndex[i ] ].x = 1.0;
			//			cout << "setting color of  line point " << i << " to node index " << endl;
//			traversedNodePoints.getNormals()[ traversalIndexToNodeIndex[i] ].x = nodeSize*nodeBounce, alpha;
//			traversedNodePoints.getColors()[  traversalIndexToNodeIndex[i] ] = currentColor;
//		}
//END RE ADD
	}
	
	for(int i = vertsToHighlight; i < traversalMesh.getVertices().size(); i++){
		traversalMesh.setColor(i, ofFloatColor(0));
//JG RE ADD NODE POINTS!
//		if(traversalIndexToNodeIndex.find(i) != traversalIndexToNodeIndex.end()){
//			traversedNodePoints.getNormals()[ traversalIndexToNodeIndex[i] ].x = 0.0;
//		}
//END RE ADD		
	}
	*/
	
//	if(traversalMesh.getVertices().size() > 0 && vertsToHighlight < traversalMesh.getVertices().size()-1){
//		trailHead = traversalMesh.getVertices()[vertsToHighlight];
//		easeCamera.setDistance(100);
//		ofVec3f curTarget = easeCamera.getTarget().getPosition();
//		ofVec3f newTarget = (trailHead*meshExpansion - curTarget) * .01;
//		easeCamera.setTarget( trailHead*meshExpansion );
//	}
	
}


// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemClusterMap::selfDraw(){

	glDisable(GL_DEPTH_TEST);
	
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	
	////POINTS
	ofPushStyle();
	clusterShader.begin();
	clusterShader.setUniformTexture("tex", sprite, 0);
	clusterShader.setUniform1f("expansion", meshExpansion);
	clusterShader.setUniform1f("minSize", pointSize);
	clusterShader.setUniform3f("attractor", 0, 0, 0);
	clusterShader.setUniform1f("radius", 300.);
	
	ofEnablePointSprites();
	ofDisableArbTex();
	nodeMesh.draw();
	ofDisablePointSprites();
	ofEnableArbTex();

	clusterShader.end();
	
	ofPopStyle();
	/////END POINTS
	
	///NETWORK LINES
	ofPushStyle();
	lineShader.begin();
	lineShader.setUniform1f("focalPlane", powf(lineFocalDistance,2));
	lineShader.setUniform1f("focalRange", powf(lineFocalRange,2));
	lineShader.setUniform1f("lineFade", lineAlpha);
	lineShader.setUniform1f("expansion", meshExpansion);
	lineShader.setUniform3f("attractor", trailHead.x, trailHead.y, trailHead.z);
	lineShader.setUniform1f("radius", 300.);
	
	ofSetColor(100, 150, 200);
	connectionMesh.draw();
	lineShader.end();
	
	ofPopStyle();
	///END NETWORK LINES
	
	//////TRAVERSAL
	ofPushMatrix();
	ofEnableAlphaBlending();
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	ofScale(meshExpansion,meshExpansion,meshExpansion);
	traversalShader.begin();
	traversalShader.setUniform1f("percentTraverseRevealed", percentToDest);
	traversalMesh.draw();
	traversalShader.end();
	ofPopMatrix();
	///END TRAVERSAL
	
	
	/////OPTIONS
//	ofPushStyle();
//	ofPushMatrix();
//	ofScale(meshExpansion,meshExpansion,meshExpansion);
//	optionsShader.begin();
//	optionsShader.setUniform1f("percentOptionsRevealed", percentToDest);
//	optionsShader.setUniform1i("fadeIn", 1);
//	optionsMeshNext.draw();
//	optionsShader.setUniform1i("fadeIn", 0);
//	optionsMeshPrev.draw();
//	optionsShader.end();
//	ofPopMatrix();
//	ofPopStyle();
	/////END OPTIONS
	
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
		cam.reset();
	}
	
	if(key == 'G'){
		traverse();
	}
	
	if(key == 'R'){
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
};
