#include "CloudsD3Exporter.h"

CloudsD3Exporter::CloudsD3Exporter(){
	
}

void CloudsD3Exporter::saveChordMatrix(CloudsFCPParser& database){

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
