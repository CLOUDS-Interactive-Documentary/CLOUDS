//
//  Caster.cpp
//  OpenP5Caustics
//
//  Created by Elie Zananiri on 11/13/2013.
//
//

#include "Caster.h"

//--------------------------------------------------------------
Caster::Caster(int width, int height)
{
    this->width = width;
    this->height = height;
    count = width * height;
    
    bg = new float[width * height];
    
    n = 1.3f;
    d = 100;
    intensity = 1;
    
    bReset = false;
}

//--------------------------------------------------------------
Caster::~Caster()
{
    delete [] bg;
}

//--------------------------------------------------------------
void Caster::reset()
{
    for (int i = 0; i < count; i++) {
        bg[i] = 0;
    }
}

//--------------------------------------------------------------
void Caster::cast(float x, float y, float nX, float nY, float nZ, float thetaI)
{    
    // The angle of the refracted ray relative to the incident ray.
    float thetaR = asin(n*sin(thetaI));
    
    // Refracted vector.
    float rX = -nX;
    float rY = -nY;
    float rZ;
    if (rX == 0 && rY == 0) {
        rZ = -1;
    }
    else {
        rZ = -abs(sqrt(rX*rX+rY*rY)/sin(thetaI+PI-thetaR));
    }
    //float rZ = -sqrt(rX*rX + rY*rY)/sq(sin(thetaI+PI-thetaR));
    if (rZ > 0) {
        ofLogError("Caster::cast") << "Refracted Z must be always negative, was " << rZ;
        return;
    }
    
    // How much we need to scale the refracted vector to hit the back wall.
    float s = d / rZ;
    
    // Coordinates where the vector hits the back wall.
    float hX = x + s * rX;
    float hY = y + s * rY;
    
    if (hX > 0 && hX < width && hY > 0 && hY < height) {
        // Integer of closest pixel to the point of impact.
        int flX = round(hX);
        int flY = round(hY);
        
        int index = flY * width + flX;
        if (index < count && index >= 0) {
            bg[index] += intensity * (1 - ABS(flX - hX)) * (1 - ABS(flY - hY));
        }
        
        // Right neighbour.
        index++;
        if(flX < hX && index < count && index >= 0) {
            bg[index] += intensity * (hX-flX) * abs(flY-hY);
        }
        
        // Left neighbour.
        index -= 2;
        if(flX > hX && index < count && index >= 0) {
            bg[index] += intensity * (flX-hX) * abs(flY - hY);
        }
        
        // Top neighbour.
        index++;
        index -= width;
        if (flY > hY  && index < count && index >= 0) {
            bg[index] += intensity * (flY-hY) * abs(flX - hX);
        }
        
        // Bottom neighbour.
        index += 2 * width;
        if (flY < hY  && index < count && index >= 0) {
            bg[index] += intensity * (hY - flY) * abs(flX - hX);
        }
    }
}
