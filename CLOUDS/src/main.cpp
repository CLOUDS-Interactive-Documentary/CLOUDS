#include "testApp.h"
#include "ofAppGLFWWindow.h"
//--------------------------------------------------------------
int main(){

#ifdef OCULUS_RIFT
	ofAppGLFWWindow window;
	window.setMultiDisplayFullscreen(true);
	ofSetupOpenGL(&window, 1024, 768, OF_WINDOW);
#else
	ofSetupOpenGL(1024, 768, OF_WINDOW);
#endif
	ofRunApp(new testApp()); // start the app

}
