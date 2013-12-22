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

	ringsGui = new ofxUISuperCanvas("RINGS", gui);
	ringsGui->copyCanvasStyle(gui);
	ringsGui->copyCanvasProperties(gui);
	ringsGui->setName("Rings");
	ringsGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	ringsGui->addIntSlider("count", 1, 30, &ringsCount);
	ringsGui->addSlider("innerRadius", .1, 10, &ringsInnerRad);
	ringsGui->addSlider("width", .01, 10, &ringsWidth);
	ringsGui->addSlider("thickness", .01, 10, &ringsThickness);
	ringsGui->addSlider("lowRadians", 0, TWO_PI, &ringsLowRadians);
	ringsGui->addSlider("hiRadians", 0, TWO_PI, &ringsHiRadians);
	ringsGui->addSlider("XRot", -180, 180, &ringsXRot )->setIncrement( 1 );;
	ringsGui->addSlider("YRot", -180, 180, &ringsYRot )->setIncrement( 1 );;
	ringsGui->addSlider("ZRot", -180, 180, &ringsZRot )->setIncrement( 1 );;
	ringsGui->addSlider("Spacing", 0, 20, &ringsSpacing );
	ringsGui->addSlider("MinTickSpeed", 10, 10000, &ringsMinTickSpeed )->setIncrement( 1 );
	
	ofAddListener(ringsGui->newGUIEvent, this, &CloudsVisualSystemAstrolabe::selfGuiEvent);
	guis.push_back(ringsGui);
	guimap[ringsGui->getName()] = ringsGui;
	
//	customGui = new ofxUISuperCanvas("CUSTOM", gui);
//	customGui->copyCanvasStyle(gui);
//	customGui->copyCanvasProperties(gui);
//	customGui->setName("Custom");
//	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
//	
//	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemAstrolabe::selfGuiEvent);
//	guis.push_back(customGui);
//	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemAstrolabe::selfGuiEvent(ofxUIEventArgs &e)
{
//	if(e.widget->getName() == "Custom Button"){
//		cout << "Button pressed!" << endl;
//	}
	string name = e.widget->getName();
	string parentName = e.widget->getParent()->getName();
	
	cout << "parent: " + parentName << " : " << "name: " + name << endl;
	
	if(parentName == "Rings")
	{
		if(parentName == "Rings")
		{
			if( name == "count" )
			{
				bSetupRings = true;
			}
			else if( name == "innerRadius" )
			{
				bSetupRings = true;
			}
			else if( name == "width" )
			{
				bSetupRings = true;
			}
			else if( name == "thickness" )
			{
				bSetupRings = true;
			}
			else if( name == "XRot" )
			{
				bSetupRings = true;
			}
			else if( name == "YRot" )
			{
				bSetupRings = true;
			}
			else if( name == "ZRot" )
			{
				bSetupRings = true;
			}
			else if( name == "Spacing" )
			{
				bSetupRings = true;
			}
			else if( name == "MinTickSpeed" )
			{
				bSetupRings = true;
			}
			else if( name == "lowRadians" )
			{
				bSetupRings = true;
			}
			else if( name == "hiRadians" )
			{
				bSetupRings = true;
			}
		}
	}
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
	
	//defaults
	ringsCount = 15;
	ringsInnerRad = 5;
	ringsWidth = 40;
	ringsThickness = 10;
	ringsXRot = 45;
	ringsYRot = 40;
	ringsZRot = 60;
	ringsSpacing = 2;
	ringsMinTickSpeed = 100;
	ringsLowRadians = 0;
	ringsHiRadians = TWO_PI;
}

void CloudsVisualSystemAstrolabe::setupRings(int count,
											 float innerRad,
											 float width,
											 float thickness,
											 float xRot,
											 float yRot,
											 float zRot,
											 float spacing,
											 float minTickSpeed,
											 float lowRadians,
											 float hiRadians
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
		innerRad = outerRadius + spacing;
		
		float radianOffset = 0;// ofRandom(-3.14, 3.14);
		float lowRadian = lowRadians + radianOffset;
		float hiRadian = hiRadians + radianOffset;
		
		astrolabes[i]->setupMesh(lowRadian, hiRadian, innerRadius, outerRadius, thickness );
		
		float tickSpeed = minTickSpeed + minTickSpeed * i;
		
		astrolabes[i]->addRotationTween("x", xRot * i  , 90, 1000, 2000 * i, 8000);
		astrolabes[i]->addRotationTween("y", yRot * i , step, tickSpeed, tickSpeed, tickSpeed);
		astrolabes[i]->addRotationTween("z", zRot * i , 180, 2000, 500 * i, 4000);

//		int randColorIndex = ofRandom(4);
//		if(randColorIndex == 0)	astrolabes[i]->color.set(255,255,0,255);
//		if(randColorIndex == 1)	astrolabes[i]->color.set(0,255,255,255);
//		if(randColorIndex == 2)	astrolabes[i]->color.set(255,0,255,255);
//		
//		float dist = abs(lowRadian + TWO_PI - hiRadian);
//		if( dist > 3)
//		{
//			Astrolabe* temp = new Astrolabe();
//			temp->setupMesh(hiRadian, hiRadian + dist, innerRadius, outerRadius, thickness * 3. );
//			
//			temp->addRotationTween("x", xRot * i  , 90, 1000, 2000 * i, 8000);
//			temp->addRotationTween("y", yRot * i , step, tickSpeed, tickSpeed, tickSpeed);
//			temp->addRotationTween("z", zRot * i , 180, 2000, 500 * i, 4000);
//			
//			temp->color = astrolabes[i]->color;
//			temp->color.setSaturation( 200 );
//			fillerAstrolabes.push_back(temp);
//		}
	}
	
//	for (int i=0; i<fillerAstrolabes.size(); i++)
//	{
//		astrolabes.push_back(fillerAstrolabes[i]);
//	}
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
	if(bSetupRings)
	{
		bSetupRings = false;
		
		cout << "ringsCount: " << ringsCount << endl;
		setupRings(ringsCount,
				   ringsInnerRad,
				   ringsWidth,
				   ringsThickness,
				   ringsXRot,
				   ringsYRot,
				   ringsZRot,
				   ringsSpacing,
				   ringsMinTickSpeed,
				   ringsLowRadians,
				   ringsHiRadians);
	}
	
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