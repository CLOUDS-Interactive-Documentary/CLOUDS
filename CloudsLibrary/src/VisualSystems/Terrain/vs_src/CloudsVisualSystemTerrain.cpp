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
    
    
    customGui = new ofxUISuperCanvas("Terrain", gui);
    customGui->copyCanvasStyle(gui);
    customGui->copyCanvasProperties(gui);
    customGui->setName("Terrain");
    customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    
    customGui->addLabel("Noise");
    customGui->addSlider("noise_zoom", 0.0, 10.0, &noiseZoom);
    customGui->addSlider("noise_speed", 0.0, 1.0, &noiseSpeed);
    
    customGui->addLabel("GrayScott");
    customGui->addSlider("Feed", 0.0, 0.1, &grayscottFade);
    customGui->addSlider("Loops", 1.0, 25, &grayscottLoops);
    customGui->addSlider("DiffV", 0.0, 1.0, &diffV);
    customGui->addSlider("DiffU", 0.0, 1.0, &diffU);
    customGui->addSlider("k", 0.0, 1.0, &k);
    customGui->addSlider("t", 0.0, 1.0, &f);
    customGui->addToggle("enable", &bGrayscott);
    customGui->addButton("clean", &bCleanGrayscott);
    
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
}

void CloudsVisualSystemTerrain::selfSetup()
{
    grayscottLoops = 10;
    terrainResolution = 1.0;
    diffU=0.25;
    diffV=0.04;
    k=0.047;
    f=0.2;

    terrainResolution = 1.0;
    setResolution(200, 200);
    
//    noiseShader.load("", getDataPath()+"shaders/VisualSystems/Terrain/noise.fs");
    noiseShader.load("", getVisualSystemDataPath()+"shaders/mNoise.fs");
    normalsShader.load("", getVisualSystemDataPath()+"shaders/normals.fs");
    patternShader.load("", getVisualSystemDataPath()+"shaders/pattern.fs");
    grayscottShader.load("", getVisualSystemDataPath()+"shaders/grayscott.fs");
    
    patternScale = 50.0;
    
    stripeAlpha = 1.0;
    hexAlpha = 1.0;
    dotsAlpha = 1.0;
    
    noiseSpeed = 0.0;
    
    bChange = true;
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
    }}

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
                grayscottShader.setUniformTexture("tex0", noiseFbo, 2);
                grayscottShader.setUniform1f("diffU", diffU);
                grayscottShader.setUniform1f("diffV", diffV);
                grayscottShader.setUniform1f("k", k);
                grayscottShader.setUniform1f("f", f);
                grayscottShader.setUniform1f("time", ofGetElapsedTimef());
                grayscottShader.setUniform1f("fade", grayscottFade);
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
        
        if(bGrayscott){
            makeTerrain(grayscottFbo[nPingPong%2].getTextureReference());
        } else {
            makeTerrain(noiseFbo.getTextureReference());
        }
        
        //  Pattern
        //
        patternFbo.begin();
        ofClear(0);
        patternShader.begin();
        patternShader.setUniformTexture("tex0", noiseFbo, 0);
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
}


void CloudsVisualSystemTerrain::makeTerrain( ofTexture &_heightMap ){
    
    //  Set VARIABLES
    //
    int width = _heightMap.getWidth();
    int height = _heightMap.getHeight();
    
    float flResolution = (int)terrainResolution;
    float flHeightScale = terrainHeight*50;
    float textureScale = patternScale;
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
                
                
                // 3        0 --- 1                nTri reference
                // | \          \          |
                // |   \        \ |
                // 4 --- 5          2
                
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
    mat->begin();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    ofPushMatrix();
    
    if(bGrayscott){
        grayscottFbo[nPingPong%2].getTextureReference().bind();
    } else {
//        noiseFbo.getTextureReference().bind();
        patternFbo.getTextureReference().bind();
    }
    

    ofSetColor(255);
    glEnable(GL_SMOOTH);
    glShadeModel(GL_SMOOTH);
    terrainVbo.draw(GL_TRIANGLES , 0, nVertexCount);
    
    if(bGrayscott){
        grayscottFbo[nPingPong%2].getTextureReference().unbind();
    } else {
        //noiseFbo.getTextureReference().unbind();
        patternFbo.getTextureReference().unbind();
    }
    
    
    ofPopMatrix();
    
    glDisable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);
    mat->end();
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