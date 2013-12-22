//
//  CloudsVisualSystemPhotoGlitch.cpp
//

#include "CloudsVisualSystemPhotoGlitch.h"


//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

static int kVertsPerCell   = 4;
static int kCoordsPerVert  = 2;
static int kIndicesPerCell = 6;

static ofxEasingQuad easing;

PGCell * CloudsVisualSystemPhotoGlitch::cells;
PGCell * CloudsVisualSystemPhotoGlitch::targetCells;

bool CloudsVisualSystemPhotoGlitch::sortIdxForHue(int i, int j) {
    return (cells[i].avgColor.getHue() < cells[j].avgColor.getHue());
}

bool CloudsVisualSystemPhotoGlitch::sortIdxForHueTarget(int i, int j) {
    return (targetCells[i].avgColor.getHue() < targetCells[j].avgColor.getHue());
}

bool CloudsVisualSystemPhotoGlitch::sortIdxForBri(int i, int j) {
    return (cells[i].avgColor.getBrightness() < cells[j].avgColor.getBrightness());
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemPhotoGlitch::selfSetupGui()
{
	customGui = new ofxUISuperCanvas("PhotoGlitch", gui);
	customGui->copyCanvasStyle(gui);
	customGui->copyCanvasProperties(gui);
	customGui->setName("PhotoGlitch");
	customGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    customGui->addSpacer();
    vector<string> imageNames;
    for (int i = 0; i < imagesDir.size(); i++) {
        imageNames.push_back(imagesDir.getName(i));
    }
    
    vector<string> targetImageNames;
    for(int i =0; i<targetImagesDir.size(); i++){
        targetImageNames.push_back(targetImagesDir.getName(i));
    }
    customGui->addLabel("SOURCE IMAGE");
    customGui->addRadio("SOURCE IMAGES", imageNames);
    customGui->addLabel("TARGET IMAGE");
    customGui->addRadio("TARGET IMAGES", targetImageNames);

    customGui->addSpacer();
    customGui->addIntSlider("NUM COLS", 1, 400, &numDivCols);
    customGui->addIntSlider("NUM ROWS", 1, 400, &numDivRows);

    customGui->addSpacer();
    customGui->addToggle("USE COLORS", &bUseColors);
    customGui->addToggle("USE TEXTURE", &bUseTexture);

    customGui->addSpacer();
    customGui->addToggle("DRAW TARGET", &bDrawTarget);
	customGui->addToggle("SHUFFLE", &bShouldShuffle);
    customGui->addToggle("SORT HUE", &bShouldSortHue);
    customGui->addToggle("SORT BRI", &bShouldSortBri);
    customGui->addToggle("SORT TARGET", &bShouldSortTarget);
    customGui->addToggle("REORDER", &bShouldReorder);
    customGui->addToggle("PERPENDICULAR", &bDoPerpendicular);
    customGui->addIntSlider("TWEEN DURATION", 1, 1000, &tweenDuration);
    customGui->addIntSlider("TWEEN DELAY", 0, 1000, &tweenDelay);

	ofAddListener(customGui->newGUIEvent, this, &CloudsVisualSystemPhotoGlitch::selfGuiEvent);
	guis.push_back(customGui);
	guimap[customGui->getName()] = customGui;
}

void CloudsVisualSystemPhotoGlitch::selfGuiEvent(ofxUIEventArgs &e)
{

    if (e.getName() == "NUM COLS" || e.getName() == "NUM ROWS") {
        bShouldGenerate = true;
    }
    else {
        // Look through the files dropdown for a match.
        string name = e.widget->getName();
        for (int i = 0; i < imagesDir.numFiles(); i++) {
            if (name == imagesDir.getName(i) && ((ofxUIToggle *)e.widget)->getValue()) {
                cout<<"source img name : "<<name<<endl;
                selectedSrcImageIdx = i;
                bShouldGenerate = true;
                break;
            }
        }
        
        for (int i =0; i < targetImagesDir.numFiles(); i++){
            if (name == targetImagesDir.getName(i) && ((ofxUIToggle *)e.widget)->getValue()){
                cout<<"target img name : "<<name<<endl;
                selectedTargetImageIdx = i;
                bShouldGenerate = true;
                break;
            }
        }
    }
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemPhotoGlitch::selfSetupSystemGui(){
	
}

void CloudsVisualSystemPhotoGlitch::guiSystemEvent(ofxUIEventArgs &e){
	
}

//use render gui for display settings, like changing colors
void CloudsVisualSystemPhotoGlitch::selfSetupRenderGui(){

}

void CloudsVisualSystemPhotoGlitch::guiRenderEvent(ofxUIEventArgs &e){
	
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemPhotoGlitch::selfSetup()
{
    bIs2D = true;
    
    // Set defaults.
    bUseColors  = false;
    bUseTexture = true;
    
    bShouldShuffle = false;
    bShouldSortHue = false;
    bShouldSortBri = false;
    bShouldReorder = false;
    bShouldSortTarget = false;
    
    bDoPerpendicular = false;
        
    tweenDuration = 200;
    tweenDelay = 0;
    
    numDivCols = 20;
    numDivRows = 20;
    
    imagesDir.listDir(getVisualSystemDataPath() + "sourceImages" );
    imagesDir.sort();
    
    targetImagesDir.listDir(getVisualSystemDataPath() + "targetImages" );
    targetImagesDir.sort();
    selectedSrcImageIdx = 0;
    selectedTargetImageIdx = 0;
    
    bShouldGenerate = true;
}

void CloudsVisualSystemPhotoGlitch::clear()
{
    if (cells != NULL) {
        delete [] cells;
        cells = NULL;
    }
    
    if (verts != NULL) {
        delete [] verts;
        verts = NULL;
    }
    
    if (texCoords != NULL) {
        delete [] texCoords;
        texCoords = NULL;
    }
    if( targetTexCoords != NULL){
        delete [] targetTexCoords;
        targetTexCoords = NULL;
    }
    
    if (colors != NULL) {
        delete [] colors;
        colors = NULL;
    }
    
    if (targetColors != NULL) {
        delete [] targetColors;
        targetColors = NULL;
    }
    
    if (indices != NULL) {
        delete [] indices;
        indices = NULL;
    }
    
    tex.clear();
}

void CloudsVisualSystemPhotoGlitch::generate()
{
    clear();
    
    tex.loadImage(imagesDir.getPath(selectedSrcImageIdx));
    ofPixels pixels = tex.getPixelsRef();
    
    targetTex.loadImage(targetImagesDir.getPath(selectedTargetImageIdx));
    ofPixels targetPixels = targetTex.getPixelsRef();
    
    numCells = numDivCols * numDivRows;
    
    cells = new PGCell[numCells];
    targetCells = new PGCell[numCells];
    
    screenSliceWidth = ofGetWidth() / (float)numDivCols;
    screenSliceHeight = ofGetHeight() / (float)numDivRows;
    texSliceWidth = tex.getWidth() / (float)numDivCols;
    texSliceHeight = tex.getHeight() / (float)numDivRows;
    targetTexSliceWidth =  targetTex.getWidth() / (float)numDivCols;
    targetTexSliceHeight = targetTex.getHeight() / (float)numDivRows;
    
    // Build the mesh data.
    numVerts = numCells * kVertsPerCell;
    verts = new GLfloat[numVerts * kCoordsPerVert];
    texCoords = new GLfloat[numVerts * 2];
    targetTexCoords = new GLfloat[numVerts * 2];
    colors = new GLfloat[numVerts * 4];
    targetColors = new GLfloat[numVerts * 4];
    
    numIndices = numCells * kIndicesPerCell;
    indices = new GLuint[numIndices];
    
    for (int j = 0; j < numDivRows; j++) {
        for (int i = 0; i < numDivCols; i++) {
            int idx = j * numDivCols + i;
            
            // Add verts. Same for both source and target
            verts[idx * kVertsPerCell * kCoordsPerVert + 0] = (i + 0) * screenSliceWidth;
            verts[idx * kVertsPerCell * kCoordsPerVert + 1] = (j + 0) * screenSliceHeight;
            
            verts[idx * kVertsPerCell * kCoordsPerVert + 2] = (i + 1) * screenSliceWidth;
            verts[idx * kVertsPerCell * kCoordsPerVert + 3] = (j + 0) * screenSliceHeight;
            
            verts[idx * kVertsPerCell * kCoordsPerVert + 4] = (i + 0) * screenSliceWidth;
            verts[idx * kVertsPerCell * kCoordsPerVert + 5] = (j + 1) * screenSliceHeight;
            
            verts[idx * kVertsPerCell * kCoordsPerVert + 6] = (i + 1) * screenSliceWidth;
            verts[idx * kVertsPerCell * kCoordsPerVert + 7] = (j + 1) * screenSliceHeight;
            
            ////////

//            // Add Target image verts.
//            targetVerts[idx * kVertsPerCell * kCoordsPerVert + 0] = (i + 0) * screenSliceWidth;
//            targetVerts[idx * kVertsPerCell * kCoordsPerVert + 1] = (j + 0) * screenSliceHeight;
//            
//            targetVerts[idx * kVertsPerCell * kCoordsPerVert + 2] = (i + 1) * screenSliceWidth;
//            targetVerts[idx * kVertsPerCell * kCoordsPerVert + 3] = (j + 0) * screenSliceHeight;
//            
//            targetVerts[idx * kVertsPerCell * kCoordsPerVert + 4] = (i + 0) * screenSliceWidth;
//            targetVerts[idx * kVertsPerCell * kCoordsPerVert + 5] = (j + 1) * screenSliceHeight;
//            
//            targetVerts[idx * kVertsPerCell * kCoordsPerVert + 6] = (i + 1) * screenSliceWidth;
//            targetVerts[idx * kVertsPerCell * kCoordsPerVert + 7] = (j + 1) * screenSliceHeight;
//            ////////
            
            // Add tex coords.
            texCoords[idx * kVertsPerCell * 2 + 0] = (i + 0) * texSliceWidth;
            texCoords[idx * kVertsPerCell * 2 + 1] = (j + 0) * texSliceHeight;
            
            texCoords[idx * kVertsPerCell * 2 + 2] = (i + 1) * texSliceWidth;
            texCoords[idx * kVertsPerCell * 2 + 3] = (j + 0) * texSliceHeight;
            
            texCoords[idx * kVertsPerCell * 2 + 4] = (i + 0) * texSliceWidth;
            texCoords[idx * kVertsPerCell * 2 + 5] = (j + 1) * texSliceHeight;
            
            texCoords[idx * kVertsPerCell * 2 + 6] = (i + 1) * texSliceWidth;
            texCoords[idx * kVertsPerCell * 2 + 7] = (j + 1) * texSliceHeight;
            

            // Add target tex coords.
            targetTexCoords[idx * kVertsPerCell * 2 + 0] = (i + 0) * targetTexSliceWidth;
            targetTexCoords[idx * kVertsPerCell * 2 + 1] = (j + 0) * targetTexSliceHeight;
         
            targetTexCoords[idx * kVertsPerCell * 2 + 2] = (i + 1) * targetTexSliceWidth;
            targetTexCoords[idx * kVertsPerCell * 2 + 3] = (j + 0) * targetTexSliceHeight;
         
            targetTexCoords[idx * kVertsPerCell * 2 + 4] = (i + 0) * targetTexSliceWidth;
            targetTexCoords[idx * kVertsPerCell * 2 + 5] = (j + 1) * targetTexSliceHeight;
         
            targetTexCoords[idx * kVertsPerCell * 2 + 6] = (i + 1) * targetTexSliceWidth;
            targetTexCoords[idx * kVertsPerCell * 2 + 7] = (j + 1) * targetTexSliceHeight;

       
                        
            // Add indices for triangles.
            indices[idx * kIndicesPerCell + 0] = idx * kVertsPerCell + 0;  // top-left
            indices[idx * kIndicesPerCell + 1] = idx * kVertsPerCell + 1;  // top-right
            indices[idx * kIndicesPerCell + 2] = idx * kVertsPerCell + 2;  // bottom-left
            
            indices[idx * kIndicesPerCell + 3] = idx * kVertsPerCell + 1;  // top-right
            indices[idx * kIndicesPerCell + 4] = idx * kVertsPerCell + 3;  // bottom-right
            indices[idx * kIndicesPerCell + 5] = idx * kVertsPerCell + 2;  // bottom-left
            
            // Calculate the average source color.
            int avgR = 0, avgG = 0, avgB = 0, avgA = 0;
            for (int y = (j + 0) * texSliceHeight; y < (j + 1) * texSliceHeight; y++) {
                for (int x = (i + 0) * texSliceWidth; x < (i + 1) * texSliceWidth; x++) {
                    ofColor c = pixels.getColor(x, y);
                    avgR += c.r;
                    avgG += c.g;
                    avgB += c.b;
                    avgA += c.a;
                }
            }
            cells[idx].avgColor.set((avgR / (texSliceWidth * texSliceHeight)) / 255.0f,
                                    (avgG / (texSliceWidth * texSliceHeight)) / 255.0f,
                                    (avgB / (texSliceWidth * texSliceHeight)) / 255.0f,
                                    (avgA / (texSliceWidth * texSliceHeight)) / 255.0f);
            // Add colors.
            colors[idx * kVertsPerCell * 4 +  0] = cells[idx].avgColor.r;
            colors[idx * kVertsPerCell * 4 +  1] = cells[idx].avgColor.g;
            colors[idx * kVertsPerCell * 4 +  2] = cells[idx].avgColor.b;
            colors[idx * kVertsPerCell * 4 +  3] = cells[idx].avgColor.a;
            
            colors[idx * kVertsPerCell * 4 +  4] = cells[idx].avgColor.r;
            colors[idx * kVertsPerCell * 4 +  5] = cells[idx].avgColor.g;
            colors[idx * kVertsPerCell * 4 +  6] = cells[idx].avgColor.b;
            colors[idx * kVertsPerCell * 4 +  7] = cells[idx].avgColor.a;
            
            colors[idx * kVertsPerCell * 4 +  8] = cells[idx].avgColor.r;
            colors[idx * kVertsPerCell * 4 +  9] = cells[idx].avgColor.g;
            colors[idx * kVertsPerCell * 4 + 10] = cells[idx].avgColor.b;
            colors[idx * kVertsPerCell * 4 + 11] = cells[idx].avgColor.a;
            
            colors[idx * kVertsPerCell * 4 + 12] = cells[idx].avgColor.r;
            colors[idx * kVertsPerCell * 4 + 13] = cells[idx].avgColor.g;
            colors[idx * kVertsPerCell * 4 + 14] = cells[idx].avgColor.b;
            colors[idx * kVertsPerCell * 4 + 15] = cells[idx].avgColor.a;
            
            //doing the same for target image
            avgR = 0;
            avgG = 0;
            avgB = 0;
            avgA = 0;
            for (int y = (j + 0) * targetTexSliceHeight; y < (j + 1) * targetTexSliceHeight; y++) {
                for (int x = (i + 0) * targetTexSliceWidth; x < (i + 1) * targetTexSliceWidth; x++) {
                    ofColor c = targetPixels.getColor(x, y);
                    avgR += c.r;
                    avgG += c.g;
                    avgB += c.b;
                    avgA += c.a;
                }
            }
            
            targetCells[idx].avgColor.set((avgR / (targetTexSliceWidth * targetTexSliceHeight)) / 255.0f,
                                          (avgG / (targetTexSliceWidth * targetTexSliceHeight)) / 255.0f,
                                          (avgB / (targetTexSliceWidth * targetTexSliceHeight)) / 255.0f,
                                          (avgA / (targetTexSliceWidth * targetTexSliceHeight)) / 255.0f);
            
            // Add colors.
            targetColors[idx * kVertsPerCell * 4 +  0] = targetCells[idx].avgColor.r;
            targetColors[idx * kVertsPerCell * 4 +  1] = targetCells[idx].avgColor.g;
            targetColors[idx * kVertsPerCell * 4 +  2] = targetCells[idx].avgColor.b;
            targetColors[idx * kVertsPerCell * 4 +  3] = targetCells[idx].avgColor.a;
            
            targetColors[idx * kVertsPerCell * 4 +  4] = targetCells[idx].avgColor.r;
            targetColors[idx * kVertsPerCell * 4 +  5] = targetCells[idx].avgColor.g;
            targetColors[idx * kVertsPerCell * 4 +  6] = targetCells[idx].avgColor.b;
            targetColors[idx * kVertsPerCell * 4 +  7] = targetCells[idx].avgColor.a;
            
            targetColors[idx * kVertsPerCell * 4 +  8] = targetCells[idx].avgColor.r;
            targetColors[idx * kVertsPerCell * 4 +  9] = targetCells[idx].avgColor.g;
            targetColors[idx * kVertsPerCell * 4 + 10] = targetCells[idx].avgColor.b;
            targetColors[idx * kVertsPerCell * 4 + 11] = targetCells[idx].avgColor.a;
            
            targetColors[idx * kVertsPerCell * 4 + 12] = targetCells[idx].avgColor.r;
            targetColors[idx * kVertsPerCell * 4 + 13] = targetCells[idx].avgColor.g;
            targetColors[idx * kVertsPerCell * 4 + 14] = targetCells[idx].avgColor.b;
            targetColors[idx * kVertsPerCell * 4 + 15] = targetCells[idx].avgColor.a;

            // Save the cell struct.
            cells[idx].idx = idx;
            cells[idx].col = cells[idx].origCol = i;
            cells[idx].row = cells[idx].origRow = j;
            
            targetCells[idx].idx = idx;
            targetCells[idx].col = targetCells[idx].origCol = i;
            targetCells[idx].row = targetCells[idx].origRow = j;
        }
    }
    
    vbo.setVertexData(verts, kCoordsPerVert, numVerts, GL_STREAM_DRAW, kCoordsPerVert * sizeof(GLfloat));
    vbo.setTexCoordData(texCoords, numVerts, GL_STATIC_DRAW, 2 * sizeof(GLfloat));
    vbo.setColorData(colors, numVerts, GL_STATIC_DRAW, 4 * sizeof(GLfloat));
    vbo.setIndexData(indices, numIndices, GL_STATIC_DRAW);

    targetVbo.setVertexData(verts, kCoordsPerVert, numVerts, GL_STREAM_DRAW, kCoordsPerVert * sizeof(GLfloat));
    targetVbo.setTexCoordData(targetTexCoords, numVerts, GL_STATIC_DRAW, 2 * sizeof(GLfloat));
    targetVbo.setColorData(targetColors, numVerts, GL_STATIC_DRAW, 4 * sizeof(GLfloat));
    targetVbo.setIndexData(indices, numIndices, GL_STATIC_DRAW);
    
    bShouldReorder = true;
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemPhotoGlitch::selfPresetLoaded(string presetPath)
{

}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemPhotoGlitch::selfBegin(){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemPhotoGlitch::selfSceneTransformation(){
	
}

//normal update call
void CloudsVisualSystemPhotoGlitch::selfUpdate()
{    
    if (bShouldGenerate) {
        generate();
        bShouldGenerate = false;
    }
    
    if (bShouldShuffle) {
        shuffle();
        bShouldShuffle = false;
    }
    if (bShouldSortHue) {
        sortHue();
        bShouldSortHue = false;
    }
    if (bShouldSortBri) {
        sortBri();
        bShouldSortBri = false;
    }
    if (bShouldReorder) {
        reorder();
        bShouldReorder = false;
    }
    if (bShouldSortTarget){
        sortTarget();
        bShouldSortTarget = false;
    }
    
    // tween them cells!
    for (int i = 0; i < numCells; i++) {
        int vertIdx = cells[i].idx * kVertsPerCell * kCoordsPerVert;
        
//        float tweenX = ofLerp(verts[vertIdx + 0], cells[i].col * screenSliceWidth,  0.5f);
//        float tweenY = ofLerp(verts[vertIdx + 1], cells[i].row * screenSliceHeight, 0.5f);
        float tweenX = cells[i].tweenX.update();
        float tweenY = cells[i].tweenY.update();
        
        // update the vert data
        // top-left
        verts[vertIdx + 0] = tweenX;
        verts[vertIdx + 1] = tweenY;
        // top-right
        verts[vertIdx + 2] = tweenX + screenSliceWidth;
        verts[vertIdx + 3] = tweenY;
        // bottom-left
        verts[vertIdx + 4] = tweenX;
        verts[vertIdx + 5] = tweenY + screenSliceHeight;
        // bottom-right
        verts[vertIdx + 6] = tweenX + screenSliceWidth;
        verts[vertIdx + 7] = tweenY + screenSliceHeight;
    }
    
    // update that data
    vbo.setVertexData(verts, kCoordsPerVert, numVerts, GL_STREAM_DRAW, kCoordsPerVert * sizeof(GLfloat));
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemPhotoGlitch::selfDraw(){
    
}

// draw any debug stuff here
void CloudsVisualSystemPhotoGlitch::selfDrawDebug(){

}

// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemPhotoGlitch::selfDrawBackground()
{    
    if (bUseColors) {
     vbo.enableColors();
        targetVbo.enableColors();
    }
    else{
     vbo.disableColors();
        targetVbo.disableColors();
    }
    
    if (bUseTexture){
        vbo.enableTexCoords();
        targetVbo.enableTexCoords();
    }
    else {
     vbo.disableTexCoords();
        targetVbo.disableTexCoords();
    }
    
    ofSetColor(255);
//    tex.draw(0, 0);
    tex.bind();
    {
//        vbo.draw(GL_TRIANGLES, 0, 24);
//        glPushAttrib(GL_POLYGON_BIT);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        vbo.drawElements(GL_TRIANGLES, numIndices);
//        glPopAttrib();
    }
    tex.unbind();

    if(bDrawTarget){
        targetTex.bind();
        targetVbo.drawElements(GL_TRIANGLES, numIndices);
        targetTex.unbind();
    } 

    // Debug avg colors.
//    for (int i = 0; i < numCells; i++) {
//        ofSetColor(cells[i].avgColor);
//        ofRect(cells[i].col * screenSliceWidth, cells[i].row * screenSliceHeight, screenSliceWidth, screenSliceHeight);
//    }
}

// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemPhotoGlitch::selfEnd(){
	
}

// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemPhotoGlitch::selfExit()
{
    clear();
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemPhotoGlitch::selfKeyPressed(int key)
{

}

void CloudsVisualSystemPhotoGlitch::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemPhotoGlitch::selfMouseDragged(int x, int y, int button){

}

void CloudsVisualSystemPhotoGlitch::selfMouseMoved(int x, int y, int button){
	
}

void CloudsVisualSystemPhotoGlitch::selfMousePressed(int x, int y, int button){
	
}

void CloudsVisualSystemPhotoGlitch::selfMouseReleased(int x, int y, int button){
	
}

void CloudsVisualSystemPhotoGlitch::shuffle()
{
    bool * slots = new bool[numCells];
    for (int i = 0; i < numCells; i++) {
        slots[i] = false;
    }
    
    int numOccupied = 0;
    int lastOpenSlotIdx = 0;
    for (int i = 0; i < numCells; i++) {
        int openSlotIdx = 0;
        if (numOccupied < numCells * 0.75f) {
            // find an open slot randomly
            do {
                openSlotIdx = ofRandom(numCells - 1);
            } while (slots[openSlotIdx] == true);
        }
        else {
            // find an open slot sequentially
            for (int j = lastOpenSlotIdx; j < numCells; j++) {
                if (slots[j] == false) {
                    // boom, found one
                    openSlotIdx = lastOpenSlotIdx = j;
                    break;
                }
            }
        }
        
        // get the row and col from the index
        int openCol = openSlotIdx % numDivCols;
        int openRow = openSlotIdx / numDivCols;
        
        // set the target position using the row and col indices
        cells[i].col = openCol;
        cells[i].row = openRow;
        
        tween(i, openSlotIdx);
//        cout << "setting new pos to " << cells[i].destX << " x " << cells[i].destY << endl;
        
        slots[openSlotIdx] = true;
        ++numOccupied;
    }
    
    delete [] slots;
}

void CloudsVisualSystemPhotoGlitch::sortHue()
{
    vector<int> slots;
    for (int i = 0; i < numCells; i++) {
        slots.push_back(i);
    }
    
    sort(slots.begin(), slots.end(), CloudsVisualSystemPhotoGlitch::sortIdxForHue);
    
    for (int i = 0; i < numCells; i++) {
        int slotIdx = slots[i];
        
        // set the target position using the row and col indices
        cells[i].col = slotIdx % numDivCols;
        cells[i].row = slotIdx / numDivCols;
        
        tween(i, slotIdx);
    }   
}

void CloudsVisualSystemPhotoGlitch::sortBri()
{
    vector<int> slots;
    for (int i = 0; i < numCells; i++) {
        slots.push_back(i);
    }
    
    sort(slots.begin(), slots.end(), CloudsVisualSystemPhotoGlitch::sortIdxForBri);
    
    for (int i = 0; i < numCells; i++) {
        int slotIdx = slots[i];
        
        // set the target position using the row and col indices
        cells[i].col = slotIdx % numDivCols;
        cells[i].row = slotIdx / numDivCols;
        
        tween(i, slotIdx);
    }
}

void CloudsVisualSystemPhotoGlitch::sortTarget(){
    vector<int> sourceSlots;
    vector<int> targetSlots;
    for (int i = 0; i < numCells; i++) {
        sourceSlots.push_back(i);
        targetSlots.push_back(i);
    }
    
    sort(sourceSlots.begin(), sourceSlots.end(), CloudsVisualSystemPhotoGlitch::sortIdxForHue);
    sort(targetSlots.begin(), targetSlots.end(), CloudsVisualSystemPhotoGlitch::sortIdxForHueTarget);
    
    for (int i = 0; i < numCells; i++) {

        cells[sourceSlots[i]].col = targetCells[targetSlots[i]].col;
        cells[sourceSlots[i]].row = targetCells[targetSlots[i]].row;
        
        tween(i);
    }
}

void CloudsVisualSystemPhotoGlitch::reorder()
{
    for (int i = 0; i < numCells; i++) {
        cells[i].col = cells[i].origCol;
        cells[i].row = cells[i].origRow;
        
        tween(i);
    }
}

void CloudsVisualSystemPhotoGlitch::tweenAll()
{
    for (int i = 0; i < numCells; i++) {
        tween(i);
    }
}

void CloudsVisualSystemPhotoGlitch::tween(int i, int j)
{   
    if (j == -1) j = i;
    
    int vertIdx = cells[i].idx * kVertsPerCell * kCoordsPerVert;
    
    if (bDoPerpendicular) {
        cells[i].tweenX.setParameters(easing, ofxTween::easeOut, verts[vertIdx + 0], cells[i].col * screenSliceWidth,  tweenDuration / 2, tweenDelay * j);
        cells[i].tweenY.setParameters(easing, ofxTween::easeOut, verts[vertIdx + 1], cells[i].row * screenSliceHeight, tweenDuration / 2, tweenDuration / 2 + tweenDelay * j);
    }
    else {
        cells[i].tweenX.setParameters(easing, ofxTween::easeOut, verts[vertIdx + 0], cells[i].col * screenSliceWidth,  tweenDuration, tweenDelay * j);
        cells[i].tweenY.setParameters(easing, ofxTween::easeOut, verts[vertIdx + 1], cells[i].row * screenSliceHeight, tweenDuration, tweenDelay * j);
    }
    
    cells[i].tweenX.start();
    cells[i].tweenY.start();
}

