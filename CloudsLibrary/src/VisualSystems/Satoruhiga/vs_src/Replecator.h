#pragma once

#include "ofMain.h"

#if (_MSC_VER || _LIBCPP_VERSION)
#include <memory.h>
#else
#include <tr1/functional>
#endif
namespace std
{
#if (_MSC_VER || _LIBCPP_VERSION)
//	using function;
#else
	using tr1::function;
#endif
}

class Replecator
{
public:
        
        typedef function<void(Replecator *repl)> SetupOp;
        typedef function<void(Replecator *repl)> UpdateOp;
        typedef function<void(Replecator *repl)> DrapOp;
        
        int idx;
        ofMatrix4x4 m;
        
        Replecator *parent;
        vector<Replecator> children;
        
        Replecator() : parent(NULL) {}
        Replecator(Replecator* parent) : parent(parent) {}
        
        void setup(SetupOp sOp, int iteration)
        {
                children.clear();
                
                sOp(this);
                
                for (int i = 0; i < children.size(); i++)
                {
                        children[i].idx = i;
                        children[i].parent = this;
                }
                
                iteration--;
                
                if (iteration > 0)
                {
                        for (int i = 0; i < children.size(); i++)
                        {
                                children[i].setup(sOp, iteration);
                        }
                }
        }
        
        void update(UpdateOp gOp)
        {
                gOp(this);
                
                for (int i = 0; i < children.size(); i++)
                {
                        children[i].update(gOp);
                }
        }
        
        void draw(DrapOp dOp)
        {
                ofPushMatrix();
                ofMultMatrix(m);
                dOp(this);
                
                for (int i = 0; i < children.size(); i++)
                {
                        children[i].draw(dOp);
                }
                ofPopMatrix();
        }
};