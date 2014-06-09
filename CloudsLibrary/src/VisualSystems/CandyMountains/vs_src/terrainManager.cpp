//
//  terrainManager.cpp
//  audioshadersmesh
//
//  Created by Jason Levine on 2014-02-14.
//
//

#include "terrainManager.h"

void terrainManager::setup(audioAnalytics * _aa,
                           colorSchemeDesigner * _cs) {
    aa = _aa;
    cs = _cs;
    
    for (int i = 0; i < 120; i++) {
        vector<float> wave;
        wave.assign(512, 0);
        waveHistory.push_back(wave);
        waveLoHistory.push_back(ofFloatColor(0.3, 0.5, 0.3));
        waveHiHistory.push_back(ofFloatColor(0.8, 0.5, 0.7));
    }
}


void terrainManager::update() {
    vector<float> wave;
    aa->taps[0]->getSamples(wave, 0);
    wave.resize(512);
    
    waveHistory.push_back(wave);
    if (waveHistory.size() > 120) waveHistory.erase(waveHistory.begin());
    
    for (int i = 0; i < wave.size(); i++) {
        if (waveHistory[waveHistory.size()-1][i] < 0) {
            waveHistory[waveHistory.size()-1][i] = 0;//*= -1;
        }
    }
    
    for (int i = 0; i < wave.size(); i++) {
        smoother heightSmooth;
        heightSmooth.setNumPValues(2);
        for (int j = waveHistory.size()-10; j < waveHistory.size(); j++) {
            heightSmooth.addValue(waveHistory[j][i]);
            waveHistory[j][i] = heightSmooth.getMean();
        }
    }
    
    waveHiHistory.push_back(cs->getRandomColor());
    if (waveHiHistory.size() > 120) waveHiHistory.erase(waveHiHistory.begin());
    
    waveLoHistory.push_back(cs->getRandomColor());
    if (waveLoHistory.size() > 120) waveLoHistory.erase(waveLoHistory.begin());
    
}

//--------------------------------------------------------------
void terrainManager::draw(){
    
    if (waveHistory[0].size() > 0) {
        int width = 512;
        int height = waveHistory.size();
        
        ofMesh mesh;
        
        for (int y = 0; y < height; y++){
            for (int x = 0; x<width; x++){
                float h = -waveHistory[y][x];
                mesh.addVertex(ofPoint(x, h, y)); // * hScale
                
                //                float col = h * colScale;
                //                mesh.addColor(waveHiHistory[y] * col + waveLoHistory[y] * (1.0 - col));
                
                mesh.addColor(waveHiHistory[y]);
//                mesh.addColor(*cs->assignColors[int((h+1)*180) % (int)cs->assignColors.size()]);
//                mesh.addColor(*cs->assignColors[0]);
            }
        }

        for (int y = 0; y < height-1; y++){
            for (int x = 0; x < width-1; x++){
                mesh.addIndex(x+y*width);       // 0
                mesh.addIndex((x+1)+y*width);     // 1
                mesh.addIndex(x+(y+1)*width);     // 10
                
                mesh.addIndex((x+1)+y*width);     // 1
                mesh.addIndex((x+1)+(y+1)*width);   // 11
                mesh.addIndex(x+(y+1)*width);     // 10
            }
        }
        
        meshUtils.calcNormals(mesh, true);
        //        ofDrawAxis(100);
        ofPushMatrix();
        ofRotateY(180);
        ofRotateZ(180);
        ofScale(7,180 * 20,20);
        ofTranslate(-width/2, 0, -height/2);
        mesh.draw();
        ofPopMatrix();
        
    }
    
}

