
#pragma once
#include "ofMain.h"
#include "CloudsFCPParser.h"

class CloudsExporter {
  public:

	// Create a matrix for D3 chord visualization
	static void saveChordMatrix(CloudsFCPParser& parser);
	static void saveGephiCSV(CloudsFCPParser& parser);
	static void savePajekNetwork(CloudsFCPParser& parser);

};