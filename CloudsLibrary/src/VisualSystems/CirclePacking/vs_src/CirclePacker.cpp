//
//  CirclePacker.cpp
//  CirclePacking
//
//  Created by Jonathan Minard on 12/9/13.
//
//

#include "CirclePacker.h"


CirclePacker::CirclePacker()
{
    
    
}

CirclePacker::CirclePacker(float _width, float _height)
{
    width = _width;
    height = _height;
    xcenter = width/2;
    ycenter = height/2;
    padding = 5;
    damping = 0.01;
    iterations = 1;
}

float CirclePacker::fast_distance(float _x1, float _y1, float _x2, float _y2)
{
    return (_x1 - _x2) * (_x1 - _x2) + (_y1 - _y2) * (_y1 - _y2);
}

void CirclePacker::pack()
{
    
    for (int i = 0; i < circles.size(); i++)
    {
        Circle& c1 = circles[i];
        
        for (int j = i+1; j < circles.size(); j++)
        {
            Circle& c2 = circles[j];   // Circle& c2 = circles[j]
            
            float d = fast_distance(c1.x, c1.y, c2.x, c2.y);
            float r = c1.r + c2.r + padding;
            
            if (d < (r*r))
            {
                float dx = c2.x - c1.x;
                float dy = c2.y - c1.y;
                float droot = sqrt(d);
                
                // proviamo a dare un peso rispetto al centro
                float cd1 = ofDistSquared(c1.x, c1.y, xcenter, ycenter);
                float cd2 = ofDistSquared(c1.x, c1.y, xcenter, ycenter);
//                float cd1 = fast_distance(c1.x, c1.y, xcenter, ycenter);
//                float cd2 = fast_distance(c1.x, c1.y, xcenter, ycenter);
                
                float total = dx + dy;
                
                float vx = (dx/droot) * (r-droot);
                float vy = (dy/droot) * (r-droot);
                
                c1.x -= vx * cd1/(cd1+cd2);
                c1.y -= vy * cd1/(cd1+cd2);
                c2.x += vx * cd2/(cd1+cd2);
                c2.y += vy * cd2/(cd1+cd2);
            }
        }
    }
    
    // contraction...
    //

     for (int i = 0; i < circles.size(); i++) {
		 Circle& c = circles[i];
		 float vx = (c.x - xcenter) * damping;
		 float vy = (c.y - ycenter) * damping;
		 c.x -= vx;
		 c.y -= vy;
     }

}

void CirclePacker::update() {
    for (int w=0; w<iterations; w++)
    {
        this->pack();
    }
}
/**
 * Draw all the circles
 */

void CirclePacker::draw(bool _nasdaq, bool _blanks)
{
    if (_blanks == true){
    for (int i = 0; i < circles.size(); i++)
    {
        Circle& c = circles[i];
        if (c.r < 1)
        {
          circles.erase(circles.begin() + i);
        }
        else
        {
            c.draw();
        }
        }
    }
    
    if (_nasdaq == true){
        for (int i = 0; i < circles.size(); i++)
        {
            Circle& c = circles[i];
            if (c.r < 1)
            {
                circles.erase(circles.begin() + i);
            }
            else
            {
                c.drawCompanies();
            }
        }
        
    }
    
}

