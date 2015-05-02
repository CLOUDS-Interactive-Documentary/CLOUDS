#include "testApp.h"
#include "ofAppGLFWWindow.h"
//--------------------------------------------------------------
int main(){

	ofAppGLFWWindow window;
#ifdef OCULUS_RIFT
	window.setMultiDisplayFullscreen(true);
	ofSetupOpenGL(&window, 1920, 1080, OF_WINDOW);
#else
	ofSetupOpenGL(&window, 1920, 1080, OF_WINDOW);
#endif
	ofRunApp(new testApp()); // start the app

}
