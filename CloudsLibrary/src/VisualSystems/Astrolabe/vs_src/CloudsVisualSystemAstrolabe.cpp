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
	ringsGui->addSlider("width", .01, 100, &ringsWidth);
	ringsGui->addSlider("thickness", .01, 10, &ringsThickness);
	ringsGui->addSlider("lowRadians", 0, TWO_PI, &ringsLowRadians);
	ringsGui->addSlider("hiRadians", 0, TWO_PI, &ringsHiRadians);
	ringsGui->addSlider("Spacing", 0, 20, &ringsSpacing );
	
	ofAddListener(ringsGui->newGUIEvent, this, &CloudsVisualSystemAstrolabe::selfGuiEvent);
	guis.push_back(ringsGui);
	guimap[ringsGui->getName()] = ringsGui;
	
	
	//tick behavior
	ticksGui = new ofxUISuperCanvas("TICKS", gui);
	ticksGui->copyCanvasStyle(gui);
	ticksGui->copyCanvasProperties(gui);
	ticksGui->setName("Ticks");
	ticksGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	ringsGui->addSpacer();
	
	ticksGui->addIntSlider("XRot", -180, 180, &ringsXRot )->setIncrement( 1 );
	ticksGui->addIntSlider("xTickSpeed", 10, 2000, &xTickSpeed )->setIncrement( 1 );
	ticksGui->addIntSlider("xTickDelay", 0, 2000, &xTickDelay )->setIncrement( 1 );
	ticksGui->addToggle("xReverse", &bAutoReverseX);
	
	ticksGui->addSpacer();
	
	ticksGui->addIntSlider("YRot", -180, 180, &ringsYRot )->setIncrement( 1 );
	ticksGui->addIntSlider("yTickSpeed", 10, 2000, &yTickSpeed )->setIncrement( 1 );
	ticksGui->addIntSlider("yTickDelay", 0, 2000, &yTickDelay )->setIncrement( 1 );
	ticksGui->addToggle("yReverse", &bAutoReverseY);
	
	ticksGui->addSpacer();
	
	ticksGui->addIntSlider("ZRot", -180, 180, &ringsZRot )->setIncrement( 1 );
	ticksGui->addIntSlider("zTickSpeed", 10, 2000, &zTickSpeed )->setIncrement( 1 );
	ticksGui->addIntSlider("zTickDelay", 0, 2000, &zTickDelay )->setIncrement( 1 );
	ticksGui->addToggle("zReverse", &bAutoReverseZ);
	
	ofAddListener(ticksGui->newGUIEvent, this, &CloudsVisualSystemAstrolabe::selfGuiEvent);
	guis.push_back(ticksGui);
	guimap[ticksGui->getName()] = ticksGui;
	
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
	
	
	//RINGS
	if(parentName == "Rings")
	{
		if( name == "count" )
		{
			bSetupRings = true;
		}
		else if( name == "innerRadius" || name == "width" || name == "thickness" || name == "Spacing" )
		{
			float innerRad = ringsInnerRad;
			for (int i=0; i<astrolabes.size(); i++)
			{
				float outerRad = innerRad + ringsWidth;
				astrolabes[i]->setupMesh( ringsLowRadians, ringsHiRadians, innerRad, outerRad, ringsThickness );
				innerRad = outerRad + ringsSpacing;
			}
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
	
	//TICKS
	else if(parentName == "Ticks")
	{
		if( name == "XRot" || name == "YRot" || name == "ZRot" )
		{
			for(int i=0 ; i<astrolabes.size(); i++)
			{
				//		astrolabes[i]->addRotationTween(string axis, float startVal, float step, float duration, float delay, float increment)
				astrolabes[i]->addRotationTween("x", i * ringsXRot, ringsXRot, xTickSpeed, xTickDelay, xTickDelay);
				astrolabes[i]->addRotationTween("y", i * ringsYRot, ringsYRot, yTickSpeed, xTickDelay, yTickDelay);
				astrolabes[i]->addRotationTween("z", i * ringsZRot, ringsZRot, zTickSpeed, zTickDelay, zTickSpeed);
				
				float scl = ofSignedNoise(i*.1) * .75 + 1.;
				astrolabes[i]->setTweenScale("x", scl );
				astrolabes[i]->setTweenScale("y", scl );
				astrolabes[i]->setTweenScale("z", scl );
			}
		}
		else if( name == "xTickSpeed" || name == "yTickSpeed" || name == "zTickSpeed" )
		{
			for (int i=0; i<astrolabes.size(); i++)
			{
				astrolabes[i]->setTweenIncrement("x", xTickSpeed);
				astrolabes[i]->setTweenIncrement("y", yTickSpeed);
				astrolabes[i]->setTweenIncrement("z", zTickSpeed);
			}
		}
		else if( name == "xTickDelay" || name == "yTickDelay" || name == "zTickDelay" )
		{
			for (int i=0; i<astrolabes.size(); i++)
			{
				astrolabes[i]->setTweenDelay("x", xTickDelay);
				astrolabes[i]->setTweenDelay("y", yTickDelay);
				astrolabes[i]->setTweenDelay("z", zTickDelay);
			}
		}
		else if(name == "xReverse" || name == "yReverse" || name == "zReverse")
		{
			for (int i=0; i<astrolabes.size(); i++)
			{
				astrolabes[i]->setTweenReverse("x", bAutoReverseX);
				astrolabes[i]->setTweenReverse("y", bAutoReverseY);
				astrolabes[i]->setTweenReverse("z", bAutoReverseZ);
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
	ringsXRot = 0;
	ringsYRot = 0;
	ringsZRot = 0;
	ringsSpacing = 2;
	ringsMinTickSpeed = 100;
	ringsLowRadians = 0;
	ringsHiRadians = TWO_PI;
	
	bAutoReverseX = bAutoReverseY = bAutoReverseZ = false;
	xTickSpeed = yTickSpeed = zTickSpeed = 250;
	xTickDelay = yTickDelay = zTickDelay = 750;
}

void CloudsVisualSystemAstrolabe::setupRings(int count,
											 float innerRad,
											 float width,
											 float thickness,
											 float xRot,
											 float yRot,
											 float zRot,
											 float spacing,
											 float xTickSpeed,
											 float lowRadians,
											 float hiRadians
											 )
{
	clearAstrolabes();
	
	astrolabes.resize(count);
	
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
		
//		astrolabes[i]->addRotationTween(string axis, float startVal, float step, float duration, float delay, float increment)
		astrolabes[i]->addRotationTween("x", i * ringsXRot, ringsXRot, xTickSpeed, xTickDelay, xTickDelay);
		astrolabes[i]->addRotationTween("y", i * ringsYRot, ringsYRot, yTickSpeed, xTickDelay, yTickDelay);
		astrolabes[i]->addRotationTween("z", i * ringsZRot, ringsZRot, zTickSpeed, zTickDelay, zTickSpeed);
		
		float scl = ofSignedNoise(i*.1) * .75 + 1.;
		astrolabes[i]->setTweenScale("x", scl );
		astrolabes[i]->setTweenScale("y", scl );
		astrolabes[i]->setTweenScale("z", scl );

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
//			temp->addRotationTween("y", yRot * i , step, xTickSpeed, xTickSpeed, xTickSpeed);
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