//
//  Cog.h
//  MandalaVisualSystem
//
//  Created by laserstorm on 10/12/13.
//
//

#include "ofMain.h"

class Cog : public ofNode{
public:
	Cog(float _radius, float _thickness, float _startU, float _sweepU, float _startV, float _sweepV);
	Cog( float _radius, float _thickness, float _startU, float _sweepU, float _startV, float _sweepV, int _subdU, int _subdV);
	
	Cog();
	
	~Cog();
	
	void setup(float _radius, float _thickness, float _startU, float _sweepU, float _startV, float _sweepV);
	void setup(float _radius,float _thickness, float _startU, float _sweepU, float _startV, float _sweepV, int _subdU, int _subdV);
	
	void draw( ofShader* _shader = NULL, int renderMode = 0);
	
	void drawBorders(ofShader* _shader);
	
	void setupMesh();
	
	void clearMesh();
	
	void clear();
	
	static int radiansToSdubd( float radians );
	
	ofVec3f normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2);
	
	ofVboMesh* createCog( float uRadiansMin, float uRadiansMax, float vRadiansMin, float vRadiansMax, float thickness, int subdX, int subdY );
	
	ofVboMesh* createCogMesh( float low_w, float hi_w, float low_h, float hi_h, float low_d, float hi_d, int _subx, int _suby, int _subz);
	
	
//protected:
	ofVboMesh* m;
	ofVbo edges;
	
	float radius, thickness, minU, maxU, minV, maxV, subdU, subdV, subdScl, sweepU, sweepV;
	bool bDrawWireframe, bDrawBorders, bDrawMesh;
	ofVec2f radianOffset;
	
	//edge vars
	int edgeIndexCount;
	float edgeLinewidth;
	
	ofShader* shader;
	ofTexture* frontTexture;
	ofTexture* sideTexture;
	
	ofFloatColor frontColor, sideColor;
	int useNoiseOfset;
	
};