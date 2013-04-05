
#pragma once
#include "ofMain.h"
#include "CloudsFCPParser.h"

class CloudsD3Exporter {
  public:
	CloudsD3Exporter();

	// Create a matrix for D3 chord visualization
	void saveChordMatrix(CloudsFCPParser& parse);
	
};