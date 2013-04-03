#include "CloudsD3Exporter.h"

CloudsD3Exporter::CloudsD3Exporter(){
	
}

void CloudsD3Exporter::saveChordMatrix(CloudsFCPParser& database){

	database.sortKeywordsByOccurrence(true);
	vector<string>& allKeywords = database.getAllKeywords();
	ofBuffer buf;
	int entries = MIN(allKeywords.size(), 50);
	buf.append("[");
    for(int y = 0; y < entries; y++){
		for(int x = 0; x < entries; x++){
			
			buf.append( ofToString(database.getSharedClips(allKeywords[x], allKeywords[y]).size()) );
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
}