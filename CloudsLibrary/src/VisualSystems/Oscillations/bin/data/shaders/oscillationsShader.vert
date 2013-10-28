#version 110

uniform float numPoints;
uniform vec4 targetColor;
uniform vec2 resolution;
uniform float curveProgress;
uniform float precision;
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
	float px = cos((0.7+ 0.09 * 0.3 / resolution.x) * pointID * precision + offsetX);
	float py = sin(pointID * precision + offsetY);
	float pz = abs((-numPoints / 2.0) + pointID) / (numPoints / 2.0);
	return vec3( -curveWidth / 2.0 + px * curveWidth, -curveHeight / 2.0 + py * curveHeight, -curveDepth / 2.0 + curveZPos + pz * curveDepth);
}


void main(){

	gl_TexCoord[0] = gl_MultiTexCoord0;
	vec4 v = vec4(getThePoint(gl_Vertex.z) , 1.); //infer position from z
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * v;
	gl_FrontColor = targetColor; 

}

