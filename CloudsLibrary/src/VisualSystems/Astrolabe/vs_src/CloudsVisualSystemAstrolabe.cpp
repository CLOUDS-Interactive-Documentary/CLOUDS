//
//  CloudsVisualSystemAstrolabe.cpp
//

#include "CloudsVisualSystemAstrolabe.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemAstrolabe::selfSetupGui(){

	customGui = new ofxUISuperCanvas("CUSTOM", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Custom");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemAstrolabe::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemAstrolabe::selfGuiEvent(ofxUIEventArgs &e)
{
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
}


void CloudsVisualSystemAstrolabe::selfSetupSystemGui()
{
	
}

void CloudsVisualSystemAstrolabe::guiSystemEvent(ofxUIEventArgs &e)
{
	
}

void CloudsVisualSystemAstrolabe::selfSetupRenderGui()
{

}

void CloudsVisualSystemAstrolabe::guiRenderEvent(ofxUIEventArgs &e){
	
}


void CloudsVisualSystemAstrolabe::selfSetup()
{	
//	someImage.loadImage( getVisualSystemDataPath() + "images/someImage.png";
	
	loadShaders();
	
	setupClock();
	
	
}

void CloudsVisualSystemAstrolabe::setupClock(	 int count,
											 float innerRad,
											 float width,
											 float thickness,
											 float xRot,
											 float yRot,
											 float zRot,
											 float spacing,
											 float minTickSpeed
											 )
{
	clearAstrolabes();
	
	astrolabes.resize(count);
	float step = yRot;// 360. / astrolabes.size();
	
	vector<Astrolabe*> fillerAstrolabes;
	for(int i=0 ; i<astrolabes.size(); i++)
	{
		astrolabes[i] = new Astrolabe();
		float innerRadius = innerRad;
		float outerRadius = innerRadius + width;// * (i+1);
		innerRad = outerRadius + 2;
		
		float radianOffset = ofRandom(-3.14, 3.14);
		float lowRadian = ofRandom(-1, 0) * PI;// + radianOffset;
		float hiRadian = ofRandom( 0, 1) * PI;// + radianOffset;
		
		astrolabes[i]->setupMesh(lowRadian, hiRadian, innerRadius, outerRadius, thickness );
		
		float tickSpeed = minTickSpeed + 25 * i * i;
		
		astrolabes[i]->addRotationTween("x", xRot * i  , 90, 1000, 2000 * i, 8000);
		astrolabes[i]->addRotationTween("y", yRot * i , step, tickSpeed, tickSpeed, tickSpeed);
		astrolabes[i]->addRotationTween("z", zRot * i , 180, 2000, 500 * i, 4000);
		
		astrolabes[i]->color.setSaturation( 255 );
		astrolabes[i]->color.setBrightness( 200 );
		astrolabes[i]->color.setHue( int(ofRandom(25)) * 10 );
		
		float dist = abs(lowRadian + TWO_PI - hiRadian);
		if( dist > 3)
		{
			Astrolabe* temp = new Astrolabe();
			temp->setupMesh(hiRadian, hiRadian + dist, innerRadius, outerRadius, thickness * 3. );
			
			temp->addRotationTween("x", xRot * i  , 90, 1000, 2000 * i, 8000);
			temp->addRotationTween("y", yRot * i , step, tickSpeed, tickSpeed, tickSpeed);
			temp->addRotationTween("z", zRot * i , 180, 2000, 500 * i, 4000);
			
			temp->color = astrolabes[i]->color;
			temp->color.setSaturation( 100 );
			fillerAstrolabes.push_back(temp);
		}
	}
	
	for (int i=0; i<fillerAstrolabes.size(); i++)
	{
		astrolabes.push_back(fillerAstrolabes[i]);
	}
}

void CloudsVisualSystemAstrolabe::loadShaders()
{
	unloadShaders();
	
	facingRatio.load( getVisualSystemDataPath() + "shaders/facingRatio");
	normalShader.load( getVisualSystemDataPath() + "shaders/normalShader");
}
void CloudsVisualSystemAstrolabe::unloadShaders()
{
	facingRatio.unload();
	normalShader.unload();
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemAstrolabe::selfPresetLoaded(string presetPath)
{
	
}


void CloudsVisualSystemAstrolabe::selfBegin()
{
	
}

void CloudsVisualSystemAstrolabe::selfSceneTransformation()
{
	
}


void CloudsVisualSystemAstrolabe::selfUpdate()
{
	
	for(int i=0 ; i<astrolabes.size(); i++)
	{
		astrolabes[i]->updateRotations();
	}
}

void CloudsVisualSystemAstrolabe::selfDraw()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	facingRatio.begin();
	for(int i=0 ; i<astrolabes.size(); i++)
	{
		astrolabes[i]->draw();
	}
	facingRatio.end();
	
	glDisable(GL_CULL_FACE);
}

// draw any debug stuff here
void CloudsVisualSystemAstrolabe::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemAstrolabe::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemAstrolabe::selfEnd(){
	
//	simplePointcloud.clear();
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemAstrolabe::selfExit()
{
	clearAstrolabes();
}

void CloudsVisualSystemAstrolabe::clearAstrolabes()
{
	for(int i=astrolabes.size()-1; i>=0; i--)
	{
		astrolabes[i]->clear();
	}
	
	astrolabes.clear();
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemAstrolabe::selfKeyPressed(ofKeyEventArgs & args)
{
	if(args.key == 'l' || args.key == 'L')
	{
		loadShaders();
	}
}
void CloudsVisualSystemAstrolabe::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemAstrolabe::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemAstrolabe::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemAstrolabe::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemAstrolabe::selfMouseReleased(ofMouseEventArgs& data){
	
}