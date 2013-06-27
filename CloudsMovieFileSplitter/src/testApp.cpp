#include "testApp.h"
#define FRAME_PADDING 24
//--------------------------------------------------------------
void testApp::setup(){
    
    string xmlFile = "xmls/Binx.xml";
    
    if(fcpXML.loadFile(xmlFile)){
        fcpXML.pushTag("xmeml");
        int numSequences = fcpXML.getNumTags("sequence");
        for(int i = 0; i < numSequences; i++){
            string name = fcpXML.getAttribute("sequence", "id", "", i);
            //            cout << "name is " << name << endl;
            fcpXML.pushTag("sequence", i);
            fcpXML.pushTag("media");
            fcpXML.pushTag("video");
            
            int numTracks = fcpXML.getNumTags("track");
            //          cout << "   found " << numTracks << " tracks" << endl;
            for(int t = 0; t < numTracks; t++){
                fcpXML.pushTag("track", t);
                int numClipItems = fcpXML.getNumTags("clipitem");
                //                cout << "   found " << numClipItems << " clips" << endl;
                for(int c = 0; c < numClipItems; c++){
                    fcpXML.pushTag("clipitem", c);
                    string clipFileName = fcpXML.getValue("name", "");
                    int duration = ofToInt(fcpXML.getValue("duration", ""));
                    int startFrame = ofToInt(fcpXML.getValue("start", ""));
                    int endFrame = ofToInt(fcpXML.getValue("end", ""));
                    if(duration!= (endFrame-startFrame)){
                        ofLogError() <<"Duration not equal to EndFrame - startFrame for "<<clipFileName<<endl;
                    }
                    else{
 
                        
                        float numOfSec = startFrame/23.976;
                        float durationInSecs  = duration/23.976;
                        
                        //float remainingDurationFrames = (duration%24);
                        //float remaingDurationMS = (1000/24.)*remainingDurationFrames;
                        
                        //float remainingFrames = (startFrame%24);
                        //float remaingMs = (1000/24.)*remainingFrames;
                        //stringstream timeCode;
                        //stringstream duration;
                        

                        //duration<<int(durationInSecs/3600)<<":"<<int((durationInSecs%3600)/60)<<":"<<durationInSecs%60<<"."<<remainingDurationFrames<<" ";
                        //timeCode<<int(numOfSec/3600)<<":"<<int((numOfSec%3600)/60)<<":"<<numOfSec%60<<"."<<remaingMs<<" ";
                        //cout<<"Clip id: "<<clipFileName<<", duration: "<<duration<<", startFrame:"<<startFrame<<", endFrame:"<<endFrame<<", timeCode: " <<timeCode.str()<<", duration: " <<duration.str()<< endl;
                        
                        stringstream ffmpegCommand;
                        ffmpegCommand<<"ffmpeg -i \""<<name<<".mov"<< "\""" -vcodec copy -acodec copy "<<"-ss "<<numOfSec<<" -t "<<durationInSecs<<" "<<clipFileName<<".mov"<<endl;
                        
                        cout<<ffmpegCommand.str()<<endl;
                    }
                    fcpXML.popTag(); //clipitem
                }
                fcpXML.popTag(); // track
            }
            
            fcpXML.popTag(); //video
            fcpXML.popTag(); //media
            fcpXML.popTag(); //sequence
        }
        fcpXML.popTag(); //xmeml
        
    }
    else{
        ofLogError("CloudsFCPParser::addXMLFile -- file parse failed " + xmlFile);
    }
    
}

//--------------------------------------------------------------
void testApp::update(){
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
}

void testApp::parseClipItem(ofxXmlSettings& fcpXML, string currentName){
    string fileID = fcpXML.getAttribute("file", "id", "");
    string clipFileName = fcpXML.getValue("file:name", "");
    string clipFilePath = fcpXML.getValue("file:pathurl", "");
    if(clipFileName != ""){
        //        fileIdToName[fileID] = clipFileName;
        //        fileIdToPath[fileID] = clipFilePath;
    }
    else{
    }
    ofStringReplace(clipFilePath, "file://localhost", "");
    int numMarkers = fcpXML.getNumTags("marker");
    for(int m = 0; m < numMarkers; m++){
        fcpXML.pushTag("marker", m);
        
        CloudsClip cm;
        cm.startFrame = fcpXML.getValue("in", 0);
        cm.endFrame = fcpXML.getValue("out", 0);
        string comment = fcpXML.getValue("comment", "");
        //validate
        if(comment != "" && cm.endFrame - cm.startFrame > 1 && cm.endFrame > 0){
            cm.name = fcpXML.getValue("name", "");
            cm.person = currentName;
			cm.fcpFileId = fileID;
			cm.clip = clipFileName;
			cm.sourceVideoFilePath = clipFilePath;
			cout<<"CLip Number "<<m<<": "<<"Name - "<<cm.name<<",Start Frame - "<<cm.startFrame<<", EndFrame - "<<cm.endFrame<<endl;
            

        }
    fcpXML.popTag(); //marker
    }
  
}
//    keywordsDirty = true;



