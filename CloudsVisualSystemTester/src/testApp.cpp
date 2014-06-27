#include "testApp.h"
#include "CloudsGlobal.h"
#include "CloudsVisualSystem.h"
 


//--------------------------------------------------------------
void testApp::setup(){
	
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
	if(ofGetElapsedTimef() - lastSystemChangedTime > 30){
		currentSystemIndex = (currentSystemIndex + 1) % presetIndices.size();
		advanceSystem();
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
				toLog( "success " + to_string(elapsedTime(shortStartTime))  + "ms \n");
				toLog( "beginning exit -- ");
				shortStartTime =  ofGetElapsedTimef();
				currentSystem->exit();
				toLog( "success " + to_string(elapsedTime(shortStartTime))  + "ms \n");
				toLog( "beginning deallocation -- ");
				shortStartTime =  ofGetElapsedTimef();
				CloudsVisualSystemManager::DeallocateSystems();
				toLog( "success " + to_string(elapsedTime(shortStartTime))  + "ms \n");
				toLog("------------------------------------------ \n");
			}

			toLog("============================================================== \n");
			toLog( "Advancing Visual System -- current index: " +  to_string(currentSystemIndex) + "\n" );
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
			toLog( "success " + to_string(elapsedTime(shortStartTime))  + "ms \n");
			toLog( "beginning loadPreSetGUI -- ");
			shortStartTime =  ofGetElapsedTimef();
			currentSystem->loadPresetGUISFromName(preset.presetName);
			toLog( "success " + to_string(elapsedTime(shortStartTime))  + "ms \n");
			toLog( "beginning setDrawToScreen -- ");
			currentSystem->setDrawToScreen(false);
			currentSystem->setNumSamples(4);
			toLog( "success \n");
			toLog( "beginning playSystem -- ");
			shortStartTime =  ofGetElapsedTimef();
			currentSystem->playSystem();
			toLog( "success " + to_string(elapsedTime(shortStartTime))  + "ms \n");
			toLog( "Complete time to advance system: " + to_string(elapsedTime(totalStartTime))  + "ms \n" );
			lastSystemChangedTime = ofGetElapsedTimef();
			toLog("============================================================== \n");

		}
	

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