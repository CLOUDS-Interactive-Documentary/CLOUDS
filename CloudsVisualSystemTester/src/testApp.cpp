#include "testApp.h"
#include "CloudsGlobal.h"
#include "CloudsVisualSystem.h"

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

//--------------------------------------------------------------
void testApp::setup(){
    
    version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    card = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    
    bEnableSpeedLog = true;
    prevLoadedSpeedLog = false;
    vsFrameCount = 0;
    vsLoadTime = 0.;
    vsStartTime = 0.;
    vsAvgFPS = 0.;
    vsLastSampleFrame = 0;
    vsFPSLastSampleTime = 0.;
    
#ifdef OCULUS_RIFT
    
    speedLogFilenameXML = "clouds_vs_speed_log.oculus."+replaceAll(card, " ", "_")+".xml";
    speedLogFilenameCSV = "clouds_vs_speed_log.oculus."+replaceAll(card, " ", "_")+".csv";
    
#else
    
    speedLogFilenameXML = "clouds_vs_speed_log."+replaceAll(card, " ", "_")+".xml";
    speedLogFilenameCSV = "clouds_vs_speed_log."+replaceAll(card, " ", "_")+".csv";
    
#endif
    
    
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofBackground(0);
    
    systemManager.loadPresets();
    systemManager.loadCachedDataForSystems();
    
    currentSystem = NULL;
    currentSystemIndex = 0;
    
    lastSystemChangedTime = 0;
    
    bForward = true;
    
    blackList.clear();
    
    blackList.push_back(make_pair("Memory", "ThinBlueRows_2")); //crash in loadingPresetGUI
    blackList.push_back(make_pair("Memory", "ThinBlueRows_wide")); //crash in loadingPresetGUI
    blackList.push_back(make_pair("Memory", "ThinTealRows")); //crash in loadingPresetGUI
    blackList.push_back(make_pair("Memory", "randomWaves")); //crash in loadingPresetGUI
    /*
     blackList.push_back(make_pair("Histogram", "Waveform1")); //not registered
     blackList.push_back(make_pair("Histogram", "Waveform1_Happen")); //not registered
     blackList.push_back(make_pair("Histogram", "Waveform3_GTNonStop")); //not registered
     blackList.push_back(make_pair("Histogram", "Waveform3")); //not registered
     blackList.push_back(make_pair("Histogram", "DataHistogram4")); //not registered
     blackList.push_back(make_pair("Histogram", "DataHistogram3")); //not registered
     blackList.push_back(make_pair("Histogram", "Waveform1_Happen")); //not registered
     */
    blackList.push_back(make_pair("Rulez", "Ico_rotate")); //running and registered but showing nothing
    blackList.push_back(make_pair("RandomDigits", "BasicOC"));//not registered
    blackList.push_back(make_pair("OpenP5DrawingMachine10", "chaos")); //not registered
    blackList.push_back(make_pair("WebHistory", "AutoSpiderShell")); //not registered
    blackList.push_back(make_pair("WebHistory", "Hazy_widesuccess")); //not registered
    
    //blackList.push_back(make_pair("Orbit", "Lorenz_Dolly")); //crash during run
    blackList.push_back(make_pair("Xstatic", "StarChurn")); //something wrong with preset
    blackList.push_back(make_pair("Cosmic", "JM_biological2")); //running and registered but showing nothing, or causing problems, also weird preset
    //blackList.push_back(make_pair("Schlabberbox", "jm_whirl")); //maybe crashed during run? it followed the mysterious Mandala
    blackList.push_back(make_pair("Xstatic", "fireflies")); //something wrong with preset
    blackList.push_back(make_pair("CubeCraft", "MC_FlyoverDesert")); //looks bad in preset
    
    //cubeCraft MC_FlyoverDesert looks like shit
    //cubeCraft has a bgColor state leak
    //cubecraft fog tweaks
    //xstatic fireflies all particles drawing in the same place on top of eachother, state leak!
    //xstatic star churn is all in a line?
    //voro dream crystal1 check alpha blending??
    //cosmic JM_biological2  , preset has some problems and is suseptibable to depth_test state leak
    //connectors susseptible to line width state leak
    //prior to 128 was a lineWidth return to 0?
    //egg astrolabe far clipping plane in oculus needs extension
    //OpenP5Machine  is the slowest general system, needs optimization
    
    //line width state leak victims = ocean, world, twitter, connections,
    
    //check OpenP5Hackpact  for line state leak? openP5Spaghetti?
    
    presetIndices = systemManager.getFilteredPresetIndeces(false,true);
    for(int i = 0; i < presetIndices.size(); i++){
        cout << systemManager.getPresets()[ presetIndices[i] ].systemName << " " << systemManager.getPresets()[ presetIndices[i] ].presetName << endl;
    }
    
    shuffleSystemIndices();
    advanceSystem();
    
}

void testApp::shuffleSystemIndices(){
    random_shuffle( presetIndices.begin(), presetIndices.end() );
}

//--------------------------------------------------------------
void testApp::update(){
    if(ofGetElapsedTimef() - lastSystemChangedTime > 14){
        CloudsVisualSystemPreset& preset = systemManager.getPresets()[presetIndices[currentSystemIndex]];
        if(bEnableSpeedLog){
            toSpeedLog( preset.systemName, preset.presetName, vsLoadTime, vsAvgFPS);
        }
        currentSystemIndex = (currentSystemIndex + 1) % presetIndices.size();
        advanceSystem();
        vsFrameCount = 0;
        vsLastSampleFrame = 0;
    }
    
    vsFrameCount++;
    double now = ofGetElapsedTimef();
    if( now > vsFPSLastSampleTime + 1 ) {
        uint32_t framesPassed = vsFrameCount - vsLastSampleFrame;
        vsAvgFPS = (float)(framesPassed / (now - vsFPSLastSampleTime));
        vsFPSLastSampleTime = now;
        vsLastSampleFrame = vsFrameCount;
    }
    
}

float testApp::elapsedTime( float& start_time ){
    return ofGetElapsedTimef() - start_time;
}

void testApp::advanceSystem(){
    
    float totalStartTime = ofGetElapsedTimef();
    float shortStartTime = 0;
    
    CloudsVisualSystemPreset& preset = systemManager.getPresets()[presetIndices[currentSystemIndex]];
    
    bool allow = true;
    auto it = blackList.begin();
    auto end = blackList.end();
    while(it != end){
        if( preset.presetName == it->second ){
            allow = false;
            break;
        }
        ++it;
    }
    
    if(!allow){
        if(bForward) currentSystemIndex = currentSystemIndex + 1 % presetIndices.size();
        else currentSystemIndex = currentSystemIndex - 1 % presetIndices.size();
        advanceSystem();
    }
    else{
        
        if(currentSystem != NULL){
            shortStartTime =  ofGetElapsedTimef();
            toLog("------------------------------------------ \n");
            toLog( currentSystem->getSystemName() + " beginning stop -- " );
            currentSystem->stopSystem();
            toLog( "success " + ofToString(elapsedTime(shortStartTime))  + "ms \n");
            toLog( "beginning exit -- ");
            shortStartTime =  ofGetElapsedTimef();
            currentSystem->exit();
            toLog( "success " + ofToString(elapsedTime(shortStartTime))  + "ms \n");
            toLog( "beginning deallocation -- ");
            shortStartTime =  ofGetElapsedTimef();
            CloudsVisualSystemManager::DeallocateSystems();
            toLog( "success " + ofToString(elapsedTime(shortStartTime))  + "ms \n");
            toLog("------------------------------------------ \n");
        }
        
        toLog("============================================================== \n");
        toLog( "Advancing Visual System -- current index: " +  ofToString(currentSystemIndex) + "\n" );
        toLog("System: " + preset.systemName + " Preset: " + preset.presetName + "  \n");
        toLog("Instantiating system -- ");
        currentSystem = CloudsVisualSystemManager::InstantiateSystem(preset.systemName);
        toLog( "success \n");
        
        if(currentSystem->getSystemName() == "Empty"){
            toLog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
            toLog( "SYSTEM " + preset.systemName + " NOT REGISTERED \n");
            toLog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
        }
        
        //log that this is about to set up
        toLog( "beginning setup -- ");
        shortStartTime =  ofGetElapsedTimef();
        currentSystem->setup();
        toLog( "success " + ofToString(elapsedTime(shortStartTime))  + "ms \n");
        toLog( "beginning loadPreSetGUI -- ");
        shortStartTime =  ofGetElapsedTimef();
        currentSystem->loadPresetGUISFromName(preset.presetName);
        toLog( "success " + ofToString(elapsedTime(shortStartTime))  + "ms \n");
        toLog( "beginning setDrawToScreen -- ");
        currentSystem->setDrawToScreen(false);
        currentSystem->setNumSamples(4);
        toLog( "success \n");
        toLog( "beginning playSystem -- ");
        shortStartTime =  ofGetElapsedTimef();
        currentSystem->playSystem();
        toLog( "success " + ofToString(elapsedTime(shortStartTime))  + "ms \n");
        toLog( "Complete time to advance system: " + ofToString(elapsedTime(totalStartTime))  + "ms \n" );
        
        vsLoadTime = elapsedTime(totalStartTime);
        
        lastSystemChangedTime = ofGetElapsedTimef();
        toLog("============================================================== \n");
        
    }
    
    
}

void testApp::writeSpeedLog(){
    
    
    ofBuffer out;

    ofxXmlSettings xml;
    
    xml.addTag("machine");
    xml.pushTag("machine");
    xml.addValue("version", version);
    xml.addValue("vendor", vendor);
    xml.addValue("card", card);
    
#ifdef OCULUS_RIFT
    out.append( "machine : "+version+" : "+vendor+" : "+card+" : display oculusHD\n" );
    xml.addValue("display", "oculusHD");
#else
    out.append( "machine : "+version+" : "+vendor+" : "+card+" : display monitor\n" );
    xml.addValue("display", "monitor");
#endif
    
    xml.popTag();
    
    out.append( "visual system,preset,loadtime,fps\n" );
    
    int vs_ind = 0;
    
    for( auto & _vs : vs ){
        
        xml.addTag("visual_system");
        xml.addAttribute("visual_system", "name", _vs.name, vs_ind );
        xml.pushTag("visual_system", vs_ind);
    
        int p_ind = 0;
        for( auto& p : _vs.presets ){
            
            out.append(_vs.name+","+p.name+","+ofToString(p.loadtime)+","+ofToString(p.fps)+"\n");
            
            xml.addTag("preset");
            xml.addAttribute("preset", "name", p.name, p_ind );
            xml.pushTag("preset",p_ind);
            
            xml.addValue("loadtime", p.loadtime);
            xml.addValue("fps", p.fps);
            
            xml.popTag();
            
            p_ind++;
        }
        
        xml.popTag();
        
        vs_ind++;
    }
    
    ofBufferToFile(speedLogFilenameCSV, out);
    xml.save(speedLogFilenameXML);
}

void testApp::toSpeedLog( const std::string& vs_name, const std::string& preset, float loadtime, float avg_fps ){
    
    bool foundVS = false;
    for( auto& _vs : vs ){
        
        if( _vs.name == vs_name ){
            
            bool foundPre = false;
            for(auto& _p : _vs.presets){
                
                if( _p.name == preset){
                    _p.loadtime = loadtime;
                    _p.fps = avg_fps;
                    foundPre = true;
                    break;
                }
            }
            
            if(!foundPre){
                VS_PRESET p;
                p.name = preset;
                p.loadtime = loadtime;
                p.fps = avg_fps;
                _vs.presets.push_back(p);
            }
            
            foundVS = true;
            break;
        }
    }
    
    if(!foundVS){
        VS v;
        v.name = vs_name;
        VS_PRESET p;
        p.name = preset;
        p.loadtime = loadtime;
        p.fps = avg_fps;
        v.presets.push_back(p);
        vs.push_back(v);
    }
    
    writeSpeedLog();
}

void testApp::toLog(const std::string& to_log){
    
    static bool initLog = false;
    
    if(!initLog){
        sprintf(logFilename, "log_%02d_%02d_%02d_%02d_%02d.txt", ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds());
        log.append("Clouds Visual Systems Test Log \n ================================================ \n ");
        initLog = true;
    }
    
    log.append(to_log);
    logEnd();
}

void testApp::logEnd(){
    ofBufferToFile(logFilename, log);
}

//--------------------------------------------------------------
void testApp::draw(){
    if(currentSystem != NULL){
        currentSystem->selfPostDraw();
        //draw the name of the system overlayed so that we know what we're looking at
    }
    if(bEnableSpeedLog){
        ofDrawBitmapString("Speed Logging Enabled", 0, ofGetHeight() - 20);
    }
    else{
        ofDrawBitmapString("Speed Logging Disabled", 0, ofGetHeight() - 20);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    //TODO: Quarentine a system
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    if(key == OF_KEY_LEFT){
        currentSystemIndex = (currentSystemIndex - 1) % presetIndices.size();
        advanceSystem();
        bForward = false;
    }
    if(key == OF_KEY_RIGHT){
        currentSystemIndex = (currentSystemIndex + 1) % presetIndices.size();
        advanceSystem();
        bForward = true;
    }
    if(key == 's'){
        shuffleSystemIndices();
    }
    
    if(key == ' '){
        bEnableSpeedLog = !bEnableSpeedLog;
    }
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}