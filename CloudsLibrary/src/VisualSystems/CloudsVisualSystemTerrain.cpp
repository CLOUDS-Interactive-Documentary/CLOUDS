//
//  CloudsVisualSystemTerrain.cpp
//  VSTerrain
//
//  Created by Patricio Gonzalez Vivo on 5/28/13.
//
//

#include "CloudsVisualSystemTerrain.h"

string CloudsVisualSystemTerrain::getSystemName()
{
	return "Terrain";
}

void CloudsVisualSystemTerrain::selfSetup()
{
    nPingPong = 0;
    
    grayscottLoops = 10;
    terrainResolution = 1.0;
    diffU=0.25;
    diffV=0.04;
    k=0.047;
    f=0.2;
    
    setResolution(200, 200);
    
    noiseShader.load("", getDataPath()+"shaders/VisualSystems/Terrain/noise.fs");
    grayscottShader.load("", getDataPath()+"shaders/VisualSystems/Terrain/grayscott.fs");
    normalsShader.load("", getDataPath()+"shaders/VisualSystems/Terrain/normals.fs");
    terrainShader.load(getDataPath()+"shaders/VisualSystems/Terrain/terrain");
}

void CloudsVisualSystemTerrain::selfSetupSystemGui()
{
    sysGui->addLabel("Noise");
    sysGui->addSlider("noise_zoom", 0.0, 10.0, &noiseZoom);
    sysGui->addSlider("noise_speed", 0.0, 2.0, &noiseSpeed);
    
    sysGui->addLabel("GrayScott");
    sysGui->addSlider("Feed", 0.0, 0.1, &grayscottFade);
    sysGui->addSlider("Loops", 1.0, 25, &grayscottLoops);
    //    sysGui->addSlider("DiffV", 0.0, 1.0, &diffV);
    //    sysGui->addSlider("DiffU", 0.0, 1.0, &diffU);
    //    sysGui->addSlider("k", 0.0, 1.0, &k);
    //    sysGui->addSlider("t", 0.0, 1.0, &f);
    sysGui->addToggle("enable", &bGrayscott);
    sysGui->addButton("clean", &bCleanGrayscott);
    
    sysGui->addLabel("Terrain");
    sysGui->addSlider("Terrain_Size", 10, 200, &size);
    sysGui->addSlider("Terrain_Altitud", 0, 2, &terrainHeight);
    sysGui->addSlider("Terrain_Resolution", 1, 10, &terrainResolution);
}

void CloudsVisualSystemTerrain::selfSetupRenderGui()
{
    rdrGui->addSlider("Points Alpha", 0, 1.0, &pointsAlpha);
    rdrGui->addSlider("V Wireframe Alpha", 0, 1.0, &terrainVerticalAlpha);
    rdrGui->addSlider("H Wireframe Alpha", 0, 1.0, &terrainHorizontalAlpha);
}

void CloudsVisualSystemTerrain::guiSystemEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    
    if ( name == "Terrain_Size" |name == "Terrain_Width" || name == "Terrain_Height" || name == "Terrain_Resolution"){
        setResolution(size, size);
    }
}

void CloudsVisualSystemTerrain::setResolution( int _width, int _height ){
    int width = _width;
    int height = _height;
    
    noiseFbo.allocate(width, height);
    normalsFbo.allocate(width, height);
    for (int i = 0; i < 2; i++) {
        grayscottFbo[i].allocate(width, height);
        grayscottFbo[i].begin();
        ofClear(0);
        grayscottFbo[i].end();
    }
    
    makeMesh(noiseFbo.getTextureReference());
}

//-------------------------------------------------------------- LOOP
void addFace(ofVboMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c) {
	ofVec3f normal = ((b - a).cross(c - a)).normalize();
	mesh.addNormal(normal);
	mesh.addVertex(a);
    mesh.addTexCoord(a);
	mesh.addNormal(normal);
	mesh.addVertex(b);
    mesh.addTexCoord(b);
	mesh.addNormal(normal);
	mesh.addVertex(c);
    mesh.addTexCoord(c);
}

//--------------------------------------------------------------
void addFace(ofVboMesh& mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d) {
	addFace(mesh, a, b, c);
	addFace(mesh, a, c, d);
}

//--------------------------------------------------------------
ofVec3f getVertexFromTexture(ofFloatPixels& _pixels, int x, int y) {
	return ofVec3f(x, y, 100 * _pixels.getColor(x, y).getBrightness());
}

void CloudsVisualSystemTerrain::makeMesh(ofTexture &_heightMap){
    
    terrainMesh.clear();
    
	int skip = terrainResolution;
	int width = size;
    int height = size;
    

    ofFloatPixels heightMap;
    heightMap.allocate(_heightMap.getWidth(),
                       _heightMap.getHeight(),
                       OF_PIXELS_RGBA);
    _heightMap.readToPixels(heightMap);
    
    terrainMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    for(int y = 0; y < height - skip; y += skip) {
        for(int x = 0; x < width - skip; x += skip) {
            ofVec3f nw = getVertexFromTexture(heightMap, x, y);
            ofVec3f ne = getVertexFromTexture(heightMap, x + skip, y);
            ofVec3f sw = getVertexFromTexture(heightMap, x, y + skip);
            ofVec3f se = getVertexFromTexture(heightMap, x + skip, y + skip);
            
            addFace(terrainMesh, nw, ne, se, sw);
        }
    }
    
    terrainVertical.clear();
    terrainVertical.setMode(OF_PRIMITIVE_LINES);
    for(int y = 0; y < height-skip; y += skip) {
        for(int x = 0; x < width-skip; x += skip) {
            terrainVertical.addVertex(ofPoint(x, y));
            terrainVertical.addVertex(ofPoint(x+skip, y+skip));
        }
    }
    
    terrainHorizontal.clear();
    terrainHorizontal.setMode(OF_PRIMITIVE_LINES);
    for(int y = 0; y < height-skip; y += skip) {
        for(int x = 0; x < width-skip; x += skip) {
            terrainHorizontal.addVertex(ofPoint(x, y+skip));
            terrainHorizontal.addVertex(ofPoint(x+skip, y));
        }
    }
}

void CloudsVisualSystemTerrain::makeTerrain( ofTexture &_heightMap ){
    
    //  Set VARIABLES
    //
    int width = _heightMap.getWidth();
    int height = _heightMap.getHeight();
    
    float flResolution = (int)terrainResolution;
    float flHeightScale = terrainHeight*50;
    float textureScale = 1.0;
    nVertexCount = (int) ( width * height * 6 / ( flResolution * flResolution ) );
    
    pVertices	= new ofVec3f[nVertexCount];		// Allocate Vertex Data
    pTexCoords	= new ofVec2f[nVertexCount];		// Allocate Tex Coord Data
    pNormals	= new ofVec3f[nVertexCount];		// Allocate Normals
    
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
    
    //  Construct the VBO
    //
    int nIndex = 0;
    for( int nY = 0; nY < height-flResolution ; nY += (int) flResolution ){
        for( int nX = 0; nX < width-flResolution ; nX += (int) flResolution ){
            
            for( int nTri = 0; nTri < 6; nTri++ ){
                
                // Using This Quick Hack, Figure The X,Y Position Of The Point
                //
                float flX = (float) nX + ( ( nTri == 1 || nTri == 2 || nTri == 5 ) ? flResolution : 0.0f );
                float flY = (float) nY + ( ( nTri == 2 || nTri == 4 || nTri == 5 ) ? flResolution : 0.0f );
                
                // Set The Data, Using PtHeight To Obtain The Y Value
                //
                pVertices[nIndex].x = flX - ( width * 0.5 );
                pVertices[nIndex].y = flY - ( height * 0.5 );
                pVertices[nIndex].z = heightMap.getColor((int)flX, (int)flY).r * flHeightScale;
                
                // 3	0 --- 1		nTri reference
                // | \	  \	  |
                // |   \	\ |
                // 4 --- 5	  2
                
                // Stretch The Texture Across The Entire Mesh
                //
                pTexCoords[nIndex].x = flX * textureScale;// / width;
                pTexCoords[nIndex].y = flY * textureScale;// / height;
                
                // Normals by vert
                //
                pNormals[nIndex].x = normalMap.getColor((int)flX, (int)flY).r * 2.0 - 1.0;
                pNormals[nIndex].y = normalMap.getColor((int)flX, (int)flY).g * 2.0 - 1.0;
                pNormals[nIndex].z = normalMap.getColor((int)flX, (int)flY).b * 2.0 - 1.0;
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

void CloudsVisualSystemTerrain::selfUpdate()
{
    //  NOISE
    //
    int width = noiseFbo.getWidth();
    int height = noiseFbo.getHeight();
    noiseFbo.begin();
    ofClear(0);
    noiseShader.begin();
    noiseShader.setUniform2f("resolution", width,height);
    noiseShader.setUniform1f("time", ofGetElapsedTimef()*noiseSpeed);
    noiseShader.setUniform1f("zoom", noiseZoom);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(width, 0); glVertex3f(width, 0, 0);
    glTexCoord2f(width, height); glVertex3f(width, height, 0);
    glTexCoord2f(0,height);  glVertex3f(0,height, 0);
    glEnd();
    noiseShader.end();
    noiseFbo.end();
    
    //  Grayscott
    //
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
    
    //  Normals;
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
    
    cam.setPosition(0, 0, camAltitud);
    
}

void CloudsVisualSystemTerrain::drawVboTerrain(){
    glEnable(GL_NORMALIZE);
    ofPushMatrix();
    if(bGrayscott){
        grayscottFbo[nPingPong%2].getTextureReference().bind();
    } else {
        noiseFbo.getTextureReference().bind();
    }
    ofSetColor(255);
    glEnable(GL_SMOOTH);
	glShadeModel(GL_SMOOTH);
	terrainVbo.draw(GL_TRIANGLES , 0, nVertexCount);
    if(bGrayscott){
        grayscottFbo[nPingPong%2].getTextureReference().unbind();
    } else {
        noiseFbo.getTextureReference().unbind();
    }
    ofPopMatrix();
	glDisable(GL_NORMALIZE);
}

void CloudsVisualSystemTerrain::selfDraw()
{
    mat->begin();
    glEnable(GL_DEPTH_TEST);
    
    drawVboTerrain();
    
//    terrainShader.begin();
//    if(bGrayscott){
//        terrainShader.setUniformTexture("depthTex", grayscottFbo[nPingPong%2], 0);
//    } else {
//        terrainShader.setUniformTexture("depthTex", noiseFbo, 0);
//    }
//    terrainShader.setUniformTexture("normalTex", normalsFbo, 1);
//    terrainShader.setUniform2f("resolution", noiseFbo.getWidth(), noiseFbo.getHeight());
//    terrainShader.setUniform1f("terrainHeight", (0.15*size)*terrainHeight);
//    
//    if ( pointsAlpha>0.0){
//        ofSetColor(255);
//        terrainMesh.drawVertices();
//	}
//    
//    if ( terrainHorizontalAlpha>0.0){
//        ofSetColor(255*terrainHorizontalAlpha);
//        terrainHorizontal.drawWireframe();
//    }
//    
//    if ( terrainVerticalAlpha>0.0){
//        ofSetColor(255*terrainVerticalAlpha);
//        terrainVertical.drawWireframe();
//    }
//    
//    terrainShader.end();
    
    
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

void CloudsVisualSystemTerrain::selfBegin()
{
    
}

void CloudsVisualSystemTerrain::selfEnd()
{
    
}

void CloudsVisualSystemTerrain::selfKeyPressed(ofKeyEventArgs & args)
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

void CloudsVisualSystemTerrain::selfGuiEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemTerrain::guiRenderEvent(ofxUIEventArgs &e)
{
    
}