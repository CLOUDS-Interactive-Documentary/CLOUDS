
#include "lineMatchUtils.h"



bool sortFunc(const distanceResult & a, const distanceResult & b){
    return a.distance < b.distance;
}

bool sortFuncPP(const polyPtr & a, const polyPtr & b){
    return a.distance < b.distance;
}


void distancePP( ofPolyline & a, polyPtr & pp, int nPts ){
    
    
    float dist = 0;
    
    if (nPts != 2){
        float div = 40.0 / (float)nPts;
        for (int i = 0; i < nPts; i++){
            int w = (i * div);
            dist += (a[w] - (*pp.line)[w]).lengthSquared();
        }
        dist /= (float)nPts;
        pp.distance = dist;
    } else {
        
        dist += (a[10] - (*pp.line)[10]).lengthSquared();
        dist += (a[30] - (*pp.line)[30]).lengthSquared();
        dist /= (float)nPts;
        pp.distance = dist;
    }
}



transformation normalizeLineSetGetTrans (ofPolyline & input){
    
    transformation t;
    ofPolyline output = input;
    vector< float > x;
    vector< float > y;
    for (int i = 0; i < output.getVertices().size(); i++){
        x.push_back(output[i].x);
        y.push_back(output[i].y);
    }
    float sumx, meanx, varx, devx, skewx, kurtx;
    float sumy, meany, vary, devy, skewy, kurty;
    computeStats(x.begin( ), x.end( ), sumx, meanx, varx, devx, skewx, kurtx);
    computeStats(y.begin( ), y.end( ), sumy, meany, vary, devy, skewy, kurty);
    float stdDev = sqrt(devx*devx + devy*devy);
    ofPoint midPt (meanx, meany);
    ofPoint dev (stdDev, stdDev);
    
    //ofMatrix4x4 mat;
    t.mat.makeTranslationMatrix(-midPt.x, -midPt.y, 0);
    
    //ofMatrix4x4 mat2;
    t.mat2.makeScaleMatrix(100.0/dev.x, 100.0/dev.y, 1.0);
    return t;
}



void normalizeLineSet(lineSet & lineSet, ofPolyline & input){
    
    ofPolyline output = input;
    vector< float > x;
    vector< float > y;
    for (int i = 0; i < output.getVertices().size(); i++){
        x.push_back(output[i].x);
        y.push_back(output[i].y);
    }
    float sumx, meanx, varx, devx, skewx, kurtx;
    float sumy, meany, vary, devy, skewy, kurty;
    computeStats(x.begin( ), x.end( ), sumx, meanx, varx, devx, skewx, kurtx);
    computeStats(y.begin( ), y.end( ), sumy, meany, vary, devy, skewy, kurty);
    float stdDev = sqrt(devx*devx + devy*devy);
    ofPoint midPt (meanx, meany);
    ofPoint dev (stdDev, stdDev);
    
    ofMatrix4x4 mat;
    mat.makeTranslationMatrix(-midPt.x, -midPt.y, 0);
    
    ofMatrix4x4 mat2;
    mat2.makeScaleMatrix(100.0/dev.x, 100.0/dev.y, 1.0);
    
    
    lineSet.normalizeLines = lineSet.lines;
    
    for (int i = 0; i < lineSet.normalizeLines.size(); i++){
        for (int j = 0; j < lineSet.normalizeLines[i].size(); j++){
            lineSet.normalizeLines[i][j] = lineSet.normalizeLines[i][j]  * mat * mat2;
        }
    }
}




ofPolyline returnNormalizedLine (ofPolyline & input){
    ofPolyline output = input;
    vector< float > x;
    vector< float > y;
    for (int i = 0; i < output.getVertices().size(); i++){
        x.push_back(output[i].x);
        y.push_back(output[i].y);
    }
    float sumx, meanx, varx, devx, skewx, kurtx;
    float sumy, meany, vary, devy, skewy, kurty;
    computeStats(x.begin( ), x.end( ), sumx, meanx, varx, devx, skewx, kurtx);
    computeStats(y.begin( ), y.end( ), sumy, meany, vary, devy, skewy, kurty);
    float stdDev = sqrt(devx*devx + devy*devy);
    ofPoint midPt (meanx, meany);
    ofPoint dev (stdDev, stdDev);
    
    
    ofMatrix4x4 mat;
    mat.makeTranslationMatrix(-midPt.x, -midPt.y, 0);
    
    ofMatrix4x4 mat2;
    mat2.makeScaleMatrix(100.0/dev.x, 100.0/dev.y, 1.0);
    // mat.scale(100,100,1.0);
    
    //mat *= mat2;
    
    
    for (int i = 0; i < output.getVertices().size(); i++){
        
        ofPoint input = output[i];
        
        output[i] -= midPt;
        output[i] /= dev;
        output[i]*= 100.0;
        
        //        cout << output[i] << endl;
        //        cout << "--> " << input << endl;
        //        cout << input * mat * mat2 << endl;
        //        cout << "--> " << (input * mat * mat2) * mat2.getInverse() * mat.getInverse() << endl;
        
    }
    //    ofRectangle boxOrig = input.getBoundingBox();
    //    ofRectangle box = boxOrig;
    //    ofRectangle outputBox(-100,-100,200,200);
    //    box.scaleTo(outputBox);
    //
    //    for (int i = 0; i < output.getVertices().size(); i++){
    //        output.getVertices()[i].x = ofMap( output.getVertices()[i].x, boxOrig.position.x, boxOrig.position.x + boxOrig.width,
    //                                           box.position.x, box.x + box.width);
    //        output.getVertices()[i].y = ofMap( output.getVertices()[i].y, boxOrig.position.y, boxOrig.position.y + boxOrig.height,
    //                                          box.position.x, box.y + box.height);
    //
    //    }
    
    return output;
    
}