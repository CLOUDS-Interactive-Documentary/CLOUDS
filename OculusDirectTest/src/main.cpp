#include "testApp.h"
#include "ofAppGLFWWindow.h"

//--------------------------------------------------------------
int main(){
	//ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	ofAppGLFWWindow window;
	window.setDoubleBuffering(false);

	ofSetupOpenGL(&window, 1224, 768, OF_WINDOW);
	ofRunApp(new testApp()); // start the app
}
