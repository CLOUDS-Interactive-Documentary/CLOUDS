#include "testApp.h"

//--------------------------------------------------------------
int main(){
	// set width, height, mode (OF_WINDOW or OF_FULLSCREEN)
<<<<<<< HEAD
	ofSetupOpenGL(1024, 768, OF_WINDOW);
=======
	ofSetupOpenGL(&window, 1280, 720, OF_WINDOW);
>>>>>>> master
	ofRunApp(new testApp()); // start the app
}
