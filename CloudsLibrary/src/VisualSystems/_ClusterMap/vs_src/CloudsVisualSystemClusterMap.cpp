//
//  CloudsVisualSystemClusterMap.cpp
//

#include "CloudsVisualSystemClusterMap.h"
#include "CloudsGlobal.h"
#include "CloudsFCPParser.h"

CloudsVisualSystemClusterMap::CloudsVisualSystemClusterMap(){
	run = NULL;
	matchLineColor = false;
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
	
	clipsShowTopic = ofIntRange(20, 40);
	drawType = true;
	baseFontSize = 12;
	currentTypeSizeRange.min = currentTypeSizeRange.max = -1;
	typeSizeRange.min = 5;
	typeSizeRange.max = 14;
	lineDensity = 200;
	
//	currentTraversalIndex = -1;
	
	lineFlickerIntensity = 7.;
	lineFlickerFrequency = 100;
	lineFlickerDampening = .05;
    
    bConstrainTraversal = false;
    maxTraverseDistance = 1000;
    maxTraverseAngle = 180;
    
    minTraverseNextOptions = 3;
    minTraverseDistance = 5;
    

}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemClusterMap::selfSetupGui(){
	
	followCamGui = new ofxUISuperCanvas("FOLLOW CAM", gui);
	followCamGui->copyCanvasStyle(gui);
	followCamGui->copyCanvasProperties(gui);
	followCamGui->setName("FollowCam");
	followCamGui->addSlider("CAMERA DISTANCE", 10, 400, &traversCameraDistance);
	followCamGui->addToggle("LOCK AXIS", &lockCameraAxis);
	followCamGui->addSlider("FOV", 4, 90, &traverseCamFOV);
	
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
	//	typeGui->addRangeSlider("FADE RANGE", 0, 300, &typeDistanceRange.min, &typeDistanceRange.max);
	typeGui->addIntSlider("MIN CLIP NUM", 1, 60, &clipsShowTopic.min);
	typeGui->addIntSlider("MAX CLIP SCALE", 1, 60, &clipsShowTopic.max);
	
	ofAddListener(typeGui->newGUIEvent, this, &CloudsVisualSystemClusterMap::selfGuiEvent);
	guis.push_back(typeGui);
	guimap[typeGui->getName()] = typeGui;
	
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

void CloudsVisualSystemClusterMap::resetGeometry(){
	
	firstClip = true;
//	lastTraverseStartedIndex = -1;

	networkMesh.clear();
	traversalMesh.clear();
	optionsMeshNext.clear();
	optionsMeshPrev.clear();
	nodeMesh.clear();
	nodes.clear();
	traversalPath.clear();
	clipIdToNodeIndex.clear();
	
	currentTraversalIndex = 0;
	
	networkCentroid = ofVec3f(0,0,0);
	ofVec3f maxBounds(0,0,0);
	ofVec3f minBounds(0,0,0);
	
	ofVec2f flickerCoord = ofVec2f(0,0);
	int flickerWidth = 100;
	for(int i = 0; i < parser->getAllClips().size(); i++){
		CloudsClusterNode n;
		CloudsClip& clip = parser->getAllClips()[i];
		n.clipId = clip.getID();
		n.mesh = &nodeMesh;
		n.vertexIndex = nodeMesh.getNumVertices();
		n.networkPosition = clip.networkPosition;
		clipIdToNodeIndex[clip.getID()] = nodes.size();
		nodeMesh.addVertex( n.networkPosition );
		n.flickerTextureCoord = flickerCoord;
		n.luckyNumber = ofRandomuf();
		
		flickerCoord.x++;
		if(flickerCoord.x >= flickerWidth){
			flickerCoord.x = 0;
			flickerCoord.y++;
		}
		networkCentroid += clip.networkPosition;
		maxBounds = ofVec3f(MAX(maxBounds.x,clip.networkPosition.x),
							MAX(maxBounds.y,clip.networkPosition.y),
							MAX(maxBounds.z,clip.networkPosition.z));
		
		minBounds = ofVec3f(MIN(minBounds.x,clip.networkPosition.x),
							MIN(minBounds.y,clip.networkPosition.y),
							MIN(minBounds.z,clip.networkPosition.z));
		
		nodes.push_back(n);
	}

	
	flickerNoise.allocate(flickerWidth, flickerCoord.y+1, OF_IMAGE_GRAYSCALE);
	flickerNoiseTarget.allocate(flickerWidth, flickerCoord.y+1, OF_IMAGE_GRAYSCALE);
	flickerNoise.getPixelsRef().set(0);
	flickerNoiseTarget.set(0);
	
	networkCentroid /= nodes.size();
	float maxDistance = 0;
	for(int i = 0; i < parser->getAllClips().size(); i++){
		maxDistance = MAX(maxDistance, parser->getAllClips()[i].networkPosition.distance(networkCentroid));
	}
	
	//add all connections to connection mesh
	set< pair<string,string> > connections;
	
	for(int i = 0; i < parser->getAllClips().size(); i++){
		CloudsClip& clip = parser->getAllClips()[i];
		vector<CloudsClip> meta = parser->getClipsWithKeyword(clip.getKeywords());
		vector<CloudsLink> links = parser->getLinksForClip(clip);
		string nameA = clip.getID();
		CloudsClusterNode& n1 = nodes[ clipIdToNodeIndex[nameA] ];
		
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
				
				if(clip.networkPosition.distance(networkCentroid) > maxDistance * .6) {
//					cout << "filtering outer ring clip " << meta[j].getLinkName() << endl;
					continue;
				}
				
				if(meta[j].networkPosition.distance(networkCentroid) > maxDistance * .6 ) {
//					cout << "filtering outer ring clip " << meta[j].getLinkName() << endl;
					continue;
				}
				
				NetworkEdge connectionEdge;
				//create curved connection mesh
				//naive simple spherical interpolation over 10 steps
				connectionEdge.startIndex = networkMesh.getNumVertices();
				ofVec3f vecToStart = clip.networkPosition - networkCentroid;
				ofVec3f vecToDest  = meta[j].networkPosition - networkCentroid;
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
				networkMesh.addVertex(clip.networkPosition);

				float distance = clip.networkPosition.distance(meta[j].networkPosition);
				
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
				networkMesh.addVertex(meta[j].networkPosition);
				
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
}

void CloudsVisualSystemClusterMap::populateTopicPoints(){
	
	topicPoints.clear();
	
	vector<string>& keywords = parser->getContentKeywords();
	int numClips = parser->getNumberOfClipsWithKeyword(keywords[0]);
	clipCountRange = ofRange(numClips,numClips);
	for(int i = 0; i < keywords.size(); i++){
		TopicPoint tp;
		tp.keyword  = keywords[i];
		tp.position = parser->getKeywordCentroid(keywords[i]);
		tp.numClips = parser->getNumberOfClipsWithKeyword(keywords[i]);
		clipCountRange.growToInclude(tp.numClips);
//		cout << "num clips " << tp.numClips << " current range " << clipCountRange << endl;
		topicPoints.push_back(tp);
	}
//	cout << "clip size range is " << clipCountRange << endl;
	for(int i = 0; i < topicPoints.size(); i++){
		topicPoints[i].normalizedTopicScale =  clipCountRange.getNormalized(topicPoints[i].numClips);
//		cout << "Normal for num clips " << topicPoints[i].numClips << " " << topicPoints[i].normalizedTopicScale << endl;
	}
}

void CloudsVisualSystemClusterMap::traverse(){
	
	//cout << " CloudsVisualSystemClusterMap::traverse TRAVERSING 1" << endl;
	
	if(run == NULL){
		ofLogError("CloudsVisualSystemClusterMap::traverse") << "Traversed without RUN" << endl;
		return;
	}
	
	if(currentTraversalIndex < run->clipHistory.size()){
		CloudsClip& clip = run->clipHistory[currentTraversalIndex];
		traverseToClip( clip );
		currentTraversalIndex++;
	}
	else if(autoTraversePoints){
		timeline->stop(); //finished!
	}
		
}

void CloudsVisualSystemClusterMap::traverseToClip(CloudsClip clip){
	
	if(clipIdToNodeIndex.find(clip.getID()) == clipIdToNodeIndex.end()){
		ofLogError("CloudsVisualSystemClusterMap::traverseToClip") << "Isn't included in cluster map";
		return;
	}
	

	ofIndexType newNodeIndex = clipIdToNodeIndex[ clip.getID() ];
	CloudsClusterNode n = nodes[ newNodeIndex ];

	ofVec3f startDirection;
	if(firstClip){
//		currentTraversalDirection = randomDirection();
        currentTraversalDirection = clip.networkPosition - networkCentroid;
		currentTraversalPosition = clip.networkPosition;
	}
	else{

		//this is to correct the scales in the sliders to more friendly numbers...
		float localMinSolve = traverseMinSolvedDistance * .001;
		float localStepSize = traverseStepSize * .001;
		float localMinHomingDist = traverseHomingMinDistance * .001;
		TraversalSegment newSegment;
		newSegment.startIndex = traversalMesh.getNumVertices();
		int maxSteps = 1000;
		float currentDistance = currentTraversalPosition.distance(clip.networkPosition);
        
        //check traversal parameters to potentiall fake a new clip
        if(bConstrainTraversal){
        
            int constrainedPositionIndex = -1;
            float traversalAngle = (clip.networkPosition - currentTraversalPosition).angle(currentTraversalDirection);
            int numOptions = n.connectionCurves.size();
            if(currentDistance > maxTraverseDistance*.001 ||
               currentDistance < minTraverseDistance ||
               traversalAngle > maxTraverseAngle ||
               numOptions < minTraverseNextOptions)
            {
                cout << "CLIP FAILED TEST DISTANCE " << currentDistance*100 << " and angle " << traversalAngle << endl;
                
                //find a new options
                vector<float> distsSq;
                vector<NNIndex> indices;
                //search for 100 nearby clips and take the first acceptable one
                kdtree.findNClosestPoints(clip.networkPosition, 1000, indices, distsSq);
                float localMaxTraversedDistance = maxTraverseDistance*.001;
                float localMinTraverseDistance = minTraverseDistance*.001;
                for(int i = 0; i < indices.size(); i++){
                    ofVec3f testPosition = nodeMesh.getVertex( indices[i] );
                    float testDistance = currentTraversalPosition.distance( testPosition );
                    float testAngle = (testPosition - currentTraversalPosition).angle(currentTraversalDirection);
                    int testOptions = nodes[ indices[i] ].connectionCurves.size();
                    
                    cout << "\tTest node stats" << endl
                         << "\t\tDIST: " << testDistance*100 << " - max:" << maxTraverseDistance << " min " << minTraverseDistance << endl

                         << "\t\tANGLE: " << testAngle << "/" << maxTraverseAngle << endl
                         << "\t\tOPTIONS# " << testOptions << "/" << minTraverseNextOptions << endl;
                    
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
                    ofLogError() <<"Couldn't find a better neighbor going with original clip";
                }
            }
            
            cout << "TRAVERSED DISTANCE " <<  currentDistance*100 << " ANGLE " << traversalAngle << endl;
        }
        
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
//	lineColor = timeline->addColors("line color", "LineColor.xml");
//	nodeColor = timeline->addColorsWithPalette("node color","NodeColor.xml", getVisualSystemDataPath() + "images/nerve_palette.png");
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
	
	///UPDATE ANIMATION
	percentTraversed = ofMap(ofGetElapsedTimef(),
							 traverseStartTime, traverseStartTime+traverseAnimationDuration,
							 0, 1.0, true);
	percentOptionsRevealed = ofMap(ofGetElapsedTimef(),
								   traverseStartTime+traverseAnimationDuration,
								   traverseStartTime+traverseAnimationDuration+optionsAnimationDuration,
								   0.0, 1.0, true);
	
	if(autoTraversePoints && (firstClip || percentOptionsRevealed >= 1.0) ){
		traverse();
	}
	
	//UPDATE CAMERA
	gameCamera.applyRotation = gameCamera.applyTranslation = !cursorIsOverGUI();
	if(cursorIsOverGUI()){
		easyCamera.disableMouseInput();
	}
	else{
		easyCamera.enableMouseInput();
	}
	

	if(traversalPath.size() > 0){
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
			ofVec3f curTarget = trailHead*meshExpansion + ofVec3f(0,0,traversCameraDistance);
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
	if(currentTypeSizeRange != typeSizeRange && typeSizeRange.span() > 0){
		topicFont.clear();
		topicFont.resize(typeSizeRange.span());
		int fontIndex = 0;
		for(int i = typeSizeRange.min; i < typeSizeRange.max; i++){
			topicFont[fontIndex++].loadFont( GetCloudsDataPath() + "font/Blender-BOOK.ttf", i);
		}
		currentTypeSizeRange = typeSizeRange;
	}
	
	if(drawType){
		for(int i = 0; i < topicPoints.size(); i++){
			if(topicPoints[i].numClips >= clipsShowTopic.min){
				topicPoints[i].screenPosition = getCameraRef().worldToScreen(topicPoints[i].position*meshExpansion,
																			 ofRectangle(0,0,getCanvasWidth(),getCanvasHeight()));
			}
		}
	}

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
		ofSphere(currentTraversalPosition, traverseMinSolvedDistance*.001);
		
		ofSetColor(ofColor::yellowGreen);
		ofSphere(currentTraversalPosition, traverseHomingMinDistance*.001);
		
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
}

// draw any debug stuff here
void CloudsVisualSystemClusterMap::selfDrawDebug(){
	
}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemClusterMap::selfDrawBackground(){

}

void CloudsVisualSystemClusterMap::selfDrawOverlay(){
	//turn the background refresh off
	if(drawType){
		glDisable(GL_LIGHTING);
		ofRectangle screenRect(0,0,getCanvasWidth(), getCanvasHeight());
		for(int i = 0; i < topicPoints.size(); i++){

			TopicPoint& p = topicPoints[i];
			if(p.numClips >= clipsShowTopic.min){
//				int fontIndex = p.normalizedTopicScale * topicFont.size();
				int fontIndex = ofMap(p.numClips, clipsShowTopic.min, clipsShowTopic.max,
									  0, topicFont.size()-1,true);
//				cout << "Font index is " << fontIndex << " from " << topicFont.size() << endl;
				if(fontIndex > 0 && fontIndex < topicFont.size()){
					ofPushMatrix();
					ofTranslate(p.screenPosition.x,p.screenPosition.y);
					ofxFTGLFont& font = topicFont[ fontIndex ];
					font.drawString( ofToUpper(p.keyword), 0, 0);
					ofPopMatrix();
				}
			}
		}
		glEnable(GL_LIGHTING);
	}
	
	if(drawLineFlickerDebug){
		flickerNoise.draw(0,0,getCanvasWidth(),getCanvasHeight());
	}
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
