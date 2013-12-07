//
//  Page.h
//  FloatingPage
//
//  Created by Elie Zananiri on 2012-11-01.
//
//

#pragma once

#include "ofMain.h"

extern float fillAlpha;
extern float meshAlpha;
extern float offsetAmount;
extern float alignAmount;
extern float twirlAmount;
extern float tiltAmount;
extern float flipAmount;
extern float swayAmount;
extern bool bendTail;
extern bool bendWings;
extern bool bendFresh;
extern float topBendAmount;
extern float bottomBendAmount;
extern float tornadoAmount;
extern float speedAmount;
extern float expandAmount;
extern float liftAmount;

static int groundSize = 300;
static int pageSize = 16;
static int apex = 200;

//--------------------------------------------------------------
//--------------------------------------------------------------
enum PageOverwriteMode
{
    PageOverwriteNone,
    PageOverwriteFirst,
    PageOverwriteLast,
    PageOverwriteAll
};

//--------------------------------------------------------------
//--------------------------------------------------------------
class Page
{
    public:
        Page();

        void rebuild();
        void rebuild(float bendPct);
        void rebuild(float bendTopPct, float bendBottomPct);
        void remesh(enum PageOverwriteMode mode);

        void update();
        void draw();
    
        void preDraw();
        void fillDraw();
        void meshDraw();
        void stringDraw(string text, float x, float y, float z);
        void postDraw();
    
        ofPoint boundsMin, boundsMax;

        static float sharedFlipAngle;
        static int sharedLastFlipFrame;
        float localFlipAngle;
        float flipAngle;
        float flipSpeed;

        float animateOffset;
        float animateCounter;

        float alignAngle;
        ofPoint alignPivot;

        bool bGoingUp;
        float posInc;
        float posSpeed;
        ofPoint startPos;
        ofPoint pos;
        ofPoint rainSpeed;
        float tornadoAngle;
        ofPoint tornadoOffset;

        float rotInc;
        float twirlAngle;
        float tiltAngle;

        float vertAngle;
        float vertOffsetY;
        float vertBendPct;

        float topBendPct;
        float bottomBendPct;

        float swayInc;
        float swaySpeed;
        float swayAngle;
        float swayRadius;
        ofPoint swayPos;

        ofPath path;
        ofMesh mesh;
};
