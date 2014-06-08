//
//  smoother.cpp
//  just_you_AA_NI_GLSL
//
//  Created by Jason Levine on 2013-11-12.
//
//

#include "smoother.h"

void smoother::addValue(float value){
    pastValues.push_back(value);
    while (pastValues.size() > numPVals) pastValues.erase(pastValues.begin());
}

float smoother::getMean(){
    float sum = 0;
    for (int i = 0; i < pastValues.size(); i++) sum+=pastValues[i];
    float mean = sum / pastValues.size();
    
    return mean;
}

float smoother::getWMean(float curWeight, float pastWeight){
    float sum = 0;
    for (int i = 0; i < pastValues.size() - 1; i++) sum+=pastValues[i];
    float wMean = (sum / pastValues.size() - 1) * pastWeight + pastValues[pastValues.size() - 1] * curWeight;
    
    return wMean;
}

float smoother::getMedian(){
    vector<float> valuesSorted = pastValues;
    if (valuesSorted.size() % 2 == 0) valuesSorted.erase(valuesSorted.begin());
    ofSort(valuesSorted);
    int middle = valuesSorted.size() / 2 + 1;
    float median = valuesSorted[middle];
    
    return median;
}