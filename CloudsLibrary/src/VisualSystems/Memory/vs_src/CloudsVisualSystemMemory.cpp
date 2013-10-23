//
//  CloudsVisualSystemMemory.cpp
//  VSMemory
//
//  Created by Patricio Gonzalez Vivo on 6/20/13.
//
//

#include "CloudsVisualSystemMemory.h"


string CloudsVisualSystemMemory::getSystemName()
{
	return "Memory";
}

void CloudsVisualSystemMemory::selfSetup()
{
    blockScale = 1.0;
    blockWidth = 4;
    blockHeight = 8;
    margin = 2;
    noiseLerp = 0;
    randomUp = 0;
    randomDown = 0;

    borderColor = ofFloatColor(0.0823, 0.8509, 0.7960);
    bTexture = false;
    bSort = true;
    bDeFrag = false;
    bBiDirectionalSort = false;
    
    ofEnableAlphaBlending();
    generate();
}

void CloudsVisualSystemMemory::selfSetupSystemGui()
{
    sysGui->addLabel("Source");
    sysGui->addToggle("Texture", &bTexture);
    
    sysGui->addLabel("Order");
    sysGui->addToggle("Sort", &bSort);
    sysGui->addToggle("BiDirectionalSort", &bBiDirectionalSort);
    sysGui->addSlider("Random_Sort", 0.0, 1000, &randomSort);
    sysGui->addToggle("DeFrag", &bDeFrag);
    
    sysGui->addLabel("Chaos");
    sysGui->addSlider("Random_Mix", 0.0, 100, &randomMix);
    sysGui->addSlider("Random_Up", 0.0, 100, &randomUp);
    sysGui->addSlider("Random_Down", 0.0, 100, &randomDown);
    sysGui->addSlider("Noise_Lerp", 0.0, 1.0, &noiseLerp);
}

void CloudsVisualSystemMemory::selfSetupRenderGui()
{
    rdrGui->addSlider("margin", 0.0, 10, &margin);
    
    rdrGui->addSlider("block_width", 0.0, 20, &blockWidth);
    rdrGui->addSlider("block_height", 0.0, 20, &blockHeight);
    rdrGui->addSlider("block_scale", 0.5
                      , 10, &blockScale);
    rdrGui->addSlider("block_border", 0.0, 1.0, &(borderColor.a));
    rdrGui->addSlider("border_red", 0.0, 1.0, &(borderColor.r));
    rdrGui->addSlider("border_green", 0.0, 1.0, &(borderColor.g));
    rdrGui->addSlider("border_blue", 0.0, 1.0, &(borderColor.b));
}

void CloudsVisualSystemMemory::selfBegin()
{
    generate();
}

void CloudsVisualSystemMemory::selfEnd()
{
    
}

void CloudsVisualSystemMemory::guiSystemEvent(ofxUIEventArgs &e)
{
    
}

void CloudsVisualSystemMemory::guiRenderEvent(ofxUIEventArgs &e)
{
    generate();
}

void CloudsVisualSystemMemory::selfKeyPressed(ofKeyEventArgs & args){
    generate();
}

void CloudsVisualSystemMemory::generate(){
    if (bTexture){
        
        //  Player
        //
//        generateFromTexture( sharedRenderer->getPlayer().getTextureReference() );
        
        //  Image
        //
        ofImage img;
        img.loadImage( getVisualSystemDataPath() + "images/img.jpg");
        generateFromTexture( img.getTextureReference() );
        
        //  FBO
        //
//        ofFbo fbo;
//        fbo.allocate(ofGetScreenWidth(), ofGetScreenHeight());
//        generateFromTexture( fbo.getTextureReference() );
    } else {
        generateFromMemory();
    }
}

void CloudsVisualSystemMemory::generateFromMemory(){
    
    int blocksTotal = 10000*(10-blockScale);
    unsigned char * data = new unsigned char[blocksTotal];
    
    int xMargin = 20;
    int yMargin = 20;
    
    int width = ofGetWidth()-xMargin*2.0;
    int height = ofGetHeight()-yMargin*2.0;
    
    float widthBlocks = blockWidth*blockScale;
    float heightBlocks = blockHeight*blockScale;
    
    xBlocks = (float)width/(widthBlocks+margin*blockScale);
    yBlocks = (float)height/(heightBlocks+margin*blockScale);
    
    blocks.clear();
    int index = 0;
    for (int j = 0; j < yBlocks; j++) {
        for (int i = 0; i < xBlocks; i++){
            
            if (index < blocksTotal ){
                
                int x = xMargin + ((margin + blockWidth)*blockScale)*i ;
                int y = yMargin + ((margin + blockHeight)*blockScale)*j ;
                
                if ( y > (ofGetHeight()+margin+heightBlocks))
                    break;
                
                Block block;
                block.x = x+widthBlocks*0.5;
                block.y = y+heightBlocks*0.5;
                block.width = widthBlocks;
                block.height = heightBlocks;
                block.color = ofColor((unsigned char)data[index]);
                block.borderColor = borderColor;
                block.value = (int)data[index];
                block.bSelected = false;
                
                blocks.push_back(block);
                
            } else {
                break;
            }
            
            index++;
        }
    }
    
    delete []data;
}

void CloudsVisualSystemMemory::generateFromTexture(ofTexture &_tex){

    ofPixels pixels;
    _tex.readToPixels(pixels);
    
    int xMargin = 20;
    int yMargin = 20;
    
    int width = ofGetWidth()-xMargin*2.0;
    int height = ofGetHeight()-yMargin*2.0;
    
    ofRectangle block;
    block.width = blockWidth*blockScale;
    block.height = blockHeight*blockScale;
    
    xBlocks = (float)width/((blockWidth+margin)*blockScale);
    yBlocks = (float)height/((blockHeight+margin)*blockScale);
    
    blocks.clear();
    for (int j = 0; j < yBlocks; j++) {
        for (int i = 0; i < xBlocks; i++){
            
            int x = xMargin + ((margin + blockWidth)*blockScale)*i ;
            int y = yMargin + ((margin + blockHeight)*blockScale)*j ;
            
            Block newBlock;
            newBlock.set(block);
            newBlock.x = x+block.width*0.5;
            newBlock.y = y+block.height*0.5;
            
            ofPoint st = ofPoint( ((float)i)/((float)xBlocks), ((float)j)/((float)yBlocks));
            st *= ofPoint(_tex.getWidth(),_tex.getHeight());
            
            newBlock.value = pixels.getColor( st.x, st.y ).r;//.getBrightness() ;
            newBlock.color = ofColor( newBlock.value );
            newBlock.borderColor = borderColor;
            newBlock.bSelected = false;
            
            blocks.push_back(newBlock);
        }
    }
}

void CloudsVisualSystemMemory::unSelectAll(){
    for(int i = 0; i < blocks.size(); i++){
        blocks[i].bSelected = false;
    }
}


void CloudsVisualSystemMemory::selfUpdate()
{
//    unSelectAll();
    
    if (bSort){
        applySort();
    }
    
    if (bBiDirectionalSort){
        applyBiDirectionalSort();
    }
    
    if (noiseLerp > 0.0){
        int index = 0;
        for(int x = 0; x < xBlocks; x++){
            for(int y = 0; y < yBlocks; y++){
                if ( index < blocks.size() ){
                    blocks[index].color = ofFloatColor( ofLerp( blocks[index].color.getBrightness(), ofNoise(x,y,ofGetElapsedTimef()*0.1), noiseLerp) );
                    blocks[index].value = blocks[index].color.getBrightness()*255;
                    index++;
                } else {
                    break;
                }
            }
        }
    }
    
    for(int i = 0; i < randomSort; i++){
        applyRandomSort();
    }
    
    for(int i = 0; i < randomMix; i++){
        applyRandomMix();
    }
    
    for(int i = 0; i < randomDown; i++){
        applyRandomDown();
    }
    
    for(int i = 0; i < randomUp; i++){
        applyRandomUp();
    }
    
    if (bDeFrag){
        applyDeFrag();
    }
    
}

void CloudsVisualSystemMemory::applySort(){
    for(float i = 0; i < blocks.size()-1; i++){
        
        if ( blocks[i] > blocks[i+1]){
            swapBlocks(i, i+1);
        } else {
            blocks[i].bSelected = false;
        }
    }
}

void CloudsVisualSystemMemory::applyBiDirectionalSort()
{
    int left = 0;
    int right = blocks.size()-1;
    
    int i, j;
    while (left < right){
        
        for (int poz = left; poz < right; poz++){
            if (blocks[poz] > blocks[poz+1]){
                swapBlocks(poz, poz+1);
            } else {
                blocks[poz].bSelected = false;
            }
            right--;
            
            for (int pos = right; pos > left; pos--){
                if (blocks[pos] < blocks[pos-1]){
                    swapBlocks(pos, pos-1);
                } else {
                    blocks[pos].bSelected = false;
                }
                left++;
            }
        }
    }
}

void CloudsVisualSystemMemory::applyRandomSort(){

    int randomCell = int(ofRandom(blocks.size()-1));
    
    if (blocks[randomCell] > blocks[randomCell+1]) {
        swapBlocks(randomCell, randomCell+1);
    } else {
        blocks[randomCell].bSelected = false;
    }
}

void CloudsVisualSystemMemory::applyRandomMix(){
    
    swapBlocks(ofRandom(blocks.size()-1),
               ofRandom(blocks.size()-1));

}

void CloudsVisualSystemMemory::applyRandomUp(){
    
    int A = ofRandom(xBlocks,blocks.size()-1);
    int B = A - xBlocks;
    
    swapBlocks(A,B, false);
}

void CloudsVisualSystemMemory::applyRandomDown(){
    int A = ofRandom(0,blocks.size()-1-xBlocks);
    int B = A + xBlocks;
    
    swapBlocks(A,B, false);
}

void CloudsVisualSystemMemory::applyDeFrag()
{
    //  Let's start in a random place
    //
    int startIndex = ofRandom(blocks.size()-2);
    
    //  Search for the first pair that are not sorted
    //
    for(float i = startIndex; i < blocks.size()-1; i++){
        if ( blocks[i].value > blocks[i+1].value ){
            blocks[i+1].bSelected = true;
            startIndex = i+1;
            break;
        } else {
            blocks[i].bSelected = false;
        }
    }
    
    //  Define the header and what to search for
    //
    int searchFor = blocks[ startIndex ].value;
    int header = startIndex+1;
    
    //  Search for all the elements that have that value
    //
    vector< int > indexes;
    for(float i = header; i < blocks.size(); i++){
        if ( blocks[i].value == searchFor ){
            blocks[i].bSelected = true;
            indexes.push_back(i);
        } else {
            blocks[i].bSelected = false;
        }
    }
    
    //  Go one by one and put them continusly on right next to the beging of the header.
    //
    for (int i = 0; i < indexes.size(); i++){
        if ( header < blocks.size() ){
            swapBlocks( header, indexes[i]);
            header++;
        } else {
            break;
        }
    }
}

void CloudsVisualSystemMemory::swapBlocks(int _indexA, int _indexB, bool _colored){
    swap(blocks[_indexA].value, blocks[_indexB].value);
    swap(blocks[_indexA].color, blocks[_indexB].color);
    blocks[_indexA].bSelected = _colored;
    blocks[_indexB].bSelected = _colored;
}

void CloudsVisualSystemMemory::selfDrawBackground()
{

    ofNoFill();
    ofSetColor(150);
    ofSetLineWidth(0.5);
    ofRect(14,14,ofGetWidth()-28,ofGetHeight()-28);
    
    ofSetColor(100);
    ofSetLineWidth(1);
    ofRect(10,10,ofGetWidth()-20,ofGetHeight()-20);
    
    ofSetLineWidth(0.01);
    for (int i = 0; i < blocks.size(); i++) {
        blocks[i].draw();
    }
}


void CloudsVisualSystemMemory::selfSetupGuis()
{
    
}

void CloudsVisualSystemMemory::selfAutoMode()
{
    
}

void CloudsVisualSystemMemory::selfDrawDebug()
{
    
}

void CloudsVisualSystemMemory::selfSceneTransformation()
{
    
}

void CloudsVisualSystemMemory::selfExit()
{
    
}

void CloudsVisualSystemMemory::selfKeyReleased(ofKeyEventArgs & args)
{

}

void CloudsVisualSystemMemory::mouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemMemory::mouseMoved(ofMouseEventArgs &args)
{
    
}

void CloudsVisualSystemMemory::mousePressed(ofMouseEventArgs &args)
{

}

void CloudsVisualSystemMemory::mouseReleased(ofMouseEventArgs &args)
{
    
}

void CloudsVisualSystemMemory::selfSetupGui()
{
    
}

void CloudsVisualSystemMemory::selfGuiEvent(ofxUIEventArgs &e)
{
    
}