//
//  CloudsVisualSystemClusterMap.cpp
//

#include "CloudsVisualSystemClusterMap.h"
//#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

#include "CloudsFCPParser.h"

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemClusterMap::selfSetupGui(){

	generatorGui = new ofxUISuperCanvas("GENERATOR", gui);
	generatorGui->copyCanvasStyle(gui);
	generatorGui->copyCanvasProperties(gui);
	generatorGui->setName("Generator");
	generatorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
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
//	generatorGui->addSlider("replicate point size", 1, 50, &replicatePointSize);
//	generatorGui->addSlider("min attract radius",  10, 1000, &minAttractRadius);
//	generatorGui->addSlider("min repel radius",  0, 1000, &minRepelRadius);
//	generatorGui->addSlider("min fuse radius",  1, 100, &minFuseRadius);
//	generatorGui->addSlider("max attract force",  0, 1.0, &maxAttractForce);
//	generatorGui->addSlider("max repel force",  0, 1.0, &maxRepelForce);
//	generatorGui->addSlider("max traverse angle",  0, 180, &maxTraverseAngle);

	ofAddListener(generatorGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(generatorGui);
	guimap[generatorGui->getName()] = generatorGui;

	displayGui = new ofxUISuperCanvas("DISPLAY", gui);
	displayGui->copyCanvasStyle(gui);
	displayGui->copyCanvasProperties(gui);
	displayGui->setName("Display");
	displayGui->setWidgetFontSize(OFX_UI_FONT_SMALL);

//	displayGui->addSlider("node pop length", 50, 2000, &nodePopLength);
//	displayGui->addSlider("line blur amount",  0, 10, &lineBlurAmount);
//	displayGui->addSlider("line blur fade",  0, 1.0, &lineBlurFade);
//	
//	displayGui->addSlider("line start",  0, 1.0, &lineStartTime);
//	displayGui->addSlider("line end",  0, 1.0, &lineEndTime);
//	displayGui->addSlider("line fade verts", 1, 10, &lineFadeVerts);
//	displayGui->addSlider("line focal dist", 0, sqrt(3000), &lineFocalDistance);
//	displayGui->addSlider("line focal range", 0, sqrt(3000), &lineFocalRange);
//	displayGui->addSlider("line width", .5, 2.0, &lineWidth);
//	displayGui->addSlider("line dissolve", 0, 1.0, &lineDissolve);
//	displayGui->addSlider("line thickness",  1, 10, &lineThickness);
//	displayGui->addSlider("line alpha",  0, 1.0, &lineAlpha);
//
//	displayGui->addSlider("node bounce", 0, 1.0, &nodeBounce);
//	displayGui->addSlider("cluster node size", 1, 10, &clusterNodeSize);
//	displayGui->addSlider("traversed node size", 1, 10, &traversedNodeSize);

	ofAddListener(displayGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(displayGui);
	guimap[generatorGui->getName()] = displayGui;

//	traversedNodePoints.setUsage( GL_DYNAMIC_DRAW );
//	traversedNodePoints.setMode(OF_PRIMITIVE_POINTS);
//	nodeCloudPoints.enableNormals();
//	
//	loadShader();
}

//void CloudsVisualSystemClusterMap::loadShader(){
//	billboard.load(getVisualSystemDataPath() + "shaders/Billboard");
//	billboard.begin();
//	billboard.setUniform1i("tex", 0);
//	billboard.end();
//	
//	ofDisableArbTex();
//	nodeSprite.loadImage(getVisualSystemDataPath() + "images/dot.png");
//	nodeSpriteBasic.loadImage(getVisualSystemDataPath() + "images/dot_no_ring.png");
//	ofEnableArbTex();
//	
//	lineAttenuate.load(getVisualSystemDataPath() +"shaders/attenuatelines");
//	
//	gaussianBlur.load(getVisualSystemDataPath() + "shaders/gaussianblur");
//}

void CloudsVisualSystemClusterMap::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

void CloudsVisualSystemClusterMap::setQuestions(vector<CloudsClip>& questionClips){
	selectedQuestion = NULL;
	questions.clear();
	
	for(int i = 0; i < questionClips.size(); i++){
		
		CloudsQuestion q;
		q.cam = &getCameraRef();
		q.font = &font;
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

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemClusterMap::selfSetup(){
	cam.setup();
	cam.autosavePosition = true;
	cam.loadCameraPosition();
	
	buildEntireCluster();
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemClusterMap::selfPresetLoaded(string presetPath){
	timeline->setLoopType(OF_LOOP_NONE);
//	generate();
//	traverse();
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
	/*
	int vertEndIndex = ofMap(timeline->getPercentComplete(), lineStartTime, lineEndTime, 0, traversal.getVertices().size());
	int vertsToHighlight = ofClamp(vertEndIndex,0,traversal.getVertices().size()-1);
	int lineDissolveVerts = vertEndIndex*lineDissolve;
	
	float nodeSize = powf(traversedNodeSize, 2);
	for(int i = 0; i < vertsToHighlight; i++){;
		//		float fade = ofMap(i, vertsToHighlight*.9, vertsToHighlight, 1.0, 0, true);
		float alpha = ofMap(i, vertEndIndex, vertEndIndex-nodePopLength, 0.0, 1.0, true);
		float dissolveAlpha = 1.0;
		if(lineDissolveVerts > 0){
			dissolveAlpha = ofMap(i, lineDissolveVerts, lineDissolveVerts+20, 0.0, 1.0, true);
		}
		
		ofFloatColor currentColor = lineColor->getColorAtPosition(alpha);
		traversal.setColor(i, currentColor * dissolveAlpha);
		if(traversalIndexToNodeIndex.find(i) != traversalIndexToNodeIndex.end()){
			//traversedNodePoints.getNormals()[ traversalIndexToNodeIndex[i ] ].x = 1.0;
			//			cout << "setting color of  line point " << i << " to node index " << endl;
			traversedNodePoints.getNormals()[ traversalIndexToNodeIndex[i] ].x = nodeSize*nodeBounce, alpha;
			traversedNodePoints.getColors()[  traversalIndexToNodeIndex[i] ] = currentColor;
		}
	}
	
	for(int i = vertsToHighlight; i < traversal.getVertices().size(); i++){
		traversal.setColor(i, ofFloatColor(0));
		if(traversalIndexToNodeIndex.find(i) != traversalIndexToNodeIndex.end()){
			traversedNodePoints.getNormals()[ traversalIndexToNodeIndex[i ] ].x = 0.0;
		}
	}
	
	if(traversal.getVertices().size() > 0){
		trailHead = traversal.getVertices()[vertsToHighlight];
	}
	
	
	float clusterNodeBaseSize =  clusterNodeSize;
	vector<ofVec3f>& normals = nodeCloudPoints.getNormals();
	for(int i = 0; i < nodes.size(); i++){
		if(nodes[i]->nodePointIndex != -1){
			normals[ nodes[i]->nodePointIndex ].x = (normals[ nodes[i]->nodePointIndex ].y - normals[ nodes[i]->nodePointIndex ].z)* clusterNodeBaseSize;
		}
	}
	 
	 */
	
}


void CloudsVisualSystemClusterMap::buildEntireCluster(){
	CloudsFCPParser parser;
	parser.loadFromFiles();
	
	for(int i = 0; i < parser.getAllClips().size(); i++){
		testPoints.push_back(parser.getAllClips()[i].networkPosition * 500);
	}

}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemClusterMap::selfDraw(){
	
/*
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	
	
	ofSetColor(255);
	
	ofSetLineWidth(	lineWidth );
	lineAttenuate.begin();
	lineAttenuate.setUniform1f("focalPlane", powf(lineFocalDistance,2));
	lineAttenuate.setUniform1f("focalRange", powf(lineFocalRange,2));
	lineAttenuate.setUniform1f("lineFade", lineAlpha);
	lineAttenuate.setUniform3f("attractor", trailHead.x, trailHead.y, trailHead.z);
	lineAttenuate.setUniform1f("radius", 300.);
	
	geometry.setMode(OF_PRIMITIVE_LINE_STRIP);
	geometry.draw();
	
	lineAttenuate.end();
	
	traversal.setMode(OF_PRIMITIVE_LINE_STRIP);
	traversal.draw();
	
	//POINT SPRITES
	billboard.begin();
	ofEnablePointSprites();
	ofDisableArbTex();
	
	nodeSpriteBasic.getTextureReference().bind();
//	ofSetColor(255*.5);
	
	//glPointSize(replicatePointSize);
	billboard.setUniform1f("minSize", replicatePointSize);
	billboard.setUniform3f("attractor", trailHead.x, trailHead.y, trailHead.z);
	billboard.setUniform1f("radius", 300.);
	
	nodeCloudPoints.drawVertices();
	nodeSpriteBasic.getTextureReference().unbind();
	
	//FUZZIES
	//	glPointSize(pointSize);
	//	ofSetColor(255*.15);
	//	nodeRingSprite.getTextureReference().bind();
	//	nodeCloudPoints.drawVertices();
	//	nodeRingSprite.getTextureReference().unbind();
	
	//NODES
	nodeSprite.getTextureReference().bind();
	traversedNodePoints.drawVertices();
	nodeSprite.getTextureReference().unbind();
	
	billboard.end();
	ofDisablePointSprites();
	ofEnableArbTex();
	
	ofPopStyle();

	//line blur
//	gaussianBlur.begin();
	
	ofPushStyle();
	
	//ofSetColor(255 * lineBlurFade);
//	ofSetColor(255*lineBlurFade);
	
//	gaussianBlur.setUniform1i("tex", 0);
//	gaussianBlur.setUniform2f("sampleOffset", lineBlurAmount, 0);
//	lineBlurTarget.getTextureReference().draw(0, 0, 1920, 1080);
//	
//	gaussianBlur.setUniform2f("sampleOffset", 0, lineBlurAmount);
//	lineBlurTarget.getTextureReference().draw(0, 0,1920, 1080);
	
//	gaussianBlur.end();
	
	ofPopStyle();
 */
	
	
	ofMesh m;
	m.addVertices(testPoints);
	m.drawVertices();
	

	easeCamera.setTarget( m.getCentroid() );
	
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
	
//	if(key == ' '){
//		timeline.togglePlay();
//	}
	
//	if(key == 'g'){
//		generate();
//	}
//	
//	if(key == 't'){
//		traverse();
//	}
	
//	if(key == 'S'){
//		cout << "Loading shader!" << endl;
//		loadShader();
//	}
}

/*
void CloudsVisualSystemClusterMap::generate(){
	for(int i = 0; i < nodes.size(); i++){
		delete nodes[i];
	}
	
	nodes.clear();
	geometry.clear();
	nodeCloudPoints.clear();
	fusePoints.clear();
	srand(seed);
	
	for(int i = 0; i < heroNodes; i++){
		Node* n = new Node();
		//n->position = ofRandomPointOnUnitSphere() * (heroRadius + ofRandom(heroRadiusVariance));
		n->position = ofRandomPointOnUnitSphere() * (heroRadius + powf(ofRandomuf(),2)*heroRadiusVariance);
		n->sourceId = i;
		n->minDistance = minDistance;
		n->distanceRange = distanceRange;
		n->numReplications = numBranches;
		n->numIterations = numIterations;
		n->mesh = &geometry;
		n->stepSize = stepSize;
		n->numSurvivingBranches = numSurvivingBranches;
		n->minFuseRadius = minFuseRadius;
		
		//TODO: add color
		n->nodeColorTrack = nodeColor;
		n->lineColor = timeline->getColor("line color");
		n->nodeColor = timeline->getColor("node color");
		n->replicatePointDistance = replicatePointDistance;
		n->replicatePointSize = replicatePointSize;
		n->numPointsAtReplicate = numPointsAtReplicate;
		
		n->minAttractRadius = minAttractRadius;
		n->minRepelRadius = minRepelRadius;
		n->minFuseRadius = minFuseRadius;
		
		n->maxAttractForce = maxAttractForce;
		n->maxRepelForce = maxRepelForce;
		
		nodes.push_back(n);
	}
	
	for(int i = 0; i < numIterations; i++){
		
		if(geometry.getVertices().size() > 200000){
			break;
		}
		
		int numNodes = nodes.size();
		for(int n = 0; n < numNodes; n++){
			nodes[n]->targeted = false;
		}
		for(int n = 0; n < numNodes; n++){
			if(nodes[n]->leaf){
				nodes[n]->replicate( nodes, nodeCloudPoints.getVertices(), nodeCloudPoints.getNormals());
			}
		}
	}
	
	int numNodes = nodes.size();
	for(int n = 0; n < numNodes; n++){
		if(nodes[n]->leaf){
			nodes[n]->finalize();
		}
	}
}
*/

/*
void CloudsVisualSystemClusterMap::traverse(){
	
	cout << "Traversing!" << endl;
	
	//find the nodes with the greatest distance and to get there
	Node* n1, *n2 = NULL;
	float maxDistance = 0;
	for(int i = 0; i < nodes.size(); i++){
		for(int j = 0; j < nodes.size(); j++){
			float dist = nodes[i]->position.distanceSquared(nodes[j]->position);
			if( i!=j && dist > maxDistance &&
			   nodes[j]->replicated && nodes[j]->numConnections > 4 &&
			   nodes[i]->replicated && nodes[i]->numConnections > 4){
				maxDistance = dist;
				n1 = nodes[j];
				n2 = nodes[i];
			}
		}
	}
	
	if(n1 == NULL || n2 == NULL) return;
	
	traversal.clear();
	
	int numTries = 0;
	ofVec3f position = n1->position;
	ofVec3f direction = (n2->position - n1->position).normalized();
	cout << "traversing " << position << " to " << n2->position << endl;
	
	//select an array of nodes along the path
	traversedNodes.clear();
	traversalIndexToNodeIndex.clear();
	traversedNodePoints.clear();
	
	Node* cur = n1;
	while(cur != n2){
		traversalIndexToNodeIndex[traversal.getVertices().size()] = traversedNodes.size();
		
		traversedNodes.push_back(cur);
		traversedNodePoints.addVertex(cur->position);
		traversedNodePoints.addNormal(ofVec3f(0,0,0));
		traversedNodePoints.addColor(ofFloatColor(1));
		
		ofVec3f dirToTarget = (n2->position - cur->position);
		float closestDistance = dirToTarget.lengthSquared();
		Node* closestNode = n2;
		for(int i = 0; i < nodes.size(); i++){
			
			if(!nodes[i]->replicated){
				continue;
			}
			
			bool containsNode = false;
			for(int j = 0; j < traversedNodes.size(); j++){
				if(traversedNodes[j] == nodes[i]){
					containsNode = true;
					break;
				}
			}
			
			if(containsNode){
				//				cout << "	already contains node at index continuing " << i << endl;
				continue;
			}
			
			dirToTarget = (n2->position - position);
			ofVec3f dirToNode = (nodes[i]->position - position);
			float angleTo = dirToTarget.angle(dirToNode);
			if(angleTo > maxTraverseAngle){
				continue;
			}
			
			float lengthSquared = dirToNode.lengthSquared();
			if(lengthSquared < closestDistance){
				//				cout << "	current closest node is " << sqrt(lengthSquared) << " at index " << i << endl;
				closestNode = nodes[i];
				closestDistance = lengthSquared;
			}
		}
		
		//		cout << " distance " << sqrt(closestDistance) << " / " << position.distance(n2->position) << endl;
		//move towards this node smoothly
		
		int numSteps = 0;
		float currentDistance = dirToTarget.length();
		while(currentDistance > 2){
			float dampen = ofMap(currentDistance, 20, 2, .05, 1, true);
			direction += ( (dirToTarget / currentDistance) - direction) * dampen;
			direction.normalize();
			//			direction = dirToTarget.normalized();
			position  += direction * MIN(1, direction.length());
			traversal.addColor(ofFloatColor(0));
			traversal.addVertex(position);
			dirToTarget = (closestNode->position - position);
			currentDistance = dirToTarget.length();
			if(numSteps++ > 10000){
				cout << "failed with 10000 steps";
				break;
			}
		}
		
		//		cout << "added " << numSteps << " steps " << endl;
		cur = closestNode;
		
	}
	
	
	traversalIndexToNodeIndex[traversedNodes.size()] = traversal.getVertices().size();
	traversedNodes.push_back(n2);
	traversal.addColor(ofFloatColor(0));
	traversal.addVertex(n2->position);
	
	cout << "traversing took " << traversal.getVertices().size() << " steps " << endl;
}
*/


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