//
//  PanelTenPrint.h
//  Code
//
//  Created by James George on 1/3/14.
//
//

#pragma once

#include "Panel.h"

class PanelTenPrint : public Panel {
public:
	
	PanelTenPrint();
	
	void selfSetup();
	void selfUpdate();
	void selfDraw();
	
	float maxChars;

  protected:
	vector< pair<string,string> > pairs;
	
	
};