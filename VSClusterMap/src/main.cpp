#include "testApp.h"

//--------------------------------------------------------------
int main(){
	// set width, height, mode (OF_WINDOW or OF_FULLSCREEN)
//	ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
	ofSetupOpenGL(1024, 768, OF_WINDOW);
	ofRunApp(new testApp()); // start the app
}
