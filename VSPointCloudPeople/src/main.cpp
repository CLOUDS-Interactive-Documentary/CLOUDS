#include "testApp.h"
#include "ofAppGLFWWindow.h"
//--------------------------------------------------------------
int main(){
	ofAppGLFWWindow win;
	#ifdef OCULUS_RIFT
	win.setDoubleBuffering(false);
	#endif
	ofSetupOpenGL(&win, 1224, 768, OF_WINDOW);
	ofRunApp(new testApp()); // start the app
}
    