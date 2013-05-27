//
//  CloudsVisualSystemCities.cpp
//
//  Created by Patricio Gonzalez Vivo on 5/26/13.
//
//

#include "CloudsVisualSystemCities.h"

string CloudsVisualSystemCities::getSystemName()
{
	return "Cities";
}

void CloudsVisualSystemCities::selfSetup()
{
    size = 100;
    nGrains = 10;
    nPingPong = 0;
    
    noiseShader.load("", getDataPath()+"shaders/noise.fs");
    grayscottShader.load("", getDataPath()+"shaders/grayscott.fs");
    maskShader.load("", getDataPath()+"shaders/cMask.fs");
    
    makeGrid(100, 10);
}

void CloudsVisualSystemCities::selfSetupGuis()
{
    
}

void CloudsVisualSystemCities::selfAutoMode()
{
    
}


void CloudsVisualSystemCities::selfDrawBackground()
{
    
}

void CloudsVisualSystemCities::selfDrawDebug()
{
    
}

void CloudsVisualSystemCities::selfSceneTransformation()
{
    
}

void generateCube(float sizeX, float sizeY, float sizeZ)
{
    ofPushMatrix();
    
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    if(ofGetStyle().bFill) {
        GLfloat vertices[] = {
            +sizeX,-sizeY,+sizeZ, +sizeX,-sizeY,-sizeZ, +sizeX,+sizeY,-sizeZ, +sizeX,+sizeY,+sizeZ,
            +sizeX,+sizeY,+sizeZ, +sizeX,+sizeY,-sizeZ, -sizeX,+sizeY,-sizeZ, -sizeX,+sizeY,+sizeZ,
            +sizeX,+sizeY,+sizeZ, -sizeX,+sizeY,+sizeZ, -sizeX,-sizeY,+sizeZ, +sizeX,-sizeY,+sizeZ,
            -sizeX,-sizeY,+sizeZ, -sizeX,+sizeY,+sizeZ, -sizeX,+sizeY,-sizeZ, -sizeX,-sizeY,-sizeZ,
            -sizeX,-sizeY,+sizeZ, -sizeX,-sizeY,-sizeZ, +sizeX,-sizeY,-sizeZ, +sizeX,-sizeY,+sizeZ,
            -sizeX,-sizeY,-sizeZ, -sizeX,+sizeY,-sizeZ, +sizeX,+sizeY,-sizeZ, +sizeX,-sizeY,-sizeZ
        };
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        
        static GLfloat normals[] = {
            +1,0,0, +1,0,0, +1,0,0, +1,0,0,
            0,+1,0, 0,+1,0, 0,+1,0, 0,+1,0,
            0,0,+1, 0,0,+1, 0,0,+1, 0,0,+1,
            -1,0,0, -1,0,0, -1,0,0, -1,0,0,
            0,-1,0, 0,-1,0, 0,-1,0, 0,-1,0,
            0,0,-1, 0,0,-1, 0,0,-1, 0,0,-1
        };
        glNormalPointer(GL_FLOAT, 0, normals);
        
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        static GLfloat tex[] = {
            1,0, 0,0, 0,1, 1,1,
            1,1, 1,0, 0,0, 0,1,
            0,1, 1,1, 1,0, 0,0,
            0,0, 0,1, 1,1, 1,0,
            0,0, 0,1, 1,1, 1,0,
            0,0, 0,1, 1,1, 1,0
        };
        glTexCoordPointer(2, GL_FLOAT, 0, tex);
        
        GLubyte indices[] = {
            0,1,2, // right top left
            0,2,3, // right bottom right
            4,5,6, // bottom top right
            4,6,7, // bottom bottom left
            8,9,10, // back bottom right
            8,10,11, // back top left
            12,13,14, // left bottom right
            12,14,15, // left top left
            16,17,18, // ... etc
            16,18,19,
            20,21,22,
            20,22,23
        };
        glDrawElements(GL_TRIANGLES, 3 * 6 * 2, GL_UNSIGNED_BYTE, indices);
        
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    } else {
        GLfloat vertices[] = {
            +sizeX,+sizeY,+sizeZ,
            +sizeX,+sizeY,-sizeZ,
            +sizeX,-sizeY,+sizeZ,
            +sizeX,-sizeY,-sizeZ,
            -sizeX,+sizeY,+sizeZ,
            -sizeX,+sizeY,-sizeZ,
            -sizeX,-sizeY,+sizeZ,
            -sizeX,-sizeY,-sizeZ
        };
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        
        static float n = sqrtf(3);
        static GLfloat normals[] = {
            +n,+n,+n,
            +n,+n,-n,
            +n,-n,+n,
            +n,-n,-n,
            -n,+n,+n,
            -n,+n,-n,
            -n,-n,+n,
            -n,-n,-n
        };
        glNormalPointer(GL_FLOAT, 0, normals);
        
        static GLubyte indices[] = {
            0,1, 1,3, 3,2, 2,0,
            4,5, 5,7, 7,6, 6,4,
            0,4, 5,1, 7,3, 6,2
        };
        glDrawElements(GL_LINES, 4 * 2 * 3, GL_UNSIGNED_BYTE, indices);
    }
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    
    ofPopMatrix();
}

void CloudsVisualSystemCities::makeGrid(float _size, int _nGrains)
{
    size = _size;
    nGrains = _nGrains;
    grainResolution = size/nGrains;
    
    cube = glGenLists(1);
    float cubeSize = grainResolution*0.5;
    glNewList(cube, GL_COMPILE);
    generateCube(cubeSize, cubeSize, cubeSize);
    glEndList();
    
    int textureSize = nGrains*2;
    noiseFbo.allocate(textureSize,textureSize);
    noiseFbo.begin();
    ofClear(0);
    noiseFbo.end();
    
    for(int i = 0; i < 2; i++){
        grayscottFbo[i].allocate(textureSize, textureSize);
        grayscottFbo[i].begin();
        ofClear(0);
        grayscottFbo[i].end();
    }
    
    maskFbo.allocate(textureSize, textureSize);
    maskFbo.begin();
    ofClear(0);
    maskFbo.end();
}

void CloudsVisualSystemCities::selfUpdate()
{
    ofPushStyle();
    
    ofSetColor(255);
    
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
    
    if(bGrayscott){
        nPingPong = (nPingPong+1)%2;
        
        if (bCleanGrayscott){
            grayscottFbo[(nPingPong+1)%2].begin();
            ofClear(0,0);
            grayscottFbo[(nPingPong+1)%2].end();
        }
        
        grayscottFbo[nPingPong%2].begin();
        if (bCleanGrayscott){
            ofClear(0,0);
            bCleanGrayscott = false;
        }
        
        grayscottShader.begin();
        grayscottShader.setUniformTexture("backbuffer", grayscottFbo[(nPingPong+1)%2], 1);
        grayscottShader.setUniformTexture("tex0", noiseFbo, 2);
        grayscottShader.setUniform1f("diffU", 0.25);
        grayscottShader.setUniform1f("diffV", 0.04);
        grayscottShader.setUniform1f("k", 0.047);
        grayscottShader.setUniform1f("f", 0.2);
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
    
    maskFbo.begin();
    ofClear(0);
    maskShader.begin();
    maskShader.setUniform2f("resolution", maskFbo.getWidth(), maskFbo.getHeight());
    maskShader.setUniform1f("size", maskSize);
    maskShader.setUniform1f("curve",maskCurve);
    
    if(bGrayscott){
        grayscottFbo[nPingPong%2].draw(0, 0);
    } else {
        noiseFbo.draw(0, 0);
    }
    maskShader.end();
    maskFbo.end();
    
    ofPopStyle();
}

void CloudsVisualSystemCities::selfDraw()
{
    mat->begin();
    ofPushMatrix();
    
    glEnable(GL_DEPTH_TEST);
    ofTranslate(-size*0.5,-size*0.5);
    ofFill();
    
    ofSetColor(255);
    ofFloatPixels heightPixels;
    maskFbo.getTextureReference().readToPixels(heightPixels);
    
    int jump = heightPixels.getWidth()/nGrains;
    
    for(int x = 0; x < nGrains; x++){
        for(int y = 0; y < nGrains; y++){

            float value = heightPixels.getColor(x*jump,y*jump).b;
            
            ofPushMatrix();
            ofTranslate(x*grainResolution,y*grainResolution, maxHeight*value*0.5*grainResolution );
            
            if ( value > 0.0){
                ofSetColor(255, MAX(minAlpha*255,55+value*200.0) );
                ofScale((1.0*(1.0-minDist))-(value*minSize),
                        (1.0*(1.0-minDist))-(value*minSize),
                        maxHeight*value);
                glCallList(cube);
            }
            ofPopMatrix();
        }
    }
    
    glDisable(GL_DEPTH_TEST);
    
    ofPopMatrix();
    mat->end();
    
//    grayscottFbo[nPingPong%2].draw(0, 0);

}

void CloudsVisualSystemCities::billBoard()
{
    
}

void CloudsVisualSystemCities::selfExit()
{
    
}

void CloudsVisualSystemCities::selfBegin()
{
    
}

void CloudsVisualSystemCities::selfEnd()
{
    
}

void CloudsVisualSystemCities::selfKeyPressed(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemCities::selfKeyReleased(ofKeyEventArgs & args)
{
    
}

void CloudsVisualSystemCities::selfMouseDragged(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemCities::selfMouseMoved(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemCities::selfMousePressed(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemCities::selfMouseReleased(ofMouseEventArgs& data)
{
    
}

void CloudsVisualSystemCities::selfSetupGui()
{
    
}

void CloudsVisualSystemCities::selfGuiEvent(ofxUIEventArgs &e)
{
    
}


void CloudsVisualSystemCities::selfSetupSystemGui()
{
    sysGui->addLabel("Blocks");
    sysGui->addSlider("Grid_size", 10, 200, &size);
    sysGui->addSlider("Blocks_number", 1, 100, &nGrains);
    sysGui->addSlider("Max_Height", 0, 100, &maxHeight);
    
    sysGui->addSlider("Min_Dist", 0.0, 0.5, &minDist);
    sysGui->addSlider("Min_Size", 0.0, 1.0, &minSize);
    sysGui->addSlider("Min_Alpha", 0.0, 1.0, &minAlpha);
    
    sysGui->addLabel("Noise");
    sysGui->addSlider("noise_zoom", 0.0, 100.0, &noiseZoom);
    sysGui->addSlider("noise_speed", 0.0, 5.0, &noiseSpeed);
    
    sysGui->addLabel("GrayScott");
    sysGui->addSlider("Feed", 0.0, 0.1, &grayscottFade);
    sysGui->addToggle("enable", &bGrayscott);
    sysGui->addButton("clean", &bCleanGrayscott);
    
    sysGui->addLabel("Mask");
    sysGui->addSlider("maskSize", 1.0, 2.0, &maskSize);
    sysGui->addSlider("maskCurve", 0.0, 1.0, &maskCurve);
}

void CloudsVisualSystemCities::guiSystemEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    if (name == "Grid_size" || name == "Blocks_number"){
        makeGrid(size, nGrains);
    }
}

void CloudsVisualSystemCities::selfSetupRenderGui()
{
    
}

void CloudsVisualSystemCities::guiRenderEvent(ofxUIEventArgs &e)
{
    
}