//
//  CodePanel.h
//  Code
//
//  Created by James George on 12/1/13.
//
//

#pragma once

#include "ofMain.h"
#include "Panel.h"

class PanelGraph : public Panel {
  public:
	
	PanelGraph();
	void selfSetup();
	void selfDraw();
	
	float pixelStep;
  protected:	
	
};