#include "testApp.h"
#include "ofAppGLFWWindow.h"
//--------------------------------------------------------------
int main(){

	ofAppGLFWWindow window;
#ifdef OCULUS_RIFT
#ifdef TARGET_OSX
	window.setMultiDisplayFullscreen(true);
#else
	window.setDoubleBuffering(false);
#endif
#endif

	ofSetupOpenGL(&window, 1280, 720, OF_WINDOW);
	ofRunApp(new testApp()); // start the app

}
