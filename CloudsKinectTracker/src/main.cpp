#include "ofMain.h"
#include "testApp.h"

#include "ofGLProgrammableRenderer.h"

#define USE_PROGRAMMABLE_RENDERER

//========================================================================
int main() 
{
#ifdef USE_PROGRAMMABLE_RENDERER
	ofPtr<ofBaseRenderer> renderer(new ofGLProgrammableRenderer(true));
	ofSetCurrentRenderer(renderer);
#endif

	ofSetupOpenGL(1024, 768, OF_WINDOW);
	ofRunApp(new testApp());
}
