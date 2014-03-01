#include "testApp.h"
#include "ofAppGlutWindow.h"
#include "ofAppGlfwWindow.h"

//--------------------------------------------------------------
int main(){
	//ofAppGlutWindow window; // create a window
	ofAppGLFWWindow window;
	// set width, height, mode (OF_WINDOW or OF_FULLSCREEN)
	ofSetupOpenGL(&window, 1280, 720, OF_WINDOW);
	ofRunApp(new testApp()); // start the app
}
