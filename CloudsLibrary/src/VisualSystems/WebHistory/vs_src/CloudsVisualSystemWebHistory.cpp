//
//  CloudsVisualSystemWebHistory.cpp
//

#include "CloudsVisualSystemWebHistory.h"
#include "CloudsRGBDVideoPlayer.h"

#include <Poco/URI.h>

//--------------------------------------------------------------
//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemWebHistory::selfSetupGui()
{
	listGui = new ofxUISuperCanvas("SEARCH TERM LIST", gui);
	listGui->copyCanvasStyle(gui);
	listGui->copyCanvasProperties(gui);
	listGui->setName("SEARCH_TERM_LIST");
	listGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
    listGui->addSpacer();
    listGui->addSlider("TYPE SPEED", 1, 100, &typeSpeed);
    listGui->addToggle("CLEAR SCREEN", &bClearScreen);
    
    listGui->addSpacer();
    listHue = new ofx1DExtruder(0);
    listHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(listHue);
    listGui->addSlider("LIST HUE", 0.0, 255.0, listHue->getPosPtr());
    listSat = new ofx1DExtruder(0);
    listSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(listSat);
    listGui->addSlider("LIST SAT", 0.0, 255.0, listSat->getPosPtr());
    listBri = new ofx1DExtruder(0);
    listBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(listBri);
    listGui->addSlider("LIST BRI", 0.0, 255.0, listBri->getPosPtr());
    listAlpha = new ofx1DExtruder(0);
    listAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(listAlpha);
    listGui->addSlider("LIST ALPHA", 0.0, 255.0, listAlpha->getPosPtr());
    
    ofAddListener(listGui->newGUIEvent, this, &CloudsVisualSystemWebHistory::selfGuiEvent);
	
	guis.push_back(listGui);
	guimap[listGui->getName()] = listGui;
    
    treeGui = new ofxUISuperCanvas("URL TREE", gui);
	treeGui->copyCanvasStyle(gui);
	treeGui->copyCanvasProperties(gui);
	treeGui->setName("URL_TREE");
	treeGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    treeGui->addSpacer();
    treeGui->addSlider("LEVEL DEPTH", 1, 100, &HistoryNode::levelDepth);
    treeGui->addSlider("SPIN SPEED", 0, 5, &spinSpeed);
	treeGui->addSlider("NOISE STEP", 0, 0.1, &HistoryNode::noiseStep);
	treeGui->addSlider("NOISE AMOUNT", 1, 10, &HistoryNode::noiseAmount);
    treeGui->addSpacer();
    treeGui->addSlider("MIN Z", -2000.0, 0.0, &HistoryNode::minZ);
    treeGui->addSlider("MAX Z", 0.0, 2000.0, &HistoryNode::maxZ);
    treeGui->addSlider("MIN ALPHA", 0.0, 1.0, &HistoryNode::minAlpha);

    treeGui->addSpacer();
    treeGui->addSlider("TEXT ALIGN X", -1.0, 1.0, &HistoryNode::textAlignX);
    treeGui->addSlider("TEXT ALIGN Y", -1.0, 1.0, &HistoryNode::textAlignY);
    textHue = new ofx1DExtruder(0);
    textHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textHue);
    treeGui->addSlider("TEXT HUE", 0.0, 255.0, textHue->getPosPtr());
    textSat = new ofx1DExtruder(0);
    textSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textSat);
    treeGui->addSlider("TEXT SAT", 0.0, 255.0, textSat->getPosPtr());
    textBri = new ofx1DExtruder(0);
    textBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textBri);
    treeGui->addSlider("TEXT BRI", 0.0, 255.0, textBri->getPosPtr());
    textAlpha = new ofx1DExtruder(0);
    textAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(textAlpha);
    treeGui->addSlider("TEXT ALPHA", 0.0, 255.0, textAlpha->getPosPtr());
    
    treeGui->addSpacer();
    treeGui->addSlider("LINE WIDTH", 0.1, 10.0, &HistoryNode::lineWidth);
    lineHue = new ofx1DExtruder(0);
    lineHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(lineHue);
    treeGui->addSlider("LINE HUE", 0.0, 255.0, lineHue->getPosPtr());
    lineSat = new ofx1DExtruder(0);
    lineSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(lineSat);
    treeGui->addSlider("LINE SAT", 0.0, 255.0, lineSat->getPosPtr());
    lineBri = new ofx1DExtruder(0);
    lineBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(lineBri);
    treeGui->addSlider("LINE BRI", 0.0, 255.0, lineBri->getPosPtr());
    lineAlpha = new ofx1DExtruder(0);
    lineAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(lineAlpha);
    treeGui->addSlider("LINE ALPHA", 0.0, 255.0, lineAlpha->getPosPtr());
    
    treeGui->addSpacer();
    treeGui->addSlider("NODE RADIUS", 0.0, 20.0, &HistoryNode::nodeRadius);
    nodeHue = new ofx1DExtruder(0);
    nodeHue->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(nodeHue);
    treeGui->addSlider("NODE HUE", 0.0, 255.0, nodeHue->getPosPtr());
    nodeSat = new ofx1DExtruder(0);
    nodeSat->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(nodeSat);
    treeGui->addSlider("NODE SAT", 0.0, 255.0, nodeSat->getPosPtr());
    nodeBri = new ofx1DExtruder(0);
    nodeBri->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(nodeBri);
    treeGui->addSlider("NODE BRI", 0.0, 255.0, nodeBri->getPosPtr());
    nodeAlpha = new ofx1DExtruder(0);
    nodeAlpha->setPhysics(0.95, 5.0, 25.0);
    extruders.push_back(nodeAlpha);
    treeGui->addSlider("NODE ALPHA", 0.0, 255.0, nodeAlpha->getPosPtr());

    ofAddListener(treeGui->newGUIEvent, this, &CloudsVisualSystemWebHistory::selfGuiEvent);
	
	guis.push_back(treeGui);
	guimap[treeGui->getName()] = treeGui;
}

//--------------------------------------------------------------
void CloudsVisualSystemWebHistory::selfGuiEvent(ofxUIEventArgs &e)
{
    if (e.widget->getName() == "LIST HUE") {
        listHue->setPosAndHome(listHue->getPos());
	}
    else if (e.widget->getName() == "LIST SAT") {
        listSat->setPosAndHome(listSat->getPos());
	}
    else if (e.widget->getName() == "LIST BRI") {
        listBri->setPosAndHome(listBri->getPos());
	}
    else if (e.widget->getName() == "LIST ALPHA") {
        listAlpha->setPosAndHome(listAlpha->getPos());
    }
    
    else if (e.widget->getName() == "TEXT HUE") {
        textHue->setPosAndHome(textHue->getPos());
	}
    else if (e.widget->getName() == "TEXT SAT") {
        textSat->setPosAndHome(textSat->getPos());
	}
    else if (e.widget->getName() == "TEXT BRI") {
        textBri->setPosAndHome(textBri->getPos());
	}
    else if (e.widget->getName() == "TEXT ALPHA") {
        textAlpha->setPosAndHome(textAlpha->getPos());
    }
    
    else if (e.widget->getName() == "LINE HUE") {
        lineHue->setPosAndHome(lineHue->getPos());
	}
    else if (e.widget->getName() == "LINE SAT") {
        lineSat->setPosAndHome(lineSat->getPos());
	}
    else if (e.widget->getName() == "LINE BRI") {
        lineBri->setPosAndHome(lineBri->getPos());
	}
    else if (e.widget->getName() == "LINE ALPHA") {
        lineAlpha->setPosAndHome(lineAlpha->getPos());
    }
    
    else if (e.widget->getName() == "NODE HUE") {
        nodeHue->setPosAndHome(nodeHue->getPos());
	}
    else if (e.widget->getName() == "NODE SAT") {
        nodeSat->setPosAndHome(nodeSat->getPos());
	}
    else if (e.widget->getName() == "NODE BRI") {
        nodeBri->setPosAndHome(nodeBri->getPos());
	}
    else if (e.widget->getName() == "NODE ALPHA") {
        nodeAlpha->setPosAndHome(nodeAlpha->getPos());
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemWebHistory::selfSetupSystemGui(){
	
}

void CloudsVisualSystemWebHistory::guiSystemEvent(ofxUIEventArgs &e){
	
}
//use render gui for display settings, like changing colors
void CloudsVisualSystemWebHistory::selfSetupRenderGui(){

}

void CloudsVisualSystemWebHistory::guiRenderEvent(ofxUIEventArgs &e){
	
}

//--------------------------------------------------------------
// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemWebHistory::selfSetup()
{
    // Load fonts.
    listColor.set(255, 128, 64);  // Pick a non-gray color so that HSB gets set properly.
    if (!listFont.loadFont(getVisualSystemDataPath() + "Andale Mono.ttf", 12)) {
        ofLogError("WebHistory") << "Could not load list font " << getVisualSystemDataPath() << "Andale Mono.ttf";
    }
    if (!HistoryNode::font.loadFont(getVisualSystemDataPath() + "Andale Mono.ttf", 12, true, true, true)) {
        ofLogError("WebHistory") << "Could not load node font " << getVisualSystemDataPath() << "Andale Mono.ttf";
    }
    
    // Set defaults.
    currSpin = 0.0f;
    spinSpeed = 0.5f;
    typeSpeed = 10;
    bClearScreen = false;
    
    if (fetchChromeHistory()) {
        ofLogNotice("WebHistory") << "Using live Chrome data" << endl;
    }
//    else if (fetchSafariHistory()) {
//        ofLogNotice("WebHistory") << "Using live Safari data" << endl;
//    }
//    else if (fetchFirefoxHistory()) {
//        ofLogNotice("WebHistory") << "Using live Firefox data" << endl;
//    }
    else if (fetchChromeHistory(true)) {
        ofLogNotice("WebHistory") << "Using sample Chrome data" << endl;
    }
    else {
        ofLogError("WebHistory") << "No available web history!" << endl;
    }
}

//--------------------------------------------------------------
bool CloudsVisualSystemWebHistory::fetchChromeHistory(bool bUseSample)
{
    string chromeHistoryPath;
    if (bUseSample) {
        chromeHistoryPath = getVisualSystemDataPath() + "SampleChromeHistory";
    }
    else {
        chromeHistoryPath = ofFilePath::getUserHomeDir() + "/Library/Application Support/Google/Chrome/Default/History";
    }

    if(!ofFile(chromeHistoryPath).doesFileExist(chromeHistoryPath)){
        ofLogError("WebHistory") << (bUseSample ? "Sample " : "Actual") << " database file does not exist at path " << chromeHistoryPath;
        return false;
	}
	
    ofxSQLite sqlite;
    if (!sqlite.setup(chromeHistoryPath)) {
        // No dice :(
        ofLogError("WebHistory") << "Couldn't load " << (bUseSample ? "sample " : "actual") << " database at path " << chromeHistoryPath;
        return false;
    }
    	
	ofxSQLiteSelect sel = sqlite.select("url, last_visit_time").from("urls")
                                .order("last_visit_time", "DESC")
                                .execute().begin();
    
    if (!sel.hasRow()) {
        // No rows, db is probably locked.
        return false;
    }
    
    int count = 0;
	while (sel.hasNext() && count < 100) {
        string url = sel.getString();
        time_t timestamp = sel.getInt();
        
		sel.next();
        ++count;
        
        Poco::URI uri(url);
        string host = uri.getHost();
        vector<string> segments;
        uri.getPathSegments(segments);
        list<string> segmentList;
        for (int i = 0; i < segments.size(); i++) {
            segmentList.push_back(segments[i]);
        }
        
        try {
            HistoryNode * node = hosts.at(host);
            node->addChild(segmentList, 1);
        }
        catch (const std::out_of_range& e) {
            HistoryNode * node = new HistoryNode(host, timestamp, 0, segmentList);
            hosts[host] = node;
        }
	}
        
    if (ofGetLogLevel() <= OF_LOG_VERBOSE) {
        for (auto& it : hosts) {
            it.second->print();
        }
    }
    
    sel = sqlite.select("lower_term").from("keyword_search_terms")
                .order("url_id", "DESC")
                .execute().begin();
    
    count = 0;
	while (sel.hasNext() && count < 100) {
        string url = sel.getString();
        ofxTextWriter * term = new ofxTextWriter(url);
        searchTerms.push_back(term);
		sel.next();
        ++count;
	}
    
    currSearchTermIdx = 0;
    topSearchTermIdx = 0;
    searchTermCount = 1;
    
    return true;
}

//--------------------------------------------------------------
bool CloudsVisualSystemWebHistory::fetchSafariHistory()
{
    string safariHistoryPath = ofFilePath::getUserHomeDir() + "/Library/Safari/History.plist";
    
    ofxXmlSettings xml;
    if (!xml.loadFile(safariHistoryPath)) {
        // No dice :(
        return false;
    }
    
    return true;
}

//--------------------------------------------------------------
bool CloudsVisualSystemWebHistory::fetchFirefoxHistory()
{
    ofDirectory dir;
    dir.listDir(ofFilePath::getUserHomeDir() + "/Library/Application Support/Firefox/Profiles/");
    if (!dir.size()) {
        // No dice :(
        return false;
    }
    
    string firefoxHistoryPath = dir.getPath(0) + "/places.sqlite";
    
    ofxSQLite sqlite;
    sqlite.setup(firefoxHistoryPath);
    
	ofxSQLiteSelect sel = sqlite.select("url, last_visit_date").from("moz_places")
                                .order("last_visit_date", "DESC")
                                .execute().begin();
    
    if (!sel.hasRow()) {
        // No rows, db is probably locked.
        return false;
    }
    
    int count = 0;
	while (sel.hasNext() && count < 50) {
        string url = sel.getString();
        time_t timestamp = sel.getInt();
        
		sel.next();
        ++count;
        
        Poco::URI uri(url);
        string host = uri.getHost();
        vector<string> segments;
        uri.getPathSegments(segments);
        list<string> segmentList;
        for (int i = 0; i < segments.size(); i++) {
            segmentList.push_back(segments[i]);
        }
        
        try {
            HistoryNode * node = hosts.at(host);
            node->addChild(segmentList, 1);
        }
        catch (const std::out_of_range& e) {
            HistoryNode * node = new HistoryNode(host, timestamp, 0, segmentList);
            hosts[host] = node;
        }
	}
    
    if (ofGetLogLevel() <= OF_LOG_VERBOSE) {
        for (auto& it : hosts) {
            it.second->print();
        }
    }
    
    return true;
}

//--------------------------------------------------------------
// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemWebHistory::selfPresetLoaded(string presetPath)
{
	
}

//--------------------------------------------------------------
// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemWebHistory::selfBegin()
{

}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemWebHistory::selfSceneTransformation(){
	
}

//--------------------------------------------------------------
//normal update call
void CloudsVisualSystemWebHistory::selfUpdate()
{
    // Update the extruders parameters.
    listColor.setHsb(listHue->getPos(), listSat->getPos(), listBri->getPos(), listAlpha->getPos());
    HistoryNode::textColor.setHsb(textHue->getPos(), textSat->getPos(), textBri->getPos(), textAlpha->getPos());
    HistoryNode::lineColor.setHsb(lineHue->getPos(), lineSat->getPos(), lineBri->getPos(), lineAlpha->getPos());
    HistoryNode::nodeColor.setHsb(nodeHue->getPos(), nodeSat->getPos(), nodeBri->getPos(), nodeAlpha->getPos());
    
    currSpin += spinSpeed;
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemWebHistory::selfDraw()
{
    ofPushStyle();
    ofPushMatrix();
    {
        ofRotate(currSpin, 0, 1, 0);
        ofSetLineWidth(HistoryNode::lineWidth);

        for (auto& it: hosts) {
            it.second->draw();
        }
    }
    ofPopMatrix();
    ofPopStyle();
}

// draw any debug stuff here
void CloudsVisualSystemWebHistory::selfDrawDebug()
{
	
}

//--------------------------------------------------------------
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemWebHistory::selfDrawBackground()
{
    if (searchTerms.empty()) {
        // Nothing to do here.
        return;
    }
    
    ofSetColor(listColor);
    
    int stringHeight = 20;
    //MA: changed ofGetHeight() to getCanvasHeight()
    int maxNumStrings = getCanvasHeight() / stringHeight;
    
    if (bClearScreen) {
        if (searchTermCount > 0 && (ofGetFrameNum() % (int)typeSpeed == 0)) {
            // Scroll up.
            topSearchTermIdx = (topSearchTermIdx + 1) % searchTerms.size();
            --searchTermCount;
        }
    }
    else {
        if (searchTerms[currSearchTermIdx]->isDone()) {
            // Start rendering the next term.
            currSearchTermIdx = (currSearchTermIdx + 1) % searchTerms.size();
            searchTerms[currSearchTermIdx]->reset(typeSpeed);
            
            // Go to the next line.
            ++searchTermCount;
            if (searchTermCount > maxNumStrings) {
                searchTermCount = maxNumStrings;

                // Scroll up.
                topSearchTermIdx = (topSearchTermIdx + 1) % searchTerms.size();
            }
        }
    }
    
    // Render the search terms.
    for (int i = 0; i < searchTermCount; i++) {
        int idx = (topSearchTermIdx + i) % searchTerms.size();
        searchTerms[idx]->update();
        listFont.drawString(searchTerms[idx]->textToRender(), 10, (i + 1) * stringHeight);
    }
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemWebHistory::selfEnd()
{
		
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemWebHistory::selfExit()
{

}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemWebHistory::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemWebHistory::selfKeyReleased(ofKeyEventArgs & args){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemWebHistory::selfMouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemWebHistory::selfMouseMoved(ofMouseEventArgs& data){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemWebHistory::selfMousePressed(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemWebHistory::selfMouseReleased(ofMouseEventArgs& data){
	
}
