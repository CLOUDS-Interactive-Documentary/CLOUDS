#version 120

uniform float numPoints;
uniform vec4 targetColor;
uniform vec2 resolution;
uniform float curveProgress;
uniform float selfPrecision;
uniform float offsetX;
uniform float offsetY;
uniform float curveWidth;
uniform float curveHeight;
uniform float curveZPos;
uniform float curveDepth;

/**
 * Gets the point in 3-space. 
 */
vec3 getThePoint(float pointID){
	//Magic numbers in yo face
	float px = cos((0.7+ 0.09 * 0.3 / resolution.x) * pointID * selfPrecision + offsetX);
	float py = sin(pointID * selfPrecision + offsetY);
	float pz = abs((-numPoints / 2.0) + pointID) / (numPoints / 2.0);
	return vec3( -curveWidth / 2.0 + px * curveWidth, -curveHeight / 2.0 + py * curveHeight, -curveDepth / 2.0 + curveZPos + pz * curveDepth);
}


void main(){
	float pointID = gl_Vertex.z;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	vec4 v = vec4(getThePoint(pointID) , 1.); //infer position from z
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * v;
	float progressFalloff = float(pointID/numPoints < curveProgress) * float(pointID != 0.); //TODO: make continuous;
	gl_FrontColor = vec4(targetColor.xyz,  progressFalloff * targetColor.w);

}

