#include "testApp.h"
#include "ofAppGLFWWindow.h"

//--------------------------------------------------------------
int main(){
	ofAppGLFWWindow win;
	win.setDoubleBuffering(false);
	ofSetupOpenGL(&win, 1224, 768, OF_WINDOW);
	ofRunApp(new testApp()); // start the app
}
    