#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

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
	for(int i = 0; i < dragInfo.files.size(); i++){
		ofxXmlSettings file;
		if(file.loadFile(dragInfo.files[i])){
			file.pushTag("svg");
			int numGs = file.getNumTags("g");
			for(int t = numGs-1; t >= 0; t--){
				if(file.attributeExists("g", "display", t)  && file.getAttribute("g", "display", "", t) == "none" ){
					file.removeTag("g",t);
					cout << "deleting g tag " << t << endl;
				}
				else{
					cout << "display is visible for g tag " << t << endl;
				}
			}
			
			file.saveFile(dragInfo.files[i]);
		}
	}
}