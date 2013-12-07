#include "CloudsExporter.h"

void CloudsExporter::saveChordMatrix(CloudsFCPParser& database){

	database.sortKeywordsByOccurrence(true);
	vector<string>& allKeywords = database.getAllKeywords();
	ofBuffer buf;
	int entries = MIN(allKeywords.size(), 25);
	ofBuffer csvBuffer;
	csvBuffer.append("name\n");
	
	buf.append("[");
    for(int y = 0; y < entries; y++){
		buf.append("[");
		csvBuffer.append(allKeywords[y] + "\n");
		for(int x = 0; x < entries; x++){
			if(x == y){
				buf.append("0");
			}
			else{
				buf.append( ofToString(database.getSharedClips(allKeywords[x], allKeywords[y]).size()) );	
			}
			
			if(x != entries-1){
				buf.append(",");
			}
		}
		
		buf.append("]");
		if(y != entries-1){
			buf.append(",");
		}
		buf.append("\n");
    }
	buf.append("]");
	
	
	ofBufferToFile("D3matrix.json", buf);
	ofBufferToFile("D3keywords.csv", csvBuffer);
}

void CloudsExporter::saveGephiCSV(CloudsFCPParser& parser){
	ofBuffer csvBuffer;
	csvBuffer.append("source,target\n");
	for(int i = 0; i < parser.getAllClips().size(); i++){
		CloudsClip& clipA = parser.getAllClips()[i];
		string nameA = clipA.getLinkName();
		vector<CloudsClip> connections = parser.getClipsWithKeyword(clipA.getKeywords());
		for(int j = 0; j < connections.size(); j++){
			CloudsClip& clipB = connections[j];
			string nameB = connections[j].getLinkName();
			if(nameA != nameB &&
			   clipA.person != clipB.person &&
			   !parser.linkIsSuppressed(nameA, nameB) &&
//			   !parser.clipLinksTo(nameA, nameB) &&
			   parser.getNumberOfSharedKeywords(clipA, clipB) > 1 )
			{
				csvBuffer.append(nameA + "," + nameB + "\n");
			}
		}
		
		vector<CloudsLink>& links = parser.getLinksForClip(clipA);
		for(int i = 0; i < links.size(); i++){
//			csvBuffer.append(nameA + "," + links[i].targetName + "\n");
//			cout << "	Added LINK to CSV " << (nameA + "," + links[i].targetName + "\n") << endl;
		}
		
	}
	
	ofBufferToFile("gephi_edges.csv", csvBuffer);
	
}

void CloudsExporter::savePajekNetwork(CloudsFCPParser& parser){
	ofBuffer pajekBuffer;
	pajekBuffer.append("*Vertices " + ofToString(parser.getAllClips().size()) + "\n");
	map<string,string> nodeIndex;
	for(int i = 0; i < parser.getAllClips().size(); i++){
		nodeIndex[parser.getAllClips()[i].getLinkName()] = ofToString(i);
		pajekBuffer.append( ofToString(i) + " \"" + parser.getAllClips()[i].getLinkName() + "\"\n" );
	}
	
	pajekBuffer.append("\n*Edgeslist\n");
	for(int i = 0; i < parser.getAllClips().size(); i++){
		
		CloudsClip& clipA = parser.getAllClips()[i];
		string nameA = clipA.getLinkName();
		vector<CloudsClip> connections = parser.getClipsWithKeyword(clipA.getKeywords());
		vector<CloudsLink> links = parser.getLinksForClip(clipA);
		for(int l = 0; l < links.size(); l++){
			connections.push_back(parser.getClipWithLinkName(links[l].targetName));
		}

		vector<string> connectingNodes;
		
		for(int j = 0; j < connections.size(); j++){
			
			CloudsClip& clipB = connections[j];
			string nameB = connections[j].getLinkName();
			
			if(nameA != nameB &&
			   (parser.clipLinksTo(nameA, nameB) ||
			   (clipA.person != clipB.person &&
			   !parser.linkIsSuppressed(nameA, nameB) &&
			   parser.getNumberOfSharedKeywords(clipA, clipB) > 1) ))
			{
//				pajekBuffer.append(nameA + "," + nameB + "\n");
				connectingNodes.push_back( nodeIndex[nameB] );
			}
		}
		
		if(connectingNodes.size() > 0){
			pajekBuffer.append(nodeIndex[nameA] + " " + ofJoinString(connectingNodes, " ") + "\n" );
		}
		
	}
	
	ofBufferToFile("CloudsCluster.net", pajekBuffer);
}

