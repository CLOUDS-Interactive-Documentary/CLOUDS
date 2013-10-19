//
//  Node.cpp
//  emptyExample
//
//  Created by Jim on 11/16/12.
//
//

#include "Node.h"

bool nodesort(Node* a, Node* b){
	return a->actualDistance > b->actualDistance;
}

Node::Node(){
	terminated = false;
	leaf = true;
	replicated = false;
	generation = 0;
	parent = NULL;
	startIndex = 0;
	endIndex = 0;
	endEnergy = startEnergy = 0;
	targeted = false;
	numConnections = 0;
	nodePointIndex = -1;

}

void Node::replicate(vector<Node*>& allNodes, vector<ofVec3f>& fusePoints, vector<ofVec3f>& fuseDistances){
	leaf = false;
	replicated = true;
	
	vector<Node*> newNodes;
	int numAvailableNodes = 0;
	for(int i = 0; i < allNodes.size(); i++){
		if(allNodes[i]->generation <= 1 &&
		   //!allNodes[i]->terminated &&
		   !allNodes[i]->targeted &&
		   allNodes[i]->sourceId != sourceId)
		{
			numAvailableNodes++;
		}
	}

	for(int i = 0; i < MIN(numReplications, numAvailableNodes); i++){
		Node* n = new Node();
		
		//default attributes
		n->sourceId = sourceId;
		n->minDistance = minDistance;
		n->distanceRange = distanceRange;
		n->numReplications = numReplications;
		n->mesh = mesh;
		n->stepSize = stepSize;
		n->generation = generation+1;
		n->nodeColorTrack = nodeColorTrack;
		n->lineColor = lineColor;
		n->nodeColor = nodeColor;
		
		n->position = position;
		n->direction = ofRandomPointOnUnitSphere();
		
		n->parent = this;
		n->startEnergy = n->endEnergy = endEnergy;
		n->minAttractRadius = minAttractRadius;
		n->minRepelRadius = minRepelRadius;
		n->minFuseRadius = minFuseRadius;
		n->numSurvivingBranches = numSurvivingBranches;
		n->maxAttractForce = maxAttractForce;
		n->maxRepelForce = maxRepelForce;
		n->minFuseRadius = minFuseRadius;
		n->leaf = false;
		n->replicatePointSize = replicatePointSize;
		n->numIterations = numIterations;
		
		n->numPointsAtReplicate = numPointsAtReplicate;
		n->replicatePointDistance = replicatePointDistance;
		
		n->actualDistance = minDistance * ofRandom(distanceRange);
		
		float alpha = .2;
		//blank vertices befor and after
		mesh->addColor(ofFloatColor(0,0,0,0));
		mesh->addVertex(position);

		n->startIndex = mesh->getNumVertices();

		//mesh->addColor(ofFloatColor(alpha));
		//mesh->addColor(nodeColor);
		mesh->addVertex(position);
		
		float minRepelSqr = minRepelRadius*minRepelRadius;
		float minAttractSqr = minAttractRadius*minAttractRadius;
		float minFuseSqr = minFuseRadius*minFuseRadius;

		Node* targetNode = NULL;
		Node* terminationNode = NULL;
		float closestDistance = 10000000;
		for(int i = 0; i < allNodes.size(); i++){
			if( allNodes[i]->generation <= 1 &&
			   //!allNodes[i]->terminated &&
			   !allNodes[i]->targeted &&
			   allNodes[i]->sourceId != n->sourceId)
			{
				
				float distSquared = allNodes[i]->position.distanceSquared(n->position);
				if(distSquared < closestDistance) {
					targetNode = allNodes[i];
					closestDistance = distSquared;
				}
			}
		}
		
		if(targetNode != NULL){
			if(generation != 0){
				n->direction = direction;
			}
			float distanceTraveled = 0;
			while(terminationNode == NULL && distanceTraveled < n->actualDistance){

				targetNode->targeted = true;
				float distToTarget = targetNode->position.distance(n->position);
				if(distToTarget < minFuseRadius){
					terminationNode = targetNode;
				}
				else{
					ofVec3f forceDirection = (targetNode->position - n->position).normalized();
//					float forceMagnitude = maxAttractForce * powf(1.0 - (distToTarget / minAttractSqr), 2);
//					force += forceDirection * forceMagnitude;
					float damp = ofMap(distToTarget, 100, minFuseRadius, .05, 1.0, true);
					n->direction += ( forceDirection - n->direction) * damp;
					/////////
//					n->direction = forceDirection;
					/////////
				}
				
				n->endEnergy++;
				n->position += n->direction*MIN(stepSize, distToTarget);
//				mesh->addColor(nodeColor);
				mesh->addVertex(n->position);
				distanceTraveled += stepSize;
			}
		}
		else {
			
			//cout << "target node is " << sqrt(closestDistance) << " away " << endl;
			float distanceTraveled = 0;
			while(distanceTraveled < n->actualDistance){
			
				ofVec3f force = ofVec3f(0,0,0);				
				for(int ni = 0; ni < allNodes.size(); ni++){
					
					if(allNodes[ni]->terminated){
						continue;
					}
					
					float distSquared = allNodes[i]->position.distanceSquared(n->position);
					if(allNodes[ni]->sourceId == n->sourceId && distSquared < minRepelSqr){
						ofVec3f forceDirection = (n->position - allNodes[ni]->position).normalized();
						float forceMagnitude = maxRepelForce * powf(1.0 - (distSquared / minAttractSqr), 2);
						force += forceDirection * forceMagnitude;
					}
				}
				
				//integrate
				n->direction += force;
				n->direction.normalize();
				n->endEnergy++;
				n->position += n->direction*stepSize;
				mesh->addColor(nodeColor);
				mesh->addVertex(n->position);
				distanceTraveled += stepSize;
			}
		}
		
		//mesh->addColor(nodeColor);
		mesh->addVertex(n->position);
 		n->endIndex = mesh->getNumVertices();
		
		for(int c = n->startIndex; c < n->endIndex; c++){
			mesh->addColor(nodeColorTrack->getColorAtPosition(ofMap(c, n->startIndex, n->endIndex-1, 0, 1.0)));
		}
		
		newNodes.push_back(n);
		allNodes.push_back(n);
		
		mesh->addColor(ofFloatColor(0,0,0,0));
		mesh->addVertex(n->position);
		
		if(terminationNode != NULL){
			n->terminate();
			terminationNode->numConnections++;
			//terminationNode->terminate();
			//break;
		}
	}
	
	//TO DRAW CLUSTERS
	/*
	for(int i = 0; i < numPointsAtReplicate; i++){
		ofVec3f point = position + ofRandomPointOnUnitSphere() * powf(ofRandomuf(),2)*replicatePointDistance;
		fusePoints.push_back(point);
		float spriteSize = ofMap(point.distance(position),0,replicatePointDistance, 1.0, replicatePointSize);
		fuseDistances.push_back(ofVec3f(spriteSize,0,0));
	}
	 */
	
	//JUST ONE NODE
	ofVec3f point = position;
	fusePoints.push_back(point);

	float spriteSize = 1.0;
	nodePointIndex = fuseDistances.size();
	fuseDistances.push_back(ofVec3f(1.0,numIterations,generation));
	
	for(int i = 0; i < allNodes.size(); i++){
		allNodes[i]->targeted = false;
	}
	//cut off shortest paths
	sort(newNodes.begin(), newNodes.end(), nodesort);
//	for(int i = 0; i < MIN(numSurvivingBranches,newNodes.size()); i++){
//		newNodes[i]->leaf = !newNodes[i]->terminated;
//	}
	
	for(int i = 0; i < newNodes.size(); i++){
		newNodes[i]->leaf = !newNodes[i]->terminated;
	}

//
//	for(int i = numSurvivingBranches; i < newNodes.size(); i++){
//		newNodes[i]->finalize();
//		//cout << "fading out with max energy" << newNodes[i]->endEnergy << endl;
//	}
}

void Node::finalize(){
	fade(endEnergy, true);
}

void Node::fade(int maxEnergy, bool leaf){

	int energyIndex = startEnergy;
	for(int i = startIndex; i < endIndex; i++){
		//don't get rid of our blank out ones
		if(mesh->getColor(i).a > 0){
			float positionOnPath = ofMap(energyIndex++, 0, maxEnergy, 1.0, 0);
			float alpha = 1.0;
			if(leaf){
				//alpha = ofMap(i, startIndex, endIndex, 1.0, 0);
				alpha = 1.0;
			}
			ofFloatColor color = nodeColorTrack->getColorAtPosition(positionOnPath);
//			alpha *= .8;
			//mesh->setColor(i, ofFloatColor(alpha*125./255,alpha*142/255., alpha*193/255.));
//			mesh->setColor(i, color*alpha);

		}
	}
	
	if(parent != NULL){
		parent->fade(maxEnergy, false);
	}
}

void Node::terminate(){
	
	if(terminated){
		return;
	}
//	cout << "Terminated" << endl;
	if(parent!=NULL){
		terminated = true;
		leaf = false;
		for(int i = startIndex; i < endIndex; i++){
			if(mesh->getColor(i).a > 0){
//				mesh->setColor(i, ofFloatColor(.7,.4,.2	,1.0));
//				mesh->setColor(i, lineColor);
				//mesh->setColor(i, ofFloatColor(.3,.5,.3));
			}
		}
		
		parent->terminate();
	}
}

