/*
 *  Gesture.cpp
 *  YellowTail
 *
 *
 */

#include "Gesture.h"

#include "GestureRendererTraveling.h"
#include "GestureRendererInPlace.h"


Gesture::Gesture(int maxWidth, int maxHeight)
{
    this->maxWidth = maxWidth;
    this->maxHeight = maxHeight;
    
	// primary init
	damp	= 5.0f;
	dampInv = 1.0f/damp;
	damp1   = damp - 1;
	thickness = INITIAL_THICKNESS;
	
	points = new Vec3f*[GESTURE_CAPACITY];
	for (int i=0; i<GESTURE_CAPACITY; i++){
		points[i] = new Vec3f();
	}
	
	for (int i=0;i<GESTURE_CAPACITY;i++){
		crosses[i] = CrossLocationNone;
	}
	pointCount = 0;
	polygonCount = 0;
	
	jumpDx = 0;
	jumpDy = 0;
	initialX = 0;
	inity = 0;
    
    mStyle  = GestureStyleInPlace;
    mDirection = GestureDirectionBackwards;
    mFillColor = Color(255, 255, 255, 192);
    mStrokeColor = Color(255, 255, 255, 220);
	
	hasCross = false;
}

//---------------------------------------------------------------------------
void Gesture::clear(){
	pointCount = 0;
	thickness = INITIAL_THICKNESS;
}

//---------------------------------------------------------------------------
void Gesture::fade() {
    mFillColor.a = (unsigned char) ((float)mFillColor.a * 0.925f);
    mStrokeColor.a = (unsigned char) ((float)mStrokeColor.a * 0.900f);
}

//---------------------------------------------------------------------------
bool Gesture::isVisible() {
    return mFillColor.a > 2 || mStrokeColor.a > 2;
}

//---------------------------------------------------------------------------
bool Gesture::isValid() {
    return pointCount > 2;
}

//---------------------------------------------------------------------------
void Gesture::clearPolygons(){
	polygonCount = 0;
	hasCross = false;
}

//---------------------------------------------------------------------------
void Gesture::addPointAndHalfwayPoint (float x, float y, float distanceThresholdForAdding){
	
	if (pointCount < GESTURE_CAPACITY){
		if (pointCount == 0) {
			initialX = x;
			inity = y;
		}
		
		if (pointCount > 0){
			Vec3f *v = points[pointCount-1];
			float prevx = v->x;
			float prevy = v->y;
            
			float hx = (prevx + x)/2.0;
			float hy = (prevy + y)/2.0;
			
			float dx = hx - x;
			float dy = hy - y;
			float dh = sqrtf(dx*dx + dy*dy);
            
			if (dh > distanceThresholdForAdding){
				float p = getPressureFromVelocity(dh);
				points[pointCount++]->set(hx,hy,p); 
			}
		}
		
		if (pointCount < GESTURE_CAPACITY){
			float dh = distanceToLastPoint(x, y);
			float p = getPressureFromVelocity(dh);
			points[pointCount++]->set(x,y,p);
		}
		
		if (pointCount > 1) {
			jumpDx = points[pointCount-1]->x - points[0]->x;
			jumpDy = points[pointCount-1]->y - points[0]->y;
		}
	}
}


//---------------------------------------------------------------------------
void Gesture::addPoint(float x, float y){
	if (pointCount >= GESTURE_CAPACITY){
		// there are all sorts of possible solutions here,
		// but for abject simplicity, I don't do anything.
	} 
	else {
		if (pointCount == 0) {
			initialX = x;
			inity = y;
		}
		
		float v = distanceToLastPoint(x, y);
		float p = getPressureFromVelocity(v);
		points[pointCount++]->set(x,y,p);
		
		if (pointCount > 1) {
			jumpDx = points[pointCount-1]->x - points[0]->x;
			jumpDy = points[pointCount-1]->y - points[0]->y;
		}

	}
	
}

//---------------------------------------------------------------------------
float Gesture::getPressureFromVelocity(float v){
	float scale = 18.0f;
	float minP = 0.02f;
	float oldP = (pointCount > 0) ? points[pointCount-1]->p : 0;
	return ( (minP + MAX(0.0f, 1.0f - v/scale)) + (damp1*oldP) )*dampInv;
}

//---------------------------------------------------------------------------
float Gesture::distanceToLastPoint(float ix, float iy){
	if (pointCount > 0){
		Vec3f *v = points[pointCount-1];
		float dx = v->x - ix;
		float dy = v->y - iy;
		return sqrtf(dx*dx + dy*dy);
	}
	return 30;
}

//---------------------------------------------------------------------------
void Gesture::handleCoordinateOverflow(){
	
	if (pointCount > 0){
		float POS_LIMIT_X =   ofGetWidth()*100;
		float NEG_LIMIT_X = 0-POS_LIMIT_X;
		
		float POS_LIMIT_Y =   ofGetHeight()*100;
		float NEG_LIMIT_Y = 0-POS_LIMIT_Y;
		
		bool bOverflowPosX = false;
		bool bOverflowNegX = false;
		bool bOverflowPosY = false;
		bool bOverflowNegY = false;
		
		for (int i=0; i<pointCount; i++){
			float px = points[i]->x;
			float py = points[i]->y;
			
			if          (px > POS_LIMIT_X){
				bOverflowPosX = true;
			} else if   (px < NEG_LIMIT_X){
				bOverflowNegX = true;
			}
			
			if          (py > POS_LIMIT_Y){
				bOverflowPosY = true;
			} else if   (py < NEG_LIMIT_Y){
				bOverflowNegY = true;
			}
		}
		
		
		if (bOverflowPosX){
			for (int i=0; i<pointCount; i++){
				points[i]->x -= POS_LIMIT_X;
			}
		} else if (bOverflowNegX){
			for (int i=0; i<pointCount; i++){
				points[i]->x -= NEG_LIMIT_X;
			}
		}
		
		if (bOverflowPosY){
			for (int i=0; i<pointCount; i++){
				points[i]->y -= POS_LIMIT_Y;
			}
		} else if (bOverflowNegY){
			for (int i=0; i<pointCount; i++){
				points[i]->y -= NEG_LIMIT_Y;
			}
		}
		
	}
}

//---------------------------------------------------------------------------
void Gesture::advance (){
    int nPts = pointCount; 
    int nPts1 = nPts-1; 
    
    if (nPts > 0){  
        float jx = jumpDx;
        float jy = jumpDy;
     
	    if (mDirection == GestureDirectionBackwards){
	 // if ((mDirection == GestureDirectionBackwards) && (mStyle == GestureStyleInPlace)) { 
            for (int i=nPts1; i>0; i--){ 
                points[i]->x = points[i-1]->x; 
                points[i]->y = points[i-1]->y; 
            } 
            points[0]->x = points[nPts1]->x - jx; 
            points[0]->y = points[nPts1]->y - jy;
            
        } else { 
            // home-style curly fries just like mama used to make
            for (int i=0; i<nPts1; i++){ 
                points[i]->x = points[i+1]->x; 
                points[i]->y = points[i+1]->y; 
            } 
            points[nPts1]->x = points[0]->x + jx; 
            points[nPts1]->y = points[0]->y + jy; 
        }
        
        if (mStyle == GestureStyleInPlace){
            float ix = initialX;
            float iy = inity;
            float ox = points[0]->x;
            float oy = points[0]->y;
            for (int i=0; i<nPts; i++){
                points[i]->x += (ix - ox);
                points[i]->y += (iy - oy);
            }
        }
		
         
        compile();       
    } 
}

void Gesture::smooth (float weight){
	// average neighboring points
	
	float scaling  = 1.0f/(weight + 2.0f);
	int pointCountMinusTwo = pointCount - 2;
	Vec3f * lower;
	Vec3f * center;
	Vec3f * upper;
	
	for (int i=1; i<pointCountMinusTwo; i++){
		lower = points[i-1];
		center = points[i];
		upper = points[i+1];
		
		center->x = (lower->x + weight*(center->x) + upper->x)*scaling;
		center->y = (lower->y + weight*(center->y) + upper->y)*scaling;
	}
}

//---------------------------------------------------------------------------
std::string Gesture::toXML() {
    std::string outString;
    
    ofxXmlSettings xmlSerializer;
    
    xmlSerializer.addTag("gesture");
    xmlSerializer.setAttribute("gesture", "style", mStyle, 0);
    
    xmlSerializer.pushTag("gesture");
    
    for (int i = 0; i < pointCount; i++) {
        int tagID = xmlSerializer.addTag("point");
        xmlSerializer.setValue("point:x", points[i]->x, tagID);
        xmlSerializer.setValue("point:y", points[i]->y, tagID);
    }
    
    xmlSerializer.popTag();
    
    xmlSerializer.copyXmlToString(outString);
    return outString;
}

void Gesture::fromXMLSerializer(ofxXmlSettings xmlSerializer) {
    xmlSerializer.pushTag("gesture");
    
    this->clear();
    mStyle = (GestureStyle) xmlSerializer.getAttribute("", "style", 0, 0);
    
    for (int i = 0; i < xmlSerializer.getNumTags("point"); i++) {
        float x = xmlSerializer.getValue("point:x", 0, i);
        float y = xmlSerializer.getValue("point:y", 0, i);
        this->addPoint(x, y);
    }
    
    xmlSerializer.popTag();
}

void Gesture::fromXMLFile(string xmlFile) {
    ofxXmlSettings xmlSerializer;
    
    xmlSerializer.loadFile(xmlFile);
    
    fromXMLSerializer(xmlSerializer);
}

void Gesture::render() {
    switch (mStyle) {
        case GestureStyleTraveling:
            GestureRendererTraveling::renderGesture(this, maxWidth, maxHeight);
            break;
        case GestureStyleInPlace:
            GestureRendererInPlace::renderGesture(this, maxWidth, maxHeight);
            break;
    }
}

//---------------------------------------------------------------------------
void Gesture::compile(){
	// compute the polygons from the path of Vec3f's
    clearPolygons();
    handleCoordinateOverflow();
    
    float b1x, b2x; //bez
    float b1y, b2y;
    float d1x, d2x; //bez
    float d1y, d2y;
    
    float hyp0, hyp1;
    float ddcx, ddcy, ddch;
    float dabx, daby, dabh;
    
    Vec3f *p0, *p1, *p2;
    float radius0, radius1;
    float ax, bx, cx, dx;
    float ay, by, cy, dy;
    float axi, bxi, cxi, dxi, axip, axid;
    float ayi, byi, cyi, dyi, ayip, ayid;
    float p1x, p1y;
    float dx01, dy01, hp01, si01, co01;
    float dx02, dy02, hp02, si02, co02;
    float taper = 1.0f;
    
    int  nPathPoints = pointCount - 1;
    int  lastPolyIndex = nPathPoints - 1;
    float npm1finv =  1.0f/(float)(MAX(1, nPathPoints - 1));
    
    // handle the first point
    p0 = points[0];
    p1 = points[1];
    radius0 = p0->p*thickness;
    dx01 = p1->x - p0->x;
    dy01 = p1->y - p0->y;
    hyp0 = hyp1 = hp01 = sqrtf (dx01*dx01 + dy01*dy01);
    if (hp01 == 0) {
        hp02 = 0.0001f;
    }
    co01 = radius0 * dx01 / hp01;
    si01 = radius0 * dy01 / hp01;
    dx = ax = p0->x - si01;	
    dy = ay = p0->y + co01;
    cx = bx = p0->x + si01;	
    cy = by = p0->y - co01;
    
    
    int LC = 20;
    int RC = maxWidth-LC;
    int TC = 20;
    int BC = maxHeight-TC;
    float mint = 0.618f;
    double tapow = 0.4f;
    
    // handle the middle points
    int i=1;
    YtPolygon *apoly;
    float* xpoints;
    float* ypoints;
    
    
    const float U1   = 0.33333;
    const float U2   = U1*U1;
    const float U3   = U2*U1;
    const float omU1 = 1.0 - U1;
    const float omU2 = omU1*omU1;
    const float omU3 = omU2*omU1;
    const float U2omU13 = 3.0*U2*omU1;
    const float U1omU23 = 3.0*U1*omU2;
    
    const float V1   = 0.66666;
    const float V2   = V1*V1;
    const float V3   = V2*V1;
    const float omV1 = 1.0 - V1;
    const float omV2 = omV1*omV1;
    const float omV3 = omV2*omV1;
    const float V2omV13 = 3.0*V2*omV1;
    const float V1omV23 = 3.0*V1*omV2;
    
    float ddcyDdchHyp1025;
    float ddcxDdchHyp1025;
    float dabyDabhHyp0025; 
    float dabxDabhHyp0025; 
    
    const float epsilon = 0.00001f;
    
    for (i=1; i<nPathPoints; i++){
        taper =  (powf((lastPolyIndex-i)*npm1finv,tapow));
        
        p0 = points[i-1];
        p1 = points[i  ];
        p2 = points[i+1];
        
        p1x = p1->x;
        p1y = p1->y;
        radius1 = MAX(mint,taper*p1->p*thickness);
        
        // assumes all segments are roughly the same length...
        dx02 = p2->x - p0->x;
        dy02 = p2->y - p0->y;
        hyp1 = hp02 = sqrtf (dx02*dx02 + dy02*dy02);
        if (hp02 > epsilon) {
            hp02 = radius1/hp02;
        }
        co02 = dx02 * hp02;
        si02 = dy02 * hp02;
        
        // translate the coordinates to the viewing rectangle
        axi = axip = ax;
        ayi = ayip = ay;
        if (mStyle == GestureStyleTraveling){
            axi = GMOD(axi, maxWidth); // (axi<0)?(w-((-axi)%w)):axi%w; 
            ayi = GMOD(ayi, maxHeight); // (ayi<0)?(h-((-ayi)%h)):ayi%h;
        }
        axid = axi-axip;
        ayid = ayi-ayip;
        
        // set the vertices of the polygon
        apoly = &polygons[polygonCount++];
        xpoints = apoly->xpoints;
        ypoints = apoly->ypoints;
        
        xpoints[0] = axi = axid + axip;
        ypoints[0] = ayi = ayid + ayip;
        xpoints[1] = bxi = axid + bx;
        ypoints[1] = byi = ayid + by;
        
        xpoints[2] = cxi = axid + (cx = p1x + si02);
        ypoints[2] = cyi = ayid + (cy = p1y - co02);
        xpoints[3] = dxi = axid + (dx = p1x - si02);
        ypoints[3] = dyi = ayid + (dy = p1y + co02);
        
        /*
         2  3
         6  7
         5  4
         1  0
         */
        
        //----------------------------------------
        // bez control pts:
        ddcx = dxi - cxi; 
        ddcy = dyi - cyi;
        ddch = hyp1*0.25 / sqrt(ddcx*ddcx + ddcy*ddcy);
        dabx = axi - bxi; 
        daby = ayi - byi;
        dabh = hyp0*0.25 / sqrt(dabx*dabx + daby*daby);
        
        ddcyDdchHyp1025 = ddcy*ddch;
        ddcxDdchHyp1025 = ddcx*ddch;
        dabyDabhHyp0025 = daby*dabh; 
        dabxDabhHyp0025 = dabx*dabh; 
        
        b2x = dxi - ddcyDdchHyp1025;
        b2y = dyi + ddcxDdchHyp1025;
        b1x = axi + dabyDabhHyp0025;
        b1y = ayi - dabxDabhHyp0025;
        d2x = cxi - ddcyDdchHyp1025;
        d2y = cyi + ddcxDdchHyp1025;
        d1x = bxi + dabyDabhHyp0025;
        d1y = byi - dabxDabhHyp0025;
        
        xpoints[4] = (axi*omU3) + (b1x*U1omU23) + (b2x*U2omU13) + (dxi*U3);
        ypoints[4] = (ayi*omU3) + (b1y*U1omU23) + (b2y*U2omU13) + (dyi*U3);
        xpoints[5] = (bxi*omU3) + (d1x*U1omU23) + (d2x*U2omU13) + (cxi*U3);
        ypoints[5] = (byi*omU3) + (d1y*U1omU23) + (d2y*U2omU13) + (cyi*U3);
        xpoints[6] = (bxi*omV3) + (d1x*V1omV23) + (d2x*V2omV13) + (cxi*V3);
        ypoints[6] = (byi*omV3) + (d1y*V1omV23) + (d2y*V2omV13) + (cyi*V3);
        xpoints[7] = (axi*omV3) + (b1x*V1omV23) + (b2x*V2omV13) + (dxi*V3);
        ypoints[7] = (ayi*omV3) + (b1y*V1omV23) + (b2y*V2omV13) + (dyi*V3);
        
        crosses[i] = CrossLocationNone;
        if (mStyle == GestureStyleTraveling){
            
            // keep a record of where we cross the edge of the screen
            if ((axi<=LC)||(bxi<=LC)||(cxi<=LC)||(dxi<=LC)){ 
                crosses[i] = (CrossLocation) (crosses[i] | CrossLocationLeft);
            }
            if ((axi>=RC)||(bxi>=RC)||(cxi>=RC)||(dxi>=RC)){ 
                crosses[i] = (CrossLocation) (crosses[i] | CrossLocationRight);
            }
            if ((ayi<=TC)||(byi<=TC)||(cyi<=TC)||(dyi<=TC)){ 
                crosses[i] = (CrossLocation) (crosses[i] | CrossLocationTop);
            }
            if ((ayi>=BC)||(byi>=BC)||(cyi>=BC)||(dyi>=BC)){ 
                crosses[i] = (CrossLocation) (crosses[i] | CrossLocationBottom);
            }
            
            hasCross |= crosses[i];
        }
        
        //swap data for next time
        ax = dx; 
        ay = dy;
        bx = cx; 
        by = cy;
        hyp0 = hyp1;
    }
    
    // handle the last point
    p2 = points[nPathPoints];
    apoly = &polygons[polygonCount++];
    
    apoly->xpoints[0] = ax;
    apoly->ypoints[0] = ay;
    apoly->xpoints[1] = bx;
    apoly->ypoints[1] = by;
    apoly->xpoints[2] = cxi = (p2->x);
    apoly->ypoints[2] = cyi = (p2->y);
    apoly->xpoints[3] = dxi = (p2->x);
    apoly->ypoints[3] = dyi = (p2->y);
    
    b2x = cxi - (dyi - cyi)*0.5;
    b2y = cyi + (dxi - cxi)*0.5;
    apoly->xpoints[4] = b2x;
    apoly->ypoints[4] = b2y;
}
