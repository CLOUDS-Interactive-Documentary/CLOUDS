//
//  CloudsVisualSystemTerrain.cpp
//  VSTerrain
//
//  Created by Patricio Gonzalez Vivo on 5/28/13.
//
//

#include "CloudsVisualSystemTerrain.h"
#include "CloudsGlobal.h"

string CloudsVisualSystemTerrain::getSystemName()
{
	return "Terrain";
}

void CloudsVisualSystemTerrain::selfSetupSystemGui()
{
    
    float length = (gui->getGlobalCanvasWidth()-gui->getWidgetSpacing()*5)/3.;
    float dim = gui->getGlobalSliderHeight();
    
    
    customGui = new ofxUISuperCanvas("Terrain", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
    customGui->setName("Terrain");
    customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    
    customGui->addLabel("Noise");
    customGui->addSlider("noise_zoom", 0.0, 10.0, &noiseZoom);
    customGui->addSlider("noise_speed", 0.0, 1.0, &noiseSpeed);
    customGui->addToggle("Show Debug", &bShowDebug);
    customGui->addLabel("GrayScott");
    customGui->addSlider("Height Scale", 0.0, 100.f, &mHeightScale);
    customGui->addSlider("Feed", 0.0, 0.1, &grayscottFade);
    customGui->addSlider("Loops", 1.0, 25, &grayscottLoops);
    customGui->addSlider("DiffV", 0.0, 1.0, &diffV);
    customGui->addSlider("DiffU", 0.0, 1.0, &diffU);
    customGui->addSlider("k", 0.0, 1.0, &k);
    customGui->addSlider("t", 0.0, 1.0, &f);
    customGui->addToggle("enable", &bGrayscott);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addButton("clean", &bCleanGrayscott);
    customGui->addToggle("Draw", &bDoDraw);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    customGui->addSlider("Blur Radius", 0, 5., &blurRadius);
	customGui->addSlider("Dry Rate", .01, .1, &dryRate);
    customGui->addLabel("Colors");
    customGui->addMinimalSlider("High R", 0, 1, &mHighColor.r, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("High G", 0, 1, &mHighColor.g, length, dim)->setShowValue(false);
    customGui->addMinimalSlider("High B", 0, 1, &mHighColor.b, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    customGui->addMinimalSlider("High A", 0, 1, &mHighColor.a);
    customGui->addSpacer();
    customGui->addMinimalSlider("Low R", 0, 1, &mLowColor.r, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("Low G", 0, 1, &mLowColor.g, length, dim)->setShowValue(false);
    customGui->addMinimalSlider("Low B", 0, 1, &mLowColor.b, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    customGui->addMinimalSlider("Low A", 0, 1, &mLowColor.a);
    customGui->addSpacer();
    customGui->addMinimalSlider("Trace R", 0, 1, &mTraceColor.r, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    customGui->addMinimalSlider("Trace G", 0, 1, &mTraceColor.g, length, dim)->setShowValue(false);
    customGui->addMinimalSlider("Trace B", 0, 1, &mTraceColor.b, length, dim)->setShowValue(false);
    customGui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    customGui->addMinimalSlider("Trace A", 0, 1, &mTraceColor.a);
    customGui->addSpacer();
    customGui->addSlider("Balance", 0, 1, &mBalance);
    customGui->addSlider("Texture Mix", 0, 1, &mTexMix);


    customGui->addLabel("Terrain");
    customGui->addSlider("Terrain_Size", 10, 200, &size);
    customGui->addSlider("Terrain_Altitud", 0, 2, &terrainHeight);
    customGui->addSlider("Terrain_Resolution", 1, 10, &terrainResolution);
    
    customGui->addLabel("Patern");
    customGui->addSlider("Hex", 0.0, 1.0, &hexAlpha);
    customGui->addSlider("Stripes", 0.0, 1.0, &stripeAlpha);
    customGui->addSlider("Dots", 0.0, 1.0, &dotsAlpha);
    
    ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemTerrain::selfGuiEvent);
    guis.push_back(customGui);
    guimap[customGui->getName()] = customGui;
    
    //fog gui
	fogGui = new ofxUISuperCanvas("FOG", gui);
	fogGui->copyCanvasStyle(gui);
	fogGui->copyCanvasProperties(gui);
	fogGui->setName("FOG");
	fogGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	fogGui->addSpacer();
	fogGui->addSlider("fogDist", 10, 500, &fogDist);
	fogGui->addSlider("fogExpo", .6, 3., &fogExpo);
	
    //fogGui->addImageSampler("fogColor", &colorMap, 100, 100);
	
	ofAddListener(fogGui->newGUIEvent, this, &CloudsVisualSystemTerrain::selfGuiEvent);
	guis.push_back(fogGui);
	guimap[fogGui->getName()] = fogGui;
}

void CloudsVisualSystemTerrain::selfSetup()
{
    grayscottLoops = 1;
    terrainResolution = 1.0;
    diffU=0.25;
    diffV=0.04;
    k=0.47;
    f=0.2;

    terrainResolution = 1.0;
    setResolution(200, 200);
    
//    noiseShader.load("", getDataPath()+"shaders/VisualSystems/Terrain/noise.fs");
    noiseShader.load("", getVisualSystemDataPath()+"shaders/mNoise.fs");
    normalsShader.load("", getVisualSystemDataPath()+"shaders/normals.fs");
    patternShader.load("", getVisualSystemDataPath()+"shaders/pattern.fs");
    grayscottShader.load("", getVisualSystemDataPath()+"shaders/grayscott.fs");
    colorShader.load(getVisualSystemDataPath()+"shaders/color.vs", getVisualSystemDataPath()+"shaders/color.fs");
    circleShader.load(getVisualSystemDataPath()+"shaders/circle.vs", getVisualSystemDataPath()+"shaders/circle.fs");
    vBlurShader.load(getVisualSystemDataPath()+"shaders/vblur.vs", getVisualSystemDataPath()+"shaders/blur.fs");
    hBlurShader.load(getVisualSystemDataPath()+"shaders/hblur.vs", getVisualSystemDataPath()+"shaders/blur.fs");

    
    patternScale = 50.0;
    
    stripeAlpha = 1.0;
    hexAlpha = 1.0;
    dotsAlpha = 1.0;
    mTexMix = 0.;
    
    brushSize = 25.;

    blurRadius = .9;
	dryRate = .06;
    
    mDepositScale = 25.f;
    
    noiseSpeed = 0.0;
    
    mHeightScale = 30.f;
    
    bChange = true;
    
    bDoNoise = false;
    bDoDraw = true;
    bShowDebug = false;
    
    mTraceColor = ofFloatColor(1.,0.,0.,1.);
    mHighColor = ofFloatColor(0.,1.,1.,1.);
    mLowColor = ofFloatColor(1.,0.,0.,1.);
    mAtten = 0.f;
    mBalance = .5f;
    mouse = ofVec2f(0,0);
    
    resizeBrush();
    
    blurMesh.clear();
	blurMesh.addVertex(ofVec3f(0,0,0));
	blurMesh.addVertex(ofVec3f(200,0,0));
	blurMesh.addVertex(ofVec3f(0,200,0));
	blurMesh.addVertex(ofVec3f(200,200,0));
	
	blurMesh.addTexCoord(ofVec2f(0,0));
	blurMesh.addTexCoord(ofVec2f(200,0));
	blurMesh.addTexCoord(ofVec2f(0,200));
	blurMesh.addTexCoord(ofVec2f(200,200));
	
	blurMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    makeTerrain(grayscottFbo[0].getTextureReference());

    
}

void CloudsVisualSystemTerrain::resizeBrush(){
    
    brushMesh.clear();
	brushMesh.addVertex(ofVec3f(0,0,0));
	brushMesh.addVertex(ofVec3f(mDepositScale,0,0));
	brushMesh.addVertex(ofVec3f(0,mDepositScale,0));
	brushMesh.addVertex(ofVec3f(mDepositScale,mDepositScale,0));
	
	brushMesh.addTexCoord(ofVec2f(-1,-1));
	brushMesh.addTexCoord(ofVec2f(1,-1));
	brushMesh.addTexCoord(ofVec2f(-1,1));
	brushMesh.addTexCoord(ofVec2f(1,1));
	
	brushMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	
	mCurDepositScale = mDepositScale;
}

void CloudsVisualSystemTerrain::selfBegin()
{
    
}

void CloudsVisualSystemTerrain::selfEnd()
{
    
}


void CloudsVisualSystemTerrain::selfGuiEvent(ofxUIEventArgs &e){
    if(e.widget->getName() == "Custom Button"){
        cout << "Button pressed!" << endl;
    }
}


void CloudsVisualSystemTerrain::selfSetupRenderGui()
{

}

void CloudsVisualSystemTerrain::guiSystemEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    
    if ( name == "Terrain_Size" |name == "Terrain_Width" || name == "Terrain_Height" || name == "Terrain_Resolution"){
        setResolution(size, size);
    }
    
    if(name == "Brush Size") resizeBrush();
    
    bChange = true;
    
    
}

void CloudsVisualSystemTerrain::setResolution( int _width, int _height ){
    int width = _width;
    int height = _height;
    
    noiseFbo.allocate(width, height);
    normalsFbo.allocate(width, height);
    patternFbo.allocate(width*patternScale, height*patternScale);
    
    for (int i = 0; i < 2; i++) {
        grayscottFbo[i].allocate(width, height);
        grayscottFbo[i].begin();
        ofClear(0);
        grayscottFbo[i].end();
    }
    
    canvasSrc.allocate(width, height);
    canvasSrc.begin();
    ofClear(0, 0, 0);
    canvasSrc.end();
    
    canvasDest.allocate(width, height);
    canvasDest.begin();
    ofClear(0, 0, 0);
    canvasDest.end();
}

void CloudsVisualSystemTerrain::selfKeyPressed(ofKeyEventArgs & args){
    if (args.key == OF_KEY_UP){
        camPosition.y += 1;
    } else if (args.key == OF_KEY_DOWN){
        camPosition.y -= 1;
    } else if (args.key == OF_KEY_LEFT){
        camPosition.x += 1;
    } else if (args.key == OF_KEY_RIGHT){
        camPosition.x -= 1;
    }
    
    bChange = true;
}

void CloudsVisualSystemTerrain::selfUpdate()
{
    
    ofFbo& canvas = (ofGetFrameNum()%2==0) ? canvasSrc : canvasDest;
    
    if ( bChange || noiseSpeed > 0.0){
    
        //  NOISE
        //
        int width = noiseFbo.getWidth();
        int height = noiseFbo.getHeight();
        noiseFbo.begin();
        ofClear(0);
        noiseShader.begin();
        noiseShader.setUniform2f("resolution", width,height);
        noiseShader.setUniform2f("position", camPosition.x, camPosition.y);
        noiseShader.setUniform1f("zoom", noiseZoom);
        noiseShader.setUniform1f("time", ofGetElapsedTimef()*noiseSpeed);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
        glTexCoord2f(width, 0); glVertex3f(width, 0, 0);
        glTexCoord2f(width, height); glVertex3f(width, height, 0);
        glTexCoord2f(0,height);  glVertex3f(0,height, 0);
        glEnd();
        noiseShader.end();
        noiseFbo.end();
        
        if(bGrayscott){
            if (bCleanGrayscott){
                for (int i = 0; i < 2; i++) {
                    grayscottFbo[i].begin();
                    ofClear(0);
                    grayscottFbo[i].end();
                }
                bCleanGrayscott = false;
            }
            
            for (int i = 0; i < grayscottLoops; i++) {
                nPingPong = (nPingPong+1)%2;
                
                grayscottFbo[nPingPong%2].begin();
                grayscottShader.begin();
                grayscottShader.setUniformTexture("backbuffer", grayscottFbo[(nPingPong+1)%2], 1);
                
                if(bDoDraw)
                    grayscottShader.setUniformTexture("tex0", canvas, 2);
                else
                    grayscottShader.setUniformTexture("tex0", noiseFbo, 2);

                grayscottShader.setUniform1f("diffU", diffU);
                grayscottShader.setUniform1f("diffV", diffV);
                grayscottShader.setUniform1f("k", k);
                grayscottShader.setUniform1f("f", f);
                grayscottShader.setUniform1f("time", ofGetElapsedTimef()+500.f);
                grayscottShader.setUniform1f("fade", grayscottFade);
                //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
                grayscottShader.setUniform2f("mouse", mouse.x/getCanvasWidth(), mouse.y/getCanvasHeight() );
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
                glTexCoord2f(width, 0); glVertex3f(width, 0, 0);
                glTexCoord2f(width, height); glVertex3f(width, height, 0);
                glTexCoord2f(0,height);  glVertex3f(0,height, 0);
                glEnd();
                grayscottShader.end();
                grayscottFbo[nPingPong%2].end();
                
            }
        }
        
        //  Normals
        //
        normalsFbo.begin();
        normalsShader.begin();
        if(bGrayscott){
            grayscottFbo[nPingPong%2].draw(0, 0);
        } else {
            noiseFbo.draw(0, 0);
        }
        normalsShader.end();
        normalsFbo.end();
        
//        if(bGrayscott){
//            makeTerrain(grayscottFbo[nPingPong%2].getTextureReference());
//        } else {
//            makeTerrain(noiseFbo.getTextureReference());
//        }
        
        //  Pattern
        //
        patternFbo.begin();
        ofClear(0);
        patternShader.begin();
        if(bDoDraw)  patternShader.setUniformTexture("tex0", canvas, 0);
        else         patternShader.setUniformTexture("tex0", noiseFbo, 0);

        patternShader.setUniform1f("textureScale", patternScale);
        patternShader.setUniform1f("scale", 0.48);
        
        patternShader.setUniform1f("hexAlpha", hexAlpha);
        patternShader.setUniform1f("stripeAlpha", stripeAlpha);
        
        patternShader.setUniform1f("pointsGap", 10.0);
        patternShader.setUniform1f("pointsAlpha", dotsAlpha);
        
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
        glTexCoord2f(width*patternScale, 0); glVertex3f(width*patternScale, 0, 0);
        glTexCoord2f(width*patternScale, height*patternScale); glVertex3f(width*patternScale, height*patternScale, 0);
        glTexCoord2f(0,height*patternScale);  glVertex3f(0,height*patternScale, 0);
        glEnd();
        
        patternShader.end();
        patternFbo.end();
        
        bChange = false;
    }
    
    if(bDoDraw){
        
        glDisable(GL_DEPTH_TEST);
        ofSetColor(255);
        
        canvasDest.begin();
        
        ofDisableAlphaBlending();
        ofClear(0, 0, 0, 0);
        
        
        hBlurShader.begin();
        hBlurShader.setUniformTexture("s_texture", canvasSrc.getTextureReference(), 1);
        hBlurShader.setUniform2f("dimensions", canvasDest.getWidth(), canvasDest.getHeight());
        hBlurShader.setUniform1f("dryRate", dryRate);
        hBlurShader.setUniform1f("blurRadius", blurRadius);
        blurMesh.draw();
        hBlurShader.end();
        
        canvasDest.end();
        
        swap(canvasSrc, canvasDest);
        
        canvasDest.begin();
        
        ofDisableAlphaBlending();
        ofClear(0, 0, 0, 0);
        
        vBlurShader.begin();
        vBlurShader.setUniformTexture("s_texture", canvasSrc.getTextureReference(), 1);
        vBlurShader.setUniform2f("dimensions", canvasDest.getWidth(), canvasDest.getHeight());
        vBlurShader.setUniform1f("dryRate", dryRate);
        vBlurShader.setUniform1f("blurRadius", blurRadius);
        blurMesh.draw();
        vBlurShader.end();
        
        ofEnableAlphaBlending();

        circleShader.begin();
        ofVec2f centerTranslate = ofVec2f(mDepositScale*.5,mDepositScale*.5);
        map<int, vector<ofVec2f> >::iterator it;
        for (it = playerDepositPoints.begin(); it != playerDepositPoints.end(); it++) {
            for(int i = 0; i <it->second.size(); i++){
                ofPushMatrix();
                //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
                ofTranslate(ofVec2f(ofMap(it->second[i].x,0,getCanvasWidth(), 0,canvasDest.getWidth()),ofMap(it->second[i].y,0,getCanvasHeight(), 0,canvasDest.getHeight())) - centerTranslate);
                brushMesh.draw();
                ofPopMatrix();
            }
        }

        circleShader.end();
        canvasDest.end();
        ofDisableAlphaBlending();
        
        swap(canvasSrc, canvasDest);
        
        for (it = playerDepositPoints.begin(); it != playerDepositPoints.end(); it++) {
            it->second.clear();
        }
        
    }
    
    fc = bgColor;
}


void CloudsVisualSystemTerrain::makeTerrain( ofTexture &_heightMap ){
    
    //  Set VARIABLES
    //
    int width = _heightMap.getWidth();
    int height = _heightMap.getHeight();
    
    float flResolution = (int)terrainResolution;
    float flHeightScale = terrainHeight*50;
    float textureScale = 1.;
 //   float textureScale = width;

    nVertexCount = (int) ( width * height * 6 / ( flResolution * flResolution ) );
    
    pVertices        = new ofVec3f[nVertexCount];                // Allocate Vertex Data
    pTexCoords        = new ofVec2f[nVertexCount];                // Allocate Tex Coord Data
    pNormals        = new ofVec3f[nVertexCount];                // Allocate Normals
    
    //  texture -> pixels
    //
    ofFloatPixels heightMap;
    heightMap.allocate(_heightMap.getWidth(),
                       _heightMap.getHeight(),
                       OF_PIXELS_RGBA);
    _heightMap.readToPixels(heightMap);
    
    //  normals -> pixels;
    //
    ofFloatPixels normalMap;
    normalMap.allocate(normalsFbo.getWidth(),
                       normalsFbo.getHeight(),
                       OF_PIXELS_RGBA);
    normalsFbo.getTextureReference().readToPixels(normalMap);
    
    camAltitud = ofLerp( camAltitud, heightMap.getColor(width*0.5, height*0.5).r * flHeightScale + 7, 0.1);
//    cam.setPosition(0, camAltitud, 0);
    
    //  Construct the VBO
    //
    int nIndex = 0;
    for( int nY = 0; nY < height-flResolution ; nY += (int) flResolution ){
        for( int nX = 0; nX < width-flResolution ; nX += (int) flResolution ){
            
            for( int nTri = 0; nTri < 6; nTri++ ){
                
                // Using This Quick Hack, Figure The X,Y Position Of The Point
                //
                float flX = (float) nX + ( ( nTri == 1 || nTri == 2 || nTri == 5 ) ? flResolution : 0.0f );
                float flZ = (float) nY + ( ( nTri == 2 || nTri == 4 || nTri == 5 ) ? flResolution : 0.0f );
                
                // Set The Data, Using PtHeight To Obtain The Y Value
                //
                pVertices[nIndex].x = flX - ( width * 0.5 );
                pVertices[nIndex].y = 1.0 - heightMap.getColor((int)flX, (int)flZ).r * flHeightScale;
                pVertices[nIndex].z = flZ - ( height * 0.5 );
                
//                pColors[nIndex].r = ofMap(heightMap.getColor((int)flX, (int)flZ).r, 0.f, 1.f, .0f, .3f);
//                pColors[nIndex].g = ofMap(heightMap.getColor((int)flX, (int)flZ).r, 0.f, 1.f, .0f, 1.f);
//                pColors[nIndex].b = ofMap(heightMap.getColor((int)flX, (int)flZ).r, 0.f, 1.f, .3f, 6.f);
//                pColors[nIndex].a = 1.f;

                
                // 3        0 --- 1                nTri reference
                // | \        \   |
                // |   \        \ |
                // 4 --- 5        2
                
                // Stretch The Texture Across The Entire Mesh
                //
                pTexCoords[nIndex].x = flX * textureScale;
                pTexCoords[nIndex].y = flZ * textureScale;
                
                
                // Normals by vert
                //
                pNormals[nIndex].x = normalMap.getColor((int)flX, (int)flZ).r * 2.0 - 1.0;
                pNormals[nIndex].y = normalMap.getColor((int)flX, (int)flZ).g * 2.0 - 1.0;
                pNormals[nIndex].z = normalMap.getColor((int)flX, (int)flZ).b * 2.0 - 1.0;
                // Increment Our Index
                //
                nIndex++;
            }
        }
    }
    
    terrainVbo.setVertexData(pVertices, nVertexCount, GL_STREAM_DRAW);
    terrainVbo.setNormalData(pNormals, nVertexCount, GL_STREAM_DRAW);
    terrainVbo.setTexCoordData(pTexCoords, nVertexCount, GL_STREAM_DRAW);
    
    // Our Copy Of The Data Is No Longer Necessary, It Is Safe In The Graphics Card
    delete [] pVertices; pVertices = NULL;
    delete [] pTexCoords; pTexCoords = NULL;
    delete [] pNormals; pNormals = NULL;
}

void CloudsVisualSystemTerrain::selfDraw()
{
    ofFbo& canvas = (ofGetFrameNum()%2==0) ? canvasSrc : canvasDest;
    
    mat->begin();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    ofPushMatrix();
    ofSetColor(255);
    glEnable(GL_SMOOTH);
    glShadeModel(GL_SMOOTH);
    
    colorShader.begin();

    if(bGrayscott){
        colorShader.setUniformTexture("map", grayscottFbo[nPingPong%2], 0);
    } else {
        colorShader.setUniformTexture("map", patternFbo, 0 );
    }
    
    colorShader.setUniformTexture("drawMap", canvas, 1);
    colorShader.setUniformTexture("normalMap", normalsFbo, 2);
    colorShader.setUniformTexture("noiseMap", noiseFbo, 3);

    colorShader.setUniform4fv("highColor", mHighColor.v);
    colorShader.setUniform4fv("lowColor", mLowColor.v);
    colorShader.setUniform1f("_atten", mAtten);
    colorShader.setUniform1f("balance", mBalance);
    colorShader.setUniform1f("texMix", mTexMix);
    colorShader.setUniform4fv("traceColor", mTraceColor.v);
    
	colorShader.setUniform1f("heightScale", mHeightScale );
    colorShader.setUniform4f("fogColor", fc.r, fc.g, fc.g, fc.a );
	colorShader.setUniform1f("fogDist", fogDist );
	colorShader.setUniform1f("fogExpo", fogExpo );
    colorShader.setUniform1f("doDraw", bDoDraw );
    
    ofVec3f cp = getCameraRef().getPosition() / 1.;
	colorShader.setUniform3f("cameraPos", cp.x, cp.y, cp.z );

    terrainVbo.draw(GL_TRIANGLES , 0, nVertexCount);
    colorShader.end();
    
    ofPopMatrix();
    
    glDisable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);
    mat->end();
    
    if(bShowDebug){
    ofPushMatrix();
    ofTranslate(0, 0, -200);
    if(bDoDraw)canvas.draw(0, 0);
    else noiseFbo.draw(0, 0);
    grayscottFbo[nPingPong%2].draw(-noiseFbo.getWidth(),0);
    ofPopMatrix();
    }
}

void CloudsVisualSystemTerrain::billBoard()
{
    
}

void CloudsVisualSystemTerrain::selfSetupGuis()
{
    
}

void CloudsVisualSystemTerrain::selfAutoMode()
{
    
}


void CloudsVisualSystemTerrain::selfDrawBackground()
{
    
}

void CloudsVisualSystemTerrain::selfDrawDebug()
{
    
}

void CloudsVisualSystemTerrain::selfSceneTransformation()
{
    
}

void CloudsVisualSystemTerrain::selfExit()
{
    
    brushMesh.clear();
    blurMesh.clear();
    playerHistoryMap.clear();
    playerDepositPoints.clear();
    depositPoints.clear();
    
    colorShader.unload();
    circleShader.unload();
    vBlurShader.unload();
    hBlurShader.unload();
    normalsShader.unload();
    patternShader.unload();
    
    terrainVbo.clear();
    grayscottShader.unload();
    noiseShader.unload();
}

ofVec2f CloudsVisualSystemTerrain::hermiteInterpolate(ofVec2f y0, ofVec2f y1, ofVec2f y2, ofVec2f y3, float pct, float tension, float bias){
	ofVec2f m0,m1;
	float pct2,pct3;
	float a0,a1,a2,a3;
	pct2 = pct * pct;
	pct3 = pct2 * pct;
	m0  = (y1-y0)*(1+bias)*(1-tension)/2;
	m0 += (y2-y1)*(1-bias)*(1-tension)/2;
	m1  = (y2-y1)*(1+bias)*(1-tension)/2;
	m1 += (y3-y2)*(1-bias)*(1-tension)/2;
	a0 =  2*pct3 - 3*pct2 + 1;
	a1 =  pct3 - 2*pct2 + pct;
	a2 =  pct3 - pct2;
	a3 =  -2*pct3 + 3*pct2;
	return(a0*y1 + a1*m0+a2*m1+a3*y2);
}

void CloudsVisualSystemTerrain::selfInteractionDragged(CloudsInteractionEventArgs& args){
    if(bDoDraw){
        
        mouse.x = args.position.x;
        mouse.y = args.position.y;
        
        playerHistoryMap[args.playerId].push_back(ofVec2f(args.position.x,args.position.y));
        
        vector<ofVec2f> splineHandles;
        //make a spline
        if(playerHistoryMap[args.playerId].size() == 1){
            return; //draw next time
        }
        if(playerHistoryMap[args.playerId].size() == 2){
            splineHandles.push_back(playerHistoryMap[args.playerId][0]);
            splineHandles.push_back(playerHistoryMap[args.playerId][0]);
            splineHandles.push_back(playerHistoryMap[args.playerId][1]);
            splineHandles.push_back(playerHistoryMap[args.playerId][1]);
        }
        else if(playerHistoryMap[args.playerId].size() == 3){
            splineHandles.push_back(playerHistoryMap[args.playerId][0]);
            splineHandles.push_back(playerHistoryMap[args.playerId][0]);
            splineHandles.push_back(playerHistoryMap[args.playerId][1]);
            splineHandles.push_back(playerHistoryMap[args.playerId][2]);
        }
        else{
            for(int i = playerHistoryMap[args.playerId].size()-4; i < playerHistoryMap[args.playerId].size(); i++){
                splineHandles.push_back(playerHistoryMap[args.playerId][i]);
            }
        }
        
        float stepsize = ofMap(brushSize, 2, 200, .005, .1, true);
        
        for(float a = 0; a < 1.; a+=stepsize){
            playerDepositPoints[args.playerId].push_back(hermiteInterpolate(splineHandles[0],
																			splineHandles[1],
																			splineHandles[2],
																			splineHandles[3], a, 0, 0));
        }
        
        if(playerHistoryMap[args.playerId].size() > 4){
            playerHistoryMap[args.playerId].erase(playerHistoryMap[args.playerId].begin());
        }
        
    }

}

void CloudsVisualSystemTerrain::selfInteractionMoved(CloudsInteractionEventArgs& args){
    if(bDoDraw){
        
        mouse.x = args.position.x;
        mouse.y = args.position.y;
        
        playerHistoryMap[args.playerId].push_back(ofVec2f(args.position.x,args.position.y));
        
        vector<ofVec2f> splineHandles;
        //make a spline
        if(playerHistoryMap[args.playerId].size() == 1){
            return; //draw next time
        }
        if(playerHistoryMap[args.playerId].size() == 2){
            splineHandles.push_back(playerHistoryMap[args.playerId][0]);
            splineHandles.push_back(playerHistoryMap[args.playerId][0]);
            splineHandles.push_back(playerHistoryMap[args.playerId][1]);
            splineHandles.push_back(playerHistoryMap[args.playerId][1]);
        }
        else if(playerHistoryMap[args.playerId].size() == 3){
            splineHandles.push_back(playerHistoryMap[args.playerId][0]);
            splineHandles.push_back(playerHistoryMap[args.playerId][0]);
            splineHandles.push_back(playerHistoryMap[args.playerId][1]);
            splineHandles.push_back(playerHistoryMap[args.playerId][2]);
        }
        else{
            for(int i = playerHistoryMap[args.playerId].size()-4; i < playerHistoryMap[args.playerId].size(); i++){
                splineHandles.push_back(playerHistoryMap[args.playerId][i]);
            }
        }
        
        float stepsize = ofMap(brushSize, 2, 200, .005, .1, true);
        
        for(float a = 0; a < 1.; a+=stepsize){
            playerDepositPoints[args.playerId].push_back(hermiteInterpolate(splineHandles[0],
																			splineHandles[1],
																			splineHandles[2],
																			splineHandles[3], a, 0, 0));
        }
        
        if(playerHistoryMap[args.playerId].size() > 4){
            playerHistoryMap[args.playerId].erase(playerHistoryMap[args.playerId].begin());
        }
        
    }
}


void CloudsVisualSystemTerrain::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemTerrain::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemTerrain::selfMouseMoved(ofMouseEventArgs& data)
{
   
}

void CloudsVisualSystemTerrain::selfMousePressed(ofMouseEventArgs& data)
{
    mouse = ofVec2f(data.x, data.y);
//    if(mouse.x > 0. && mouse.x < canvas.getWidth() && mouse.y < 0. && mouse.y < canvas.getHeight()){
//        canvas.setColor(mouse.x, mouse.y, ofColor(255));
//    }
}

void CloudsVisualSystemTerrain::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemTerrain::selfSetupGui()
{
    
}




void CloudsVisualSystemTerrain::guiRenderEvent(ofxUIEventArgs &e)
{
    bChange = true;
}