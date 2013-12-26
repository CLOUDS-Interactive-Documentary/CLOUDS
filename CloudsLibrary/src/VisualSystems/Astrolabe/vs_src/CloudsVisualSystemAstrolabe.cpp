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
	
	ticksGui->addButton("reset", &bResetTicks);
	ticksGui->addSlider("delayOffsetScale", 0, 2, &ringsDelayOffsetScale );
	ticksGui->addSlider("noiseyness", 0, 1, &ringsNoiseyness );
	
	ticksGui->addSpacer();
	
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
	
	shaderGui = new ofxUISuperCanvas("SHADERGUI", gui);
	shaderGui->copyCanvasStyle(gui);
	shaderGui->copyCanvasProperties(gui);
	shaderGui->setName("ShaderGui");
	shaderGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	shaderGui->addToggle("depthTest", &bDepthTest);
	
	shaderGui->addSpacer();
	shaderGui->addIntSlider("c0_hue", 0, 255, &c0_hue);
	shaderGui->addIntSlider("c0_brightness", 0, 255, &c0_brightness);
	shaderGui->addIntSlider("c0_saturation", 0, 255, &c0_saturation);
	shaderGui->addIntSlider("c0_alpha", 0, 255, &c0_alpha);
	
	shaderGui->addSpacer();
	shaderGui->addIntSlider("c1_hue", 0, 255, &c1_hue);
	shaderGui->addIntSlider("c1_brightness", 0, 255, &c1_brightness);
	shaderGui->addIntSlider("c1_saturation", 0, 255, &c1_saturation);
	shaderGui->addIntSlider("c1_alpha", 0, 255, &c1_alpha);
	
	shaderGui->addSpacer();
	shaderGui->addIntSlider("c2_hue", 0, 255, &c2_hue);
	shaderGui->addIntSlider("c2_brightness", 0, 255, &c2_brightness);
	shaderGui->addIntSlider("c2_saturation", 0, 255, &c2_saturation);
	shaderGui->addIntSlider("c2_alpha", 0, 255, &c2_alpha);
	
	shaderGui->addSpacer();
	
	vector<string> blendTypes;
	blendTypes.push_back("OF_BLENDMODE_ADD");
	blendTypes.push_back("OF_BLENDMODE_ALPHA");
	blendTypes.push_back("OF_BLENDMODE_DISABLED");
	blendTypes.push_back("OF_BLENDMODE_MULTIPLY");
	blendTypes.push_back("OF_BLENDMODE_SCREEN");
	blendTypes.push_back("OF_BLENDMODE_SUBTRACT");
	
	shaderGui->addRadio("BlendMode", blendTypes);

	ofAddListener(shaderGui->newGUIEvent, this, &CloudsVisualSystemAstrolabe::selfGuiEvent);
	guis.push_back(shaderGui);
	guimap[shaderGui->getName()] = shaderGui;
	
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
	
//	cout << "parent: " + parentName << " : " << "name: " + name << endl;
	
	
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
	
	if(parentName == "ShaderGui")
	{
		if( name == "c0_hue" || name == "c0_brightness" || name == "c0_saturation" || name == "c0_alpha" )
		{
			c0.setHsb(c0_hue, c0_saturation, c0_brightness);
			c0.a = c0_alpha;
			
			setAstrolabesColors();
			
			shaderGui->getWidget("c0_hue")->setColorFill(c0);
			shaderGui->getWidget("c0_saturation")->setColorFill(c0);
			shaderGui->getWidget("c0_brightness")->setColorFill(c0);
			shaderGui->getWidget("c0_alpha")->setColorFill(c0);
		}
		else if( name == "c1_hue" || name == "c1_brightness" || name == "c1_saturation" || name == "c1_alpha" )
		{
			c1.setHsb(c1_hue, c1_saturation, c1_brightness);
			c1.a = c1_alpha;
			
			setAstrolabesColors();
			
			shaderGui->getWidget("c1_hue")->setColorFill(c1);
			shaderGui->getWidget("c1_saturation")->setColorFill(c1);
			shaderGui->getWidget("c1_brightness")->setColorFill(c1);
			shaderGui->getWidget("c1_alpha")->setColorFill(c1);
		}
		else if( name == "c2_hue" || name == "c2_brightness" || name == "c2_saturation" || name == "c2_alpha" )
		{
			c2.setHsb(c2_hue, c2_saturation, c2_brightness);
			c2.a = c2_alpha;
			
			setAstrolabesColors();
			
			shaderGui->getWidget("c2_hue")->setColorFill(c2);
			shaderGui->getWidget("c2_saturation")->setColorFill(c2);
			shaderGui->getWidget("c2_brightness")->setColorFill(c2);
			shaderGui->getWidget("c2_alpha")->setColorFill(c2);
		}
	}
	if(parentName == "BlendMode")
	{
		if(name == "OF_BLENDMODE_ADD" && e.getToggle()->getValue()){
			currentBlendMode = OF_BLENDMODE_ADD;
		}

		else if(name == "OF_BLENDMODE_ALPHA" && e.getToggle()->getValue()){
			currentBlendMode = OF_BLENDMODE_ALPHA;
		}

		else if(name == "OF_BLENDMODE_DISABLED" && e.getToggle()->getValue()){
			currentBlendMode = OF_BLENDMODE_DISABLED;
		}

		else if(name == "OF_BLENDMODE_MULTIPLY" && e.getToggle()->getValue()){
			currentBlendMode = OF_BLENDMODE_MULTIPLY;
		}

		else if(name == "OF_BLENDMODE_SCREEN" && e.getToggle()->getValue()){
			currentBlendMode = OF_BLENDMODE_SCREEN;
		}
		
		else if(name == "OF_BLENDMODE_SUBTRACT" && e.getToggle()->getValue()){
			currentBlendMode = OF_BLENDMODE_SUBTRACT;
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
		else if(name == "noiseyness" || name == "delayOffsetScale")
		{
			resetRingRotations();
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
//	colorMap.loadImage( GetCloudsDataPath() + "colors/defaultColorPalette.png");
	
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
	ringsDelayOffsetScale = .25;
	
	bResetTicks = false;
	
	bDepthTest = true;
	
	currentBlendMode= OF_BLENDMODE_ADD;
	
	bAutoReverseX = bAutoReverseY = bAutoReverseZ = false;
	xTickSpeed = yTickSpeed = zTickSpeed = 250;
	xTickDelay = yTickDelay = zTickDelay = 750;
	
	c0_hue = c1_hue = c2_hue = 255;
	c0_brightness = c1_brightness = c2_brightness = 255;
	c0_saturation = c1_saturation = c2_saturation = 255;
	c0_alpha = c1_alpha = c2_alpha = 255;
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
											 float hiRadians,
											 float initialOffsetScale
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
	}
	
	resetRingRotations();
	
	//set there colors
	setAstrolabesColors();
}

void CloudsVisualSystemAstrolabe::setAstrolabesColors()
{
	float colorStep = 2. / float(astrolabes.size() - 1);
	for(int i=0; i<astrolabes.size(); i++)
	{
		float cMix = colorStep * float(i);
		if( colorStep * i >= 1 ){
			//mix c1 & c0
			cMix -= 1.;
			astrolabes[i]->color = c2 * cMix + c1 * (1. - cMix);
		}
		else{
			//mix c0 & c1
			astrolabes[i]->color = c1 * cMix + c0 * (1. - cMix);
		}
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
	if(bSetupRings)
	{
		bSetupRings = false;
		
		//cout << "ringsCount: " << ringsCount << endl;
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
				   ringsHiRadians,
				   ringsDelayOffsetScale);
	}
	
	if(bResetTicks)
	{
		bResetTicks = false;
		
		resetRingRotations();
	}
	
	for(int i=0 ; i<astrolabes.size(); i++)
	{
		astrolabes[i]->updateRotations();
	}
}

void CloudsVisualSystemAstrolabe::resetRingRotations()
{
	for(int i=0 ; i<astrolabes.size(); i++)
	{
		//astrolabes[i]->addRotationTween( axis, startVal, step, duration, delay, increment)
		astrolabes[i]->addRotationTween("x", 0, ringsXRot, xTickSpeed, xTickDelay * i * ringsDelayOffsetScale, xTickDelay);
		astrolabes[i]->addRotationTween("y", i * ringsYRot, ringsYRot, yTickSpeed, xTickDelay * i * ringsDelayOffsetScale, yTickDelay);
		astrolabes[i]->addRotationTween("z", 0, ringsZRot, zTickSpeed, zTickDelay * i * ringsDelayOffsetScale, zTickSpeed);
		
		float scl = ofSignedNoise(i*.1) * ringsNoiseyness + 1.;
		astrolabes[i]->setTweenScale("x", scl );
		astrolabes[i]->setTweenScale("y", scl );
		astrolabes[i]->setTweenScale("z", scl );

	}
}

void CloudsVisualSystemAstrolabe::selfDraw()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
//	currentBlendMode != OF_BLENDMODE_DISABLED?	ofEnableBlendMode(currentBlendMode) : ofDisableAlphaBlending();
	
	ofEnableAlphaBlending();
	ofEnableBlendMode(currentBlendMode);
	
	bDepthTest? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	
	facingRatio.begin();
	for(int i=0 ; i<astrolabes.size(); i++)
	{
		astrolabes[i]->draw();
	}
	facingRatio.end();
	
	glDisable(GL_DEPTH_TEST);
	
	glDisable(GL_CULL_FACE);
	
	ofDisableAlphaBlending();
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