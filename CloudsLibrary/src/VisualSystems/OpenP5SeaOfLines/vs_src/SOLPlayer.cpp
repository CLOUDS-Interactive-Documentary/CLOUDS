//
//  SOLPlayer.cpp
//  OpenP5SeaOfLines
//
//  Created by Elie Zananiri on 2013-11-01.
//
//

#include "SOLPlayer.h"

//--------------------------------------------------------------
float SOLPlayer::width;
float SOLPlayer::height;

//--------------------------------------------------------------
SOLPlayer::SOLPlayer(float x, float y, float max)
{
    this->x = x;
    this->y = y;
    
    size = ofRandom(2, max);

    speed = ((max - size) / 3) + 0.5f;
    float angle = ofRandom(TWO_PI);
    sx = cosf(angle) * speed;
    sy = sinf(angle) * speed;
    
    col.set(ofRandom(100, 255), ofRandom(100, 255), ofRandom(50));

//    personnages = (people[]) append (personnages, this);
}

//--------------------------------------------------------------
void SOLPlayer::update()
{
    x += sx;
    y += sy;
    
    sy += 0.1f;
    
    // Bounce off walls.
    if (x < size / 2) {
        x = size / 2;
        sx *= -1;
    }
    else if (x > width - size / 2) {
        x = width - size / 2;
        sx *= -1;
    }
    if (y < size / 2) {
        y = size / 2;
        sy *= -1;
    }
    else if (y > height - size / 2) {
        y = height - size / 2;
        sy *= -1;
    }    
}
