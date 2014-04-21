//
//  CloudsVisualSystemBalloons.cpp
//

#include "CloudsVisualSystemBalloons.h"
#include "CloudsRGBDVideoPlayer.h"

void CloudsVisualSystemBalloons::selfSetupGui(){
	
	//balloon behavior
	customGui = new ofxUISuperCanvas("BALLOONS_BEHAVIOR", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("Balloons");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	customGui->addSlider("noiseScl", .01, 1, &noiseScl)->setIncrement(.001);
	customGui->addSlider("noiseSampleScale", .001, .1, &noiseSampleScale)->setIncrement(.001);
	customGui->addSlider("velocityAttenuation", .95, 1., &velAtten)->setIncrement(.001);
	customGui->addSlider("accelScale", 0, 1, &accScl);
	customGui->addSlider("gravity", 0, .1, &gravity);
	customGui->addSlider("attractionToCenter", 0, .1, &attractionToCenter);
	
	customGui->addSpacer();
	customGui->addSlider("highSpeedPercent", 0, 1, &highSpeedPercent);
	customGui->addSlider("highSpeedScale", 1, 3, &highSpeedScale);
	customGui->addSlider("speedScaleLow", 0.1, 1, &speedLow);
	customGui->addSlider("speedScaleHi", 1, 2, &speedHi);
	
	customGui->addSpacer();
	customGui->addSlider("cameraBounce", 0, 20, &cameraBounce);
	customGui->addSlider("cameraTargetDist", 20, 500, &cameraTargetDist);
	customGui->addSlider("balloonFrameVal", 0, 1, &balloonFrameVal);
	
	customGui->addSlider("dim", 100, 500, &dim );
	
	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemBalloons::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
	
	//TEXT
	textGui = new ofxUISuperCanvas("BALLOONS_TEXT", gui);
	textGui->copyCanvasStyle(gui);
	textGui->copyCanvasProperties(gui);
	textGui->setName("Text");
	textGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	textGui->addSlider("textSpeed", -10, 10, &textSpeed)->setIncrement(.001);
	textGui->addSlider("textRadius", 1, 10, &textRadius);
	textGui->addSlider("creditLightDist", 10, 500, &creditLightDist);
	
	textGui->addSpacer();
	textGui->addIntSlider("shininess", 2, 128, &shininess);
	textGui->addSlider("lightScale", 0, 1, &lightScale);
	textGui->addSlider("creditLightScale", 0, 1, &creditLightScale);
	textGui->addSlider("facingRatioScale", 0, 1, &facingRatioScale);
	
	ofAddListener(textGui->newGUIEvent, this, &CloudsVisualSystemBalloons::selfGuiEvent);
	guis.push_back(textGui);
	guimap[textGui->getName()] = textGui;
	
	//Balloon Release
	balloonReleaseGui = new ofxUISuperCanvas("BALLOONS_RELEASE", gui);
	balloonReleaseGui->copyCanvasStyle(gui);
	balloonReleaseGui->copyCanvasProperties(gui);
	balloonReleaseGui->setName("BalloonsRelease");
	balloonReleaseGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	balloonReleaseGui->addSlider("spawnRad", 20, 300, &spawnRad );
	balloonReleaseGui->addButton("release", false);
	balloonReleaseGui->addButton("reset", false);
	
	balloonReleaseGui->addSpacer();
	vector<string> releaseTypes;
	releaseTypes.push_back("sphereVolume");
	releaseTypes.push_back("plane");
	releaseTypes.push_back("cos");
	releaseTypes.push_back("bulge");
	balloonReleaseGui->addRadio("releaseTypes", releaseTypes);
	
	ofAddListener(balloonReleaseGui->newGUIEvent, this, &CloudsVisualSystemBalloons::selfGuiEvent);
	guis.push_back(balloonReleaseGui);
	guimap[balloonReleaseGui->getName()] = balloonReleaseGui;
	
	//Balloon Colors
	colorGui = new ofxUISuperCanvas("BALLOONS_COLOR", gui);
	colorGui->copyCanvasStyle(gui);
	colorGui->copyCanvasProperties(gui);
	colorGui->setName("BalloonsColor");
	colorGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	colorGui->addSpacer();
	colorGui->addLabel("c0")->setColorFill(c0);
	colorGui->addIntSlider("c0Hue", 0, 255, &c0Hue)->setColorFill(c0);
	colorGui->addIntSlider("c0Saturation", 0, 255, &c0Sat)->setColorFill(c0);
	colorGui->addIntSlider("c0Brightness", 0, 255, &c0Bri)->setColorFill(c0);
	colorGui->addMinimalSlider("w0", 0, 1, &w0);
	
	colorGui->addSpacer();
	colorGui->addLabel("c1")->setColorFill(c1);
	colorGui->addIntSlider("c1Hue", 0, 255, &c1Hue)->setColorFill(c1);
	colorGui->addIntSlider("c1Saturation", 0, 255, &c1Sat)->setColorFill(c1);
	colorGui->addIntSlider("c1Brightness", 0, 255, &c1Bri)->setColorFill(c1);
	colorGui->addMinimalSlider("w1", 0, 1, &w1);
	
	colorGui->addSpacer();
	colorGui->addLabel("c2")->setColorFill(c2);
	colorGui->addIntSlider("c2Hue", 0, 255, &c2Hue)->setColorFill(c2);
	colorGui->addIntSlider("c2Saturation", 0, 255, &c2Sat)->setColorFill(c2);
	colorGui->addIntSlider("c2Brightness", 0, 255, &c2Bri)->setColorFill(c2);
	colorGui->addMinimalSlider("w2", 0, 1, &w2);
	
	colorGui->addSpacer();
	colorGui->addLabel("c3")->setColorFill(c3);
	colorGui->addIntSlider("c3Hue", 0, 255, &c3Hue)->setColorFill(c3);
	colorGui->addIntSlider("c3Saturation", 0, 255, &c3Sat)->setColorFill(c3);
	colorGui->addIntSlider("c3Brightness", 0, 255, &c3Bri)->setColorFill(c3);
	colorGui->addMinimalSlider("w3", 0, 1, &w3);
	
	ofAddListener(colorGui->newGUIEvent, this, &CloudsVisualSystemBalloons::selfGuiEvent);
	guis.push_back(colorGui);
	guimap[customGui->getName()] = colorGui;
}

void CloudsVisualSystemBalloons::selfGuiEvent(ofxUIEventArgs &e)
{
	string name = e.getName();
	
	if( name == "c0Hue"|| name == "c0Saturation"|| name == "c0Brightness" || name == "w0")
	{
		c0.setHue(c0Hue);
		c0.setSaturation(c0Sat);
		c0.setBrightness(c0Bri);
		
		colorGui->getWidget("c0Hue")->setColorFill(c0);
		colorGui->getWidget("c0Saturation")->setColorFill(c0);
		colorGui->getWidget("c0Brightness")->setColorFill(c0);
		
		setBalloonColors();
	}
	else if( name == "c1Hue"|| name == "c1Saturation"|| name == "c1Brightness" || name == "w1")
	{
		c1.setHue(c1Hue);
		c1.setSaturation(c1Sat);
		c1.setBrightness(c1Bri);
		
		colorGui->getWidget("c1Hue")->setColorFill(c1);
		colorGui->getWidget("c1Saturation")->setColorFill(c1);
		colorGui->getWidget("c1Brightness")->setColorFill(c1);
		
		setBalloonColors();
	}
	else if( name == "c2Hue"|| name == "c2Saturation"|| name == "c2Brightness" || name == "w2")
	{
		c2.setHue(c2Hue);
		c2.setSaturation(c2Sat);
		c2.setBrightness(c2Bri);
		
		colorGui->getWidget("c2Hue")->setColorFill(c2);
		colorGui->getWidget("c2Saturation")->setColorFill(c2);
		colorGui->getWidget("c2Brightness")->setColorFill(c2);
		
		setBalloonColors();
	}
	else if( name == "c3Hue"|| name == "c3Saturation"|| name == "c3Brightness" || name == "w3")
	{
		c3.setHue(c3Hue);
		c3.setSaturation(c3Sat);
		c3.setBrightness(c3Bri);
		
		colorGui->getWidget("c3Hue")->setColorFill(c3);
		colorGui->getWidget("c3Saturation")->setColorFill(c3);
		colorGui->getWidget("c3Brightness")->setColorFill(c3);
		
		setBalloonColors();
	}
	else if(name == "reset")
	{
		bReleased = false;
		setBalloonPositions();
	}
	
	else if(name == "release")
	{
		bReleased = true;
	}
	
	else if(name == "sphereVolume")
	{
		releaseType = 0;
		bReleased = false;
		setBalloonPositions();
	}
	
	else if(name == "plane")
	{
		releaseType = 1;
		bReleased = false;
		setBalloonPositions();
	}
	
	else if(name == "cos")
	{
		releaseType = 2;
		bReleased = false;
		setBalloonPositions();
	}
	
	
	else if(name == "bulge")
	{
		releaseType = 3;
		bReleased = false;
		setBalloonPositions();
	}
}

void CloudsVisualSystemBalloons::selfSetupSystemGui(){
	
}

void CloudsVisualSystemBalloons::guiSystemEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemBalloons::selfSetupRenderGui(){

}

void CloudsVisualSystemBalloons::guiRenderEvent(ofxUIEventArgs &e){
	
}

void CloudsVisualSystemBalloons::selfSetDefaults()
{
	noiseScl = .5;
	offset = .1;
	noiseSampleScale = .02;
	velAtten = .98;
	radius = 15;
	accScl = .2;
	gravity = .01;
	attractionToCenter = .01;
	
	cameraBounce = 10.;
	cameraAttractionToCenter = 1.;
	cameraTargetDist = 200;
	
	dim = 250;
	spawnRad = 150;
	releaseType = 1;
	
	//make data
	dimX = 64;
	dimY = 64;
	
	//colors
	c0.set(46,169,235);
	c1.set(224,181,51);
	c2.set(224,105,19);
	c3.set(205,0,0);
	
	c0Hue = c0.getHue();
	c0Bri = c0.getBrightness();
	c0Sat = c0.getSaturation();
	
	c1Hue = c1.getHue();
	c1Bri = c1.getBrightness();
	c1Sat = c1.getSaturation();
	
	c2Hue = c2.getHue();
	c2Bri = c2.getBrightness();
	c2Sat = c2.getSaturation();
	
	c3Hue = c3.getHue();
	c3Bri = c3.getBrightness();
	c3Sat = c3.getSaturation();
	
	w0 = 1;
	w1 = .2;
	w2 = .2;
	w3 = .2;
	
	l0.set(dim*.25, dim, 0);
	
	highSpeedScale= 1.4;
	speedLow = .8;
	speedHi = 1.5;
	highSpeedPercent = .1;
	
	balloonFrameVal = 1.;
	
	bReleased = false;
	
	line0.set(-30, 0,0);
	line1.set(30, 0,0);
	
	textSpeed = -1;
	textRadius = 3.;
	
	shininess = 10;
	lightScale = .75;
	creditLightScale = .75;
	facingRatioScale = .4;
}

void CloudsVisualSystemBalloons::setBalloonColors()
{
	vector<ofVec3f>col(dimY*dimX);
	
	float sumWeights = w0 + w1 + w2 + w3, w;
	
	ofFloatColor c0f = c0;
	ofFloatColor c1f = c1;
	ofFloatColor c2f = c2;
	ofFloatColor c3f = c3;
	
	float step = sumWeights / col.size();
	for(int i=0; i<col.size(); i++)
	{
		w = step * i;
		
		if(w < w0)				col[i].set(c0f.r, c0f.g, c0f.b );
		else if(w < w0+w1)		col[i].set(c1f.r, c1f.g, c1f.b );
		else if(w < w0+w1+w2)	col[i].set(c2f.r, c2f.g, c2f.b );
		else					col[i].set(c3f.r, c3f.g, c3f.b );
	}
	
	colFbo.allocate(dimX, dimY, GL_RGB16F);
	colFbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    colFbo.getTextureReference().loadData( &col[0][0], dimX, dimY, GL_RGB);
}

void CloudsVisualSystemBalloons::setBalloonPositions()
{
	vector<ofVec3f>pos(dimY*dimX);
	vector<ofVec3f>vel(dimY*dimX);
	
	if(releaseType == 0)
	{
		//sphere volume
		for (int i=0; i<dimY; i++)
		{
			for(int j=0; j<dimX; j++)
			{
				pos[i*dimX + j] = randomPointInSphere(spawnRad, ofVec3f(0,-spawnRad, 0) );
			}
		}
	}
	else if(releaseType == 1)
	{
		for (int i=0; i<dimY; i++)
		{
			for(int j=0; j<dimX; j++)
			{
				//PLANE
				pos[i*dimX + j].set( j - dimY*.5, -10, i - dimY*.5);
				pos[i*dimX + j] *= radius * .5;
			}
		}
	}
	//cos
	else if(releaseType == 2)
	{
		float iStep = TWO_PI / dimY;
		float jStep = TWO_PI / dimX;
		for (int i=0; i<dimY; i++)
		{
			for(int j=0; j<dimX; j++)
			{
				pos[i*dimX + j].set( j - dimY*.5, -10 + cos(j * jStep) * cos(i * iStep) * 30, i - dimY*.5);
				pos[i*dimX + j] *= radius;
			}
		}
	}
	
	//Bulge
	else if(releaseType == 3)
	{
		float iStep = PI / dimY;
		float jStep = PI / dimX;
		for (int i=0; i<dimY; i++)
		{
			for(int j=0; j<dimX; j++)
			{
				pos[i*dimX + j].set( j - dimY*.5, -10 + sin(j * jStep) * sin(i * iStep) * 15, i - dimY*.5);
				pos[i*dimX + j] *= radius;
			}
		}
	}
	
	random_shuffle( pos.begin(), pos.end() );
	
	for(int i=0; i<vel.size(); i++)
	{
//		vel[i].set(0,ofRandom(-1.5, 3),0);
		vel[i].set(0,ofRandom(0, 1),0);
	}
	
	//store data
	//posiiton
	posFbo0.allocate(dimX, dimY, GL_RGB32F);
	posFbo0.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    posFbo0.getTextureReference().loadData( &pos[0][0], dimX, dimY, GL_RGB);
	
	posFbo1.allocate(dimX, dimY, GL_RGB32F);
	posFbo1.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    posFbo1.getTextureReference().loadData( &pos[0][0], dimX, dimY, GL_RGB);
	
	//velocity
	velFbo0.allocate(dimX, dimY, GL_RGB32F);
	velFbo0.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    velFbo0.getTextureReference().loadData( &vel[0][0], dimX, dimY, GL_RGB);
	
	velFbo1.allocate(dimX, dimY, GL_RGB32F);
	velFbo1.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    velFbo1.getTextureReference().loadData( &vel[0][0], dimX, dimY, GL_RGB);
	
	//rotations
	quatFbo.allocate(dimX, dimY, GL_RGBA32F);
	quatFbo.getTextureReference().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	
	//color
	setBalloonColors();
}

void CloudsVisualSystemBalloons::selfSetup()
{
	//make our ballons
	setBalloonPositions();
	
	p0 = &posFbo0;
	p1 = &posFbo1;
	
	v0 = &velFbo0;
	v1 = &velFbo1;

	//load balloon mesh
	ofMesh temp;
//	ofxObjLoader::load( getVisualSystemDataPath() + "models/box.obj", temp);
//	ofxObjLoader::load( getVisualSystemDataPath() + "models/balloon_low.obj", temp);
	ofxObjLoader::load( getVisualSystemDataPath() + "models/balloon_mid.obj", temp);
//	ofxObjLoader::load( getVisualSystemDataPath() + "models/balloon.obj", temp);
		
	vector<ofVec3f>& v = temp.getVertices();
	vector<ofVec3f>& n = temp.getNormals();
	
	total = v.size();

	vbo.setVertexData(&v[0], v.size(), GL_STATIC_DRAW);
	vbo.setNormalData(&n[0], n.size(), GL_STATIC_DRAW);
	
	shader.load(getVisualSystemDataPath() + "shaders/normalShader");
	posShader.load(getVisualSystemDataPath() + "shaders/posShader");
	velShader.load(getVisualSystemDataPath() + "shaders/velShader");
	quatShader.load(getVisualSystemDataPath() + "shaders/quatShader");
	
	
	for(int i=0; i<100; i++)
	{
		ofVec3f pos( 0, i * dim, 0);
		Credit c("title", "name", pos);
		
		credits.push_back(c);
	}
}

void CloudsVisualSystemBalloons::selfPresetLoaded(string presetPath){
	
}

void CloudsVisualSystemBalloons::selfBegin()
{
	
}

void CloudsVisualSystemBalloons::selfSceneTransformation(){
	
}

void CloudsVisualSystemBalloons::selfUpdate()
{
	p0->getTextureReference().readToPixels(pospix);
	ofFloatColor poscol = pospix.getColor(0,0);
	balloon00Pos.set(poscol.r,poscol.g,poscol.b);
	
	balloon00Pos = mix(balloon00Pos, ofVec3f(0,0,0), balloonFrameVal);
	
//	getCameraRef().setPosition(balloon00Pos.x, 0, cameraTargetDist + balloon00Pos.z);
//	getCameraRef().setPosition(0, 0, cameraTargetDist);
	//getCameraRef().lookAt(ofVec3f(balloon00Pos));
	
	//words dropping down
//	line0.y = line1.y = ofMap(fmod(ofGetElapsedTimef() * 40, dim), 0, dim, dim, -dim, true);
	
	line0.y += textSpeed;
	
	if(line0.y > dim)
	{
		line0.y = -dim;
	}else if(line0.y <-dim)
	{
		line0.y = dim;
	}
	
	line1.y = line0.y;
	
	for(auto& c: credits)
	{
		c.pos.y += textSpeed;
	}
}

void CloudsVisualSystemBalloons::selfDraw()
{
	ofPushStyle();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	if(bReleased)
	{
		//update positions
		p0->begin();
		ofClear(0, 255);
		posShader.begin();
		posShader.setUniformTexture("posTexture", p1->getTextureReference(), 0);
		posShader.setUniformTexture("velTexture", v1->getTextureReference(), 1);
		posShader.setUniform3f("camOffset", balloon00Pos.x,balloon00Pos.y,balloon00Pos.z);
		posShader.setUniform1f("dimX", dimX);
		posShader.setUniform1f("dimY", dimY);
		posShader.setUniform1f("bound", dim);
		
		ofRect(-1,-1,2,2);
		
		posShader.end();
		
		p0->end();
		swap(p0, p1);
	}
	
	//get our active credits to pass to the shader
	vector<Credit*> activeCredits;
	for(auto& c: credits)
	{
		if(c.pos.y > -dim && c.pos.y<dim)
		{
			activeCredits.push_back(&c);
		}
	}
	
	vector<ofVec4f> creditPositions;
	for(auto& c: activeCredits)
	{
		ofVec4f cp = c->getLeft();
		cp.w = c->width;
		creditPositions.push_back(cp);
	}
	
	//update velocities
	ofVec3f camPos = getCameraPosition();
	v0->begin();
    ofClear(0, 255);
	velShader.begin();
	velShader.setUniformTexture("posTexture", p1->getTextureReference(), 0);
	velShader.setUniformTexture("velTexture", v1->getTextureReference(), 1);
	velShader.setUniform3f("camPos", camPos.x, camPos.y, camPos.z);;
	velShader.setUniform3f("camOffset", balloon00Pos.x,balloon00Pos.y,balloon00Pos.z);
	velShader.setUniform1f("netHeight", netHeight );
	velShader.setUniform1f("dimX", dimX);
	velShader.setUniform1f("dimY", dimY);
	velShader.setUniform1f("bound", dim);
	
	if(creditPositions.size())	velShader.setUniform4fv("credits", &creditPositions[0][0], creditPositions.size());
	velShader.setUniform1i("numCredits", creditPositions.size());
	
	velShader.setUniform1f("textRadius", textRadius);
	
	velShader.setUniform1f("time", ofGetElapsedTimef() * -1.);
	velShader.setUniform1f("noiseScl", noiseScl);
	velShader.setUniform1f("offset", offset);
	velShader.setUniform1f("noiseSampleScale", noiseSampleScale);
	velShader.setUniform1f("velAtten", velAtten);
	velShader.setUniform1f("radius", radius);
	velShader.setUniform1f("accScl", accScl);
	velShader.setUniform1f("gravity", gravity);
	velShader.setUniform1f("attractionToCenter", attractionToCenter);
	velShader.setUniform1f("cameraBounce", cameraBounce);
	
	velShader.setUniform1f("highSpeedScale", highSpeedScale );
	velShader.setUniform1f("speedLow", speedLow );
	velShader.setUniform1f("speedHi", speedHi );
	velShader.setUniform1f("highSpeedPercent", highSpeedPercent );
	
	ofRect(-1,-1,2,2);
	
	velShader.end();
	v0->end();
	swap(v0, v1);
	
	//update the rotations
	quatFbo.begin();
    ofClear(0, 255);
	quatShader.begin();
	quatShader.setUniformTexture("velTexture", v1->getTextureReference(), 0);
	quatShader.setUniform1f("dimX", dimX);
	quatShader.setUniform1f("dimY", dimY);
	
	ofRect(-1,-1,2,2);
	
	quatShader.end();
	quatFbo.end();
	
	//draw the balloons
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	shader.begin();
	shader.setUniform1f("shininess", shininess);
	shader.setUniform1f("lightScale", lightScale);
	shader.setUniform1f("screenHeight", ofGetHeight());
	
	ofFloatColor bg0 = bgColor;
	ofFloatColor bg1 = bgColor2;
	shader.setUniform4f("bg0", bg0.r, bg0.g, bg0.b, 1 );
	shader.setUniform4f("bg1", bg1.r, bg1.g, bg1.b, 1 );
	shader.setUniform1f("bgExpo", backgroundGradientExponent);
	
	shader.setUniform1f("dim", dim );
	shader.setUniform3f("camPos", camPos.x, camPos.y, camPos.z);
	shader.setUniform1f("facingRatio", facingRatioScale);//TODO: <-- slider
	shader.setUniform1f("fogDist", 400);//TODO: <-- slider
	shader.setUniform1f("dimX", dimX);
	shader.setUniform1f("dimY", dimY);
	shader.setUniform3f("l0", l0.x, l0.y, l0.z);
	shader.setUniformTexture("posTexture", p0->getTextureReference(), 0);
	shader.setUniformTexture("velTexture", v0->getTextureReference(), 1);
	shader.setUniformTexture("colTexture", colFbo.getTextureReference(), 2);
	shader.setUniformTexture("quatTexture", quatFbo.getTextureReference(), 3);
	
	shader.setUniform1f("creditThresh", creditLightDist);
	shader.setUniform1f("creditLightScale", creditLightScale);
	
	if(creditPositions.size())	shader.setUniform4fv("lights", &creditPositions[0][0], creditPositions.size());
	shader.setUniform1f("numLights", creditPositions.size());
	
	//vbo instancing
	vbo.bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, total, dimX*dimY);
	vbo.unbind();
	
	shader.end();
	
//	glLineWidth(2);
//	ofSetColor(0, 255, 0);
//	ofNoFill();
//	ofBox(balloon00Pos, 30);
	
	//draw the credits
	for(auto& c: credits)
	{
		c.draw();
	}
	
	glPopAttrib();
	ofPopStyle();
}

void CloudsVisualSystemBalloons::selfDrawDebug(){
	
}

void CloudsVisualSystemBalloons::selfDrawBackground(){

	//we are using this to draw what keywords are missing content
	if(mainKeyword != ""){
		string keystodraw = "PICKED RANDOM PRESET\n";
		keystodraw += mainKeyword + "\n" + ofJoinString(keywords, ",");
		ofPushMatrix();
		ofScale(5,5);
		ofDrawBitmapString(keystodraw, 20,20);
		ofPopMatrix();
	}
	
}

void CloudsVisualSystemBalloons::selfEnd(){	
	
}

void CloudsVisualSystemBalloons::selfExit()
{
	posFbo0.getTextureReference().clear();
	posFbo1.getTextureReference().clear();
	velFbo0.getTextureReference().clear();
	velFbo1.getTextureReference().clear();
	colFbo.getTextureReference().clear();
	quatFbo.getTextureReference().clear();
}

void CloudsVisualSystemBalloons::selfKeyPressed(ofKeyEventArgs & args)
{
	
}
void CloudsVisualSystemBalloons::selfKeyReleased(ofKeyEventArgs & args)
{
	if(args.key == ' ')
	{
		getCameraRef().setPosition(0, 0, -dim * 1.5);
		getCameraRef().lookAt(ofVec3f(0,0,0));
		setBalloonPositions();
	}
	
	else if(args.key == 'l')
	{
		shader.load(getVisualSystemDataPath() + "shaders/normalShader");
		posShader.load(getVisualSystemDataPath() + "shaders/posShader");
		velShader.load(getVisualSystemDataPath() + "shaders/velShader");
		quatShader.load(getVisualSystemDataPath() + "shaders/quatShader");
	}
	else if(args.key == 'R')
	{
		if(bReleased)
		{
			bReleased = false;
			setBalloonPositions();
		}
		else
		{
			bReleased = true;
		}
	}
}

void CloudsVisualSystemBalloons::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemBalloons::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemBalloons::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemBalloons::selfMouseReleased(ofMouseEventArgs& data){
	
}