//
//  CloudsVisualSystemMemory.cpp
//  VSMemory
//
//  Created by Patricio Gonzalez Vivo on 6/20/13.
//
//

#include "CloudsVisualSystemMemory.h"
#include "CloudsGlobal.h"



string CloudsVisualSystemMemory::getSystemName()
{
	return "Memory";
}

void CloudsVisualSystemMemory::selfSetup()
{
    generate();
    
    // sound
    fMainGain = 0;
    mainGain.value(0);
    synth.setOutputGen(buildSynth() * mainGain);
}

void CloudsVisualSystemMemory::selfSetDefaults(){
    blockScale = 1.0;
    blockWidth = 4;
    blockHeight = 8;
    margin = 2;
    noiseLerp = 0;
    randomUp = 0;
    randomDown = 0;
	borderBase = .5;
    borderColor = ofFloatColor(0.0823, 0.8509, 0.7960);
	//    brightnessOffset = 1;
    bTexture = false;
    bSort = true;
    bDeFrag = false;
    bBiDirectionalSort = false;
    bIs2D = true;
    primaryCursorMode = CURSOR_MODE_INACTIVE;
    secondaryCursorMode = CURSOR_MODE_INACTIVE;
    
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
    
    sysGui->addSlider("Main Gain", 0, 1, &fMainGain);
}

void CloudsVisualSystemMemory::selfSetupRenderGui()
{
    rdrGui->addSlider("margin", 0.0, 10, &margin);
    
    rdrGui->addSlider("block_width", 0.0, 20, &blockWidth);
    rdrGui->addSlider("block_height", 0.0, 20, &blockHeight);
    rdrGui->addSlider("block_scale", 0.5, 10, &blockScale);
	rdrGui->addRangeSlider("block_value_range", 0, 1, &baseColorRange.min, &baseColorRange.max);
	rdrGui->addSlider("block_border_base", 0, 1, &borderBase);
    rdrGui->addSlider("block_border", 0.0, 1.0, &(borderColor.a));
    rdrGui->addSlider("border_red", 0.0, 1.0, &(borderColor.r));
    rdrGui->addSlider("border_green", 0.0, 1.0, &(borderColor.g));
    rdrGui->addSlider("border_blue", 0.0, 1.0, &(borderColor.b));
}

void CloudsVisualSystemMemory::selfBegin()
{
     ofAddListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemMemory::audioRequested);
    generate();
}

void CloudsVisualSystemMemory::selfEnd()
{
    ofRemoveListener(GetCloudsAudioEvents()->diageticAudioRequested, this, &CloudsVisualSystemMemory::audioRequested);
    
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
    
    
    int xMargin = 0;
    int yMargin = 0;
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    int width = getCanvasWidth()-xMargin*2.0;
    int height = getCanvasHeight()-yMargin*2.0;
    
    float widthBlocks = blockWidth*blockScale;
    float heightBlocks = blockHeight*blockScale;
    
    xBlocks = (float)width/(widthBlocks+margin*blockScale);
    yBlocks = (float)height/(heightBlocks+margin*blockScale);
    
    blocks.clear();
	outlineMesh.clear();
	outlineMesh.setMode(OF_PRIMITIVE_LINES);
	outlineMesh.setUsage(GL_STREAM_DRAW);
	
	fillMesh.clear();
	fillMesh.setMode(OF_PRIMITIVE_TRIANGLES);
	outlineMesh.setUsage(GL_STATIC_DRAW);
	
    int index = 0;
    for (int j = 0; j < yBlocks; j++) {
        for (int i = 0; i < xBlocks; i++){
            
            if (index < blocksTotal ){
                
                int x = xMargin + ((margin + blockWidth)*blockScale)*i;
                int y = yMargin + ((margin + blockHeight)*blockScale)*j;
                
                if ( y > (getSharedRenderTarget().getHeight() + margin + heightBlocks)){
                    break;
				}
                
                Block block;
                block.x = x+widthBlocks*0.5;
                block.y = y+heightBlocks*0.5;
                block.width = widthBlocks;
                block.height = heightBlocks;
				if(baseColorRange.min == baseColorRange.max){
					block.color = ofFloatColor(baseColorRange.min);
				}
				else{
					block.color = ofFloatColor( ofMap((unsigned char)data[index], 0, 255,
													  baseColorRange.min,baseColorRange.max), 1.0);
				}
				
                block.borderColor = borderColor;
				block.borderBase = borderBase;
                block.value = (int)data[index];
                block.bSelected = false;
                block.outlineMesh = &outlineMesh;
				block.fillMesh = &fillMesh;
				block.setup();
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
    
    
    int xMargin = 0;
    int yMargin = 0;
    
    //MA: changed ofGetWidth() to getCanvasWidth() and ofGetHeight() to getCanvasHeight()
    int width = getCanvasWidth()-xMargin*2.0;
    int height = getCanvasHeight()-yMargin*2.0;
    
    ofRectangle block;
    block.width = blockWidth*blockScale;
    block.height = blockHeight*blockScale;
    
    xBlocks = (float)width/((blockWidth+margin)*blockScale);
    yBlocks = (float)height/((blockHeight+margin)*blockScale);
    
	outlineMesh.clear();
	outlineMesh.setMode(OF_PRIMITIVE_LINES);
	outlineMesh.setUsage(GL_STREAM_DRAW);
	
	fillMesh.clear();
	fillMesh.setMode(OF_PRIMITIVE_TRIANGLES);
	outlineMesh.setUsage(GL_STATIC_DRAW);

    blocks.clear();
    for (int j = 0; j < yBlocks; j++) {
        for (int i = 0; i < xBlocks; i++){
            
            int x = xMargin + ((margin + blockWidth)*blockScale)*i ;
            int y = xMargin + ((margin + blockHeight)*blockScale)*j ;
            
            Block newBlock;
            newBlock.set(block);
            newBlock.x = x+block.width*0.5;
            newBlock.y = y+block.height*0.5;
            
            ofPoint st = ofPoint( ((float)i)/((float)xBlocks), ((float)j)/((float)yBlocks));
            st *= ofPoint(_tex.getWidth(),_tex.getHeight());
            
            newBlock.value = pixels.getColor( st.x, st.y ).getBrightness();
//            newBlock.color = ofColor( newBlock.value)/brightnessOffset;
			if(baseColorRange.min == baseColorRange.max){
				newBlock.color = ofFloatColor(baseColorRange.min);
			}
			else{
				newBlock.color = ofFloatColor(ofMap(newBlock.value, 0, 255,
													baseColorRange.min,baseColorRange.max), 1.0);
			}
			newBlock.borderBase = borderBase;
            newBlock.borderColor = borderColor;
            newBlock.bSelected = false;
            
			newBlock.outlineMesh = &outlineMesh;
			newBlock.fillMesh = &fillMesh;
			newBlock.setup();

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
	
    for (int i = 0; i < blocks.size(); i++) {
        blocks[i].update();
    }
    
    // sound
    mainGain.value(fMainGain);

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
	swap(blocks[_indexA].bActivated, blocks[_indexB].bActivated	);
    blocks[_indexA].bSelected = _colored;
    blocks[_indexB].bSelected = _colored;
}

void CloudsVisualSystemMemory::selfDrawBackground()
{
    ofSetLineWidth(0.01);
	fillMesh.draw();
	outlineMesh.draw();
}


Generator CloudsVisualSystemMemory::buildSynth()
{
    string strDir = GetCloudsDataPath()+"sound/textures/";
    ofDirectory sdir(strDir);
    string strAbsPath = sdir.getAbsolutePath() + "/CPUBeepsFastDrone_.aif";
    
    SampleTable sample = loadAudioFile(strAbsPath);
    
    Generator sampleGen = BufferPlayer().setBuffer(sample).trigger(1).loop(1);
    
    return sampleGen * 5;
}

void CloudsVisualSystemMemory::audioRequested(ofAudioEventArgs& args)
{
    synth.fillBufferOfFloats(args.buffer, args.bufferSize, args.nChannels);
}


void CloudsVisualSystemMemory::selfSetupGuis()
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

void CloudsVisualSystemMemory::selfMouseDragged(ofMouseEventArgs& data)
{

}

void CloudsVisualSystemMemory::selfMouseMoved(ofMouseEventArgs &args)
{
	for(int i = 0; i < blocks.size(); i++){
		ofRectangle r(blocks[i]);
		r.scaleFromCenter(5.0);
        if(r.inside( args.x, args.y )){
			blocks[i].bActivated = true;
//			break;
		}
    }
}

void CloudsVisualSystemMemory::selfMousePressed(ofMouseEventArgs &args)
{

}

void CloudsVisualSystemMemory::selfMouseReleased(ofMouseEventArgs &args)
{
    
}

void CloudsVisualSystemMemory::selfSetupGui()
{
    
}

void CloudsVisualSystemMemory::selfGuiEvent(ofxUIEventArgs &e)
{
    
}