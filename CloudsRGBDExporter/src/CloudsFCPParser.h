/**
 * CLOUDS FCP Parser
 * Class for parsing a set of FCP XML and presenting keywords
 *
 * Copyright 2013 James George
 */

#pragma once
#include "ofMain.h"

class CloudsFCPParser {
  public:
    CloudsFCPParser();
    void setup(string directory);
    
    void refreshXML();
    vector<string>& getAllKeywords();
    
  protected:
    
};