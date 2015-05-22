//
//  CloudsVisualSystemClusterMap.cpp
//

#include "CloudsVisualSystemClusterMap.h"
#include "CloudsGlobal.h"
#include "CloudsFCPParser.h"
#include "CloudsAct.h"
#include "CloudsLocalization.h"

CloudsVisualSystemClusterMap::CloudsVisualSystemClusterMap(){
	run = NULL;
	act = NULL;

	matchLineColor = false;
	
	parser = NULL;

	autoTraversePoints = false;
	finishedTraversing = false;
	
	flickerWidth = 0;
	traverseNextFrame = false;
	lastTraverseStartedIndex = 0;
	firstClip = false;
	currentNodeIndex = 0;
	currentTraversalIndex = 0;	
	percentToDest = 0.0;	
	meshExpansion = 0.0;
	nodePopLength = 0.0;
	lineDensity = 0.0;
	lineFocalDistance = 0.0;
	lineFocalRange = 0.0;
	networkLineWidth = 0.0;
	
	drawTraversalPoints = false;
	lockCameraAxis = false;
    useQuestionCam = false;
	traverseCamFOV = 0;
	traversCameraDistance = 0;
	traversedNodeSize = 0;
	traverseStepSize = 0;
	traverseAngleDampen = 0;
	traverseHomingMinDistance = 0;
	traverseMinSolvedDistance = 0;
	traverseLineWidth = 0;

	bConstrainTraversal = false;
    maxTraverseAngle = 0;
    maxTraverseDistance = 0;
    minTraverseDistance = 0;
    minTraverseNextOptions = 0;
	matchLineColor = false;
	lineColorMixExponent = 0.0;
	drawLineFlickerDebug = false;
	lineFlickerIntensity = 0;
	lineFlickerFrequency = 0;
	lineFlickerDampening = 0;
	bSmoothLines = false;
	
	matchTraversalColor = false;
	traverseFalloff = 0;
	traverseRevealVerts = 0;
	traverseAnimationDuration = 0;
	optionsAnimationDuration = 0;
	optionLineWidth = 0;
	
	drawHomingDistanceDebug = false;
	traverseStartTime = 0;
	percentTraversed = 0;
	percentOptionsRevealed = 0;
	drawType = false;
    drawType3D = false;
	baseFontSize = 0;
	selectedTopicChanged = false;
    
    drawAssociation = false;
    associationFontSize = 0;
    currentAssociationFont = 0;
    numTraversed = 0;
	curQuestionCamRotation = 0;
    displayQuestions = false;
    skipCameraSweep = false;
    selectedTopicChangedTime = 0;
	///END INIT
}

void CloudsVisualSystemClusterMap::selfSetDefaults(){

	drawNodes = true;
	drawLines = true;
	drawTraversal = true;
	drawOptionPaths = true;
	drawTraversalPoints = false;
	drawHomingDistanceDebug = false;
	drawLineFlickerDebug = false;
	bSmoothLines = false;
	autoTraversePoints = false;
	finishedTraversing = false;

	
	flickerWidth = 100;
    numTraversed = 0;
    
	clipsShowTopic = ofIntRange(20, 40);
	drawType = true;
	baseFontSize = 12;
	currentTypeSizeRange.min = currentTypeSizeRange.max = -1;
	typeSizeRange.min = 5;
	typeSizeRange.max = 14;
	lineDensity = 200;
	
	lineFlickerIntensity = 7.;
	lineFlickerFrequency = 100;
	lineFlickerDampening = .05;
    
    bConstrainTraversal = false;
    maxTraverseDistance = 1000;
    maxTraverseAngle = 180;
    
    minTraverseNextOptions = 3;
    minTraverseDistance = 5;
    
    drawAssociation = false;
    associationFontSize = -1;
    currentAssociationFont = 8;
    
	traverseNextFrame = false;
	questionScale = .1;
    questionCameraDistance = 10;
    
    useQuestionCam = false;
    selectedQuestion = NULL;
    caughtQuestion = NULL;
    type3DScale = 1.0;
    drawType3D = false;
    maxTypeAttenuateDistance = 50;
    minTypeAttenuateDistance = 300;
    typeClipDistance = 20;
    
	questionScale = 0.1f;
	questionFontScale = 14;
	currentQuestionFontSize = 10;
	questionFontSize = 5;
	questionFontScale = 1.0;
	questionFontTracking = 0;
	questionFontY = 0;

	hideCursors = false;

	firstClip = true;
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemClusterMap::selfSetupGui(){
	
	followCamGui = new ofxUISuperCanvas("FOLLOW CAM", gui);
	followCamGui->copyCanvasStyle(gui);
	followCamGui->copyCanvasProperties(gui);
	followCamGui->setName("FollowCam");
	followCamGui->addSlider("CAMERA DISTANCE", 10, 400, &traversCameraDistance);
	followCamGui->addToggle("LOCK AXIS", &lockCameraAxis);
	followCamGui->addToggle("USE TOPIC CAM", &useTopicCam);
	followCamGui->addSlider("FOV", 4, 90, &traverseCamFOV);
	followCamGui->addToggle("HIDE CURSOR", &hideCursors);

	ofAddListener(followCamGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(followCamGui);
	guimap[followCamGui->getName()] = followCamGui;
	
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
	
	float length = (linesGui->getGlobalCanvasWidth()-linesGui->getWidgetSpacing()*5)/3.;
    float dim = linesGui->getGlobalSliderHeight();
	linesGui->addSlider("DENSITY", 100, 1000., &lineDensity);
	linesGui->addSlider("FOCAL DISTANCE", 0, sqrt(1000.0f), &lineFocalDistance);
	linesGui->addSlider("FOCAL RANGE", 0, sqrt(1000.0f), &lineFocalRange);
	linesGui->addSlider("FLICKER FREQUENCY", 10, 100, &lineFlickerFrequency);
	linesGui->addSlider("FLICKER INTENSITY", 3, 40, &lineFlickerIntensity);
	linesGui->addSlider("FLICKER DAMPENING", 0.0, 1.0, &lineFlickerDampening);
	linesGui->addToggle("FLICKER DEBUG", &drawLineFlickerDebug);
	linesGui->addLabel("NODE COLOR");
    linesGui->addMinimalSlider("LN_HUE", 0.0, 1.0, &lineNodeColorHSV.r, length, dim)->setShowValue(false);
    linesGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    linesGui->addMinimalSlider("LN_SAT", 0.0, 1.0, &lineNodeColorHSV.g, length, dim)->setShowValue(false);
    linesGui->addMinimalSlider("LN_BRI", 0.0, 1.0, &lineNodeColorHSV.b, length, dim)->setShowValue(false);
    linesGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	linesGui->addMinimalSlider("LN_A", 0.0, 1.0, &lineNodeColorHSV.a);
	linesGui->addLabel("EDGE COLOR");
    linesGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	linesGui->addButton("MATCH", &matchLineColor);
    linesGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    linesGui->addMinimalSlider("LE_HUE", 0.0, 1.0, &lineEdgeColorHSV.r, length, dim)->setShowValue(false);
    linesGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    linesGui->addMinimalSlider("LE_SAT", 0.0, 1.0, &lineEdgeColorHSV.g, length, dim)->setShowValue(false);
    linesGui->addMinimalSlider("LE_BRI", 0.0, 1.0, &lineEdgeColorHSV.b, length, dim)->setShowValue(false);
    linesGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	linesGui->addMinimalSlider("LE_A", 0.0, 1.0, &lineEdgeColorHSV.a);
	linesGui->addMinimalSlider("COLOR MIX EXPONENT", 0, 20.0, &lineColorMixExponent);
	linesGui->addSlider("LINE WIDTH", .5, 3, &networkLineWidth);
	linesGui->addToggle("SMOOTHING", &bSmoothLines);
	
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
	
	//this is for the main cluster map visual
	traversalGui->addToggle("AUTO TRAVERSE", &autoTraversePoints);
	traversalGui->addLabel("HEAD COLOR");
    traversalGui->addMinimalSlider("TH_HUE", 0.0, 1.0, &traverseHeadColorHSV.r, length, dim)->setShowValue(false);
    traversalGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    traversalGui->addMinimalSlider("TH_SAT", 0.0, 1.0, &traverseHeadColorHSV.g, length, dim)->setShowValue(false);
    traversalGui->addMinimalSlider("TH_BRI", 0.0, 1.0, &traverseHeadColorHSV.b, length, dim)->setShowValue(false);
    traversalGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	traversalGui->addMinimalSlider("TH_A", 0.0, 1.0, &traverseHeadColorHSV.a);
	traversalGui->addLabel("TAIL COLOR");
    traversalGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	traversalGui->addButton("MATCH", &matchTraversalColor);
    traversalGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    traversalGui->addMinimalSlider("TT_HUE", 0.0, 1.0, &traverseTailColorHSV.r, length, dim)->setShowValue(false);
    traversalGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    traversalGui->addMinimalSlider("TT_SAT", 0.0, 1.0, &traverseTailColorHSV.g, length, dim)->setShowValue(false);
    traversalGui->addMinimalSlider("TT_BRI", 0.0, 1.0, &traverseTailColorHSV.b, length, dim)->setShowValue(false);
    traversalGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	traversalGui->addMinimalSlider("TT_A", 0.0, 1.0, &traverseTailColorHSV.a);
	traversalGui->addMinimalSlider("FALLOFF", 0, 1.0, &traverseFalloff);
    traversalGui->addMinimalSlider("TRAIL REVEAL", 0, 2000, &traverseRevealVerts);
    
	traversalGui->addSpacer();
	traversalGui->addSlider("ANGLE DAMPEN", 0.01, .5, &traverseAngleDampen);
	traversalGui->addSlider("STEP SIZE", .2, 10, &traverseStepSize);
	traversalGui->addSlider("MIN HOMING DISTANCE", 1.0, 100.0, &traverseHomingMinDistance);
	traversalGui->addSlider("MIN SOLVE DISTANCE", .1, 5, &traverseMinSolvedDistance);
	traversalGui->addToggle("DRAW POINTS", &drawTraversalPoints);
	traversalGui->addToggle("DRAW DISTANCE DEBUG", &drawHomingDistanceDebug);
    traversalGui->addSpacer();
    traversalGui->addToggle("CONSTRAIN", &bConstrainTraversal);
    traversalGui->addSlider("MAX DISTANCE", 0, 30, &maxTraverseDistance);
    traversalGui->addSlider("MIN DISTANCE", 0, 5, minTraverseDistance);
    traversalGui->addSlider("MAX ANGLE", 0, 180, &maxTraverseAngle);
    traversalGui->addIntSlider("MIN NEXT OPTIONS", 0, 4, &minTraverseNextOptions);
    
	traversalGui->addLabel("ANIMATE");
	traversalGui->addSlider("ANIMATE DURATION", 1, 4, &traverseAnimationDuration);
	traversalGui->addSlider("LINE WIDTH", .5, 3, &traverseLineWidth);
	
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
	optionPathsGui->addSlider("LINE WIDTH", .5, 3, &optionLineWidth);
	optionPathsGui->addLabel("COLOR");
    optionPathsGui->addMinimalSlider("OP_HUE", 0.0, 1.0, &optionColorHSV.r, length, dim)->setShowValue(false);
    optionPathsGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    optionPathsGui->addMinimalSlider("OP_SAT", 0.0, 1.0, &optionColorHSV.g, length, dim)->setShowValue(false);
    optionPathsGui->addMinimalSlider("OP_BRI", 0.0, 1.0, &optionColorHSV.b, length, dim)->setShowValue(false);
    optionPathsGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	optionPathsGui->addMinimalSlider("OP_A", 0.0, 1.0, &optionColorHSV.a);
	
	ofAddListener(optionPathsGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(optionPathsGui);
	guimap[optionPathsGui->getName()] = optionPathsGui;
	
	typeGui = new ofxUISuperCanvas("TOPIC TYPE", gui);
	typeGui->copyCanvasStyle(gui);
	typeGui->copyCanvasProperties(gui);
	typeGui->setName("TopicType");
	typeGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	toggle = typeGui->addToggle("ENABLE", &drawType);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    typeGui->resetPlacer();
    typeGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    typeGui->addWidgetToHeader(toggle);
	
	typeGui->addIntSlider("SIZE MIN", 5, 20, &typeSizeRange.min);
	typeGui->addIntSlider("SIZE MAX", 5, 20, &typeSizeRange.max);
	typeGui->addIntSlider("MIN CLIP NUM", 1, 60, &clipsShowTopic.min);
	typeGui->addIntSlider("MAX CLIP SCALE", 1, 60, &clipsShowTopic.max);
	
    typeGui->addSpacer();
    typeGui->addToggle("DRAW ASSOCIATION", &drawAssociation);
    typeGui->addToggle("DRAW 3D", &drawType3D);
    typeGui->addSlider("3D TYPE SCALE", .01, 1.0, &type3DScale);
    typeGui->addIntSlider("ASSOCATION SIZE", 5, 30, &associationFontSize);
	typeGui->addRangeSlider("TYPE 3D ATTENUATION", 0, 500, &minTypeAttenuateDistance, &maxTypeAttenuateDistance);
    typeGui->addSlider("TYPE 3D CLIP", 0, 100, &typeClipDistance);
	ofAddListener(typeGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(typeGui);
	guimap[typeGui->getName()] = typeGui;
	
	
	questionGui = new ofxUISuperCanvas("QUESTIONS", gui);
	questionGui->copyCanvasStyle(gui);
	questionGui->copyCanvasProperties(gui);
	questionGui->setName("Questions");
	questionGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	toggle = questionGui->addToggle("ENABLE", &displayQuestions);
    toggle->setLabelPosition(OFX_UI_WIDGET_POSITION_LEFT);
    questionGui->resetPlacer();
    questionGui->addWidgetDown(toggle, OFX_UI_ALIGN_RIGHT, true);
    questionGui->addWidgetToHeader(toggle);
    
    
    questionGui->addToggle("QUESTION CAM", &useQuestionCam);
	questionGui->addSlider("QUESTION SCALE", 0, 1.0, &questionScale);
	questionGui->addSlider("QUESTION CAM DIST", 0, 100, &questionCameraDistance);
	questionGui->addRangeSlider("QUESTION TUG DISTANCE", 10, 300, &questionTugDistance.min, &questionTugDistance.max);
    questionGui->addSlider("QUESTCAM SPIN", .0, 1.0, &questionCameraSpinSpeed);
    questionGui->addSlider("QUESTCAM AXIS DIST", 0.0, 100.0, &questionCameraAxisDist);
	questionGui->addLabel("QUESTION TYPE");
	questionGui->addIntSlider("FONT SIZE", 4, 100, &questionFontSize);
	questionGui->addSlider("FONT SCALE", 0, 1.0, &questionFontScale);
	questionGui->addSlider("TRACKING", 0, 30, &questionFontTracking);
	questionGui->addSlider("Y POSITION", -200, 200, &questionFontY);

	ofAddListener(questionGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(questionGui);
	guimap[questionGui->getName()] = questionGui;
	
}

void CloudsVisualSystemClusterMap::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//called once from start up of the app
void CloudsVisualSystemClusterMap::buildEntireCluster(CloudsFCPParser& parser){
	this->parser = &parser;
	resetGeometry();
}

void CloudsVisualSystemClusterMap::setRun(CloudsRun& newRun){
	run = &newRun;
}

void CloudsVisualSystemClusterMap::setAct(CloudsAct* newAct){
	act = newAct;
}

void CloudsVisualSystemClusterMap::resetGeometry(){
	
//	cout << "**** RESETING GEOMETRY" << endl;

	firstClip = true;
//	lastTraverseStartedIndex = -1;
	flickerCoord = ofVec2f(0,0);
    
    
	networkMesh.clear();
	traversalMesh.clear();
	optionsMeshNext.clear();
	optionsMeshPrev.clear();
	nodeMesh.clear();
	nodes.clear();
	traversalPath.clear();
	clipIdToNodeIndex.clear();
	
	currentTraversalIndex = 0;
	startTraverseClip = 0;
    
	networkCentroid = ofVec3f(0,0,0);
	ofVec3f maxBounds(0,0,0);
	ofVec3f minBounds(0,0,0);
	
	for(int i = 0; i < parser->getAllClips().size(); i++){

		CloudsClusterNode n;
		CloudsClip* clip = parser->getAllClips()[i];
		//cout << "****POSITIONER GETTING CLIP " << i << " " << clip.getLinkName() << endl;
		n.clipId = clip->getID();
		n.mesh = &nodeMesh;
		n.vertexIndex = nodeMesh.getNumVertices();
		n.networkPosition = clip->networkPosition;
		clipIdToNodeIndex[clip->getID()] = nodes.size();
		nodeMesh.addVertex( n.networkPosition );
		nodeMesh.addNormal( ofVec3f(powf(ofRandomuf(), 3.0), 0, 0) ); //normals control the size, 0 - 1 min to max
		n.flickerTextureCoord = flickerCoord;
		n.luckyNumber = ofRandomuf();
		
		flickerCoord.x++;
		if(flickerCoord.x >= flickerWidth){
			flickerCoord.x = 0;
			flickerCoord.y++;
		}
		networkCentroid += clip->networkPosition;
		maxBounds = ofVec3f(MAX(maxBounds.x,clip->networkPosition.x),
							MAX(maxBounds.y,clip->networkPosition.y),
							MAX(maxBounds.z,clip->networkPosition.z));
		
		minBounds = ofVec3f(MIN(minBounds.x,clip->networkPosition.x),
							MIN(minBounds.y,clip->networkPosition.y),
							MIN(minBounds.z,clip->networkPosition.z));
		
		nodes.push_back(n);
	}
	
	networkCentroid /= nodes.size();

//	cout << "CENTROID IS " << networkCentroid << endl;
	
	float maxDistance = 0;
	for(int i = 0; i < parser->getAllClips().size(); i++){
		maxDistance = MAX(maxDistance, parser->getAllClips()[i]->networkPosition.distance(networkCentroid));
	}
	
	//add all connections to connection mesh
	set< pair<string,string> > connections;
	unsigned long start;
	for(int i = 0; i < parser->getAllClips().size(); i++){

		CloudsClip* clip = parser->getAllClips()[i];

		//cout << "****CONNECTOR GETTING CLIP " << i << "" << parser->getAllClips().size() << " " << clip.getLinkName() << endl;

		vector<CloudsClip*> meta = parser->getClipsWithKeyword(clip->getKeywords());
		vector<CloudsLink> links = parser->getLinksForClip(clip);
		string nameA = clip->getID();
		CloudsClusterNode& n1 = nodes[ clipIdToNodeIndex[nameA] ];
		
		start = ofGetElapsedTimeMillis(); 
		for(int l = 0; l < links.size(); l++){
			meta.push_back(parser->getClipWithLinkName(links[l].targetName));
		}
		
		ofVec3f randDir = randomDirection();
		for(int j = 0; j < meta.size(); j++){
			
			string nameB = meta[j]->getID();
//			cout << "****	CONNECTING CLIP " << j << "" << meta.size() << " " << nameB << endl;
			bool valid = true;
			valid &= (nameA != nameB);
			valid &= (clip->person != meta[j]->person || parser->clipLinksTo(nameA, nameB));
			valid &= !parser->linkIsSuppressed(nameA, nameB);

			if(valid) {
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
				
				if(clip->networkPosition.distance(networkCentroid) > maxDistance * .6) {
//					cout << "filtering outer ring clip " << meta[j].getLinkName() << endl;
					continue;
				}
				
				if(meta[j]->networkPosition.distance(networkCentroid) > maxDistance * .6 ) {
//					cout << "filtering outer ring clip " << meta[j].getLinkName() << endl;
					continue;
				}
				
				NetworkEdge connectionEdge;
				//create curved connection mesh
				//naive simple spherical interpolation over 10 steps
				connectionEdge.startIndex = networkMesh.getNumVertices();
				ofVec3f vecToStart = clip->networkPosition - networkCentroid;
				ofVec3f vecToDest  = meta[j]->networkPosition - networkCentroid;
				float radStart = vecToStart.length();
				float radDest  = vecToDest.length();
				ofVec3f dirToStart = vecToStart / radStart;
				ofVec3f dirToDest  = vecToDest  / radDest;
				ofQuaternion arc;
				arc.makeRotate(dirToStart,dirToDest);
				
				//handle
				ofFloatColor flickermix = ofFloatColor(n1.flickerTextureCoord.x,
													   n1.flickerTextureCoord.y,
													   n2.flickerTextureCoord.x,
													   n2.flickerTextureCoord.y);
				networkMesh.addColor(flickermix);
				networkMesh.addNormal(ofVec3f(0.0, 0.0, 0.0));
				networkMesh.addVertex(clip->networkPosition);

				float distance = clip->networkPosition.distance(meta[j]->networkPosition);
				
				int numSteps = distance * lineDensity;

//				cout << "DISTANCE IS " << distance << " completing with " << numSteps << " steps " << endl;
				
				for(int i = 0; i <= numSteps; i++){
					float stepPercent = (1.* i) / numSteps;
					ofQuaternion step;
					step.slerp(stepPercent, ofQuaternion(), arc);
					ofVec3f arcDir = step * dirToStart;
					float arcRad = ofLerp(radStart, radDest, stepPercent);
					ofVec3f arcPoint = arcDir * arcRad + networkCentroid;
					networkMesh.addColor(flickermix);					
					networkMesh.addNormal(ofVec3f(stepPercent, 0.0, 1.0));
					networkMesh.addVertex(arcPoint);
				}
				
				//handle
				networkMesh.addColor(flickermix);
				networkMesh.addNormal(ofVec3f(1.0, 0.0, 0.0));
				networkMesh.addVertex(meta[j]->networkPosition);
				
				connectionEdge.endIndex = networkMesh.getNumVertices();
				connectionEdge.source = true;
				n1.connectionCurves[n2.clipId] = connectionEdge;
				connectionEdge.source = false;
				n2.connectionCurves[n1.clipId] = connectionEdge;
			}
		}
	}
	
    
    nodeMesh.setMode(OF_PRIMITIVE_POINTS);
	networkMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	traversalMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	optionsMeshNext.setMode(OF_PRIMITIVE_LINE_STRIP);
	optionsMeshPrev.setMode(OF_PRIMITIVE_LINE_STRIP);
	
    kdtree.buildIndex( nodeMesh.getVertices() );
    
	populateTopicPoints();
    //populateAssociations();
    
}

void CloudsVisualSystemClusterMap::populateTopicPoints(){
	
    topicPoints.clear();
    
	//vector<string>& keywords = parser->getContentKeywords();
    set<string>& masterTopics = parser->getMasterTopics();
	int numClips = parser->getNumberOfClipsWithKeyword(*masterTopics.begin());
	clipCountRange = ofRange(numClips,numClips);
    //int i = 0;
    for (set<string>::iterator it = masterTopics.begin(); it != masterTopics.end(); it++) {
		TopicPoint tp;
		tp.keyword  = *it;
		tp.position = parser->getMasterTopicPosition(tp.keyword);
		tp.numClips = parser->getNumberOfClipsWithKeyword(tp.keyword);
		clipCountRange.growToInclude(tp.numClips);
        tp.hoverChangeTime = 0;
        tp.hovered = false;
//		cout << "num clips " << tp.numClips << " current range " << clipCountRange << endl;
		topicPoints.push_back( tp );
    }
    
//	for(int i = 0; i < topicPoints.size(); i++){
//		topicPoints[i].normalizedTopicScale =  clipCountRange.getNormalized(topicPoints[i].numClips);
//	}

}

//void CloudsVisualSystemClusterMap::populateAssociations(){
//    associations.clear();
//    topicset.clear();
//    ofBuffer topicAssociations = ofBufferFromFile(GetCloudsDataPath() + "storyEngineParameters/TopicAssociations.txt");
//    while(!topicAssociations.isLastLine()){
//        
//        string line = topicAssociations.getNextLine();
//        if(line.find(":") == string::npos){
////            cout << "Skipping line " << line << endl;
//            continue;
//        }
//        
//        vector<string> association = ofSplitString(line, ":", true, true);
//        if(association.size() != 2){
////            cout << "line " << line << " has more than one :"<<endl;
//            continue;
//        }
//        
//        vector<string> clipcount = ofSplitString(line, "\t",true,true);
//        if(clipcount.size() != 2){
////            cout << "line " << line << " has more than one tab"<<endl;
//            continue;
//        }
//        
//        string associatedKeyword = association[1];
//        string subtopic = ofSplitString(association[0],"\t",true,true)[1];
//        associations[subtopic] = associatedKeyword;
//        topicset.insert(associatedKeyword);
////        cout << "associated " << subtopic << " with " << associatedKeyword << endl;
//    }
//    
////    for(set<string>::iterator it = topicset.begin(); it != topicset.end(); it++){
////        cout << *it << endl;
////    }
//}

//set<string>& CloudsVisualSystemClusterMap::getTopicSet(){
//    return topicset;
//}

void CloudsVisualSystemClusterMap::allocateFlickerTexture(){

	flickerNoise.allocate(flickerWidth, flickerCoord.y+1, OF_IMAGE_GRAYSCALE);
	flickerNoiseTarget.allocate(flickerWidth, flickerCoord.y+1, OF_IMAGE_GRAYSCALE);
	flickerNoise.getPixelsRef().set(0);
	flickerNoiseTarget.set(0);
}

void CloudsVisualSystemClusterMap::startTraverse(){
    startTraverseClip = traversalPath.size();
    traverse();
}

void CloudsVisualSystemClusterMap::traverse(){
	
	//cout << " CloudsVisualSystemClusterMap::traverse TRAVERSING 1" << endl;
	
	if(act == NULL){
		ofLogError("CloudsVisualSystemClusterMap::traverse") << "Traversed without ACT" << endl;
		finishedTraversing = true;
		percentTraversed = 1.0;
		return;
	}

//	if(currentTraversalIndex < run->clipHistory.size()){
	if(currentTraversalIndex < MIN(4,act->getAllClips().size()) ){
		traverseToClip( act->getClip(currentTraversalIndex) );
		percentTraversed = 0.0;
		currentTraversalIndex++;
	}
	else if(autoTraversePoints){
		finishedTraversing = true;
		percentTraversed = 1.0;
	}

}

void CloudsVisualSystemClusterMap::traverseToClip(CloudsClip* clip){
	
	if(clipIdToNodeIndex.find(clip->getID()) == clipIdToNodeIndex.end()){
		ofLogError("CloudsVisualSystemClusterMap::traverseToClip") << "Isn't included in cluster map";
		return;
	}
	
    numTraversed++;
    
	ofIndexType newNodeIndex = clipIdToNodeIndex[ clip->getID() ];
	CloudsClusterNode n = nodes[ newNodeIndex ];

	ofVec3f startDirection;
	if(firstClip){
//		currentTraversalDirection = randomDirection();
        currentTraversalDirection = clip->networkPosition - networkCentroid;
		currentTraversalPosition = clip->networkPosition;
	}
	else{

		//this is to correct the scales in the sliders to more friendly numbers...
		float localMinSolve = traverseMinSolvedDistance * .001;
		float localStepSize = traverseStepSize * .001;
		float localMinHomingDist = traverseHomingMinDistance * .001;
		TraversalSegment newSegment;
		newSegment.startIndex = traversalMesh.getNumVertices();
		int maxSteps = 1000;
		float currentDistance = currentTraversalPosition.distance(clip->networkPosition);
        
        //check traversal parameters to potentiall fake a new clip
        if(bConstrainTraversal){
        
            //locals are so we can use more human readaable scales
            float localMaxTraversedDistance = maxTraverseDistance/100.0;
            float localMinTraverseDistance = minTraverseDistance/100.0;

            int constrainedPositionIndex = -1;
            float traversalAngle = (clip->networkPosition - currentTraversalPosition).angle(currentTraversalDirection);
            int numOptions = n.connectionCurves.size();
            if(currentDistance > localMaxTraversedDistance ||
               currentDistance < minTraverseDistance ||
               traversalAngle > maxTraverseAngle ||
               numOptions < minTraverseNextOptions)
            {
//                cout << "CLIP FAILED TEST DISTANCE " << currentDistance*100 << " and angle " << traversalAngle << endl;
                
                //find a new options
                vector<float> distsSq;
                vector<NNIndex> indices;
                //search for 100 nearby clips and take the first acceptable one
                kdtree.findNClosestPoints(clip->networkPosition, 1000, indices, distsSq);
                for(int i = 0; i < indices.size(); i++){
                    ofVec3f testPosition = nodeMesh.getVertex( indices[i] );
                    float testDistance = currentTraversalPosition.distance( testPosition );
                    float testAngle = (testPosition - currentTraversalPosition).angle(currentTraversalDirection);
                    int testOptions = nodes[ indices[i] ].connectionCurves.size();
                    
//                    cout << "\tTest node stats" << endl
//                         << "\t\tDIST: " << testDistance*100 << " - max:" << maxTraverseDistance << " min " << minTraverseDistance << endl
//
//                         << "\t\tANGLE: " << testAngle << "/" << maxTraverseAngle << endl
//                         << "\t\tOPTIONS# " << testOptions << "/" << minTraverseNextOptions << endl;
                    
                    if(testDistance <= localMaxTraversedDistance &&
                       testDistance >= localMinTraverseDistance &&
                       testAngle <= maxTraverseAngle &&
                       testOptions >= minTraverseNextOptions)
                    {
                        constrainedPositionIndex = indices[i];
                        cout << "FOUND A BETTER NODE AT INDEX " << i << " OF NEIGHBORS" << endl;
                        cout << "\tANGLE " << testAngle << " DISTANCE " << testDistance*100 << " NUM CONNECTION CURVES " << testOptions << endl;
                        break;
                    }
                }
                
                //we found a better node
                if(constrainedPositionIndex != -1){
                    
                    newNodeIndex = constrainedPositionIndex;
                    clip = parser->getAllClips()[newNodeIndex];
                    //newNodeIndex = clipIdToNodeIndex[ clip.getID() ];
                    n = nodes[ newNodeIndex ];
                }
                else{
//                    ofLogError() <<"Couldn't find a better neighbor going with original clip";
                    //now search for all our nearest neighbors and pick the one that is closets to the desired clip
                    distsSq.clear();
                    indices.clear();
                    //search for 100 nearby clips and take the first acceptable one
                    kdtree.findNClosestPoints(currentTraversalPosition, 1000, indices, distsSq);
                    
                    int closestIndex = -1;;
                    float closestDistance = INT_MAX;
                    for(int i = 0; i < indices.size(); i++){
                        ofVec3f testPosition = nodeMesh.getVertex( indices[i] );
                        float testDistance = currentTraversalPosition.distance( testPosition );
                        float distanceFromRealTarget = n.networkPosition.distance( testPosition ); //this time look at the distance from the target too
                        float testAngle = (testPosition - currentTraversalPosition).angle(currentTraversalDirection);
                        int testOptions = nodes[ indices[i] ].connectionCurves.size();
                        
                        cout << "\tRECOVERY Test node stats" << endl
                             << "\t\tDIST: " << testDistance*100 << " - max:" << maxTraverseDistance << " min " << minTraverseDistance << endl
                             << "\t\tANGLE: " << testAngle << "/" << maxTraverseAngle << endl
                             << "\t\tOPTIONS# " << testOptions << "/" << minTraverseNextOptions << endl;
                        
                        if(testDistance >= localMaxTraversedDistance){
                            break;
                        }
                        
                        if(testDistance <= localMaxTraversedDistance &&
                           testDistance >= localMinTraverseDistance &&
                           testAngle <= maxTraverseAngle &&
                           testOptions >= minTraverseNextOptions)
                        {
                            //constrainedPositionIndex = indices[i];
                            
                            if(distanceFromRealTarget < closestDistance ){
                                closestDistance = distanceFromRealTarget;
                                closestIndex    = indices[i];
                            }
                            
                            cout << "FOUND AN ACCEPTABLE NODE AT INDEX " << i << " OF NEIGHBORS" << endl;
                            cout << "\tANGLE " << testAngle << " DISTANCE " << testDistance*100 << " NUM CONNECTION CURVES " << testOptions << endl;
                            
                        }
                    }
                    
                    if(closestIndex == -1){
                        ofLogError("Couldn't even find an acceptable node close to us, defaulting");
                    }
                    else{
                        newNodeIndex = closestIndex;
                        clip = parser->getAllClips()[newNodeIndex];
                        //newNodeIndex = clipIdToNodeIndex[ clip.getID() ];
                        n = nodes[ newNodeIndex ];
                    }
                }
            }
            
            cout << "TRAVERSED DISTANCE " <<  currentDistance*100 << " ANGLE " << traversalAngle << endl;
        }
        
		while(localMinSolve < currentDistance && maxSteps-- > 0){
			ofVec3f toNodeDirection  = (clip->networkPosition - currentTraversalPosition).normalized();
			ofVec3f vectorToNode = clip->networkPosition - currentTraversalPosition;
			float distanceToNode = vectorToNode.length();
			ofVec3f directionToNode = vectorToNode/distanceToNode;
			
			ofQuaternion rotToNode, incrementalRotate;
			rotToNode.makeRotate(currentTraversalDirection, directionToNode);
			float angleDampen = ofMap(currentDistance, 0, localMinHomingDist, 1.0, traverseAngleDampen, true);
			incrementalRotate.slerp(angleDampen, ofQuaternion(), rotToNode);

			currentTraversalDirection = incrementalRotate * currentTraversalDirection;
			currentTraversalPosition += currentTraversalDirection * localStepSize;
			
			traversalMesh.addVertex(currentTraversalPosition);
			currentDistance = currentTraversalPosition.distance(clip->networkPosition);
		}
		
		//cap it off
		currentTraversalPosition = clip->networkPosition;
		traversalMesh.addVertex(clip->networkPosition);
		
		newSegment.endIndex = traversalMesh.getNumVertices();
		cout << "Step took " << (newSegment.endIndex - newSegment.startIndex) << " vertices" << endl;

		//solidify old segement by flagging n.y = 1
		if(traversalPath.size() > 0){
			TraversalSegment& s = traversalPath.back();
			for(ofIndexType i = s.startIndex; i < s.endIndex; i++){
				traversalMesh.getNormals()[i].x = 1.0;
				traversalMesh.getNormals()[i].y = 1.0;
			}
		}
		
		//add normals along percent to facilitate transition
		for(int i = newSegment.startIndex; i < newSegment.endIndex; i++){
			float percentAlong = ofMap(i, newSegment.startIndex,newSegment.endIndex,0, 1.0);
			traversalMesh.addNormal(ofVec3f(percentAlong,0., i));
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
			
			//z attenuates the handles
			optionsMeshNext.addNormal(ofVec3f(percentComplete,0,networkMesh.getNormal(i).z) );
			optionsMeshNext.addVertex(networkMesh.getVertex(i));
		}
	}
	
	optionsMeshPrev.setMode(OF_PRIMITIVE_LINE_STRIP);
	
	currentNodeIndex  = newNodeIndex;
	traverseStartTime = ofGetElapsedTimef();

	firstClip = false;
}

void CloudsVisualSystemClusterMap::clearTraversal(){
    
    firstClip = true;
    traversalMesh.clear();
    traversalPath.clear();
    
    currentTraversalIndex = 0;
    startTraverseClip = 0;
    
}
void CloudsVisualSystemClusterMap::moveToTopic(string topic){
    topic = ofToLower(topic);
    if(topic == currentTopic || topic == movingToTopic){
        return;
    }
    
    movingToTopic = topic;
    setCurrentTopic("");
    
    bool positionFound = false;
    for(int i = 0; i < topicPoints.size(); i++){
        if(topicPoints[i].keyword == topic){
            targetTopicPosition  = topicPoints[i].position * meshExpansion;
            targetCameraPosition = targetTopicPosition + targetTopicPosition.normalized() * traversCameraDistance;
            targetCameraSideDir  = targetTopicPosition.normalized().getCrossed( ofVec3f(0,1,0) );
            targetCameraUpDir    = targetTopicPosition.normalized().getCrossed( targetCameraSideDir );
            if(skipCameraSweep){
                topicNavCam.setPosition(targetCameraPosition);
                topicNavCam.lookAt(targetTopicPosition, targetCameraUpDir);
                currentCameraSideDir = targetCameraSideDir;
                currentCameraUpDir = targetCameraUpDir;
                skipCameraSweep = false;
            }
            positionFound = true;
            break;
        }
    }
    
    if(!positionFound){
        ofLogError("CloudsVisualSystemClusterMap::setCurrentTopic") << "Couldn't find position for topic " << topic;
    }
    
}

void CloudsVisualSystemClusterMap::setCurrentTopic(string topic){
    topic = ofToLower(topic);
    if(topic == currentTopic){
        return;
    }
    currentTopic = topic;
    selectedTopicChanged = true;
    selectedTopicChangedTime = ofGetElapsedTimef();
}

void CloudsVisualSystemClusterMap::skipNextCameraSweep(){
    skipCameraSweep = true;
}

bool CloudsVisualSystemClusterMap::selectionChanged(){
    bool ret = selectedTopicChanged;
    selectedTopicChanged = false;
    return ret;
}

string CloudsVisualSystemClusterMap::getSelectedKeyword(){
    return currentTopic;
}

ofVec2f CloudsVisualSystemClusterMap::getTopicScreenLocation(){
    return getCameraRef().worldToScreen(targetTopicPosition);
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
//	lineColor = timeline->addColors("line color", "LineColor.xml");
//	nodeColor = timeline->addColorsWithPalette("node color","NodeColor.xml", getVisualSystemDataPath() + "images/nerve_palette.png");
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemClusterMap::selfSetup(){

//	cout << "BUILDING WITH FLICKER WIDTH " << flickerWidth << " AND COORD " << flickerCoord << endl;
	reloadShaders();
}

ofVboMesh& CloudsVisualSystemClusterMap::getNodeMesh(){
    return nodeMesh;
}

ofVboMesh& CloudsVisualSystemClusterMap::getNetworkMesh(){
    return networkMesh;
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
	

	if(!traverseNextFrame && autoTraversePoints && (firstClip || percentTraversed >= 1.0) ){
		//traverseNextFrame = true;
		traverse();
	}

#ifdef OCULUS_RIFT
    //bUseInteractiveCamera = false;
#endif
    
	///UPDATE ANIMATION
	percentTraversed = ofMap(ofGetElapsedTimef(),
							 traverseStartTime, traverseStartTime+traverseAnimationDuration,
							 0, 1.0, true);
	if(autoTraversePoints) {
		percentOptionsRevealed = 0.0;
	}
	else{
		percentOptionsRevealed = ofMap(ofGetElapsedTimef(),
									   traverseStartTime+traverseAnimationDuration,
									   traverseStartTime+traverseAnimationDuration+optionsAnimationDuration,
									   0.0, 1.0, true);
    }

	
	//UPDATE CAMERA
	if(cursorIsOverGUI()){
		easyCamera.disableMouseInput();
	}
	else{
		easyCamera.enableMouseInput();
	}
	
	if(traversalPath.size() > 0 && traversalPath.back().endIndex- traversalPath.back().startIndex > 3){
		float curIndex = ofMap(percentTraversed,
									 0, 1.0,
									 traversalPath.back().startIndex, traversalPath.back().endIndex-2,true);
        
        ofIndexType backInd  = floor(curIndex);
        ofIndexType frontInd = ceil(curIndex);
        ofVec3f frontVert = traversalMesh.getVertex(backInd);
        ofVec3f backVert  = traversalMesh.getVertex(frontInd);
        if(backInd == frontInd){
            trailHead = frontVert;
        }
        else{
            float alpha = ofMap(curIndex, backInd, frontInd, 0., 1.0);
            trailHead = frontVert.getInterpolated(backVert, alpha);
        }
        
		if(lockCameraAxis){
			ofVec3f curPosition = axisCamera.getPosition();
            ofQuaternion nextRot,lastRot,curRot;
			if(autoTraversePoints){
				nextRot.makeRotate(0, ofVec3f(0,1,0));
				lastRot.makeRotate(0, ofVec3f(0,1,0));
			}
			else{
				nextRot.makeRotate(fmod((numTraversed+1)*90,360.0f), ofVec3f(0,1,0));
				lastRot.makeRotate(fmod(numTraversed*90,360.0f), ofVec3f(0,1,0));
			}
            curRot.slerp(percentTraversed,lastRot,nextRot);
            
            //cout << "current rot " << numTraversed << "last rot is " << lastRot.getEuler() << " Next rot is " << nextRot.getEuler() << " cur rot is " << curRot.getEuler() << endl;
            
			ofVec3f curTarget = trailHead*meshExpansion + (curRot * ofVec3f(0,0,1) ) * traversCameraDistance;
			ofVec3f newPos = curPosition + (curTarget - curPosition) * .06;
			axisCamera.setPosition(newPos);
			axisCamera.lookAt(trailHead*meshExpansion,ofVec3f(0,1,0));
		}
		else{
			easyCamera.setDistance(traversCameraDistance);
			ofVec3f curPosition = easyCamera.getTarget().getPosition();
			ofVec3f curTarget = (trailHead*meshExpansion);
			ofVec3f newPos = curPosition + (curTarget - curPosition) * .1;
			easyCamera.setTarget(newPos);
		}
		
		getCameraRef().setFov(traverseCamFOV);
	}

    if(useQuestionCam){
        //spinsies
        if(selectedQuestion == NULL){
            float curAtten;
            if(caughtQuestion == NULL){
                curAtten = 1.0;
            }
            else{
                curAtten = powf(ofMap(caughtQuestion->hoverPercentComplete, 0.0, .2, 1.0, 0.0, true), 2.0);
            }
            questionSpinAttenuate += (curAtten - questionSpinAttenuate) * .1;
            //questionSpinAttenuate = curAtten;
			curQuestionCamRotation += questionCameraSpinSpeed * questionSpinAttenuate;
			
            //questionCam.rotate(questionCameraSpinSpeed * questionSpinAttenuate, ofVec3f(0,1,0));
			questionCam.setPosition( ofVec3f(0,0,questionCameraAxisDist).getRotated(curQuestionCamRotation, ofVec3f(0,1,0)) );
			questionCam.lookAt(ofVec3f(0,0,0));
        }
        else{
            float percentZoomed = powf(ofMap(ofGetElapsedTimef(), selectedQuestionTime, selectedQuestionTime + 2.0, 0.0, 1.0, true),2.);
            questionCam.setPosition(selectQuestionStartPos.interpolate(selectedQuestion->hoverPosition, percentZoomed));
        }
    }
    else if(useTopicCam){
        
        currentCameraSideDir += (targetCameraSideDir - currentCameraSideDir) * .03;
        currentCameraUpDir   += (targetCameraUpDir - currentCameraUpDir) * .03;
        float distFromTarget = topicNavCam.getPosition().distance(targetCameraPosition);
        ofVec3f targetPos = targetCameraPosition.rotated(ofMap(GetCloudsInputX(), 0, getCanvasWidth(), 45, -45,true),
                                                         targetTopicPosition, currentCameraUpDir);
        targetPos = targetPos.rotated(ofMap(GetCloudsInputY(), 0, getCanvasHeight(), 45, -45,true), targetTopicPosition, currentCameraSideDir);
        
        ofNode n = topicNavCam;
        n.lookAt(targetTopicPosition.getInterpolated(ofVec3f(0,0,0), ofMap(distFromTarget, traversCameraDistance*2, traversCameraDistance*5, .0, 1.0, true) ), currentCameraUpDir );
        topicNavCam.setPosition( topicNavCam.getPosition() + (targetPos - topicNavCam.getPosition())*.03 );
        
        //NAN CHECK
        if(topicNavCam.getOrientationQuat().x() != topicNavCam.getOrientationQuat().x()){
            topicNavCam.setOrientation(n.getOrientationQuat()   );
        }
        else{
            ofQuaternion q;
            q.slerp(.07, topicNavCam.getOrientationQuat(), n.getOrientationQuat());
            topicNavCam.setOrientation(q);
        }
        
        if(distFromTarget < traversCameraDistance * .75){
            setCurrentTopic(movingToTopic);
        }
    }
    
	/////UPDATE COLOR
	if(matchLineColor){
		lineEdgeColorHSV = lineNodeColorHSV;
		matchLineColor = false;
	}
	lineNodeColorRGB = ofFloatColor::fromHsb(lineNodeColorHSV.r,
											 lineNodeColorHSV.g,
											 lineNodeColorHSV.b);
	lineNodeColorRGB.a = lineNodeColorHSV.a;
	lineEdgeColorRGB = ofFloatColor::fromHsb(lineEdgeColorHSV.r,
											 lineEdgeColorHSV.g,
											 lineEdgeColorHSV.b);
	lineEdgeColorRGB.a = lineEdgeColorHSV.a;

	if(matchTraversalColor){
		traverseTailColorHSV = traverseHeadColorHSV;
		matchLineColor = false;
	}
	traverseHeadColorRGB = ofFloatColor::fromHsb(traverseHeadColorHSV.r,
												 traverseHeadColorHSV.g,
												 traverseHeadColorHSV.b);
	traverseHeadColorRGB.a = traverseHeadColorHSV.a;
	traverseTailColorRGB = ofFloatColor::fromHsb(traverseTailColorHSV.r,
												 traverseTailColorHSV.g,
												 traverseTailColorHSV.b);
	traverseTailColorRGB.a = traverseTailColorHSV.a;

	optionColorRGB = ofFloatColor::fromHsb(optionColorHSV.r,
											optionColorHSV.g,
											optionColorHSV.b);
	optionColorRGB.a = optionColorHSV.a;
	
	
	//UPDATE FLICKER
	bool updateTarget = ofGetFrameNum() % 10 == 0;
	float dice = ofRandomuf();
	for(int i = 0; i < nodes.size(); i++){
		int flickerIndex = nodes[i].flickerTextureCoord.y*flickerNoise.getWidth() + nodes[i].flickerTextureCoord.x;
		float* fT = flickerNoiseTarget.getPixels() + flickerIndex;
		float* fD = flickerNoise.getPixels() + flickerIndex;
		if(*fT > .9){
			if(*fD > .7){
				*fT = 0.;
			}
		}
		else if(updateTarget){
			//set to one if we roll the dice
			*fT = abs(nodes[i].luckyNumber - dice) < lineFlickerFrequency*.0001 ? 1.0 : 0.0;
		}
		*fD += (*fT - *fD) * powf(lineFlickerDampening,2.);
	}
	
	flickerNoise.update();
	
	///UPDATE TYPE
	if( (topicFont.size() == 0 || currentTypeSizeRange != typeSizeRange) && typeSizeRange.span() > 0){
		topicFont.clear();
		topicFont.resize(typeSizeRange.span());
		int fontIndex = 0;
		for(int i = typeSizeRange.min; i < typeSizeRange.max; i++){
//			topicFont[fontIndex++].loadFont( GetCloudsDataPath() + "font/Blender-BOOK.ttf", i);
			topicFont[fontIndex++].loadFont( GetFontBuffer(), i);
		}
		currentTypeSizeRange = typeSizeRange;
	}
	
	if(drawType){
        
		for(int i = 0; i < topicPoints.size(); i++){
            
            TopicPoint& p = topicPoints[i];
            ofRectangle screenRect(0,0,getCanvasWidth(),getCanvasHeight());
            ofVec3f expandedPos = p.position * meshExpansion;
            //project the type's point onto the camera's look direction plane
            //http://stackoverflow.com/questions/9605556/how-to-project-a-3d-point-to-a-3d-plane
            ofVec3f v = expandedPos - getCameraRef().getPosition();
            float dist = v.dot(getCameraRef().getLookAtDir());
            ofVec3f lookAtPoint = expandedPos - dist*getCameraRef().getLookAtDir();
            
            //make the type node look at that projected point to billboard it
            ofNode n;
            n.setPosition(expandedPos);
            n.lookAt(lookAtPoint, getCameraRef().getUpDir());
            p.billboardMatrix = n.getGlobalTransformMatrix();
            
            p.fontIndex = ofMap(p.numClips, clipsShowTopic.min, clipsShowTopic.max, 0, topicFont.size()-1,true);
            
            float wordWidth  = topicFont[p.fontIndex].stringWidth(ofToUpper(p.keyword)) * type3DScale;
            float wordHeight = topicFont[p.fontIndex].stringHeight(ofToUpper(p.keyword)) * type3DScale;
            ofVec3f screenLeft  = getCameraRef().worldToScreen(expandedPos, screenRect);
            ofVec3f screenRight = getCameraRef().worldToScreen(expandedPos - n.getSideDir() * wordWidth, screenRect);
            ofVec3f screenTop   = getCameraRef().worldToScreen(expandedPos + n.getUpDir() * wordHeight, screenRect);
            
            float zDistFromCamera = getCameraRef().getGlobalTransformMatrix().getInverse().preMult( expandedPos ).z;
            p.onScreen = zDistFromCamera < -typeClipDistance && zDistFromCamera > -maxTypeAttenuateDistance;
            if(p.onScreen){
                p.attenuation  = ofMap(zDistFromCamera, -maxTypeAttenuateDistance, -minTypeAttenuateDistance, .0, .7, true);
                p.attenuation *= ofMap(zDistFromCamera, -typeClipDistance*2, -typeClipDistance, .7, 0.0, true);
                
                if(p.hovered){
                    p.attenuation = ofMap(ofGetElapsedTimef(), p.hoverChangeTime, p.hoverChangeTime + .25, p.attenuation, 1.0, true);
                }
                else{
                    p.attenuation = ofMap(ofGetElapsedTimef(), p.hoverChangeTime, p.hoverChangeTime + .25, 1.0, p.attenuation, true);
                }

                float padding = 15;
                p.screenRectangle.x = screenTop.x - padding;
                p.screenRectangle.y = screenTop.y - padding;
                p.screenRectangle.width = screenRight.x - screenLeft.x + padding*2;
                p.screenRectangle.height = screenLeft.y - screenTop.y + padding*2;
            }
		}
        
	}
    
    if(drawAssociation){
        trailheadScreenPos = getCameraRef().worldToScreen(trailHead * meshExpansion,
                                                          ofRectangle(0,0,getCanvasWidth(),getCanvasHeight()));        
    }
	
    updateQuestions();
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemClusterMap::selfDraw(){

	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	if(bSmoothLines){
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
	}
	else{
		glDisable(GL_LINE_SMOOTH);
	}
	ofDisableDepthTest();

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
		networkShader.setUniform3f("attractor", trailHead.x, trailHead.y, trailHead.z);
		networkShader.setUniform1f("radius", 300.);
		
		networkShader.setUniform4f("lineColorNode",
								   lineNodeColorRGB.r,
								   lineNodeColorRGB.g,
								   lineNodeColorRGB.b,
								   lineNodeColorRGB.a);
		networkShader.setUniform4f("lineColorEdge",
								   lineEdgeColorRGB.r,
								   lineEdgeColorRGB.g,
								   lineEdgeColorRGB.b,
								   lineEdgeColorRGB.a);
		networkShader.setUniformTexture("flickerTex", flickerNoise, 1);
		networkShader.setUniform1f("flickerIntensity", lineFlickerIntensity);
		ofSetLineWidth(networkLineWidth);
		networkShader.setUniform1f("colorMixExponent", lineColorMixExponent);
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
	else if(drawTraversal && traversalPath.size() > 0){
		traversalShader.begin();

		traversalShader.setUniform1f("percentTraverseRevealed", percentTraversed);
		traversalShader.setUniform4f("headColor",
									 traverseHeadColorRGB.r,
									 traverseHeadColorRGB.g,
									 traverseHeadColorRGB.b,
									 traverseHeadColorRGB.a);
		traversalShader.setUniform4f("tailColor",
									 traverseTailColorRGB.r,
									 traverseTailColorRGB.g,
									 traverseTailColorRGB.b,
									 traverseTailColorRGB.a);

        int numVertsInSegment = traversalPath.back().endIndex - traversalPath.back().startIndex;
		//cout << "NUM VERTS IN SEGMENT " << numVertsInSegment << endl;
		traversalShader.setUniform1f("colorFalloff", traverseFalloff);
		traversalShader.setUniform1f("trailVertCount", traversalMesh.getNumVertices());
		traversalShader.setUniform1f("segmentVertCount", numVertsInSegment);
        traversalShader.setUniform1f("trailRevealCount", traverseRevealVerts);
        
		ofSetLineWidth(traverseLineWidth);
		traversalMesh.draw();
		traversalShader.end();
	}
	
	if(drawHomingDistanceDebug){
		ofPushStyle();
		ofNoFill();
		
		ofSetColor(ofColor::royalBlue);
		ofDrawSphere(currentTraversalPosition, traverseMinSolvedDistance*.001);
		
		ofSetColor(ofColor::yellowGreen);
		ofDrawSphere(currentTraversalPosition, traverseHomingMinDistance*.001);
		
		ofPopStyle();
	}
	///END TRAVERSAL
	
	/////OPTIONS
	if(drawOptionPaths){
		optionsShader.begin();

		optionsShader.setUniform4f("lineColor",
								   optionColorRGB.r,
								   optionColorRGB.g,
								   optionColorRGB.b,
								   optionColorRGB.a);
		
		optionsShader.setUniform1f("percentOptionsRevealed",
                                   percentTraversed);
		optionsShader.setUniform1i("fadeIn", 0);
		optionsMeshPrev.draw();
		
		optionsShader.setUniform1f("percentOptionsRevealed",
                                   percentOptionsRevealed);
		optionsShader.setUniform1i("fadeIn", 1);
		ofSetLineWidth(optionLineWidth);
		optionsMeshNext.draw();

		optionsShader.end();
	}
	/////END OPTIONS
	

    
	ofPopMatrix();
	ofPopStyle();
	glPopAttrib();
    
    
    drawQuestions();

	if(drawType3D){
        
        ofDisableLighting();
        ofDisableDepthTest();
		for(int i = 0; i < topicPoints.size(); i++){
            
			TopicPoint& p = topicPoints[i];
            if(!p.onScreen){
                continue;
            }
            float attenuate = 1.0;
            //HUD will take care of topic drawing
            if(p.keyword == currentTopic){
                //continue;
                attenuate = ofMap(ofGetElapsedTimef(), selectedTopicChangedTime, selectedTopicChangedTime + .5, 1.0, .2, true);
            }
            ofPushStyle();
            ofSetColor(255, p.attenuation * attenuate * 255);
            
            ofPushMatrix();
            ofMultMatrix(p.billboardMatrix);
            ofScale(-type3DScale,-type3DScale, type3DScale);
            
            ofxFTGLFont& font = topicFont[ ofClamp(p.fontIndex, 0, topicFont.size()-1) ];
            font.drawString( ofToUpper(p.keyword), 0, 0);
            
            ofPopMatrix();
            ofPopStyle();

		}
        ofEnableDepthTest();
		ofEnableLighting();
        
    }

}


void CloudsVisualSystemClusterMap::drawCursors(){
	if(!hideCursors){
		CloudsVisualSystem::drawCursors();
	}
}

void CloudsVisualSystemClusterMap::updateQuestions(){

    if(!displayQuestions){
        return;
    }
    
	for(int i = 0; i < questions.size(); i++){
		CloudsPortal& curQuestion = questions[i];
        curQuestion.cam = &getCameraRef();
		curQuestion.scale = powf(questionScale,2.0);
		curQuestion.update();

        if(selectedQuestion == NULL){
            curQuestion.hoverPosition = getCameraRef().getPosition() + ofVec3f(questionCameraDistance,0,0).getRotated(1.0*i/questions.size() * 360, ofVec3f(0,1,0));
        }

#ifdef OCULUS_RIFT
        ofVec3f screenPos = getOculusRift().worldToScreen(curQuestion.hoverPosition, true);
        ofRectangle viewport = getOculusRift().getOculusViewport();
        float distanceToQuestion = ofDist(screenPos.x, screenPos.y,viewport.getCenter().x, viewport.getCenter().y);
#else
        ofVec2f mouseNode = cursor;
        float distanceToQuestion = curQuestion.screenPosition.distance(mouseNode);
#endif
//        cout << "distance to question is " << distanceToQuestion << endl;
        if(selectedQuestion == NULL && caughtQuestion == NULL){
            if( distanceToQuestion < questionTugDistance.max  && curQuestion.screenPosition.z > 0 && curQuestion.screenPosition.z < 1 ){
                if(distanceToQuestion < questionTugDistance.min){
                    caughtQuestion = &curQuestion;
                    if (caughtQuestion->startHovering()) {
                        getClick()->setPosition(0);
                        getClick()->play();
                    }
                }
            }
        }
        
        //we have a caught question make sure it's still close
        else if(caughtQuestion == &curQuestion){
            
            if( caughtQuestion->isSelected() && selectedQuestion == NULL){
                getSelectLow()->setPosition(0);
                getSelectLow()->play();
                
                selectedQuestion = caughtQuestion;
                selectedQuestionTime = ofGetElapsedTimef();
                selectQuestionStartPos = getCameraRef().getPosition();
                selectQuestionStartRot = getCameraRef().getOrientationQuat();

            }
            else if(distanceToQuestion > questionTugDistance.max && selectedQuestion == NULL){
                caughtQuestion->stopHovering();
                caughtQuestion = NULL;
            }
        }
    }

    
    if (caughtQuestion != NULL) {
        // move the sticky cursor towards the caught question
        stickyCursor.interpolate(caughtQuestion->screenPosition - ofVec2f(bleed,bleed)*.5, 0.2f);
    }
    else {
        stickyCursor.interpolate(cursor, 0.5f);
    }

}

void CloudsVisualSystemClusterMap::drawQuestions(){
    if(questions.size() == 0 || !displayQuestions){
        return;
    }
    
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
	ofDisableDepthTest();

	CloudsPortal::shader.begin();
	
	CloudsPortal::shader.setUniform1i("doAttenuate", 0);
	
    ofSetColor(255);
    ofNoFill();
	for(int i = 0; i < questions.size(); i++){
		questions[i].draw();
	}
	
	CloudsPortal::shader.end();
    
    ofEnableDepthTest();
	ofPopStyle();
	
	ofPushStyle();
	ofDisableLighting();
    
	if(!questionFont.isLoaded() || currentQuestionFontSize != questionFontSize){
		//questionFont.loadFont(GetCloudsDataPath() + "font/Blender-BOOK.ttf", questionFontSize);
		questionFont.loadFont(GetFontBuffer(), questionFontSize);
		currentQuestionFontSize = questionFontSize;
	}
	string questionText;
	ofVec3f basePosition(0,0,0);
	float questionTextOpacity = 0.0;
	float scaleModifier = 1.0;// * ofGetMouseX() / ofGetWidth();

	if(caughtQuestion != NULL){
		basePosition = caughtQuestion->hoverPosition;
		questionText = caughtQuestion->question;
		questionTextOpacity = ofMap(caughtQuestion->hoverPercentComplete, 0.0, .05, 0.0, 1.0, true);
		
		scaleModifier = .5;
		questionFont.setLetterSpacing(questionFontTracking*.1);
		
		questionText = GetTranslationForString( ofToUpper(questionText) );
		
		float questionTextWidth = questionFont.stringWidth(questionText);
		float questionTextWidth2, questionTextHeight2;
		string secondLine;
		bool twoLines = questionTextWidth > 500;
        if(questionText.find("\n") != string::npos){
            twoLines = true;
            vector<string> split = ofSplitString(questionText, "\n", true,true);
            questionText = split[0];
            secondLine = split[1];
            questionTextWidth  = questionFont.stringWidth(questionText);
            questionTextWidth2 = questionFont.stringWidth(secondLine);
        }
        else if(twoLines){
			vector<string> pieces = ofSplitString(questionText, " ", true,true);
			vector<string> firstHalf;
			vector<string> secondHalf;
			int halfsize = pieces.size() / 2;
			firstHalf.insert(firstHalf.begin(), pieces.begin(), pieces.begin() + halfsize);
			secondHalf.insert(secondHalf.begin(), pieces.begin() + halfsize, pieces.end());
			questionText = ofJoinString(firstHalf, " ");
			secondLine = ofJoinString(secondHalf, " ");
			questionTextWidth  = questionFont.stringWidth(questionText);
			questionTextWidth2 = questionFont.stringWidth(secondLine);
		}
 		float questionTextHeight = questionFont.stringHeight(questionText);
		
        ofPushMatrix();
		ofPushStyle();
		ofEnableAlphaBlending();
		ofDisableLighting();
		
#ifdef OCULUS_RIFT
		getOculusRift().multBillboardMatrix( basePosition );
#else
		ofNode n;
		n.setPosition( basePosition );
		n.lookAt(getCameraRef().getPosition(), getCameraRef().getUpDir());
		ofVec3f axis; float angle;
		n.getOrientationQuat().getRotate(angle, axis);
		// Translate the object to its position.
		ofTranslate( basePosition );
		// Perform the rotation.
		ofRotate(angle, axis.x, axis.y, axis.z);
#endif
		
		ofRotate(180, 0, 0, 1); //flip around
		ofScale(questionFontScale,questionFontScale,questionFontScale);
		ofSetColor(255,255*questionTextOpacity);
		
		questionFont.drawString(questionText, -questionTextWidth*.5, questionFontY - questionTextHeight*.5);
		if(twoLines){
			questionFont.drawString(secondLine, -questionTextWidth2*.5, questionFontY + questionTextHeight*1.5);
		}
		
		ofEnableLighting();
		ofPopStyle();
		ofPopMatrix();
	}
	
	ofEnableLighting();
	glEnable(GL_DEPTH_TEST);
	ofPopStyle();

}

void CloudsVisualSystemClusterMap::setQuestions(vector<CloudsClip*> questionClips){

	for(int i = 0; i < questionClips.size(); i++){
		
		CloudsPortal q;
        q.cam = &getCameraRef();
		q.bLookAtCamera = true;
		q.clip = questionClips[i];
        
        if(q.clip->getAllTopicsWithQuestion().size() > 0){
            q.topic = q.clip->getAllTopicsWithQuestion()[0];
            q.question = q.clip->getQuestionForTopic(q.topic);
            q.hoverPosition = getCameraRef().getPosition() + ofVec3f(questionCameraDistance,0,0).getRotated(1.0*i/questions.size() * 360, ofVec3f(0,1,0));
            
            q.setup();
            questions.push_back(q);
        }
        else{
            ofLogError("CloudsVisualSystemClusterMap::setQuestions") << "Question not found for clip " << q.clip->getLinkName();
        }
	}

}

void CloudsVisualSystemClusterMap::populateDummyQuestions(){
    
    int numQuestions = 4;
	for(int i = 0; i < numQuestions; i++){
		
		CloudsPortal q;
		q.cam = &getCameraRef();
		q.bLookAtCamera = true;
        q.hoverPosition = getCameraRef().getPosition() + ofVec3f(questionCameraDistance,0,0).getRotated(1.0*i/numQuestions * 360, ofVec3f(0,1,0));
        q.topic = "fakeTopic";
        q.question = "FAKE QUESTION " + ofToString(i);
		q.setup();
		
		questions.push_back(q);
	}
    
}

// draw any debug stuff here
void CloudsVisualSystemClusterMap::selfDrawDebug(){
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemClusterMap::selfDrawBackground(){

}

void CloudsVisualSystemClusterMap::selfDrawOverlay(){
	//turn the background refresh off
	if(drawType && !drawType3D){
		
        ofDisableLighting();
		ofRectangle screenRect(0,0, getCanvasWidth(), getCanvasHeight());
		for(int i = 0; i < topicPoints.size(); i++) {

			TopicPoint& p = topicPoints[i];

            ofPushMatrix();
            ofTranslate(p.screenRectangle.x,p.screenRectangle.y);
            ofxFTGLFont& font = topicFont[ ofClamp(p.fontIndex, 0, topicFont.size()-1) ];
            font.drawString( ofToUpper(p.keyword), 0, 0);
            
            ofPopMatrix();
		}
		ofEnableLighting();
	}
	
    if(drawAssociation && !drawType3D){
        if(!associationFont.isLoaded() || associationFontSize != currentAssociationFont){
            //associationFont.loadFont( GetCloudsDataPath() + "font/Blender-BOOK.ttf", associationFontSize);
			associationFont.loadFont( GetFontBuffer(), associationFontSize);
            currentAssociationFont = associationFontSize;
        }
        
        float associationTypeOn = ofMap(percentOptionsRevealed*percentTraversed, 0, .2,0,1.0,true);
        if( parser->hasMasterTopicAssociation(currentTopic)){
        //if(associations.find(currentTopic) != associations.end()){
            string associatedTopic = parser->getMasterKeyword(currentTopic);
            int numChars = associatedTopic.size() * associationTypeOn;
            if(numChars%2 == 1) numChars = MIN(numChars+1,associatedTopic.size());
            string partialString = associatedTopic.substr(0, numChars);
            associationFont.drawString(partialString, trailheadScreenPos.x+5,trailheadScreenPos.y-5);
//            associationFont.drawString(currentTopic, trailheadScreenPos.x, trailheadScreenPos.y);
        }
        
    }
    
//    ofPushStyle();
//    ofSetColor(255, 255);
//    ofCircle(trailheadScreenPos, 100);
//    ofPopStyle();
    
//    cout << trailheadScreenPos << endl;
    
//	if(drawLineFlickerDebug){
//		flickerNoise.draw(0,0,getCanvasWidth(),getCanvasHeight());
//	}

//    ofPushStyle();
//    ofNoFill();
//    for(int i = 0; i < topicPoints.size(); i++){
//
//        TopicPoint& p = topicPoints[i];
//        if(p.onScreen){
//            if(p.hovered){
//                ofSetColor(ofColor::red);
//            }
//            else{
//                ofSetColor(ofColor::white);
//            }
//            ofRect(p.screenRectangle);
//        }
//    }
//    
//    ofPopStyle();
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
//		gameCamera.reset();
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
//    if(key == ']'){
//        selectedQuestion = &questions.at((int)ofRandom(questions.size()-1));
//    }
	
}


void CloudsVisualSystemClusterMap::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemClusterMap::selfMouseDragged(ofMouseEventArgs& data){
    cursor.set(GetCloudsInput()->getPosition());
	
}

void CloudsVisualSystemClusterMap::selfMouseMoved(ofMouseEventArgs& data){

    cursor.set(GetCloudsInput()->getPosition());

    
    TopicPoint* hoveredPoint = NULL;
    ofVec3f camPos = getCameraRef().getPosition();
    ofVec2f mousePos(data.x + bleed, data.y + bleed);
    if(drawType3D){
        for(int i = 0; i < topicPoints.size(); i++){
            TopicPoint& p = topicPoints[i];
            if(!p.onScreen) continue;
            if(data.canceled) continue;
            bool inside = p.screenRectangle.inside(mousePos);
            //if this one is closer
            if(inside && (hoveredPoint == NULL || (hoveredPoint->position*meshExpansion).distance(camPos) > (p.position*meshExpansion).distance(camPos))){
                hoveredPoint = &p;
            }
        }
    
        //hover
        if(hoveredPoint != NULL && !hoveredPoint->hovered){
            hoveredPoint->hovered = true;
            hoveredPoint->hoverChangeTime = ofGetElapsedTimef();
        }
        
        //un hover
        for(int i = 0; i < topicPoints.size(); i++){
            TopicPoint& p = topicPoints[i];
            
            if(!p.onScreen) continue;
            
            bool inside = p.screenRectangle.inside(mousePos);
            if(p.hovered &&
               (data.canceled ||
                !inside ||
                (hoveredPoint != NULL && (hoveredPoint->position*meshExpansion).distance(camPos) < (p.position*meshExpansion).distance(camPos))) )
            {
                p.hovered = false;
                p.hoverChangeTime = ofGetElapsedTimef();
            }
        }

    }
}


void CloudsVisualSystemClusterMap::selfMousePressed(ofMouseEventArgs& data){
    if(drawType3D && !data.canceled){
        for(int i = 0; i < topicPoints.size(); i++){
            TopicPoint& p = topicPoints[i];
            if(p.hovered){
                moveToTopic(p.keyword);
                //selectedTopicChanged = true;
                //setCurrentTopic(p.keyword);
                break;
            }
        }
    }
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
